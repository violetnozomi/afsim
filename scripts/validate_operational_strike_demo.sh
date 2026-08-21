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
   "NRM_OPERATIONAL NAVIGATION GPS1 1"
   "NRM_OPERATIONAL NAVIGATION GPS2 2"
   "NRM_OPERATIONAL NAVIGATION INS -1"
   "NRM_OPERATIONAL PHASE ISR_COLLECTED"
   "NRM_OPERATIONAL PHASE SURVEY_SUMMARY_REPORTED"
   "NRM_OPERATIONAL PHASE MISSION_PLAN_DISTRIBUTED"
   "NRM_OPERATIONAL PHASE LINK11_STATUS_REPORTED"
   "NRM_OPERATIONAL PHASE LINK16_DIRECT_FAILURE"
   "NRM_OPERATIONAL PHASE RELAY_ROUTE_REQUESTED"
   "NRM_OPERATIONAL PHASE LINK16_DIRECT_RECOVERED"
   "NRM_OPERATIONAL PHASE CDL_HIGH_RATE_ACTIVE"
   "NRM_OPERATIONAL PHASE CDL_HIGH_RATE_COMPLETE"
   "NRM_OPERATIONAL PHASE GATEWAY_PAIR_L11_L16_SENT"
   "NRM_OPERATIONAL PHASE GATEWAY_PAIR_L11_SATCOM_SENT"
   "NRM_OPERATIONAL PHASE GATEWAY_PAIR_L11_CDL_SENT"
   "NRM_OPERATIONAL PHASE GATEWAY_PAIR_L16_SATCOM_SENT"
   "NRM_OPERATIONAL PHASE GATEWAY_PAIR_L16_CDL_SENT"
   "NRM_OPERATIONAL PHASE GATEWAY_PAIR_SATCOM_CDL_SENT"
   "NRM_OPERATIONAL PHASE GATEWAY_CASCADE_L11_CDL_SENT"
   "NRM_OPERATIONAL PHASE GATEWAY_CASCADE_CDL_L11_SENT"
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
if [[ "$platform_added_count" -ne 37 ]]
then
   printf 'FAIL: expected 37 PLATFORM_ADDED events, found %s.\nEvent file: %s\n' \
      "$platform_added_count" "$EVENT_FILE" >&2
   exit 1
fi

gateway_forwarded_count=$(grep -c 'NRM_GATEWAY FORWARDED' "$CONSOLE_LOG" || true)
if [[ "$gateway_forwarded_count" -ne 10 ]]
then
   printf 'FAIL: expected 10 gateway hop forwards, found %s.\nLog: %s\n' \
      "$gateway_forwarded_count" "$CONSOLE_LOG" >&2
   exit 1
fi

for route in route_l11_satcom_cdl_cascade route_cdl_l16_l11_cascade
do
   route_forwarded_count=$(grep -c "NRM_GATEWAY FORWARDED .*route=${route} " \
      "$CONSOLE_LOG" || true)
   if [[ "$route_forwarded_count" -ne 2 ]]
   then
      printf 'FAIL: expected two gateway hops for %s, found %s.\nLog: %s\n' \
         "$route" "$route_forwarded_count" "$CONSOLE_LOG" >&2
      exit 1
   fi
done

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
printf 'PASS: all six domain pairs and both explicit two-gateway cascades were observed.\n'
printf 'PASS: %s gateway hop forwards completed.\n' "$gateway_forwarded_count"
printf 'PASS: four-network operational recipients were observed.\n'
printf 'PASS: %s cooperative platforms were recorded and no weapon event occurred.\n' \
   "$platform_added_count"
printf 'Console log: %s\nEvent file: %s\n' "$CONSOLE_LOG" "$EVENT_FILE"
