/**
 * @file NrmUiScale.cpp
 * @brief Implements bounded UI scaling shared by Qt widgets and painted views.
 */

#include "NrmUiScale.hpp"

#include <algorithm>
#include <cmath>

int WkNrm::UiScale::ClampPercent(int aPercent)
{
   return std::max(cMINIMUM_PERCENT, std::min(cMAXIMUM_PERCENT, aPercent));
}

int WkNrm::UiScale::IncreasePercent(int aPercent)
{
   return ClampPercent(aPercent + cSTEP_PERCENT);
}

int WkNrm::UiScale::DecreasePercent(int aPercent)
{
   return ClampPercent(aPercent - cSTEP_PERCENT);
}

double WkNrm::UiScale::Factor(int aPercent)
{
   return static_cast<double>(ClampPercent(aPercent)) / 100.0;
}

int WkNrm::UiScale::ScalePixels(int aPixels, int aPercent)
{
   return static_cast<int>(std::lround(static_cast<double>(aPixels) * Factor(aPercent)));
}
