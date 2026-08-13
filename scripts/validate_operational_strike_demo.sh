#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/.." && pwd)
readonly OUTPUT_DIR="${ROOT}/output/operational_strike_validation"
readonly CONSOLE_LOG="${OUTPUT_DIR}/console.log"
readonly EVENT_FILE="${ROOT}/output/operational_strike_demo.evt"

mkdir -p "$OUTPUT_DIR"

"${ROOT}/scripts/ai_guard.sh" scenario operational_strike_demo >"$CONSOLE_LOG" 2>&1

required_markers=(
   "NRM_OPERATIONAL PHASE SCENARIO_START"
   "NRM_OPERATIONAL PHASE ISR_COLLECTED"
   "NRM_OPERATIONAL PHASE SURVEY_SUMMARY_REPORTED"
   "NRM_OPERATIONAL PHASE MISSION_PLAN_DISTRIBUTED"
   "NRM_OPERATIONAL PHASE LINK11_STATUS_REPORTED"
   "NRM_OPERATIONAL PHASE LINK16_DIRECT_FAILURE"
   "NRM_OPERATIONAL PHASE RELAY_ROUTE_REQUESTED"
   "NRM_OPERATIONAL PHASE LINK16_DIRECT_RECOVERED"
   "NRM_OPERATIONAL PHASE CDL_HIGH_RATE_ACTIVE"
   "NRM_OPERATIONAL PHASE CDL_HIGH_RATE_COMPLETE"
   "NRM_OPERATIONAL PHASE LINK11_RESOURCE_STATUS_REPORTED"
   "NRM_OPERATIONAL PHASE MISSION_STATUS_REPORTED"
   "NRM_OPERATIONAL PHASE SCENARIO_COMPLETE"
   "Simulation complete"
)

for marker in "${required_markers[@]}"
do
   if ! grep -Fq "$marker" "$CONSOLE_LOG"
   then
      printf 'FAIL: missing marker: %s\nLog: %s\n' "$marker" "$CONSOLE_LOG" >&2
      exit 1
   fi
done

if grep -Eq '\*\*\*\*\* (FATAL|ERROR)' "$CONSOLE_LOG"
then
   printf 'FAIL: AFSIM reported a fatal input or runtime error.\nLog: %s\n' "$CONSOLE_LOG" >&2
   exit 1
fi

if [[ ! -s "$EVENT_FILE" ]]
then
   printf 'FAIL: platform event file was not generated: %s\n' "$EVENT_FILE" >&2
   exit 1
fi

platform_added_count=$(grep -c ' PLATFORM_ADDED ' "$EVENT_FILE" || true)
if [[ "$platform_added_count" -ne 25 ]]
then
   printf 'FAIL: expected 25 PLATFORM_ADDED events, found %s.\nEvent file: %s\n' \
      "$platform_added_count" "$EVENT_FILE" >&2
   exit 1
fi

if grep -Eq ' WEAPON_(FIRED|HIT|MISSED|TERMINATED) ' "$EVENT_FILE"
then
   printf 'FAIL: a weapon event appeared in the cooperative scenario.\nEvent file: %s\n' \
      "$EVENT_FILE" >&2
   exit 1
fi

for recipient in data_processing_center regional_command network_control_center \
                 mission_aircraft_1 link11_control_station link11_service_node \
                 link11_sensor_hub
do
   if ! grep -Fq "NRM_OPERATIONAL MESSAGE_RECEIVED ${recipient}" "$CONSOLE_LOG"
   then
      printf 'FAIL: no communication delivery observed for %s.\nLog: %s\n' \
         "$recipient" "$CONSOLE_LOG" >&2
      exit 1
   fi
done

printf 'PASS: cooperative operational-network scenario completed with all phase markers.\n'
printf 'PASS: four-network operational recipients were observed.\n'
printf 'PASS: %s cooperative platforms were recorded and no weapon event occurred.\n' \
   "$platform_added_count"
printf 'Console log: %s\nEvent file: %s\n' "$CONSOLE_LOG" "$EVENT_FILE"
