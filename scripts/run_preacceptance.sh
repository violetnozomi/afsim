#!/usr/bin/env bash

set -uo pipefail

readonly ROOT=$(cd "$(dirname "$0")/.." && pwd)
readonly RUNTIME_OUTPUT_ROOT=${NRM_OUTPUT_DIR:-"${ROOT}/output"}
readonly OUTPUT_ROOT=${NRM_PREACCEPTANCE_OUTPUT_DIR:-"${RUNTIME_OUTPUT_ROOT}/preacceptance"}
readonly GUI_TIMEOUT_S=${NRM_PREACCEPTANCE_GUI_TIMEOUT_S:-150}
readonly RUN_STAMP=$(date -u +%Y%m%dT%H%M%SZ)
readonly RUN_DIR="${OUTPUT_ROOT}/${RUN_STAMP}-$$"
readonly LOG_DIR="${RUN_DIR}/logs"
readonly RESULT_FILE="${RUN_DIR}/results.tsv"
readonly REPORT_FILE="${RUN_DIR}/PREACCEPTANCE_REPORT.md"
readonly GUI_SUMMARY_FILE="${RUN_DIR}/gui_snapshot_summary.json"
readonly LATEST_STATUS_FILE="${OUTPUT_ROOT}/latest_status.json"
readonly FIXED_TEST_COUNT=40
readonly SCENARIOS=(
   framework_smoke
   four_network_overview
   link_failure
   congestion
   quality_degradation
   capability_service_smoke
   cross_domain_gateway_smoke
   environment_weather
   navigation_errors
   operational_strike_demo
)

overall_status=PASS

mkdir -p "$LOG_DIR"
: > "$RESULT_FILE"

record_result() {
   local name=$1
   local status=$2
   local evidence=$3
   local detail=$4
   printf '%s\t%s\t%s\t%s\n' "$name" "$status" "$evidence" "$detail" >> "$RESULT_FILE"
   if [[ "$status" != PASS ]]
   then
      overall_status=FAIL
   fi
   printf '%-32s %s - %s\n' "$name" "$status" "$detail"
}

require_commands() {
   local missing=()
   local command_name
   for command_name in cmake date find git grep head jq mv sort systemctl tail timeout
   do
      if ! command -v "$command_name" >/dev/null 2>&1
      then
         missing+=("$command_name")
      fi
   done
   if ((${#missing[@]} > 0))
   then
      record_result "preflight" FAIL "-" "missing commands: ${missing[*]}"
      return 1
   fi
   record_result "preflight" PASS "-" "required commands are available"
}

run_guard_step() {
   local guard_command=$1
   local log_file="${LOG_DIR}/${guard_command}.log"
   if "${ROOT}/scripts/ai_guard.sh" "$guard_command" >"$log_file" 2>&1
   then
      record_result "$guard_command" PASS "$log_file" "ai_guard ${guard_command} completed"
   else
      record_result "$guard_command" FAIL "$log_file" "ai_guard ${guard_command} failed"
   fi
}

run_deployment_check() {
   local log_file="${LOG_DIR}/deployment.log"
   local evidence_file="${RUN_DIR}/deployment_checks.jsonl"
   if NRM_DEPLOYMENT_OUTPUT="$evidence_file" \
      "${ROOT}/scripts/check_deployment_contract.sh" >"$log_file" 2>&1
   then
      record_result "deployment" PASS "$evidence_file" \
         "本机可检查项通过；甲方专有项保持CUSTOMER_BLOCKED"
   else
      record_result "deployment" FAIL "$log_file" "本机部署检查失败"
   fi
}

scenario_markers_valid() {
   local scenario=$1
   local log_file=$2
   grep -q 'Simulation complete' "$log_file" || return 1
   case "$scenario" in
      framework_smoke)
         return 0
         ;;
      four_network_overview)
         [[ $(grep -c '^NRM received on ' "$log_file" || true) -eq 8 ]] &&
            [[ $(grep -c '^NRM received on l11_member$' "$log_file" || true) -eq 2 ]] &&
            [[ $(grep -c '^NRM received on l16_command$' "$log_file" || true) -eq 2 ]] &&
            [[ $(grep -c '^NRM received on sat_gateway$' "$log_file" || true) -eq 2 ]] &&
            [[ $(grep -c '^NRM received on cdl_station$' "$log_file" || true) -eq 2 ]]
         ;;
      link_failure)
         grep -q '^NRM_SCENARIO LINK_FAILURE ACTIVE$' "$log_file" &&
            grep -q '^NRM_SCENARIO LINK_FAILURE RECOVERED$' "$log_file"
         ;;
      congestion)
         grep -q '^NRM_SCENARIO CONGESTION ACTIVE$' "$log_file" &&
            grep -q '^NRM_SCENARIO CONGESTION RECOVERED$' "$log_file"
         ;;
      quality_degradation)
         grep -q '^NRM_SCENARIO QUALITY_DEGRADATION ACTIVE ' "$log_file" &&
            grep -q '^NRM_SCENARIO QUALITY_DEGRADATION RECOVERED$' "$log_file"
         ;;
      capability_service_smoke)
         [[ $(grep -c '^NRM capability smoke received on capability_destination$' "$log_file" || true) -eq 2 ]]
         ;;
      cross_domain_gateway_smoke)
         [[ $(grep -c '^NRM_GATEWAY FORWARDED ' "$log_file" || true) -eq 2 ]] &&
            grep -q '^NRM_GATEWAY_DESTINATION_RECEIVED NRM_GATEWAY_TEST$' "$log_file"
         ;;
      environment_weather)
         [[ $(grep -c '^NRM received on ' "$log_file" || true) -eq 8 ]]
         ;;
      navigation_errors)
         grep -q '^NRM_NAVIGATION MODE INS$' "$log_file" &&
            grep -q '^NRM_NAVIGATION MODE PERFECT$' "$log_file" &&
            grep -q '^NRM_NAVIGATION MODE GPS1$' "$log_file" &&
            grep -q '^NRM_NAVIGATION MODE GPS2$' "$log_file" &&
            [[ -s /tmp/nrm-navigation-history/nav_aircraft.neh ]]
         ;;
      operational_strike_demo)
         grep -q '^NRM_OPERATIONAL PHASE SCENARIO_START$' "$log_file" &&
            grep -q '^NRM_OPERATIONAL PHASE LINK16_DIRECT_FAILURE$' "$log_file" &&
            grep -q '^NRM_OPERATIONAL PHASE RELAY_ROUTE_REQUESTED$' "$log_file" &&
            grep -q '^NRM_OPERATIONAL PHASE GATEWAY_CASCADE_L11_CDL_SENT$' "$log_file" &&
            grep -q '^NRM_OPERATIONAL PHASE GATEWAY_CASCADE_CDL_L11_SENT$' "$log_file" &&
            [[ $(grep -c '^NRM_GATEWAY FORWARDED ' "$log_file" || true) -eq 10 ]] &&
            grep -q '^NRM_OPERATIONAL PHASE SCENARIO_COMPLETE$' "$log_file"
         ;;
      *)
         return 1
         ;;
   esac
}

run_scenario() {
   local scenario=$1
   local log_file="${LOG_DIR}/scenario-${scenario}.log"
   if "${ROOT}/scripts/ai_guard.sh" scenario "$scenario" >"$log_file" 2>&1 &&
      scenario_markers_valid "$scenario" "$log_file"
   then
      record_result "scenario:${scenario}" PASS "$log_file" "exit code and fixed markers passed"
   else
      record_result "scenario:${scenario}" FAIL "$log_file" "scenario failed or fixed markers were missing"
   fi
}

resolve_warlock_output_root() {
   local process_environ=${1:-}
   local service_output_root=""
   if [[ -n "$process_environ" && -r "$process_environ" ]]
   then
      service_output_root=$(tr '\0' '\n' <"$process_environ" |
         sed -n 's/^NRM_OUTPUT_DIR=//p' | head -n 1)
   fi
   printf '%s\n' "${service_output_root:-${RUNTIME_OUTPUT_ROOT}}"
}

warlock_output_root() {
   local main_pid
   main_pid=$(systemctl --user show nrm-warlock.service --property MainPID --value 2>/dev/null || true)
   if [[ "$main_pid" =~ ^[1-9][0-9]*$ ]]
   then
      resolve_warlock_output_root "/proc/${main_pid}/environ"
   else
      resolve_warlock_output_root ""
   fi
}

latest_snapshot_file() {
   local snapshot_root=${1:-${RUNTIME_OUTPUT_ROOT}}
   find "${snapshot_root}" -mindepth 2 -maxdepth 2 -type f \
      -name resource_snapshots.jsonl -printf '%T@ %p\n' 2>/dev/null |
      sort -nr | head -n 1 | cut -d' ' -f2-
}

write_latest_status() {
   local requested_status=$1
   local completed_checks=0
   local passed_checks=0
   local passed_test_count=0
   local passed_scenario_count=0
   local name status evidence detail
   local branch revision
   local snapshot_valid=false
   local sim_time=0
   local network_count=0
   local endpoint_count=0
   local link_count=0
   local transmitted=0
   local received=0
   local discarded=0
   local routing_failed=0
   local temporary_file="${LATEST_STATUS_FILE}.tmp.$$"

   while IFS=$'\t' read -r name status evidence detail
   do
      [[ -z "$name" ]] && continue
      completed_checks=$((completed_checks + 1))
      if [[ "$status" == PASS ]]
      then
         passed_checks=$((passed_checks + 1))
         if [[ "$name" == test ]]
         then
            passed_test_count=$FIXED_TEST_COUNT
         elif [[ "$name" == scenario:* ]]
         then
            passed_scenario_count=$((passed_scenario_count + 1))
         fi
      fi
   done < "$RESULT_FILE"

   branch=$(git -C "$ROOT" branch --show-current 2>/dev/null || true)
   revision=$(git -C "$ROOT" describe --always --dirty 2>/dev/null || true)
   if [[ -s "$GUI_SUMMARY_FILE" ]]
   then
      snapshot_valid=true
      sim_time=$(jq -r '.sim_time // 0' "$GUI_SUMMARY_FILE")
      network_count=$(jq -r '.network_count // 0' "$GUI_SUMMARY_FILE")
      endpoint_count=$(jq -r '.endpoint_count // 0' "$GUI_SUMMARY_FILE")
      link_count=$(jq -r '.link_count // 0' "$GUI_SUMMARY_FILE")
      transmitted=$(jq -r '.messages.transmitted // 0' "$GUI_SUMMARY_FILE")
      received=$(jq -r '.messages.received // 0' "$GUI_SUMMARY_FILE")
      discarded=$(jq -r '.messages.discarded // 0' "$GUI_SUMMARY_FILE")
      routing_failed=$(jq -r '.messages.routing_failed // 0' "$GUI_SUMMARY_FILE")
   fi

   jq -n \
      --arg generated_at "$RUN_STAMP" \
      --arg overall_status "$requested_status" \
      --arg branch "$branch" \
      --arg revision "$revision" \
      --arg report_path "$REPORT_FILE" \
      --argjson completed_checks "$completed_checks" \
      --argjson passed_checks "$passed_checks" \
      --argjson fixed_test_count "$FIXED_TEST_COUNT" \
      --argjson passed_test_count "$passed_test_count" \
      --argjson fixed_scenario_count "${#SCENARIOS[@]}" \
      --argjson passed_scenario_count "$passed_scenario_count" \
      --argjson snapshot_valid "$snapshot_valid" \
      --argjson sim_time "$sim_time" \
      --argjson network_count "$network_count" \
      --argjson endpoint_count "$endpoint_count" \
      --argjson link_count "$link_count" \
      --argjson transmitted "$transmitted" \
      --argjson received "$received" \
      --argjson discarded "$discarded" \
      --argjson routing_failed "$routing_failed" \
      '{
         schemaVersion: "nrm.preacceptance_status.v1",
         generatedAtUtc: $generated_at,
         overallStatus: $overall_status,
         branch: $branch,
         revision: $revision,
         reportPath: $report_path,
         completedChecks: $completed_checks,
         passedChecks: $passed_checks,
         fixedTestCount: $fixed_test_count,
         passedTestCount: $passed_test_count,
         fixedScenarioCount: $fixed_scenario_count,
         passedScenarioCount: $passed_scenario_count,
         guiSnapshot: {
            valid: $snapshot_valid,
            simTime: $sim_time,
            networkCount: $network_count,
            endpointCount: $endpoint_count,
            linkCount: $link_count,
            transmitted: $transmitted,
            received: $received,
            discarded: $discarded,
            routingFailed: $routing_failed
         }
      }' > "$temporary_file" && mv "$temporary_file" "$LATEST_STATUS_FILE"
}

gui_snapshot_valid() {
   local snapshot_file=$1
   tail -n 1 "$snapshot_file" | jq -e '
      .sim_time == 120 and
      .network_count == 4 and
      .endpoint_count == 10 and
      .link_count == 8 and
      .messages.transmitted == 8 and
      .messages.received == 8 and
      .messages.discarded == 0 and
      .messages.routing_failed == 0 and
      ([.networks[].type] | sort == ["CDL", "LINK11", "LINK16", "SATCOM"]) and
      ([.networks[] |
         select(.transmitted == 2 and .received == 2 and
                .discarded == 0 and .routing_failed == 0)] | length == 4)
   ' >/dev/null
}

write_gui_summary() {
   local snapshot_file=$1
   tail -n 1 "$snapshot_file" | jq '{
      schema,
      runId,
      snapshot_version,
      sim_time,
      network_count,
      endpoint_count,
      link_count,
      messages,
      networks: [.networks[] | {
         name,
         type,
         transmitted,
         received,
         discarded,
         routing_failed
      }]
   }' > "$GUI_SUMMARY_FILE"
}

run_gui_validation() {
   local previous_snapshot
   local candidate
   local gui_output_root
   local elapsed=0
   gui_output_root=$(warlock_output_root)
   previous_snapshot=$(latest_snapshot_file "$gui_output_root")
   if ! systemctl --user restart nrm-warlock.service >"${LOG_DIR}/warlock-service.log" 2>&1
   then
      record_result "warlock-final-snapshot" FAIL "${LOG_DIR}/warlock-service.log" \
         "unable to restart nrm-warlock.service"
      return
   fi

   while ((elapsed < GUI_TIMEOUT_S))
   do
      sleep 2
      elapsed=$((elapsed + 2))
      gui_output_root=$(warlock_output_root)
      candidate=$(latest_snapshot_file "$gui_output_root")
      if [[ -z "$candidate" || "$candidate" == "$previous_snapshot" || ! -s "$candidate" ]]
      then
         continue
      fi
      if ! tail -n 1 "$candidate" | jq -e . >/dev/null 2>&1
      then
         continue
      fi
      if [[ $(tail -n 1 "$candidate" | jq -r '.sim_time // 0') == 120.000 ||
            $(tail -n 1 "$candidate" | jq -r '.sim_time // 0') == 120 ]]
      then
         write_gui_summary "$candidate"
         if gui_snapshot_valid "$candidate"
         then
            record_result "warlock-final-snapshot" PASS "$GUI_SUMMARY_FILE" \
               "120 s snapshot is 8 transmitted / 8 received / 0 discarded"
         else
            record_result "warlock-final-snapshot" FAIL "$GUI_SUMMARY_FILE" \
               "final snapshot did not match the frozen four-network baseline"
         fi
         return
      fi
   done

   record_result "warlock-final-snapshot" FAIL "${LOG_DIR}/warlock-service.log" \
      "no fresh 120 s snapshot within ${GUI_TIMEOUT_S} seconds"
}

write_report() {
   local branch
   local revision
   branch=$(git -C "$ROOT" branch --show-current)
   revision=$(git -C "$ROOT" describe --always --dirty)
   {
      printf '# AFSIM网络资源管理器一键预验收报告\n\n'
      printf -- '- 执行时间（UTC）：`%s`\n' "$RUN_STAMP"
      printf -- '- Git分支：`%s`\n' "$branch"
      printf -- '- Git修订：`%s`\n' "$revision"
      printf -- '- 总体结果：**%s**\n' "$overall_status"
      printf -- '- 验收等级：内部 `PRE_ACCEPTANCE`，不代表甲方最终验收\n\n'
      printf '## 检查结果\n\n'
      printf '| 检查项 | 结果 | 说明 | 证据 |\n'
      printf '| --- | --- | --- | --- |\n'
      while IFS=$'\t' read -r name status evidence detail
      do
         printf '| `%s` | **%s** | %s | `%s` |\n' "$name" "$status" "$detail" "$evidence"
      done < "$RESULT_FILE"
      printf '\n## 固定边界\n\n'
      printf -- '- 四网场景是内部参数化演示模型，不代表真实协议栈或装备性能。\n'
      printf -- '- 甲方正式接口、四网模块、导航/环境样包和目标环境仍需后续联调。\n'
      printf -- '- 本脚本只验证固定回归、批准场景和Warlock最终快照，不自动建链、改频或改路由。\n'
   } > "$REPORT_FILE"
}

main() {
   printf 'NRM pre-acceptance run: %s\n' "$RUN_DIR"
   if command -v jq >/dev/null 2>&1 && command -v git >/dev/null 2>&1
   then
      write_latest_status RUNNING
   fi
   if require_commands
   then
      run_guard_step static
      run_guard_step test
      run_deployment_check
      local scenario
      for scenario in "${SCENARIOS[@]}"
      do
         run_scenario "$scenario"
      done
      run_gui_validation
   fi
   write_report
   if ! write_latest_status "$overall_status"
   then
      printf 'Warning: unable to update %s\n' "$LATEST_STATUS_FILE" >&2
      overall_status=FAIL
      write_report
   fi
   printf '\nOverall: %s\nReport: %s\n' "$overall_status" "$REPORT_FILE"
   [[ "$overall_status" == PASS ]]
}

if [[ "${NRM_PREACCEPTANCE_LIBRARY_ONLY:-0}" != 1 ]]
then
   main "$@"
fi
