// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfPiecewiseExponentialAtmosphere.hpp"

#include <algorithm>
#include <cmath>

#include "UtLLAPos.hpp"

namespace wsf
{
namespace space
{

// This is from table 8-4 on page 567 of Fundamentals of Astrodynamics and
// Applications, Fourth Edition, by David A. Vallado.
const std::array<PiecewiseExponentialAtmosphere::Row, 28> PiecewiseExponentialAtmosphere::mTable = {
   Row{0.0, 7.249, 1.225},        Row{25.0, 6.349, 3.899e-2},    Row{30.0, 6.682, 1.774e-2},
   Row{40.0, 7.554, 3.972e-3},    Row{50.0, 8.382, 1.057e-3},    Row{60.0, 7.714, 3.206e-4},
   Row{70.0, 6.549, 8.770e-5},    Row{80.0, 5.799, 1.905e-5},    Row{90.0, 5.382, 3.396e-6},
   Row{100.0, 5.877, 5.297e-7},   Row{110.0, 7.263, 9.661e-8},   Row{120.0, 9.473, 2.438e-8},
   Row{130.0, 12.636, 8.484e-9},  Row{140.0, 16.149, 3.845e-9},  Row{150.0, 22.523, 2.070e-9},
   Row{180.0, 29.740, 5.464e-10}, Row{200.0, 37.105, 2.789e-10}, Row{250.0, 45.546, 7.248e-11},
   Row{300.0, 53.628, 2.418e-11}, Row{350.0, 53.298, 9.518e-12}, Row{400.0, 58.515, 3.725e-12},
   Row{450.0, 60.828, 1.585e-12}, Row{500.0, 63.822, 6.967e-13}, Row{600.0, 71.835, 1.454e-13},
   Row{700.0, 88.667, 3.614e-14}, Row{800.0, 124.64, 1.170e-14}, Row{900.0, 181.05, 5.245e-15},
   Row{1000.0, 268.00, 3.019e-15}};

PiecewiseExponentialAtmosphere::PiecewiseExponentialAtmosphere()
   : Atmosphere()
{
   SetType(cTYPE);
}

double PiecewiseExponentialAtmosphere::GetDensity(const UtCalendar& /*aEpoch*/, const UtLLAPos& aLLA) const
{
   double altKm = std::max(aLLA.GetAlt() / 1000.0, 0.0);
   auto   row   = std::upper_bound(mTable.begin(),
                               mTable.end(),
                               Row{altKm, 0.0, 0.0},
                               [](const Row& aA, const Row& aB) { return aA.mBaseHeight < aB.mBaseHeight; });
   --row;
   return row->mBaseDensity * exp(-(altKm - row->mBaseHeight) / row->mScaleHeight);
}

} // namespace space
} // namespace wsf
