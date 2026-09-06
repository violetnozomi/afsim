#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/../.." && pwd)
readonly SCENARIO="${ROOT}/test_mission/operational_strike_demo/interactive.txt"
service_was_active=false

restore_default_service() {
   if [[ "$service_was_active" == true ]]
   then
      systemctl --user start nrm-warlock.service
   fi
}

if systemctl --user is-active --quiet nrm-warlock.service
then
   service_was_active=true
   systemctl --user stop nrm-warlock.service
fi

trap restore_default_service EXIT
echo "Launching operational strike scenario: ${SCENARIO}"
echo "Warlock UI will open in the configured VNC desktop, not in this terminal."
echo "Keep this terminal open; press Ctrl+C to stop and restore the default service."
"${ROOT}/scripts/remote/run-warlock-remote.sh" "$SCENARIO"
