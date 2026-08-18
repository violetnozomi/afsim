#!/usr/bin/env bash

set -euo pipefail

readonly ROOT=$(cd "$(dirname "$0")/.." && pwd)
readonly SCHEMA_ROOT="${ROOT}/schemas/customer/v1"
readonly ANNOTATED_EXAMPLES="${SCHEMA_ROOT}/nrm-customer-interface-v1.annotated.jsonc"
readonly EXAMPLE_DIR="${ROOT}/schemas/customer/v1/examples"
readonly COMMENTED_EXAMPLE_DIR="${ROOT}/schemas/customer/v1/examples-commented"
readonly INVALID_DIR="${ROOT}/schemas/customer/v1/invalid"
readonly CONTRACTS="navigation-report environment-report resource-report assessment-request assessment-response resource-demand-request resource-demand-response network-plan network-plan-result membership-request provider-hello ingest-ack error"
readonly PYTHON_BIN="${PYTHON_BIN:-python3}"

if ! command -v "$PYTHON_BIN" >/dev/null 2>&1
then
   printf 'ERROR: Python 3 interpreter not found: %s. Set PYTHON_BIN to a valid executable.\n' \
      "$PYTHON_BIN" >&2
   exit 1
fi
if ! "$PYTHON_BIN" -c 'import jsonschema' >/dev/null 2>&1
then
   printf 'ERROR: Python module jsonschema is unavailable for PYTHON_BIN=%s. Install jsonschema for that interpreter.\n' \
      "$PYTHON_BIN" >&2
   exit 1
fi

for contract in $CONTRACTS
do
   schema="${SCHEMA_ROOT}/${contract}.schema.json"
   example="${EXAMPLE_DIR}/${contract}.example.json"
   [[ -f "$schema" ]] || { printf 'ERROR: missing %s\n' "$schema" >&2; exit 1; }
   [[ -f "$example" ]] || { printf 'ERROR: missing %s\n' "$example" >&2; exit 1; }
   "$PYTHON_BIN" -m json.tool "$schema" >/dev/null
done
"$PYTHON_BIN" -m json.tool "${SCHEMA_ROOT}/common.schema.json" >/dev/null

# JSONC 只供人工阅读。去除字符串外的 // 行注释后逐条按正式 Schema 校验，
# 保证中文注释版不会随着接口演进而成为失真的过期样例。
"$PYTHON_BIN" - "$SCHEMA_ROOT" "$ANNOTATED_EXAMPLES" "$EXAMPLE_DIR" \
   "$COMMENTED_EXAMPLE_DIR" "$INVALID_DIR" <<'PY'
import json
import sys
from pathlib import Path

from jsonschema import Draft202012Validator, RefResolver

schema_root = Path(sys.argv[1])
annotated_path = Path(sys.argv[2])
example_root = Path(sys.argv[3])
commented_example_root = Path(sys.argv[4])
invalid_root = Path(sys.argv[5])
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

def strip_jsonc_line_comments(text):
    """Remove // comments outside strings while preserving JSON string data."""
    output = []
    in_string = False
    escaped = False
    index = 0
    while index < len(text):
        character = text[index]
        if in_string:
            output.append(character)
            if escaped:
                escaped = False
            elif character == "\\":
                escaped = True
            elif character == '"':
                in_string = False
            index += 1
            continue

        if character == '"':
            in_string = True
            output.append(character)
            index += 1
            continue
        if character == "/" and index + 1 < len(text) and text[index + 1] == "/":
            while index < len(text) and text[index] not in "\r\n":
                index += 1
            continue
        output.append(character)
        index += 1
    return "".join(output)

def line_comment_offset(line):
    """Return the first // comment offset outside a JSON string, or -1."""
    in_string = False
    escaped = False
    for index, character in enumerate(line[:-1]):
        if in_string:
            if escaped:
                escaped = False
            elif character == "\\":
                escaped = True
            elif character == '"':
                in_string = False
            continue
        if character == '"':
            in_string = True
        elif character == "/" and line[index + 1] == "/":
            return index
    return -1

def is_property_line(line):
    """Return true when a line starts with one complete JSON property name."""
    stripped = line.lstrip()
    if not stripped.startswith('"'):
        return False
    escaped = False
    for index, character in enumerate(stripped[1:], start=1):
        if escaped:
            escaped = False
        elif character == "\\":
            escaped = True
        elif character == '"':
            return stripped[index + 1:].lstrip().startswith(":")
    return False

def validate_inline_field_comments(path, text):
    for line_number, line in enumerate(text.splitlines(), start=1):
        if not is_property_line(line):
            continue
        comment_offset = line_comment_offset(line)
        if comment_offset < 0:
            raise SystemExit(
                f"ERROR: {path.name}:{line_number}: missing inline field comment"
            )
        comment = line[comment_offset + 2:]
        if not any("\u4e00" <= character <= "\u9fff" for character in comment):
            raise SystemExit(
                f"ERROR: {path.name}:{line_number}: inline field comment must contain Chinese text"
            )

jsonc_text = annotated_path.read_text(encoding="utf-8")
json_text = strip_jsonc_line_comments(jsonc_text)
bundle = json.loads(json_text)

if bundle.get("documentType") != "NRM_CUSTOMER_INTERFACE_ANNOTATED_EXAMPLES_V1":
    raise SystemExit("ERROR: annotated JSONC documentType is invalid")

messages = bundle.get("messages")
if not isinstance(messages, list) or len(messages) != 13:
    raise SystemExit("ERROR: annotated JSONC must contain exactly thirteen messages")

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

print("PASS annotated JSONC syntax and 13 embedded messages")

commented_paths = sorted(commented_example_root.glob("*.example.jsonc"))
if len(commented_paths) != 13:
    raise SystemExit(
        f"ERROR: expected 13 commented JSONC examples, found {len(commented_paths)}"
    )

for commented_path in commented_paths:
    schema_name = commented_path.name.removesuffix(".example.jsonc")
    commented_text = commented_path.read_text(encoding="utf-8")
    validate_inline_field_comments(commented_path, commented_text)
    message = json.loads(
        strip_jsonc_line_comments(commented_text)
    )
    errors = sorted(
        validator_for(schema_name).iter_errors(message),
        key=lambda item: list(item.path),
    )
    if errors:
        error = errors[0]
        field_path = "/" + "/".join(str(part) for part in error.path)
        raise SystemExit(
            f"ERROR: {commented_path.name} failed at {field_path}: {error.message}"
        )
    print(f"PASS {commented_path.name}")

print("PASS: 13 commented JSONC examples validated.")

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
