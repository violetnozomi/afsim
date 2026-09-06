// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file Constants.hpp
 *  @author Ken Sewell AFRL/RYZW \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 (Sewell) \n
 *          937-656-4899 x3010 (Sawmiller)
 *  @brief Common constant values.
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_CONSTANTS_HPP
#define VCLMATH_CONSTANTS_HPP 1

namespace vm
{
    /// 2PI
    const double TWOPI= 6.28318530717959;
    /// PI
    const double PI   = 3.14159265358979323846;
    /// PI / 2
    const double PI_2 = 1.57079632679489661923;
    /// PI / 4
    const double PI_4 = 0.78539816339744830962;

    /// Radians per degree.
    const double DEGREES_TO_RADIANS = ( PI / 180.0 );
    /// Degrees per radians.
    const double RADIANS_TO_DEGREES = ( 180.0 / PI );
    /// Nautical miles per degree of latitude.
    const double ARCDEGREES_TO_NM = 60.0;
    /// Degrees of latitude per nautical mile.
    const double NM_TO_ARCDEGREES = 0.01666666666666666667;

    /// Meters per nautical mile.
    const double NM_TO_METERS = 1852.0;
    /// Nautical miles per meter.
    const double METERS_TO_NM = 0.0005399568034557235421;

    /// Meters per foot.
    const double FEET_TO_METERS = 0.3048;
    /// Feet per meter.
    const double METERS_TO_FEET = 3.2808399;

    /// Meters per degree of latitude.
    const double ARCDEGREES_TO_METERS = 111120.0;
    /// Degrees of latitude per meter.
    const double METERS_TO_ARCDEGREES = 0.000008999280057595392369;

    /// Seconds per Hour
    const double HOURS_TO_SECONDS = 3600.0;
    /// Hours per Second
    const double SECONDS_TO_HOURS = 1.0/HOURS_TO_SECONDS;
    /// Minutes per Hour
    const double HOURS_TO_MINUTES = 60.0;
    /// Hours per Minute
    const double MINUTES_TO_HOURS = 1.0/HOURS_TO_MINUTES;
    /// Seconds per Minute
    const double MINUTES_TO_SECONDS = 60.0;
    /// Minutes per Second
    const double SECONDS_TO_MINUTES = 1.0/MINUTES_TO_SECONDS;

    /******************************************************************
       From NIMA Technical Report TR8350.2 "Department of Defense World
       Geodetic System 1984, Its Definition and Relationships With Local
       Geodetic Systems"
    */

    /**
       Semi major axis of the Earth in meters.
       TR8350.2, Section 3.2.1
    */
    const double WGS84_SEMI_MAJOR = 6378137.0;
    /// Semi major axis of the Earth squared.
    const double WGS84_SEMI_MAJOR2 = WGS84_SEMI_MAJOR * WGS84_SEMI_MAJOR;

    /**
       1/flattening of the Earth.
       TR8350.2, Section 3.2.2
     */
    const double WGS84_RECIPROCAL_OF_FLATTENING = 298.257223563;

    /**
       Gravitational constant of the Earth (including atmosphere) in m^3/s^2
       TR8350.2, Section 3.2.3
     */
    const double WGS84_GRAVITATIONAL_CONSTANT = 3986004.418e8;

    /**
       Angular velocity of Earth in radians/second.
       TR8350.2, Section 3.2.4
     */
    const double WGS84_ANGULAR_VELOCITY = 7292115e-11;

    /**
       Semi minor axis of the Earth in meters.
       TR8350.2, Table 3.3
     */
    const double WGS84_SEMI_MINOR = 6356752.3142;

    /// First eccentricity.
    const double WGS84_FIRST_ECCENTRICITY = 8.1819190842622e-2;
    /// First eccentricity squared.
    const double WGS84_FIRST_ECCENTRICITY_SQUARED = 6.69437999014e-3;

    /// Second eccentricity.
    const double WGS84_SECOND_ECCENTRICITY = 8.2094437949696e-2;
    /// Second eccentricity squared.
    const double WGS84_SECOND_ECCENTRICITY_SQUARED = 6.73949674228e-3;

    /// Linear eccentricity.
    const double WGS84_LINEAR_ECCENTRICITY = 5.2185400842339e5;

    /// Polar radius of curvature.
    const double WGS84_POLAR_RADIUS_OF_CURVATURE = 6399593.6258;
    /// Axis ratio
    const double WGS84_AXIS_RATIO = 0.996647189335;

    /// Mean radius of semi axes.
    const double WGS84_MEAN_RADIUS_OF_SEMI_AXES = 6371008.7714;
    /// Radius of sphere of equal area.
    const double WGS84_RADIUS_OF_SPHERE_OF_EQUAL_AREA = 6371007.1809;
    /// Radius of sphere of equal volume.
    const double WGS84_RADIUS_OF_SPHERE_OF_EQUAL_VOLUME = 6371000.7900;

    /**
       Gravity potential of the ellipsoid.
       TR8350.2, Table 3.4
     */
    const double GRAVITY_POTENTIAL_OF_THE_ELLIPSOID = 62636851.7146;
    /// Gravity at the equator.
    const double GRAVITY_AT_THE_EQUATOR = 9.7803253359;
    /// Gravity at the pole.
    const double GRAVITY_AT_THE_POLE = 9.8321849378;
    /// Mean value of gravity.
    const double MEAN_VALUE_OF_GRAVITY = 9.7976432222;

    /**
       Mass of the Earth including atmosphere in kg.
     */
    const double MASS_OF_THE_EARTH = 5.9733328e24;

    /**
       USGS Universal Transverse Mercator Grid k0 Constant
    */
    const double USGS_UTM_K0 = 0.9996;

    /**
       End of TR8350.2 included data.
    *******************************************************************/

    /**
       @brief Semi minor axis squared.
     */
    const double WGS84_SEMI_MINOR2 = WGS84_SEMI_MINOR * WGS84_SEMI_MINOR;


    /// Speed of light in meters per second
    const double V_LIGHT = 299792458.0;

    /// RF Parameters
    const double KBOLTZMANN = 1.38E-23;  // Watts * Seconds / DegKelvin
}

#endif
