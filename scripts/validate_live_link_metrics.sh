#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/.." && pwd)
readonly SOURCE_PLATFORM=${2:-airborne_relay}
readonly DESTINATION_PLATFORM=${3:-spectrum_monitor_aircraft}

if ! command -v jq >/dev/null 2>&1
then
   printf 'FAIL: jq is required to validate the Warlock snapshot.\n' >&2
   exit 1
fi

snapshot_file=${1:-}
if [[ -z "$snapshot_file" ]]
then
   snapshot_file=$(find "${ROOT}/output" -mindepth 2 -maxdepth 2 -type f \
      -name resource_snapshots.jsonl -printf '%T@ %p\n' 2>/dev/null |
      sort -nr | sed -n '1s/^[^ ]* //p')
fi

if [[ -z "$snapshot_file" || ! -s "$snapshot_file" ]]
then
   printf 'FAIL: no non-empty Warlock resource snapshot file was found.\n' >&2
   exit 1
fi

readonly SNAPSHOT_FILE=$snapshot_file
readonly SNAPSHOT_JSON=$(tail -n 1 "$SNAPSHOT_FILE")

source_id=$(jq -er --arg platform "$SOURCE_PLATFORM" \
   'first(.endpoints[] | select(.platform == $platform)) | .id' <<<"$SNAPSHOT_JSON") || {
   printf 'FAIL: source platform was not found: %s\n' "$SOURCE_PLATFORM" >&2
   exit 1
}
destination_id=$(jq -er --arg platform "$DESTINATION_PLATFORM" \
   'first(.endpoints[] | select(.platform == $platform)) | .id' <<<"$SNAPSHOT_JSON") || {
   printf 'FAIL: destination platform was not found: %s\n' "$DESTINATION_PLATFORM" >&2
   exit 1
}

readonly SOURCE_ID=$source_id
readonly DESTINATION_ID=$destination_id

if ! jq -e \
   --arg source "$SOURCE_ID" \
   --arg destination "$DESTINATION_ID" '
      first(.links[] | select(.source == $source and .destination == $destination)) as $link
      | first($link.windows[] | select(.windowS == 10)) as $window
      | ($link.bandwidth_bps.valid == true)
        and ($link.bandwidth_bps.value > 0)
        and ($link.bandwidth_bps.source == "AFSIM_INTERNAL")
        and ($window.transmitted > 0)
        and ($window.received > 0)
        and ($window.offeredLoadBps.valid == true)
        and ($window.offeredLoadBps.value > 0)
        and ($window.deliveredThroughputBps.valid == true)
        and ($window.deliveredThroughputBps.value > 0)
        and ($window.deliveryRatioPercent.valid == true)
        and ($window.deliveryRatioPercent.value > 0)
        and ($window.deliveryRatioPercent.value <= 100)
        and ($window.averageTransportDelayMs.valid == true)
        and ($window.averageTransportDelayMs.value > 0)
        and ($window.utilizationPercent.valid == true)
        and ($window.utilizationPercent.value >= 0)
        and ($window.utilizationPercent.value <= 100)
   ' <<<"$SNAPSHOT_JSON" >/dev/null
then
   printf 'FAIL: live link metrics are missing or invalid for %s -> %s.\n' \
      "$SOURCE_PLATFORM" "$DESTINATION_PLATFORM" >&2
   printf 'Snapshot: %s\n' "$SNAPSHOT_FILE" >&2
   jq \
      --arg source "$SOURCE_ID" \
      --arg destination "$DESTINATION_ID" '
         .links[]
         | select(.source == $source and .destination == $destination)
         | {id, bandwidth_bps, window10: (.windows[] | select(.windowS == 10))}
      ' <<<"$SNAPSHOT_JSON" >&2
   exit 1
fi

jq -r \
   --arg snapshotFile "$SNAPSHOT_FILE" \
   --arg source "$SOURCE_ID" \
   --arg destination "$DESTINATION_ID" '
      .snapshot_version as $version
      | .sim_time as $time
      | first(.links[] | select(.source == $source and .destination == $destination)) as $link
      | first($link.windows[] | select(.windowS == 10)) as $window
      | "PASS: live AFSIM link metrics are valid\n"
        + "Snapshot file: " + $snapshotFile + "\n"
        + "Snapshot/time: \($version) / \($time) s\n"
        + "Link: \($link.id)\n"
        + "Bandwidth: \($link.bandwidth_bps.value) bit/s [\($link.bandwidth_bps.source)]\n"
        + "10 s TX/RX: \($window.transmitted) / \($window.received)\n"
        + "10 s offered/delivered: \($window.offeredLoadBps.value) / \($window.deliveredThroughputBps.value) bit/s\n"
        + "10 s PDR: \($window.deliveryRatioPercent.value)%\n"
        + "10 s transport delay: \($window.averageTransportDelayMs.value) ms\n"
        + "10 s utilization: \($window.utilizationPercent.value)%"
   ' <<<"$SNAPSHOT_JSON"
