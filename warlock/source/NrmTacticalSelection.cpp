// Pure selection state and hit-testing helpers for the tactical resource view.

#include "NrmTacticalSelection.hpp"

#include <limits>

std::string WkNrm::HitTestPlatform(const std::vector<PlatformHitRegion>& aRegions,
                                   double                                aX,
                                   double                                aY)
{
   std::string nearestPlatform;
   double nearestDistanceSquared = std::numeric_limits<double>::max();
   for (const PlatformHitRegion& region : aRegions)
   {
      const double deltaX = aX - region.centerX;
      const double deltaY = aY - region.centerY;
      const double distanceSquared = deltaX * deltaX + deltaY * deltaY;
      if (distanceSquared <= region.radius * region.radius &&
          distanceSquared < nearestDistanceSquared)
      {
         nearestDistanceSquared = distanceSquared;
         nearestPlatform = region.platformName;
      }
   }
   return nearestPlatform;
}

void WkNrm::BeginTacticalSelection(TacticalSelectionState&  aState,
                                   TacticalSelectionTarget aTarget)
{
   aState.target = aTarget;
}

WkNrm::TacticalSelectionAssignment
WkNrm::ApplyPlatformClick(TacticalSelectionState& aState,
                          const std::string&      aPlatformName)
{
   if (aPlatformName.empty())
   {
      aState.selectedPlatform.clear();
      return TacticalSelectionAssignment::cNONE;
   }

   aState.selectedPlatform = aPlatformName;
   if (aState.target == TacticalSelectionTarget::cSOURCE)
   {
      aState.sourcePlatform = aPlatformName;
      aState.target = TacticalSelectionTarget::cDESTINATION;
      return TacticalSelectionAssignment::cSOURCE;
   }
   if (aState.target == TacticalSelectionTarget::cDESTINATION)
   {
      aState.destinationPlatform = aPlatformName;
      aState.target = TacticalSelectionTarget::cNONE;
      return TacticalSelectionAssignment::cDESTINATION;
   }
   return TacticalSelectionAssignment::cNONE;
}

void WkNrm::PruneTacticalSelection(TacticalSelectionState&       aState,
                                   const std::set<std::string>& aAvailablePlatforms)
{
   auto prune = [&aAvailablePlatforms](std::string& aPlatform)
   {
      if (!aPlatform.empty() && aAvailablePlatforms.count(aPlatform) == 0)
      {
         aPlatform.clear();
      }
   };
   prune(aState.sourcePlatform);
   prune(aState.destinationPlatform);
   prune(aState.selectedPlatform);
}
