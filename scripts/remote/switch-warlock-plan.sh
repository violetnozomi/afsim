#!/usr/bin/env bash

set -euo pipefail

readonly SCENARIO="$1"
readonly PLAN="$2"
readonly OPEN_PAGE="${3:-}"
readonly SYSTEMCTL_COMMAND="${NRM_SYSTEMCTL_COMMAND:-systemctl}"
readonly RUNTIME_ROOT="${XDG_RUNTIME_DIR:-/run/user/$(id -u)}/nrm-vnc"
readonly REQUEST_FILE="${NRM_SWITCH_REQUEST_FILE:-${RUNTIME_ROOT}/plan-switch.request}"

if [[ ! -f "${SCENARIO}" ]]
then
   echo "Scenario does not exist: ${SCENARIO}" >&2
   exit 1
fi
if [[ ! -f "${PLAN}" ]]
then
   echo "Plan does not exist: ${PLAN}" >&2
   exit 1
fi
if [[ -n "${OPEN_PAGE}" && "${OPEN_PAGE}" != "acceptance" ]]
then
   echo "Unsupported Warlock return page: ${OPEN_PAGE}" >&2
   exit 1
fi

mkdir -p "$(dirname "${REQUEST_FILE}")"
request_temp="${REQUEST_FILE}.tmp.$$"
request_committed=false
cleanup_request() {
   rm -f -- "${request_temp}"
   if [[ "${request_committed}" != true ]]
   then
      rm -f -- "${REQUEST_FILE}"
   fi
}
trap cleanup_request EXIT HUP INT TERM

printf '%s\n%s\n' "${SCENARIO}" "${PLAN}" >"${request_temp}"
if [[ -n "${OPEN_PAGE}" ]]
then
   printf '%s\n' "${OPEN_PAGE}" >>"${request_temp}"
fi
mv -f "${request_temp}" "${REQUEST_FILE}"

if "${SYSTEMCTL_COMMAND}" --user restart nrm-warlock.service
then
   request_committed=true
   exit 0
else
   status=$?
   exit "${status}"
fi
