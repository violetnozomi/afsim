#ifndef NRM_UI_TEXT_HPP
#define NRM_UI_TEXT_HPP

#include <string>

#include "nrm/AssessmentTypes.hpp"

namespace WkNrm
{
namespace UiText
{
// UI-only translations. Stable contract/JSON codes must continue using nrm::ToString.
std::string TranslateCode(const std::string& aCode);
std::string TranslateCodeWithRaw(const std::string& aCode);
std::string TranslateListWithRaw(const std::string& aCodes);
std::string NavigationEmptyState();
std::string FormatPlanPathSource(bool aPathAvailable,
                                 bool aUsesCandidate,
                                 bool aUsesParameterizedMetrics);
std::string FormatAssessmentRouteHop(const nrm::AssessmentRouteHop& aHop);
}
} // namespace WkNrm

#endif
