// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file ECEFToGeodetic.hpp
 *  @author Ken Sewell AFRL/RYZW \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 (Sewell)
 *  @brief Template for Great circle calculations.
 *  Classification: UNCLASSIFIED
 */

#include <vclMath/Constants.hpp>
#include <cmath>

#include <iostream>

#ifndef VCLMATH_ECEFTOGEODETIC_HPP
#define VCLMATH_ECEFTOGEODETIC_HPP 1

namespace vm
{
    /**
       @brief Convert ECEF to geodetic lat/lon/alt.
       @param x Geocentric x in meters.
       @param y Geocentric y in meters.
       @param z Geocentric z in meters.
       @param lat Calculated geodetic latitude in radians.
       @param lon Calculated geodetic longitude in radians.
       @param alt Calculated geodetic height in meters.
     */
    template<typename T>
    void ecefToGeodetic(
	const T &x,
	const T &y,
	const T &z,
	T &lat,
	T &lon,
	T &alt
	)
    {
	T one_minus_e2 = 1.0 - WGS84_FIRST_ECCENTRICITY_SQUARED;
	T z2 = std::pow( z, 2 );

	T r2 = std::pow( x, 2 ) + std::pow( y, 2 );
	T r = std::sqrt( r2 );

	T E2 = WGS84_SEMI_MAJOR2 - WGS84_SEMI_MINOR2;
	T F = 54.0 * WGS84_SEMI_MINOR2 * z2;
	T G = r2 + one_minus_e2 * z2
	    - WGS84_FIRST_ECCENTRICITY_SQUARED * E2;
	T e4 = std::pow( WGS84_FIRST_ECCENTRICITY_SQUARED, 2 );
	T c = ( e4 * F * r2) / std::pow( G, 3 );
	T s = std::pow( ( 1.0 + c
			  + std::sqrt( std::pow( c, 2 ) + 2.0 * c ) ),
			(1.0/3.0)
	    );
	T P = F
	    / ( 3.0 * std::pow( ( s + 1.0/s + 1.0 ), 2 ) * std::pow( G, 2 ) );
	T Q = std::sqrt( 1.0 + ( 2.0 * e4 * P ) );
	T ro = -( WGS84_FIRST_ECCENTRICITY_SQUARED * P * r )/( 1.0 + Q )
	    + sqrt( (WGS84_SEMI_MAJOR2 / 2.0) * (1.0 + 1.0/Q)
		    - ( one_minus_e2 * P * z2)/(Q * (1.0+Q ) )
		    - (P * r2/2.0 )
		);
	T tmp = std::pow( ( r - WGS84_FIRST_ECCENTRICITY_SQUARED * ro ), 2 );
	T U = std::sqrt( tmp + z2 );
	T V = std::sqrt( tmp + one_minus_e2 * z2 );
	T aV = WGS84_SEMI_MAJOR * V;
	T zo = ( WGS84_SEMI_MINOR2 * z ) / aV;

	alt = U * ( 1.0 - WGS84_SEMI_MINOR2 / aV );
	lat = std::atan( (z + WGS84_SECOND_ECCENTRICITY_SQUARED * zo) / r );
	lon = std::atan2( y, x );
    }

    /**
       @brief Convert ECEF Round Earth to geodetic lat/lon/alt.
       @param radius Earth radius in meters.
       @param x Geocentric x in meters.
       @param y Geocentric y in meters.
       @param z Geocentric z in meters.
       @param lat Calculated geodetic latitude in radians.
       @param lon Calculated geodetic longitude in radians.
       @param alt Calculated geodetic height in meters.
     */
    template<typename T>
    void ecefToGeodeticRoundEarth(
        const T &radius,
	const T &x,
	const T &y,
	const T &z,
	T &lat,
	T &lon,
	T &alt
	)
    {
        T xy = std::sqrt( std::pow( x, 2 ) + std::pow( y, 2 ) );

        lat = std::atan( z / xy );

        lon = std::atan( y / x );

        alt = std::sqrt(
            std::pow( x, 2 ) +
            std::pow( y, 2 ) +
            std::pow( z, 2 )
            ) - radius;
    }



    /**
       @brief Convert ECEF orientation (DIS entity state orientation) to geodetic heading pitch and roll.  All values are in radians.
       @param lat The latitude position of the entity.
       @param lon The longitude position of the entity.
       @param psi Rotation in radians about z-axis.
       @param theta Rotation in radians about y-axis.
       @param phi Rotation in radians about x-axis.
       @param heading North up heading in radians.
       @param pitch pitch in radians.
       @param roll roll in radians.
     */
    template<typename T>
    void ecefToGeodeticHpr(
        const T &lat,
        const T &lon,
        const T &psi,
        const T &theta,
        const T &phi,
        T &outHeading,
        T &outPitch,
        T &outRoll
    )
    {
        T sin_lat = sin(lat);
        T cos_lat = cos(lat);
        T cos_lon = cos(lon);
        T sin_lon = sin(lon);
        T ll_sin_sin = sin_lat*sin_lon;
        T ll_sin_cos = sin_lat*cos_lon;
        T ll_cos_sin = cos_lat*sin_lon;
        T ll_cos_cos = cos_lat*cos_lon;
        /***************************************/

        // cos/sin of incoming roll
        T cosPhi = cos( phi );
        T sinPhi = sin( phi );

        // cos/sin of incoming pitch
        T cosTheta = cos( theta );
        T sinTheta = sin( theta );

        // cos/sin of incoming yaw
        T cosPsi = cos( psi );
        T sinPsi = sin( psi );

        // pitch
        outPitch = asin(ll_cos_cos * cosTheta * cosPsi   +
            ll_cos_sin * cosTheta * sinPsi   -
            sin_lat * sinTheta);

        // heading - yaw
        T poly1 = cosTheta * cosPsi;
        T poly2 = cosTheta * sinPsi;

        outHeading = atan2( -sin_lon*poly1 + cos_lon * poly2, -ll_sin_cos*poly1 - ll_sin_sin*poly2 - cos(lat)*sinTheta );

        // the simulator's roll
        outRoll = atan2( -ll_cos_cos * (-cosPhi*sinPsi + sinPhi*sinTheta*cosPsi) +
        ll_cos_sin * ( cosPhi*cosPsi + sinPhi*sinTheta*sinPsi) + sin_lat * (sinPhi*cosTheta),
        ll_cos_cos * ( sinPhi*sinPsi + cosPhi*sinTheta*cosPsi) + ll_cos_sin *
        (-sinPhi*cosPsi + cosPhi*sinTheta*sinPsi) + sin_lat * (cosPhi*cosTheta) );
    }

    /**
       @brief Convert ECEF orientation (DIS entity state orientation) to geodetic heading pitch and roll.  All values are in radians.
       @param lat The latitude position of the entity.
       @param lon The longitude position of the entity.
       @param psi Rotation in radians about z-axis.
       @param theta Rotation in radians about y-axis.
       @return North up heading in radians.
     */
    template<typename T>
    T ecefToGeodeticHeading(
        const T &lat,
        const T &lon,
        const T &psi,
        const T &theta
    )
    {
        T sin_lat = sin(lat);
        T cos_lon = cos(lon);
        T sin_lon = sin(lon);
        T ll_sin_sin = sin_lat*sin(lon);
        T ll_sin_cos = sin_lat*cos(lon);

        // cos/sin of incoming pitch
        T cosTheta = cos( theta );

        // heading - yaw
        T poly1 = cosTheta * cos(psi);
        T poly2 = cosTheta * sin(psi);

        return atan2( -sin_lon*poly1 + cos_lon * poly2, -ll_sin_cos*poly1 - ll_sin_sin*poly2 - cos(lat)*sin(theta) );
    }

}
#endif
