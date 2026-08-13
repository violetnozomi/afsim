#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/.." && pwd)
readonly SCHEMA="${ROOT}/schemas/customer/v1/nrm-customer-interface-v1.schema.json"
readonly ANNOTATED_EXAMPLES="${ROOT}/schemas/customer/v1/nrm-customer-interface-v1.annotated.jsonc"
readonly EXAMPLE_DIR="${ROOT}/schemas/customer/v1/examples"
readonly VALIDATOR=${NRM_JSONSCHEMA_COMMAND:-/usr/bin/jsonschema}

if [[ ! -x "$VALIDATOR" ]]
then
   printf 'ERROR: JSON Schema validator not found: %s\n' "$VALIDATOR" >&2
   printf 'Set NRM_JSONSCHEMA_COMMAND to a jsonschema-compatible executable.\n' >&2
   exit 1
fi

/usr/bin/python3 -m json.tool "$SCHEMA" >/dev/null

# JSONC 只供人工阅读。删除独占一行的 // 注释后逐条按正式 Schema 校验，
# 保证中文注释版不会随着接口演进而成为失真的过期样例。
/usr/bin/python3 - "$SCHEMA" "$ANNOTATED_EXAMPLES" <<'PY'
import json
import re
import sys
from pathlib import Path

from jsonschema import Draft202012Validator

schema_path = Path(sys.argv[1])
annotated_path = Path(sys.argv[2])
schema = json.loads(schema_path.read_text(encoding="utf-8"))
jsonc_text = annotated_path.read_text(encoding="utf-8")
json_text = re.sub(r"(?m)^\s*//.*(?:\n|$)", "", jsonc_text)
bundle = json.loads(json_text)

if bundle.get("documentType") != "NRM_CUSTOMER_INTERFACE_ANNOTATED_EXAMPLES_V1":
    raise SystemExit("ERROR: annotated JSONC documentType is invalid")

messages = bundle.get("messages")
if not isinstance(messages, list) or len(messages) != 8:
    raise SystemExit("ERROR: annotated JSONC must contain exactly eight messages")

validator = Draft202012Validator(schema)
for index, message in enumerate(messages, start=1):
    errors = sorted(validator.iter_errors(message), key=lambda item: list(item.path))
    if errors:
        error = errors[0]
        field_path = "/" + "/".join(str(part) for part in error.path)
        raise SystemExit(
            f"ERROR: annotated JSONC message {index} failed at {field_path}: {error.message}"
        )

print("PASS annotated JSONC syntax and 8 embedded messages")
PY

validated=0
for example in "$EXAMPLE_DIR"/*.json
do
   "$VALIDATOR" -V Draft202012Validator -i "$example" "$SCHEMA"
   validated=$((validated + 1))
   printf 'PASS %s\n' "$(basename "$example")"
done

if printf '%s\n' '{"schemaVersion":"nrm.customer.resource_report.v1"}' |
   "$VALIDATOR" -V Draft202012Validator "$SCHEMA" >/dev/null 2>&1
then
   printf 'ERROR: intentionally incomplete resource report was accepted.\n' >&2
   exit 1
fi

printf 'PASS: schema syntax, %s positive examples, and one negative example validated.\n' \
   "$validated"
