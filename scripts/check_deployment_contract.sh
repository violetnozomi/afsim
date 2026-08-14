#!/usr/bin/env bash

set -uo pipefail

readonly ROOT=$(cd "$(dirname "$0")/.." && pwd)
readonly AFSIM_SOURCE_DEFAULT=/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src
readonly AFSIM_SOURCE_VALUE=${AFSIM_SOURCE:-$AFSIM_SOURCE_DEFAULT}
readonly AFSIM_BUILD_VALUE=${AFSIM_BUILD:-${AFSIM_SOURCE_VALUE}/build-ubuntu24}
readonly OUTPUT_FILE=${NRM_DEPLOYMENT_OUTPUT:-${ROOT}/output/deployment_checks.jsonl}
readonly TEMP_DIR=$(mktemp -d /tmp/nrm-deployment-check.XXXXXX)
readonly CHECKS_FILE=${TEMP_DIR}/checks.jsonl
trap 'rm -rf "$TEMP_DIR"' EXIT

overall=PASS

record_check() {
  local check_id=$1
  local status=$2
  local observed=$3
  local requirement=$4
  if [[ "$status" == FAIL ]]; then
    overall=FAIL
  fi
  jq -cn \
    --arg checkId "$check_id" \
    --arg status "$status" \
    --arg observed "$observed" \
    --arg requirement "$requirement" \
    '{checkId:$checkId,status:$status,observed:$observed,requirement:$requirement}' \
    >> "$CHECKS_FILE"
}

if ! command -v jq >/dev/null 2>&1; then
  printf 'ERROR: jq is required to write deployment evidence.\n' >&2
  exit 1
fi

kernel=$(uname -s 2>/dev/null || true)
arch=$(uname -m 2>/dev/null || true)
cpu_cores=$(getconf _NPROCESSORS_ONLN 2>/dev/null || true)
cpu_mhz=$(awk -F: '/cpu MHz/{gsub(/^[ \t]+/, "", $2); print $2; exit}' /proc/cpuinfo 2>/dev/null || true)
memory_kb=$(awk '/MemTotal/{print $2; exit}' /proc/meminfo 2>/dev/null || true)
disk_kb=$(df -Pk "$ROOT" 2>/dev/null | awk 'NR==2{print $4}' || true)
nic_count=$(ip -o link show 2>/dev/null | awk -F': ' '$2 != "lo" {count++} END{print count+0}' || true)
compiler=$(c++ --version 2>/dev/null | head -n 1 || true)

[[ "$kernel" == Linux ]] && record_check os PASS "$kernel" "Linux运行环境" || record_check os FAIL "${kernel:-unavailable}" "Linux运行环境"
[[ -n "$arch" ]] && record_check cpu_arch PASS "$arch" "CPU架构可识别" || record_check cpu_arch FAIL unavailable "CPU架构可识别"
[[ "$cpu_cores" =~ ^[0-9]+$ && "$cpu_cores" -gt 0 ]] && record_check cpu_cores PASS "$cpu_cores" "处理器核心数可识别" || record_check cpu_cores FAIL "${cpu_cores:-unavailable}" "处理器核心数可识别"
[[ -n "$cpu_mhz" ]] && record_check cpu_frequency PASS "${cpu_mhz} MHz" "处理器频率可识别" || record_check cpu_frequency FAIL unavailable "处理器频率可识别"
[[ "$memory_kb" =~ ^[0-9]+$ && "$memory_kb" -gt 0 ]] && record_check memory PASS "${memory_kb} kB" "内存容量可识别" || record_check memory FAIL "${memory_kb:-unavailable}" "内存容量可识别"
[[ "$disk_kb" =~ ^[0-9]+$ && "$disk_kb" -gt 0 ]] && record_check disk PASS "${disk_kb} kB available" "插件目录具有可用磁盘空间" || record_check disk FAIL "${disk_kb:-unavailable}" "插件目录具有可用磁盘空间"
[[ "$nic_count" =~ ^[0-9]+$ && "$nic_count" -gt 0 ]] && record_check network_interface PASS "$nic_count non-loopback interface(s)" "至少一块非回环网卡" || record_check network_interface FAIL "${nic_count:-0}" "至少一块非回环网卡"
[[ -n "$compiler" ]] && record_check compiler PASS "$compiler" "C++编译器可用" || record_check compiler FAIL unavailable "C++编译器可用"
[[ -x "${AFSIM_BUILD_VALUE}/mission" ]] && record_check afsim_runtime PASS "${AFSIM_BUILD_VALUE}/mission" "AFSIM mission可执行" || record_check afsim_runtime FAIL "${AFSIM_BUILD_VALUE}/mission" "AFSIM mission可执行"
[[ -f "${ROOT}/wsf_module" && -f "${ROOT}/warlock/warlock_plugin.cmake" ]] && record_check plugin_source PASS "$ROOT" "插件源码挂载点完整" || record_check plugin_source FAIL "$ROOT" "插件源码挂载点完整"
plugin_library=$(find "$AFSIM_BUILD_VALUE" -type f -name 'libNetworkResourceManager_*.so' -print -quit 2>/dev/null || true)
[[ -n "$plugin_library" ]] && record_check plugin_binary PASS "$plugin_library" "Warlock插件已构建" || record_check plugin_binary FAIL unavailable "Warlock插件已构建"

record_check customer_oa CUSTOMER_BLOCKED "未提供甲方OA规范" "办公自动化环境兼容"
record_check classified_security CUSTOMER_BLOCKED "未提供保密和审计规范" "保密系统部署要求"
record_check transport_security CUSTOMER_BLOCKED "未提供TLS/鉴权规范" "跨进程接口安全"
record_check formal_model_package CUSTOMER_BLOCKED "未提供甲方正式模型封装规范" "第三方模型工具兼容"

mkdir -p "$(dirname "$OUTPUT_FILE")"
checks=$(jq -s . "$CHECKS_FILE")
temporary_file="${OUTPUT_FILE}.tmp.$$"
jq -cn \
  --arg schemaVersion "nrm.deployment_check.v1" \
  --arg generatedAt "$(date -u +%Y-%m-%dT%H:%M:%SZ)" \
  --arg overallStatus "$overall" \
  --arg afsimSource "$AFSIM_SOURCE_VALUE" \
  --arg pluginSource "$ROOT" \
  --argjson checks "$checks" \
  '{schemaVersion:$schemaVersion,generatedAtUtc:$generatedAt,overallStatus:$overallStatus,afsimSource:$afsimSource,pluginSource:$pluginSource,checks:$checks}' \
  > "$temporary_file" && mv "$temporary_file" "$OUTPUT_FILE"

printf 'Deployment check: %s\nEvidence: %s\n' "$overall" "$OUTPUT_FILE"
[[ "$overall" == PASS ]]
