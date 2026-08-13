#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/../.." && pwd)
readonly OLD_PID="$1"
readonly SCENARIO="$2"
readonly PLAN="$3"

for _ in $(seq 1 100)
do
   if ! kill -0 "$OLD_PID" 2>/dev/null
   then
      break
   fi
   sleep 0.1
done

if kill -0 "$OLD_PID" 2>/dev/null
then
   echo "Old Warlock process did not exit: $OLD_PID" >&2
   exit 1
fi

export NRM_AUTO_PLAN_FILE="$PLAN"
exec "$ROOT/scripts/remote/run-warlock-remote.sh" "$SCENARIO"
