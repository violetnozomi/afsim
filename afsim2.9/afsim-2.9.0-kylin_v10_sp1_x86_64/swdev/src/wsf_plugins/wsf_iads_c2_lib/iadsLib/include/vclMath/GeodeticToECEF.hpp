// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file GeodeticToECEF.hpp
 *  @author Ken Sewell AFRL/RYZW \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 (Sewell)
 *  @brief Template for Great circle calculations.
 *  Classification: UNCLASSIFIED
 */

#include <vclMath/Constants.hpp>
#include <cmath>

#ifndef VCLMATH_GEODETICTOECEF_HPP
#define VCLMATH_GEODETICTOECEF_HPP 1

namespace vm
{
    /**
       @brief Convert geodetic lat/lon/alt to ECEF
       @param lat Geodetic latitude in radians.
       @param lon Geodetic longitude in radians.
       @param alt Geodetic height in meters.
       @param x Calculated geocentric x in meters.
       @param y Calculated geocentric y in meters.
       @param z Calculated geocentric z in meters.
       @desc From NIMA Technical Report TR8350.2 "Department of Defense \n
       World Geodetic System 1984, Its Definition and Relationships \n
       With Local Geodetic Systems"
     */
    template<typename T>
    void geodeticToECEF(
	const T &lat,
	const T &lon,
	const T &alt,
	T &x,
	T &y,
	T &z
	)
    {
	T sinLat = std::sin( lat );
	T N = WGS84_SEMI_MAJOR
	    / std::sqrt( 1.0 - ( WGS84_FIRST_ECCENTRICITY_SQUARED
				 * std::pow( sinLat, 2 )));

	T cosLat = std::cos( lat );

	x = ( N + alt ) * cosLat * std::cos( lon );
	y = ( N + alt ) * cosLat * std::sin( lon );
	z = ( ( N * ( 1.0 - WGS84_FIRST_ECCENTRICITY_SQUARED ) ) + alt )
	    * sinLat;
	z = ( (WGS84_SEMI_MINOR2/WGS84_SEMI_MAJOR2) * N + alt ) * sinLat;
    }

    /**
       @brief Convert geodetic lat/lon/alt to ECEF
       @param radius Earth radius in meters.
       @param lat Geodetic latitude in radians.
       @param lon Geodetic longitude in radians.
       @param alt Geodetic height in meters.
       @param x Calculated geocentric x in meters.
       @param y Calculated geocentric y in meters.
       @param z Calculated geocentric z in meters.
     */
    template<typename T>
    void geodeticToECEFRoundEarth(
        const T &radius,
	const T &lat,
	const T &lon,
	const T &alt,
	T &x,
	T &y,
	T &z
	)
    {
	T sinLat = std::sin( lat );
	T cosLat = std::cos( lat );

	x = ( radius + alt ) * cosLat * std::cos( lon );
	y = ( radius + alt ) * cosLat * std::sin( lon );
	z = ( radius + alt ) * sinLat;
    }
}

#endif
