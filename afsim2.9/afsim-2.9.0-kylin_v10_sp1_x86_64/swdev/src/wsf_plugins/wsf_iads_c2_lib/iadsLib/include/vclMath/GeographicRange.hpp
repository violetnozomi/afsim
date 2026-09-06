// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file GeographicRange.hpp
 *  @author Ken Sewell AFRL/RYZW \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 (Sewell)
 *  @brief Template for Great circle calculations.
 *  Classification: UNCLASSIFIED
 */

#include <vclMath/Constants.hpp>
#include <cmath>

#ifndef VCLMATH_GEOGRAPHICRANGE_HPP
#define VCLMATH_GEOGRAPHICRANGE_HPP 1

namespace vm
{
    /**
       @brief Calculate maximum distance at which the curvature of the
       earth permits a point to be seen from a particular height of the eye.
       @param alt1 Altitude in meters (msl)
       @param alt2 Altitude in meters (msl)
       @returns Maximum distance in nautical miles.
       The American Pratical Navigator, 2002 ed.
       pg 559, Table 13. Geographic Range
     */
    template<typename T>
    T calculateGeographicRange(
	const T &alt1,
	const T &alt2
	)
    {
	return( 1.17 * ( std::sqrt( alt1 * vm::METERS_TO_FEET )
			 + std::sqrt( alt2 * vm::METERS_TO_FEET ) )
	    );
    }
}

#endif
