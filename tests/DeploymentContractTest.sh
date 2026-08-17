#!/usr/bin/env bash

set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
CHECK_SCRIPT="${ROOT}/scripts/check_deployment_contract.sh"
TEMP_ROOT=$(mktemp -d)
trap 'rm -rf "${TEMP_ROOT}"' EXIT
EVIDENCE="${TEMP_ROOT}/deployment.jsonl"

set +e
NRM_DEPLOYMENT_OUTPUT="${EVIDENCE}" \
NRM_DEPLOYMENT_CPU_CORES=2 \
NRM_DEPLOYMENT_CPU_MHZ=900 \
NRM_DEPLOYMENT_MEMORY_KB=1048576 \
NRM_DEPLOYMENT_DISK_KB=52428800 \
NRM_DEPLOYMENT_NIC_COUNT=0 \
NRM_DEPLOYMENT_NIC_SPEED_MBPS=10 \
"${CHECK_SCRIPT}" >/dev/null
status=$?
set -e

[[ "${status}" -ne 0 ]]
[[ "$(jq -r '.overallStatus' "${EVIDENCE}")" == FAIL ]]
for check_id in cpu_cores cpu_frequency memory disk network_interface network_speed
do
   [[ "$(jq -r --arg id "${check_id}" '.checks[] | select(.checkId == $id) | .status' "${EVIDENCE}")" == FAIL ]]
done

jq -e '.checks[] | select(.checkId == "cpu_cores") |
       .requirement == "不少于4个处理器核心"' "${EVIDENCE}" >/dev/null
jq -e '.checks[] | select(.checkId == "cpu_frequency") |
       .requirement == "处理器主频不低于1.0 GHz"' "${EVIDENCE}" >/dev/null
jq -e '.checks[] | select(.checkId == "memory") |
       .requirement == "内存不低于2 GiB"' "${EVIDENCE}" >/dev/null
jq -e '.checks[] | select(.checkId == "disk") |
       .requirement == "插件所在文件系统总容量不低于100 GiB"' "${EVIDENCE}" >/dev/null
jq -e '.checks[] | select(.checkId == "network_speed") |
       .requirement == "至少一块网卡支持100 Mbit/s或更高链路速率"' "${EVIDENCE}" >/dev/null
