#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/../.." && pwd)
readonly SCENARIO="${ROOT}/test_mission/navigation_errors.txt"
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

mkdir -p /tmp/nrm-navigation-history
trap restore_default_service EXIT
"${ROOT}/scripts/remote/run-warlock-remote.sh" "$SCENARIO"
