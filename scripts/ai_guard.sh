#!/usr/bin/env bash
set -eo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
DEFAULT_AFSIM_SOURCE=/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src
AFSIM_BUILD_VALUE=$(printenv AFSIM_BUILD || true)
if [ -z "$AFSIM_BUILD_VALUE" ]; then
  AFSIM_BUILD_VALUE=$DEFAULT_AFSIM_SOURCE/build-ubuntu24
fi
NRM_JOBS_VALUE=$(printenv NRM_JOBS || true)
if [ -z "$NRM_JOBS_VALUE" ]; then
  NRM_JOBS_VALUE=4
fi
SCENARIO_TIMEOUT_VALUE=$(printenv NRM_SCENARIO_TIMEOUT || true)
if [ -z "$SCENARIO_TIMEOUT_VALUE" ]; then
  SCENARIO_TIMEOUT_VALUE=120
fi

TEST_NAMES="nrm_contract_metric_enricher_test nrm_degradation_policy_test nrm_concurrent_task_assessment_test nrm_framework_types_test nrm_snapshot_reporter_test nrm_assessment_evaluator_test nrm_network_profile_test nrm_message_lifecycle_tracker_test nrm_resource_event_ledger_test nrm_constrained_path_selector_test nrm_snapshot_reporter_recovery_test nrm_communication_capability_service_test nrm_network_plan_repository_test nrm_network_plan_evaluation_test nrm_resource_demand_repository_test nrm_resource_demand_matching_test nrm_model_service_facade_test nrm_model_registry_test nrm_environment_config_repository_test nrm_environment_effect_adapter_test nrm_afsim_navigation_packet_parser_test nrm_customer_json_codec_test"

usage() {
  printf '%s\n' \
    "Usage: scripts/ai_guard.sh <status|static|build|test|check|contract|scenario> [scenario-name]" \
    "" \
    "status    Show branch, worktree and active milestone." \
    "static    Run non-destructive repository consistency checks." \
    "build     Build the two plugins and all registered NRM tests." \
    "test      Build and run all fixed NRM tests exactly once." \
    "check     Run static checks and all fixed tests." \
    "contract  Validate the proposed customer JSON Schema and all examples." \
    "scenario  Run one approved scenario once with a timeout." \
    "" \
    "Approved scenarios: framework_smoke, four_network_overview, link_failure, congestion, quality_degradation, capability_service_smoke, operational_strike_demo, environment_weather, navigation_errors."
}

require_file() {
  if [ ! -f "$1" ]; then
    printf 'ERROR: required file missing: %s\n' "$1" >&2
    return 1
  fi
}

status_cmd() {
  cd "$ROOT"
  printf 'Repository: %s\n' "$ROOT"
  printf 'Branch: '
  git branch --show-current
  printf 'HEAD: '
  git rev-parse --short HEAD
  printf 'Latest tag: '
  git describe --tags --always --dirty
  printf 'AFSIM build: %s\n' "$AFSIM_BUILD_VALUE"
  printf '\nWorktree:\n'
  git status --short
  printf '\nCurrent milestone:\n'
  sed -n '1,45p' docs/ai/CURRENT_MILESTONE.md
}

static_cmd() {
  cd "$ROOT"
  failed=0

  for file in AGENTS.md CLAUDE.md docs/ai/PROJECT_MEMORY.md docs/ai/CURRENT_MILESTONE.md docs/ai/DECISIONS.md docs/ai/SESSION_HANDOFF.md docs/NEXT_DEVELOPMENT_INSTRUCTIONS.md VERSION include/nrm/Version.hpp; do
    require_file "$file" || failed=1
  done

  if ! git diff --check; then
    failed=1
  fi

  root_version=$(tr -d '[:space:]' < VERSION)
  header_version=$(sed -n 's/.*cVERSION = "\([^"]*\)".*/\1/p' include/nrm/Version.hpp)
  if [ "$root_version" != "$header_version" ]; then
    printf 'ERROR: VERSION=%s but Version.hpp=%s\n' "$root_version" "$header_version" >&2
    failed=1
  else
    printf 'OK: version files agree at %s\n' "$root_version"
  fi

  reject_files=$(find . -path ./.git -prune -o -type f -name '*.rej' -print)
  if [ -n "$reject_files" ]; then
    printf 'ERROR: unresolved patch reject files exist:\n%s\n' "$reject_files" >&2
    failed=1
  fi

  orig_files=$(find . -path ./.git -prune -o -type f -name '*.orig' -print)
  if [ -n "$orig_files" ]; then
    printf 'WARNING: .orig files exist; review manually and do not delete blindly:\n%s\n' "$orig_files" >&2
  fi

  if git status --short | grep -E '(^|[[:space:]])(output/|.*\.log$)' >/dev/null 2>&1; then
    printf 'ERROR: generated output or log files are present in the change set.\n' >&2
    failed=1
  fi

  if [ "$failed" -ne 0 ]; then
    return 1
  fi
  printf 'Static checks passed.\n'
}

build_cmd() {
  require_file "$AFSIM_BUILD_VALUE/CMakeCache.txt"
  cmake --build "$AFSIM_BUILD_VALUE" \
    --target wsf_network_resource_manager NetworkResourceManager nrm_tests \
    -j "$NRM_JOBS_VALUE"
}

test_cmd() {
  build_cmd
  for test_name in $TEST_NAMES; do
    test_path=$AFSIM_BUILD_VALUE/$test_name
    if [ ! -x "$test_path" ]; then
      printf 'ERROR: test executable missing: %s\n' "$test_path" >&2
      return 1
    fi
    printf 'RUN %s\n' "$test_name"
    "$test_path"
    printf 'PASS %s\n' "$test_name"
  done
}

scenario_file() {
  case "$1" in
    framework_smoke) printf '%s\n' framework_smoke.txt ;;
    four_network_overview) printf '%s\n' four_network_overview.txt ;;
    link_failure) printf '%s\n' link_failure.txt ;;
    congestion) printf '%s\n' congestion.txt ;;
    quality_degradation) printf '%s\n' quality_degradation.txt ;;
    capability_service_smoke) printf '%s\n' capability_service_smoke.txt ;;
    operational_strike_demo) printf '%s\n' operational_strike_demo/validation.txt ;;
    environment_weather) printf '%s\n' environment_weather.txt ;;
    navigation_errors) printf '%s\n' navigation_errors.txt ;;
    *)
      printf 'ERROR: scenario is not approved: %s\n' "$1" >&2
      return 1
      ;;
  esac
}

scenario_cmd() {
  if [ "$#" -ne 1 ]; then
    usage
    return 2
  fi
  file_name=$(scenario_file "$1")
  mission_path=$AFSIM_BUILD_VALUE/mission
  input_path=$ROOT/test_mission/$file_name
  if [ ! -x "$mission_path" ]; then
    printf 'ERROR: mission executable missing: %s\n' "$mission_path" >&2
    return 1
  fi
  require_file "$input_path"
  if [ "$1" = navigation_errors ]; then
    mkdir -p /tmp/nrm-navigation-history
  fi
  printf 'Running approved scenario once: %s\n' "$1"
  timeout "$SCENARIO_TIMEOUT_VALUE" "$mission_path" "$input_path"
}

if [ "$#" -eq 0 ]; then
  command_name=help
else
  command_name=$1
  shift
fi

case "$command_name" in
  status) status_cmd "$@" ;;
  static) static_cmd "$@" ;;
  build) build_cmd "$@" ;;
  test) test_cmd "$@" ;;
  check)
    static_cmd
    "$ROOT/scripts/validate_customer_interface.sh"
    test_cmd
    ;;
  contract) "$ROOT/scripts/validate_customer_interface.sh" ;;
  scenario) scenario_cmd "$@" ;;
  help|-h|--help) usage ;;
  *)
    usage
    exit 2
    ;;
esac
