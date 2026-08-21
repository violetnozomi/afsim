#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
SCENARIO_DIR="$ROOT/test_mission/operational_strike_demo"
GATEWAYS="$SCENARIO_DIR/gateways.txt"
LAUNCHER="$ROOT/scripts/remote/run-operational-strike-warlock.sh"

test -f "$GATEWAYS"
grep -q '^include gateways.txt$' "$SCENARIO_DIR/main.txt"

base_platforms=$(grep -c '^platform ' "$SCENARIO_DIR/laydown.txt")
gateway_platforms=$(grep -c '^platform gw_' "$GATEWAYS")
test "$base_platforms" -eq 25
test "$gateway_platforms" -eq 12
test $((base_platforms + gateway_platforms)) -eq 37

endpoint_count=$(grep -c '^      member ' "$SCENARIO_DIR/communications.txt")
directed_link_count=$(grep -c '^      link ' "$SCENARIO_DIR/communications.txt")
test "$endpoint_count" -eq 53
test "$directed_link_count" -eq 108

for pair in l11_l16 l11_satcom l11_cdl l16_satcom l16_cdl satcom_cdl; do
  test "$(grep -c "^platform gw_${pair}_" "$GATEWAYS")" -eq 2
done

test "$(grep -c '^      gateway_id gw_' "$GATEWAYS")" -eq 12
test "$(grep -c '^nrm_gateway_route$' "$GATEWAYS")" -ge 14
grep -q '^   route_id route_l11_satcom_cdl_cascade$' "$GATEWAYS"
grep -q '^   route_id route_cdl_l16_l11_cascade$' "$GATEWAYS"

for network in nrm_link11_coordination nrm_link16_tactical nrm_satcom_command nrm_cdl_isr; do
  grep -A80 "^network $network " "$SCENARIO_DIR/communications.txt" |
    grep -q 'member gw_'
done

grep -q 'NRM_OPERATIONAL PHASE GATEWAY_CASCADE_L11_CDL_SENT' \
  "$SCENARIO_DIR/events.txt"
grep -q 'NRM_OPERATIONAL PHASE GATEWAY_CASCADE_CDL_L11_SENT' \
  "$SCENARIO_DIR/events.txt"

for marker in \
  GATEWAY_PAIR_L11_L16_SENT \
  GATEWAY_PAIR_L11_SATCOM_SENT \
  GATEWAY_PAIR_L11_CDL_SENT \
  GATEWAY_PAIR_L16_SATCOM_SENT \
  GATEWAY_PAIR_L16_CDL_SENT \
  GATEWAY_PAIR_SATCOM_CDL_SENT; do
  grep -q "NRM_OPERATIONAL PHASE $marker" "$SCENARIO_DIR/events.txt"
done

grep -q '^readonly FIXED_TEST_COUNT=40$' "$ROOT/scripts/run_preacceptance.sh"
grep -q 'nrm_ui_scale_test' "$ROOT/scripts/ai_guard.sh"
grep -q '^   cross_domain_gateway_smoke$' "$ROOT/scripts/run_preacceptance.sh"
grep -q '^      cross_domain_gateway_smoke)$' "$ROOT/scripts/run_preacceptance.sh"

grep -q 'Warlock UI will open in the configured VNC desktop' "$LAUNCHER"
grep -q 'Keep this terminal open; press Ctrl+C to stop' "$LAUNCHER"

echo "Operational gateway scenario contract passed."
