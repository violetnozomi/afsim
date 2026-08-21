// Pure selection state and hit-testing helpers for the tactical resource view.

#ifndef NRM_TACTICAL_SELECTION_HPP
#define NRM_TACTICAL_SELECTION_HPP

#include <set>
#include <string>
#include <vector>

namespace WkNrm
{
enum class TacticalSelectionTarget
{
   cNONE,
   cSOURCE,
   cDESTINATION
};

enum class TacticalSelectionAssignment
{
   cNONE,
   cSOURCE,
   cDESTINATION
};

struct PlatformHitRegion
{
   std::string platformName;
   double      centerX = 0.0;
   double      centerY = 0.0;
   double      radius  = 0.0;
};

struct TacticalSelectionState
{
   TacticalSelectionTarget target = TacticalSelectionTarget::cNONE;
   std::string sourcePlatform;
   std::string destinationPlatform;
   std::string selectedPlatform;
};

std::string HitTestPlatform(const std::vector<PlatformHitRegion>& aRegions,
                            double                                aX,
                            double                                aY);

void BeginTacticalSelection(TacticalSelectionState&  aState,
                            TacticalSelectionTarget aTarget);

TacticalSelectionAssignment ApplyPlatformClick(TacticalSelectionState& aState,
                                                const std::string&      aPlatformName);

void PruneTacticalSelection(TacticalSelectionState&       aState,
                            const std::set<std::string>& aAvailablePlatforms);
} // namespace WkNrm

#endif
