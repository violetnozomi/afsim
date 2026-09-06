// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file BasicRF.hpp
 *  @author John Gallagher Booz | Allen | Hamilton \n
 *          gallagher_john@bah.com \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3018
 *  @brief Template for radio frequency calculations.
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_BASICRF_HPP
#define VCLMATH_BASICRF_HPP 1

#include <cmath>

#include <vclMath/Constants.hpp>

namespace vm
{
    /**
       @brief Calculate the spreading loss based on distance and frequency
       (one-way)
       @param distanceInMeters Distance in meters
       @param frequencyInHz Frequency in Hz
       @param additionalLossInDB Any additional losses in DB
       @returns Spreading Loss in dB (a negative number)
       @ref Keith Sawmiller's brain
    */
    template<typename T>
    T getSpreadingLossOneWayInDB(
        const T &distanceInMeters,
        const T &frequencyInHz,
        const T &additionalLossInDB
        )
    {
        return 20.0 * std::log10( vm::V_LIGHT / frequencyInHz ) -
            20.0 * std::log10( distanceInMeters ) -
            20.0 * std::log10( 4.0 * vm::PI ) -
            additionalLossInDB;
    }

    /**
       @brief Calculate the power received in DB (one-way)
       @param powerTransmittedInDB Power from the transmitter
       @param transmitterGainInDB Gain at the transmitter
       @param receiverGainInDB Gain at the receiver
       @param distanceInMeters Distance in meters
       @param frequencyInHz Frequency in Hz
       @param additionalLossInDB Any additional losses in DB
       @returns Power received in DB(?)
       @ref EW101 Handout by David Adamy
    */
    template<typename T>
    T getPowerReceivedInDB(
        const T &powerTransmittedInDB,
        const T &transmitterGainInDB,
        const T &receiverGainInDB,
        const T &distanceInMeters,
        const T &frequencyInHz,
        const T &additionalLossInDB
        )
    {
        return
            powerTransmittedInDB +
            transmitterGainInDB +
            getSpreadingLossOneWayInDB<T>(
                distanceInMeters, frequencyInHz, additionalLossInDB
                ) +
            receiverGainInDB;
    }

    /**
       @brief Calculate the power returned based on RCS in DB (two-way)
       @param powerTransmittedInDB Power from the transmitter
       @param transmitterGainInDB Gain at the transmitter
       @param receiverGainInDB Gain at the receiver
       @param distanceInMeters Distance in meters
       @param frequencyInHz Frequency in Hz
       @param rcsInM2 Radar cross section of target in meters^2
       @param additionalLossInDB Any additional losses in DB
       @returns Power returned in DB
    */
    template<typename T>
    T getPowerReturnedInDB(
        const T &powerTransmittedInDB,
        const T &transmitterGainInDB,
        const T &receiverGainInDB,
        const T &distanceInMeters,
        const T &frequencyInHz,
        const T &rcsInM2,
        const T &additionalLossInDB
        )
    {
        return powerTransmittedInDB
            + transmitterGainInDB
            + receiverGainInDB
            + 20.0 * std::log10( vm::V_LIGHT / frequencyInHz )
            - 30.0 * std::log10( 4.0 * vm::PI )
            - 40.0 * std::log10( distanceInMeters )
            - additionalLossInDB
            + ( 10.0 * std::log10( rcsInM2 ) );
    }

    /**
       @brief Calculate the power reflected based on RCS in DB (one-way)
       @param powerTransmittedInDB Power from the transmitter
       @param transmitterGainInDB Gain at the transmitter
       @param distanceInMeters Distance in meters
       @param frequencyInHz Frequency in Hz
       @param rcsInM2 Radar cross section of target in meters^2
       @param additionalLossInDB Any additional losses in DB
       @returns Power reflected in DB
    */
    template<typename T>
    T getPowerReflectedInDB(
        const T &powerTransmittedInDB,
        const T &transmitterGainInDB,
        const T &distanceInMeters,
        const T &frequencyInHz,
        const T &rcsInM2,
        const T &additionalLossInDB
        )
    {
        return
            powerTransmittedInDB
            + transmitterGainInDB
            + getSpreadingLossOneWayInDB<T>(
                distanceInMeters, frequencyInHz, additionalLossInDB
                )
            + ( 10.0 * std::log10( rcsInM2 ) );
    }
}

#endif // VCLMATH_BASICRF_HPP
