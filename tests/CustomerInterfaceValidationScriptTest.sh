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

# Locate a test interpreter through PATH (or the caller override) without
# assuming a distribution-specific absolute path.  The production script still
# owns the dependency diagnostic; this selection only lets the positive test
# exercise an interpreter that actually has jsonschema installed.
NRM_TEST_REAL_PYTHON_BIN="${PYTHON_BIN:-}"
if [[ -n "$NRM_TEST_REAL_PYTHON_BIN" ]]
then
   NRM_TEST_REAL_PYTHON_BIN=$(command -v "$NRM_TEST_REAL_PYTHON_BIN" || true)
else
   IFS=: read -r -a path_entries <<<"$PATH"
   for path_entry in "${path_entries[@]}"
   do
      candidate="${path_entry:-.}/python3"
      if [[ -x "$candidate" ]] && "$candidate" -c 'import jsonschema' >/dev/null 2>&1
      then
         NRM_TEST_REAL_PYTHON_BIN="$candidate"
         break
      fi
   done
fi
if [[ -z "$NRM_TEST_REAL_PYTHON_BIN" ]] ||
   ! "$NRM_TEST_REAL_PYTHON_BIN" -c 'import jsonschema' >/dev/null 2>&1
then
   printf 'ERROR: no PATH/PYTHON_BIN Python 3 interpreter with jsonschema is available.\n' >&2
   exit 1
fi
export NRM_TEST_REAL_PYTHON_BIN

mkdir -p "${TEMP_ROOT}/path-bin"
cat >"${TEMP_ROOT}/path-bin/python3" <<'SH'
#!/usr/bin/env bash
exec "${NRM_TEST_REAL_PYTHON_BIN}" "$@"
SH
chmod +x "${TEMP_ROOT}/path-bin/python3"
(
   unset PYTHON_BIN
   output=$(PATH="${TEMP_ROOT}/path-bin:${PATH}" \
      "${ROOT}/scripts/validate_customer_interface.sh")
   grep -q "PASS: 13 commented JSONC examples validated." <<<"$output"
)

cat >"${TEMP_ROOT}/python-ok" <<'SH'
#!/usr/bin/env bash
printf 'called\n' >>"${NRM_PYTHON_MARKER}"
exec "${NRM_TEST_REAL_PYTHON_BIN}" "$@"
SH
chmod +x "${TEMP_ROOT}/python-ok"
export NRM_PYTHON_MARKER="${TEMP_ROOT}/python.marker"
PYTHON_BIN="${TEMP_ROOT}/python-ok" \
   "${ROOT}/scripts/validate_customer_interface.sh" >/dev/null
test -s "${NRM_PYTHON_MARKER}"

mkdir -p "${TEMP_ROOT}/comment-coverage/scripts" \
   "${TEMP_ROOT}/comment-coverage/schemas/customer"
cp "${ROOT}/scripts/validate_customer_interface.sh" \
   "${TEMP_ROOT}/comment-coverage/scripts/"
cp -R "${ROOT}/schemas/customer/v1" \
   "${TEMP_ROOT}/comment-coverage/schemas/customer/"
sed -i '0,/\/\/ 固定值：任务评估请求 V1/{s/ *\/\/ 固定值：任务评估请求 V1//}' \
   "${TEMP_ROOT}/comment-coverage/schemas/customer/v1/examples-commented/assessment-request.example.jsonc"
set +e
output=$(PYTHON_BIN="$NRM_TEST_REAL_PYTHON_BIN" \
   "${TEMP_ROOT}/comment-coverage/scripts/validate_customer_interface.sh" 2>&1)
status=$?
set -e
test "$status" -ne 0
grep -q "missing inline field comment" <<<"$output"

cat >"${TEMP_ROOT}/python-no-jsonschema" <<'SH'
#!/usr/bin/env bash
if [[ "${1:-}" == "-c" && "${2:-}" == *jsonschema* ]]; then
   exit 1
fi
exec "${NRM_TEST_REAL_PYTHON_BIN}" "$@"
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
