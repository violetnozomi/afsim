#!/usr/bin/env bash

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
SWITCH_SCRIPT="${ROOT}/scripts/remote/switch-warlock-plan.sh"
LAUNCH_SCRIPT="${ROOT}/scripts/remote/run-warlock-remote.sh"
PREACCEPTANCE_SCRIPT="${ROOT}/scripts/run_preacceptance.sh"
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
exit "${NRM_TEST_SYSTEMCTL_EXIT:-0}"
EOF
chmod +x "${TEMP_ROOT}/systemctl"

NRM_SWITCH_REQUEST_FILE="${REQUEST}" \
NRM_SYSTEMCTL_COMMAND="${TEMP_ROOT}/systemctl" \
NRM_TEST_SYSTEMCTL_LOG="${SYSTEMCTL_LOG}" \
"${SWITCH_SCRIPT}" "${SCENARIO}" "${PLAN}"

mapfile -t REQUEST_LINES <"${REQUEST}"
[[ "${#REQUEST_LINES[@]}" -eq 2 ]]
[[ "${REQUEST_LINES[0]}" == "${SCENARIO}" ]]
[[ "${REQUEST_LINES[1]}" == "${PLAN}" ]]
grep -qx -- '--user restart nrm-warlock.service' "${SYSTEMCTL_LOG}"

# Acceptance launches still preload the fixed plan, but must restore the
# acceptance workflow instead of opening the plan page after Warlock restarts.
rm -f "${REQUEST}"
NRM_SWITCH_REQUEST_FILE="${REQUEST}" \
NRM_SYSTEMCTL_COMMAND="${TEMP_ROOT}/systemctl" \
NRM_TEST_SYSTEMCTL_LOG="${SYSTEMCTL_LOG}" \
"${SWITCH_SCRIPT}" "${SCENARIO}" "${PLAN}" acceptance

mapfile -t REQUEST_LINES <"${REQUEST}"
[[ "${#REQUEST_LINES[@]}" -eq 3 ]]
[[ "${REQUEST_LINES[0]}" == "${SCENARIO}" ]]
[[ "${REQUEST_LINES[1]}" == "${PLAN}" ]]
[[ "${REQUEST_LINES[2]}" == acceptance ]]

# Exercise the managed launcher with a controlled Warlock executable. The
# one-shot request must become both the mission argument and startup-page
# environment consumed by the in-process plugin.
FAKE_AFSIM_SOURCE="${TEMP_ROOT}/afsim-source"
FAKE_AFSIM_BUILD="${TEMP_ROOT}/afsim-build"
FAKE_RESOURCES="${TEMP_ROOT}/resources"
FAKE_REMOTE_ROOT="${TEMP_ROOT}/remote"
WARLOCK_LOG="${TEMP_ROOT}/warlock.log"
mkdir -p "${FAKE_AFSIM_SOURCE}" "${FAKE_AFSIM_BUILD}" \
   "${FAKE_RESOURCES}/maps" "${TEMP_ROOT}/bin"
cat >"${FAKE_AFSIM_BUILD}/warlock" <<'EOF'
#!/usr/bin/env bash
printf 'plan=%s\npage=%s\nmission=%s\n' \
   "${NRM_AUTO_PLAN_FILE:-}" "${NRM_AUTO_OPEN_PAGE:-}" "${1:-}" \
   >"${NRM_TEST_WARLOCK_LOG}"
EOF
chmod +x "${FAKE_AFSIM_BUILD}/warlock"
cat >"${TEMP_ROOT}/bin/xdpyinfo" <<'EOF'
#!/usr/bin/env bash
exit 0
EOF
chmod +x "${TEMP_ROOT}/bin/xdpyinfo"

PATH="${TEMP_ROOT}/bin:${PATH}" \
AFSIM_SOURCE="${FAKE_AFSIM_SOURCE}" \
AFSIM_BUILD="${FAKE_AFSIM_BUILD}" \
AFSIM_RESOURCE_ROOT="${FAKE_RESOURCES}" \
NRM_SOURCE="${ROOT}" \
NRM_REMOTE_ROOT="${FAKE_REMOTE_ROOT}" \
NRM_SWITCH_REQUEST_FILE="${REQUEST}" \
NRM_OUTPUT_DIR="${TEMP_ROOT}/launcher-output" \
NRM_TEST_WARLOCK_LOG="${WARLOCK_LOG}" \
DISPLAY=:99 \
"${LAUNCH_SCRIPT}"

grep -qx "plan=${PLAN}" "${WARLOCK_LOG}"
grep -qx 'page=acceptance' "${WARLOCK_LOG}"
grep -qx "mission=${SCENARIO}" "${WARLOCK_LOG}"
[[ ! -e "${REQUEST}" ]]

# A failed restart must not leave a request that the next service start could
# consume accidentally.
rm -f "${REQUEST}"
set +e
NRM_SWITCH_REQUEST_FILE="${REQUEST}" \
NRM_SYSTEMCTL_COMMAND="${TEMP_ROOT}/systemctl" \
NRM_TEST_SYSTEMCTL_LOG="${SYSTEMCTL_LOG}" \
NRM_TEST_SYSTEMCTL_EXIT=23 \
"${SWITCH_SCRIPT}" "${SCENARIO}" "${PLAN}"
switch_status=$?
set -e
[[ "${switch_status}" -eq 23 ]]
[[ ! -e "${REQUEST}" ]]

# The pre-acceptance runner may execute from an isolated worktree while the
# managed Warlock service writes to the main checkout.  It must discover the
# service process output directory instead of assuming its own ROOT/output.
export NRM_PREACCEPTANCE_LIBRARY_ONLY=1
export NRM_OUTPUT_DIR="${TEMP_ROOT}/worktree-output"
export NRM_PREACCEPTANCE_OUTPUT_DIR="${TEMP_ROOT}/preacceptance-output"
# shellcheck source=../scripts/run_preacceptance.sh
source "${PREACCEPTANCE_SCRIPT}"
SERVICE_ENVIRON="${TEMP_ROOT}/service.environ"
printf 'DISPLAY=:1\0NRM_OUTPUT_DIR=%s\0' "${TEMP_ROOT}/service-output" >"${SERVICE_ENVIRON}"
[[ "$(resolve_warlock_output_root "${SERVICE_ENVIRON}")" == "${TEMP_ROOT}/service-output" ]]
[[ "$(resolve_warlock_output_root "${TEMP_ROOT}/missing.environ")" == "${NRM_OUTPUT_DIR}" ]]
