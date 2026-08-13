#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/.." && pwd)
readonly SCHEMA_ROOT="${ROOT}/schemas/customer/v1"
readonly ANNOTATED_EXAMPLES="${SCHEMA_ROOT}/nrm-customer-interface-v1.annotated.jsonc"
readonly EXAMPLE_DIR="${ROOT}/schemas/customer/v1/examples"
readonly INVALID_DIR="${ROOT}/schemas/customer/v1/invalid"
readonly CONTRACTS="navigation-report environment-report resource-report assessment-request assessment-response network-plan network-plan-result membership-request error"

for contract in $CONTRACTS
do
   schema="${SCHEMA_ROOT}/${contract}.schema.json"
   example="${EXAMPLE_DIR}/${contract}.example.json"
   [[ -f "$schema" ]] || { printf 'ERROR: missing %s\n' "$schema" >&2; exit 1; }
   [[ -f "$example" ]] || { printf 'ERROR: missing %s\n' "$example" >&2; exit 1; }
   /usr/bin/python3 -m json.tool "$schema" >/dev/null
done
/usr/bin/python3 -m json.tool "${SCHEMA_ROOT}/common.schema.json" >/dev/null

# JSONC 只供人工阅读。删除独占一行的 // 注释后逐条按正式 Schema 校验，
# 保证中文注释版不会随着接口演进而成为失真的过期样例。
/usr/bin/python3 - "$SCHEMA_ROOT" "$ANNOTATED_EXAMPLES" "$EXAMPLE_DIR" "$INVALID_DIR" <<'PY'
import json
import re
import sys
from pathlib import Path

from jsonschema import Draft202012Validator, RefResolver

schema_root = Path(sys.argv[1])
annotated_path = Path(sys.argv[2])
example_root = Path(sys.argv[3])
invalid_root = Path(sys.argv[4])
schemas = {}
for schema_path in schema_root.glob("*.schema.json"):
    schema = json.loads(schema_path.read_text(encoding="utf-8"))
    schemas[schema["$id"]] = schema

def validator_for(schema_name):
    schema_path = schema_root / f"{schema_name}.schema.json"
    schema = json.loads(schema_path.read_text(encoding="utf-8"))
    return Draft202012Validator(
        schema,
        resolver=RefResolver(schema["$id"], schema, store=schemas),
    )

jsonc_text = annotated_path.read_text(encoding="utf-8")
json_text = re.sub(r"(?m)^\s*//.*(?:\n|$)", "", jsonc_text)
bundle = json.loads(json_text)

if bundle.get("documentType") != "NRM_CUSTOMER_INTERFACE_ANNOTATED_EXAMPLES_V1":
    raise SystemExit("ERROR: annotated JSONC documentType is invalid")

messages = bundle.get("messages")
if not isinstance(messages, list) or len(messages) != 9:
    raise SystemExit("ERROR: annotated JSONC must contain exactly nine messages")

for index, message in enumerate(messages, start=1):
    schema_name = message["schema"].removeprefix("nrm.customer.").removesuffix(".v1").replace("_", "-")
    validator = validator_for(schema_name)
    errors = sorted(validator.iter_errors(message), key=lambda item: list(item.path))
    if errors:
        error = errors[0]
        field_path = "/" + "/".join(str(part) for part in error.path)
        raise SystemExit(
            f"ERROR: annotated JSONC message {index} failed at {field_path}: {error.message}"
        )

print("PASS annotated JSONC syntax and 9 embedded messages")

validated = 0
for example_path in sorted(example_root.glob("*.example.json")):
    schema_name = example_path.name.removesuffix(".example.json")
    message = json.loads(example_path.read_text(encoding="utf-8"))
    errors = list(validator_for(schema_name).iter_errors(message))
    if errors:
        raise SystemExit(f"ERROR: {example_path.name}: {errors[0].message}")
    validated += 1
    print(f"PASS {example_path.name}")

for invalid_path in sorted(invalid_root.glob("*.invalid.json")):
    schema_name = invalid_path.name.removesuffix(".invalid.json")
    message = json.loads(invalid_path.read_text(encoding="utf-8"))
    if not list(validator_for(schema_name).iter_errors(message)):
        raise SystemExit(f"ERROR: invalid fixture was accepted: {invalid_path}")
    print(f"EXPECTED_REJECT {invalid_path.name}")

print(f"PASS: schema syntax, {validated} positive examples, and expected negative examples validated.")
PY
