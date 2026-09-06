#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
SCENARIO_DIR="$ROOT/test_mission/operational_strike_demo"
GATEWAYS="$SCENARIO_DIR/gateways.txt"
INTERACTIVE_TRAFFIC="$SCENARIO_DIR/interactive_traffic.txt"
LAUNCHER="$ROOT/scripts/remote/run-operational-strike-warlock.sh"

test -f "$GATEWAYS"
test -f "$INTERACTIVE_TRAFFIC"
grep -q '^include gateways.txt$' "$SCENARIO_DIR/main.txt"
grep -q '^include interactive_traffic.txt$' "$SCENARIO_DIR/interactive.txt"
if grep -q '^include interactive_traffic.txt$' "$SCENARIO_DIR/main.txt"
then
  printf 'Interactive traffic must not change the deterministic main scenario\n' >&2
  exit 1
fi

for marker in LINK11 LINK16 SATCOM CDL
do
  grep -q "NRM_OPERATIONAL LIVE_TRAFFIC $marker" "$INTERACTIVE_TRAFFIC"
done
grep -q 'TIME_NOW <= 180.0' "$INTERACTIVE_TRAFFIC"

base_platforms=$(grep -c '^platform ' "$SCENARIO_DIR/laydown.txt")
gateway_platforms=$(grep -c '^platform gw_' "$GATEWAYS")
test "$base_platforms" -eq 25
test "$gateway_platforms" -eq 12
test $((base_platforms + gateway_platforms)) -eq 37

interactive_platforms=0
while read -r included_file
do
  included_platforms=$(grep -c '^platform ' "$SCENARIO_DIR/$included_file" || true)
  interactive_platforms=$((interactive_platforms + included_platforms))
done < <(awk '$1 == "include" { print $2 }' "$SCENARIO_DIR/interactive.txt")
if [[ "$interactive_platforms" -ne 37 ]]
then
  printf 'Expected interactive GUI scene to compose 37 platforms, got %s\n' \
    "$interactive_platforms" >&2
  exit 1
fi

endpoint_count=$(grep -c '^      member ' "$SCENARIO_DIR/communications.txt")
directed_link_count=$(grep -c '^      link ' "$SCENARIO_DIR/communications.txt")
test "$endpoint_count" -eq 53
test "$directed_link_count" -eq 108

for pair in l11_l16 l11_satcom l11_cdl l16_satcom l16_cdl satcom_cdl; do
  test "$(grep -c "^platform gw_${pair}_" "$GATEWAYS")" -eq 2
done

test "$(grep -c '^      gateway_id gw_' "$GATEWAYS")" -eq 12
test "$(grep -c '^nrm_gateway_route$' "$GATEWAYS")" -ge 14
grep -q '^   route_id route_l11_satcom_cdl_cascade$' "$GATEWAYS"
grep -q '^   route_id route_cdl_l16_l11_cascade$' "$GATEWAYS"

for network in nrm_link11_coordination nrm_link16_tactical nrm_satcom_command nrm_cdl_isr; do
  grep -A80 "^network $network " "$SCENARIO_DIR/communications.txt" |
    grep -q 'member gw_'
done

grep -q 'NRM_OPERATIONAL PHASE GATEWAY_CASCADE_L11_CDL_SENT' \
  "$SCENARIO_DIR/events.txt"
grep -q 'NRM_OPERATIONAL PHASE GATEWAY_CASCADE_CDL_L11_SENT' \
  "$SCENARIO_DIR/events.txt"

for marker in \
  GATEWAY_PAIR_L11_L16_SENT \
  GATEWAY_PAIR_L11_SATCOM_SENT \
  GATEWAY_PAIR_L11_CDL_SENT \
  GATEWAY_PAIR_L16_SATCOM_SENT \
  GATEWAY_PAIR_L16_CDL_SENT \
  GATEWAY_PAIR_SATCOM_CDL_SENT; do
  grep -q "NRM_OPERATIONAL PHASE $marker" "$SCENARIO_DIR/events.txt"
done

grep -q '^readonly FIXED_TEST_COUNT=45$' "$ROOT/scripts/run_preacceptance.sh"
grep -q 'nrm_ui_scale_test' "$ROOT/scripts/ai_guard.sh"
grep -q 'nrm_plan_scenario_binding_test' "$ROOT/scripts/ai_guard.sh"
grep -q 'nrm_demand_detail_tabs_test' "$ROOT/scripts/ai_guard.sh"
grep -q 'nrm_operational_detail_tabs_test' "$ROOT/scripts/ai_guard.sh"
grep -q '^   cross_domain_gateway_smoke$' "$ROOT/scripts/run_preacceptance.sh"
grep -q '^      cross_domain_gateway_smoke)$' "$ROOT/scripts/run_preacceptance.sh"

grep -q 'Warlock UI will open in the configured VNC desktop' "$LAUNCHER"
grep -q 'Keep this terminal open; press Ctrl+C to stop' "$LAUNCHER"

# Execute the real launcher against isolated process-control and Warlock
# boundaries.  The GUI entry point must use the long-running realtime scene;
# main.txt is reserved for deterministic command-line validation and can finish
# before Qt gets an opportunity to map its window.
TEMP_ROOT=$(mktemp -d)
trap 'rm -rf "${TEMP_ROOT}"' EXIT
mkdir -p \
  "$TEMP_ROOT/scripts/remote" \
  "$TEMP_ROOT/test_mission/operational_strike_demo" \
  "$TEMP_ROOT/bin"
cp "$LAUNCHER" "$TEMP_ROOT/scripts/remote/run-operational-strike-warlock.sh"
touch \
  "$TEMP_ROOT/test_mission/operational_strike_demo/main.txt" \
  "$TEMP_ROOT/test_mission/operational_strike_demo/interactive.txt"

cat >"$TEMP_ROOT/bin/systemctl" <<'EOF'
#!/usr/bin/env bash
if [[ "$*" == "--user is-active --quiet nrm-warlock.service" ]]
then
  exit 1
fi
printf 'Unexpected systemctl call: %s\n' "$*" >&2
exit 97
EOF
chmod +x "$TEMP_ROOT/bin/systemctl"

cat >"$TEMP_ROOT/scripts/remote/run-warlock-remote.sh" <<'EOF'
#!/usr/bin/env bash
printf '%s\n' "$@" >"${NRM_TEST_WARLOCK_ARGUMENTS}"
EOF
chmod +x "$TEMP_ROOT/scripts/remote/run-warlock-remote.sh"

NRM_TEST_WARLOCK_ARGUMENTS="$TEMP_ROOT/warlock-arguments.log" \
PATH="$TEMP_ROOT/bin:$PATH" \
  "$TEMP_ROOT/scripts/remote/run-operational-strike-warlock.sh"

expected_interactive_scene="$TEMP_ROOT/test_mission/operational_strike_demo/interactive.txt"
actual_scene=$(head -n 1 "$TEMP_ROOT/warlock-arguments.log")
if [[ "$actual_scene" != "$expected_interactive_scene" ]]
then
  printf 'Expected GUI launcher scene %s, got %s\n' \
    "$expected_interactive_scene" "$actual_scene" >&2
  exit 1
fi

echo "Operational gateway scenario contract passed."
