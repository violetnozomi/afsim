// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file FlightMath.hpp
 *  @author John Gallagher Booz | Allen | Hamilton \n
 *          gallagher_john@bah.com \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3018
 *  @brief Template for flight calculations.
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_FLIGHTMATH_HPP
#define VCLMATH_FLIGHTMATH_HPP 1

#include <vclMath/Constants.hpp>
#include <cmath>
#include <limits>

namespace vm
{
    /**
       @brief Calculate the turn radius based on the aircraft true airspeed
       and roll angle.
       @param trueAirspeed True airspeed in knots
       @param aircraftRoll Aircraft roll in radians
       @returns Turn radius in NM
       @ref Wikipedia - ROT, http://en.wikipedia.org/wiki/Standard_rate_turn
    */
    template<typename T>
    T calculateTurnRadiusInNM(
        const T &trueAirspeed,
        const T &aircraftRoll
        )
    {
        T aNumber =
            ( vm::MEAN_VALUE_OF_GRAVITY *
              vm::HOURS_TO_SECONDS * vm::HOURS_TO_SECONDS ) /
            vm::NM_TO_METERS;

        if( aircraftRoll == 0.0 )
            return std::numeric_limits<T>::max();
        else
            return( ( trueAirspeed * trueAirspeed ) /
                    ( aNumber * tan( aircraftRoll ) )
                );
    }

    /**
       @brief Calculate the start turn distance from waypoint.
       @param turnRadius The radius of the turn in meters
       @param angleBtwLegs The angle between the legs around the waypoint
       in radians
       @returns Distance to start turn in meters
       @ref SOHCAHTOA
    */
    template<typename T>
    T calculateTurnStartDistance(
        const T &turnRadius,
        const T &angleBtwLegs
        )
    {
        return( turnRadius / ( std::tan( angleBtwLegs/2.0 ) ) );
    }

    /**
       @brief Calculate the bank angle needed for rate one turn.
       @param tasInKts The true airspeed in knots
       @returns Angle of bank in radians
       @ref ROT - Wikipedia, http://en.wikipedia.org/wiki/Standard_rate_turn
    */
    template<typename T>
    T calculateBankROTInRadians(
        const T &tasInKts
        )
    {
        return( atan(tasInKts/364.0) );
    }
}

#endif // VCLMATH_FLIGHTMATH_HPP
