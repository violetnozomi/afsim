/**
 * @file NrmUiScale.hpp
 * @brief Shared, toolkit-independent UI scale rules for the Warlock views.
 */

#ifndef NRM_UI_SCALE_HPP
#define NRM_UI_SCALE_HPP

namespace WkNrm
{
namespace UiScale
{
constexpr int cMINIMUM_PERCENT = 50;
constexpr int cMAXIMUM_PERCENT = 500;
constexpr int cDEFAULT_PERCENT = 100;
constexpr int cSTEP_PERCENT = 25;

int ClampPercent(int aPercent);
int IncreasePercent(int aPercent);
int DecreasePercent(int aPercent);
double Factor(int aPercent);
int ScalePixels(int aPixels, int aPercent);
}
} // namespace WkNrm

#endif
