#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/.." && pwd)
readonly TEMP_ROOT=$(mktemp -d)
cleanup()
{
   local status=$?
   rm -rf "$TEMP_ROOT"
   exit "$status"
}
trap cleanup EXIT

cat >"${TEMP_ROOT}/python-ok" <<'SH'
#!/usr/bin/env bash
printf 'called\n' >>"${NRM_PYTHON_MARKER}"
exec /usr/bin/python3 "$@"
SH
chmod +x "${TEMP_ROOT}/python-ok"
export NRM_PYTHON_MARKER="${TEMP_ROOT}/python.marker"
PYTHON_BIN="${TEMP_ROOT}/python-ok" \
   "${ROOT}/scripts/validate_customer_interface.sh" >/dev/null
test -s "${NRM_PYTHON_MARKER}"

cat >"${TEMP_ROOT}/python-no-jsonschema" <<'SH'
#!/usr/bin/env bash
if [[ "${1:-}" == "-c" && "${2:-}" == *jsonschema* ]]; then
   exit 1
fi
exec /usr/bin/python3 "$@"
SH
chmod +x "${TEMP_ROOT}/python-no-jsonschema"
set +e
output=$(PYTHON_BIN="${TEMP_ROOT}/python-no-jsonschema" \
   "${ROOT}/scripts/validate_customer_interface.sh" 2>&1)
status=$?
set -e
test "$status" -ne 0
grep -q "jsonschema" <<<"$output"
grep -q "PYTHON_BIN" <<<"$output"
