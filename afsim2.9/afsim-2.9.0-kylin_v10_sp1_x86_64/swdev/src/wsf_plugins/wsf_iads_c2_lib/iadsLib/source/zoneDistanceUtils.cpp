// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: zoneDistanceUtils.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include <iadsLib/zoneDistanceUtils.hpp>
#include <vclMath/GreatCircle.hpp>
#include <vclMath/Constants.hpp>
#include <vclMath/Geometry.hpp>

namespace il
{

double getDistanceToCircularZone (const double center_lat_dd,
                                  const double center_lon_dd,
                                  const double radius_m,
                                  const double test_lat_dd,
                                  const double test_lon_dd)
{
   double distance = (vm::GreatCircle<double>::calculateDistanceInRad( center_lat_dd, center_lon_dd, test_lat_dd, test_lon_dd) * vm::ARCDEGREES_TO_METERS) - radius_m;

   return distance > 0 ? distance : 0;
}

} // namespace il
