#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
DEFAULT_AFSIM_BUILD=/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24
AFSIM_BUILD_VALUE=${AFSIM_BUILD:-$DEFAULT_AFSIM_BUILD}
MISSION="$AFSIM_BUILD_VALUE/mission"
SCENARIO="$ROOT/test_mission/operational_strike_demo/pdr_validation.txt"
TRAFFIC="$ROOT/test_mission/operational_strike_demo/interactive_traffic.txt"

test -x "$MISSION"
test -f "$SCENARIO"
test -f "$TRAFFIC"

for network in LINK11 LINK16 SATCOM CDL
do
  grep -q "NRM_PDR_PROFILE $network" "$TRAFFIC"
done

run_scenario()
{
  local output_file=$1
  timeout 120 "$MISSION" "$SCENARIO" >"$output_file" 2>&1
  grep -q 'Simulation complete' "$output_file"
  if grep -Eq 'Initialization of simulation failed|Reading of simulation input failed|Cannot open file' "$output_file"
  then
    printf 'PDR validation scenario did not initialize cleanly.\n' >&2
    return 1
  fi
}

first_log=$(mktemp /tmp/nrm-pdr-diversity-first.XXXXXX.log)
second_log=$(mktemp /tmp/nrm-pdr-diversity-second.XXXXXX.log)
trap 'rm -f "$first_log" "$second_log"' EXIT

run_scenario "$first_log"
run_scenario "$second_log"

first_results=$(grep '^NRM_PDR_PHASE_RESULT ' "$first_log")
second_results=$(grep '^NRM_PDR_PHASE_RESULT ' "$second_log")
test "$first_results" = "$second_results"

first_acceptance_results=$(grep '^NRM_ACCEPTANCE_LINK_RESULT ' "$first_log" || true)
second_acceptance_results=$(grep '^NRM_ACCEPTANCE_LINK_RESULT ' "$second_log" || true)
if test -z "$first_acceptance_results"
then
  printf 'Missing per-link acceptance probe results for airborne_relay -> spectrum_monitor_aircraft.\n' >&2
  exit 1
fi
test "$first_acceptance_results" = "$second_acceptance_results"

printf '%s\n' "$first_results" | awk '
  {
    phase = $2
    network = $3
    split($4, tx_field, "=")
    tx = tx_field[2] + 0
    split($6, pdr_field, "=")
    pdr = pdr_field[2] + 0.0
    key = phase ":" network
    seen[key] = 1
    transmitted[key] = tx
    value[key] = pdr
    if (phase != "HEALTHY") {
      nearest_five = int((pdr + 2.5) / 5.0) * 5.0
      distance_from_five = pdr - nearest_five
      if (distance_from_five < 0.0) distance_from_five = -distance_from_five
      if (distance_from_five > 0.05) non_quantized = non_quantized + 1
    }
  }
  END {
    if (length(seen) != 20) {
      print "Expected five phases for all four networks" > "/dev/stderr"
      exit 1
    }
    networks[1] = "LINK11"
    networks[2] = "LINK16"
    networks[3] = "SATCOM"
    networks[4] = "CDL"
    for (i = 1; i <= 4; ++i) {
      network = networks[i]
      if (value["HEALTHY:" network] < 97.0) {
        print "Healthy phase must remain above the acceptance floor" > "/dev/stderr"
        exit 1
      }
      if (value["STRESSED:" network] >= value["DEGRADED:" network]) {
        print "Stressed phase must be worse than degraded phase" > "/dev/stderr"
        exit 1
      }
      if (value["RECOVERY:" network] <= value["STRESSED:" network]) {
        print "Recovery phase must improve over stressed phase" > "/dev/stderr"
        exit 1
      }
      if (value["STABILIZED:" network] < value["RECOVERY:" network]) {
        print "Stabilized phase must not regress after recovery" > "/dev/stderr"
        exit 1
      }
    }
    if (!(value["DEGRADED:CDL"] > value["DEGRADED:LINK16"] &&
          value["DEGRADED:LINK16"] > value["DEGRADED:LINK11"] &&
          value["DEGRADED:LINK11"] > value["DEGRADED:SATCOM"])) {
      print "Degraded phase must retain network-specific quality ordering" > "/dev/stderr"
      exit 1
    }
    if (non_quantized < 8) {
      print "Dynamic PDR remains artificially quantized to five-percent steps" > "/dev/stderr"
      exit 1
    }
    if (transmitted["DEGRADED:LINK11"] == transmitted["DEGRADED:LINK16"] &&
        transmitted["DEGRADED:LINK16"] == transmitted["DEGRADED:SATCOM"] &&
        transmitted["DEGRADED:SATCOM"] == transmitted["DEGRADED:CDL"]) {
      print "All networks still use the same synthetic sampling cadence" > "/dev/stderr"
      exit 1
    }
  }
'

printf '%s\n' "$first_acceptance_results" | awk '
  {
    phase = $2
    split($3, tx_field, "=")
    split($4, rx_field, "=")
    split($5, pdr_field, "=")
    tx[phase] = tx_field[2] + 0
    rx[phase] = rx_field[2] + 0
    pdr[phase] = pdr_field[2] + 0.0
    seen[phase] = 1
  }
  END {
    if (length(seen) != 5) {
      print "Expected five acceptance-link quality phases" > "/dev/stderr"
      exit 1
    }
    phases[1] = "HEALTHY"
    phases[2] = "DEGRADED"
    phases[3] = "STRESSED"
    phases[4] = "RECOVERY"
    phases[5] = "STABILIZED"
    for (i = 1; i <= 5; ++i) {
      phase = phases[i]
      if (tx[phase] <= 0 || rx[phase] <= 0) {
        print "Acceptance link must carry real transmitted and received messages" > "/dev/stderr"
        exit 1
      }
    }
    if (pdr["STRESSED"] >= pdr["DEGRADED"] ||
        pdr["RECOVERY"] <= pdr["STRESSED"] ||
        pdr["STABILIZED"] < pdr["RECOVERY"]) {
      print "Acceptance-link PDR does not show degrade/stress/recovery behavior" > "/dev/stderr"
      exit 1
    }
    if (pdr["DEGRADED"] >= 99.9 || pdr["DEGRADED"] <= 70.0) {
      print "Acceptance-link degraded PDR is not realistic for the demonstration" > "/dev/stderr"
      exit 1
    }
  }
'

printf '%s\n' "$first_results"
printf '%s\n' "$first_acceptance_results"
echo 'Deterministic four-network dynamic PDR profile passed.'
