#!/usr/bin/env bash

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
SWITCH_SCRIPT="${ROOT}/scripts/remote/switch-warlock-plan.sh"
LAUNCH_SCRIPT="${ROOT}/scripts/remote/run-warlock-remote.sh"
BINDING_FILE="${ROOT}/data/network_plans/operational_25node_complex-r1.nrm.scenario"
INTERACTIVE_SCENARIO="${ROOT}/test_mission/operational_strike_demo/interactive.txt"

grep -q 'NRM_SYSTEMCTL_COMMAND' "${SWITCH_SCRIPT}"
grep -q 'NRM_SWITCH_REQUEST_FILE' "${SWITCH_SCRIPT}"
grep -q 'restart nrm-warlock.service' "${SWITCH_SCRIPT}"
if grep -q 'kill -0' "${SWITCH_SCRIPT}"
then
   echo "switch helper must delegate the complete restart to systemd" >&2
   exit 1
fi
grep -q 'NRM_SWITCH_REQUEST_FILE' "${LAUNCH_SCRIPT}"
grep -Eq '^export NRM_SOURCE$' "${LAUNCH_SCRIPT}"
grep -qx 'test_mission/operational_strike_demo/interactive.txt' "${BINDING_FILE}"
grep -Eq '^realtime[[:space:]]*$' "${INTERACTIVE_SCENARIO}"
grep -Eq '^clock_rate[[:space:]]+1([.]0)?[[:space:]]*$' "${INTERACTIVE_SCENARIO}"
grep -q 'WsfSimulation.SetClockRate(0.0001)' "${INTERACTIVE_SCENARIO}"
grep -q 'NRM_OPERATIONAL CLOCK_RATE_GUARD' "${INTERACTIVE_SCENARIO}"
grep -Eq '^end_time[[:space:]]+31536000[[:space:]]+s[[:space:]]*$' "${INTERACTIVE_SCENARIO}"

TEMP_ROOT=$(mktemp -d)
trap 'rm -rf "${TEMP_ROOT}"' EXIT
SCENARIO="${TEMP_ROOT}/scenario.txt"
PLAN="${TEMP_ROOT}/plan.nrm"
REQUEST="${TEMP_ROOT}/switch.request"
SYSTEMCTL_LOG="${TEMP_ROOT}/systemctl.log"
touch "${SCENARIO}" "${PLAN}"

cat >"${TEMP_ROOT}/systemctl" <<'EOF'
#!/usr/bin/env bash
printf '%s\n' "$*" >"${NRM_TEST_SYSTEMCTL_LOG}"
EOF
chmod +x "${TEMP_ROOT}/systemctl"

NRM_SWITCH_REQUEST_FILE="${REQUEST}" \
NRM_SYSTEMCTL_COMMAND="${TEMP_ROOT}/systemctl" \
NRM_TEST_SYSTEMCTL_LOG="${SYSTEMCTL_LOG}" \
"${SWITCH_SCRIPT}" "${SCENARIO}" "${PLAN}"

mapfile -t REQUEST_LINES <"${REQUEST}"
[[ "${REQUEST_LINES[0]}" == "${SCENARIO}" ]]
[[ "${REQUEST_LINES[1]}" == "${PLAN}" ]]
grep -qx -- '--user restart nrm-warlock.service' "${SYSTEMCTL_LOG}"
