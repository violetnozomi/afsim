#include "NrmTacticalSelection.hpp"

#include <cassert>
#include <set>
#include <string>
#include <vector>

namespace
{
void TestHitTestReturnsNearestPlatformInsideRadius()
{
   const std::vector<WkNrm::PlatformHitRegion> regions = {
      {"far", 15.0, 10.0, 12.0},
      {"near", 11.0, 10.0, 12.0},
   };

   assert(WkNrm::HitTestPlatform(regions, 10.0, 10.0) == "near");
}

void TestHitTestRejectsPointOutsideEveryNode()
{
   const std::vector<WkNrm::PlatformHitRegion> regions = {
      {"node", 10.0, 10.0, 8.0},
   };

   assert(WkNrm::HitTestPlatform(regions, 30.0, 30.0).empty());
}

void TestSourceSelectionAdvancesToDestination()
{
   WkNrm::TacticalSelectionState state;
   WkNrm::BeginTacticalSelection(state, WkNrm::TacticalSelectionTarget::cSOURCE);

   const WkNrm::TacticalSelectionAssignment assignment =
      WkNrm::ApplyPlatformClick(state, "fighter");

   assert(assignment == WkNrm::TacticalSelectionAssignment::cSOURCE);
   assert(state.sourcePlatform == "fighter");
   assert(state.destinationPlatform.empty());
   assert(state.selectedPlatform == "fighter");
   assert(state.target == WkNrm::TacticalSelectionTarget::cDESTINATION);
}

void TestDestinationSelectionCompletesSelection()
{
   WkNrm::TacticalSelectionState state;
   state.sourcePlatform = "fighter";
   WkNrm::BeginTacticalSelection(state, WkNrm::TacticalSelectionTarget::cDESTINATION);

   const WkNrm::TacticalSelectionAssignment assignment =
      WkNrm::ApplyPlatformClick(state, "command");

   assert(assignment == WkNrm::TacticalSelectionAssignment::cDESTINATION);
   assert(state.sourcePlatform == "fighter");
   assert(state.destinationPlatform == "command");
   assert(state.selectedPlatform == "command");
   assert(state.target == WkNrm::TacticalSelectionTarget::cNONE);
}

void TestViewOnlyClickDoesNotChangeAssessmentPlatforms()
{
   WkNrm::TacticalSelectionState state;
   state.sourcePlatform = "fighter";
   state.destinationPlatform = "command";

   const WkNrm::TacticalSelectionAssignment assignment =
      WkNrm::ApplyPlatformClick(state, "relay");

   assert(assignment == WkNrm::TacticalSelectionAssignment::cNONE);
   assert(state.sourcePlatform == "fighter");
   assert(state.destinationPlatform == "command");
   assert(state.selectedPlatform == "relay");
}

void TestPruneClearsPlatformsMissingFromLatestSnapshot()
{
   WkNrm::TacticalSelectionState state;
   state.sourcePlatform = "fighter";
   state.destinationPlatform = "command";
   state.selectedPlatform = "relay";

   WkNrm::PruneTacticalSelection(state, {"fighter"});

   assert(state.sourcePlatform == "fighter");
   assert(state.destinationPlatform.empty());
   assert(state.selectedPlatform.empty());
}
} // namespace

int main()
{
   TestHitTestReturnsNearestPlatformInsideRadius();
   TestHitTestRejectsPointOutsideEveryNode();
   TestSourceSelectionAdvancesToDestination();
   TestDestinationSelectionCompletesSelection();
   TestViewOnlyClickDoesNotChangeAssessmentPlatforms();
   TestPruneClearsPlatformsMissingFromLatestSnapshot();
   return 0;
}
