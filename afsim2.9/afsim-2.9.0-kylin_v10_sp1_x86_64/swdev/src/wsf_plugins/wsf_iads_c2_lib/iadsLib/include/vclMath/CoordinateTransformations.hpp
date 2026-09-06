// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file CoordinateTransformations.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *  @brief General Coordinate System Transformations
 *  Classification: UNCLASSIFIED
 *  Intent:         Designed to provide a COMPLETELY UNAMBIGUOUS set of standard transformations
 *  The following naming convention applies:
 *    Geocentric : An attribute of any coordinate system whose origin lies at the center of the Earth.
 *    Geodetic   : An attribute of any coordinate system which takes the Earth's size AND shape into consideration (i.e. requires a datum)
 *    Datum      : A survey or representation of the ellipsoid representing the Earth's size and shape. THIS IS NOT A COORDINATE SYSTEM
 *    Inertial   : An attribute any coordinate system whose origin does not accelerate AND whose axes do not rotate
 *    Topocentric: An attribute of any coorinate system whose origin is on the surface of an ellipsoid
 *    Projection : An attribute of any coordinate system which is composed of a geometric projection of one surface onto another
 *
 *    The following terms are to be avoided in transformation names an absolute sense because they are ambiguous and cause confusion
 *      1: Any of the above terms without qualification. Ex: geocentric_to_geodetic is ambiguous since many coordinate systems can be either.
 *         A proper example is: EarthCenteredInertial_J2000_to_GeodeticLatLonAlt_WGS84. Note that in this case, ECI w/ the J2000 Datum is a
 *         geocentric coordinate system, and LLA based on the WGS-84 Datum is geodetic, fitting the original ambiguous naming scheme.
 *      2: No information shall be assumed from the name. flat_earth does not necessarily mean East, North, Up, etc.
 *      3: Latitude, Longitude, Altitude without qualification. For example, does latitude = geocentric or geodetic latitude? If geodetic, which
 *         datum is used? WGS-84, or GDA-94 (Australia)?. ECEF_to_LLA would cause this confusion to those familiar with other systems such as these.
 *
 */

#ifndef VCLMATH_COORDINATETRANSFORMATIONS_HPP
#define VCLMATH_COORDINATETRANSFORMATIONS_HPP 1

#include <cmath>
#include <vclMath/Constants.hpp>
#include <vclMath/Matrix.hpp>
#include <vclMath/Vector3.hpp>

namespace vm
{
    extern "C" // Needed to eliminate C++ name mangling so symbols can be linked to from other languages (FORTRAN, etc)
    {
        /**
         * @brief Convert Geodetic Latitude/Longitude/Altitude (WGS84) to Geocentric Earth-Centered, Earth-Fixed (ECEF) Reference System
         * @param lat_rad       WGS-84 latitude, radians
         * @param lon_rad       WGS-84 longitude, radians
         * @param alt_m         WGS-84 altitude above ellipsoid, meters
         * @param ecef_x_out_m  ECEF X Position (Prime Meridian)
         * @param ecef_y_out_m  ECEF Y Position (Z cross X)
         * @param ecef_z_out_m  ECEF Z Position (Earth Spin Axis)
         */
        void convertGeodeticWGS84LatLonAltToECEF (
            const double & lat_rad, const double & lon_rad, const double & alt_m,
            double & ecef_x_out_m,  double & ecef_y_out_m , double & ecef_z_out_m
        );


        /**
         * @brief Convert Geocentric Earth-Centered, Earth-Fixed (ECEF) Reference System to Geodetic Latitude/Longitude/Altitude (WGS84)
         * @param x_m               ECEF X Position (Prime Meridian)
         * @param y_m               ECEF Y Position (Z cross X)
         * @param z_m               ECEF Z Position (Earth Spin Axis)
         * @param wgs84_lat_out_rad WGS-84 latitude, radians
         * @param wgs84_lon_out_rad WGS-84 longitude, radians
         * @param wgs84_alt_out_m   WGS-84 altitude above ellipsoid, meters
         * Reference: Kaplan, Understanding GPS: Principles and Applications, 1st Edition
         *            Uses a non-iterative algorithm to convert from ECEF to Geodetic Datum based Lat,Lon,Alt
         */
        void convertECEF_ToGeodeticWGS84LatLonAlt (
            const double & x_m, const double & y_m, const double & z_m,
            double & wgs84_lat_out_rad,  double & wgs84_lon_out_rad , double & wgs84_alt_out_m
        );

        /**
         * @brief Convert Geodetic Latitude/Longitude/Altitude (Any Datum) to Transverse Mercator Projection East, North, Up
         * @param lat_rad           geodetic system latitude, radians
         * @param lon_rad           geodetic system longitude, radians
         * @param alt_m             geodetic system altitude, meters
         * @param lat_ref_rad       geodetic system reference latitude (center of scenario), radians
         * @param lon_ref_rad       geodetic system reference longitude (center of scenario), radians
         * @param datumEarthRadius  Earth radius used during conversion from geodetic system to mercator
         * @param east_m            Transverse Mercator Projection X Axis, East, meters
         * @param north_m           Transverse Mercator Projection Y Axis, North, meters
         * @param up_m              Transverse Mercator Projection Z Axis, Up, meters
         * Reference:               Suppressor 7.0 Transverse Mercator Transformation
         */
        void convertLatLonAltToTransverseMercatorProjectionENU (
            const double & lat_rad,     const double & lon_rad,     const double & alt_m,               // Input
            const double & lat_ref_rad, const double & lon_ref_rad, const double & datumEarthRadius,    // Reference geodetic data
            double & east_m,            double & north_m,           double & up_m );                    // ENU Mercator Output

        /**
         * @brief Convert Transverse Mercator Projection East, North, Up to Geodetic Latitude/Longitude/Altitude (Any Datum)
         * @param east_m            Transverse Mercator Projection X Axis, East, meters
         * @param north_m           Transverse Mercator Projection Y Axis, North, meters
         * @param up_m              Transverse Mercator Projection Z Axis, Up, meters
         * @param lat_rad           geodetic system latitude, radians
         * @param lon_rad           geodetic system longitude, radians
         * @param alt_m             geodetic system altitude, meters
         * @param lat_ref_rad       geodetic system reference latitude (center of scenario), radians
         * @param lon_ref_rad       geodetic system reference longitude (center of scenario), radians
         * @param datumEarthRadius  Earth radius used during conversion from geodetic system to mercator
         * Reference:               Suppressor 7.0 Transverse Mercator Transformation
         */
        void convertTransverseMercatorProjectionENUToLatLonAlt (
            const double & east_m,      const double & north_m,     const double & up_m,                // Input TM ENU
            const double & lat_ref_rad, const double & lon_ref_rad, const double & datumEarthRadius,    // Reference geodetic data
            double & lat_rad,           double & lon_rad,           double & alt_m );                   // Geodetic output

        /**
         * @brief Convert Cartesian East, North, Up (ENU) to Cartesian North, East, Down (NED)
         * @param e_in   Input X Axis, East
         * @param n_in   Input Y Axis, North
         * @param u_in   Input Z Axis, Up
         * @param n_out  Output X Axis, North
         * @param e_out  Output Y Axis, East
         * @param d_out  Output Z Axis, Down
         */
        void convertENU_ToNED ( const double & e_in, const double & n_in, const double & u_in,
                                double & n_out, double & e_out, double & d_out );

        /**
         * @brief Convert Cartesian East, North, Up (ENU) to Cartesian North, East, Down (NED)
         * @param n_in  Input X Axis, North
         * @param e_in  Input Y Axis, East
         * @param d_in  Input Z Axis, Down
         * @param e_out Output X Axis, East
         * @param n_out Output Y Axis, North
         * @param u_out Output Z Axis, Up
         */
        void convertNED_ToENU ( const double & n_in, const double & e_in, const double & d_in,
                                double & e_out, double & n_out, double & u_out );

        /**
         * @brief Convert Earth Centered Earth Fixed (ECEF) to Topocentric (local geogentric tangent) East, North, Up
         * @param local_origin_ecef_x   ECEF X Position of Topocentric coordinate system origin
         * @param local_origin_ecef_y   ECEF Y Position of Topocentric coordinate system origin
         * @param local_origin_ecef_z   ECEF Z Position of Topocentric coordinate system origin
         * @param ecef_x_m              ECEF X Position Input
         * @param ecef_y_m              ECEF Y Position Input
         * @param ecef_z_m              ECEF Z Position Input
         * @param east_m                Topocentric ENU X Position Output
         * @param north_m               Topocentric ENU Y Position Output
         * @param up_m                  Topocentric ENU Z Position Output
         */
        void convertECEF_ToTopocentricENU_GeocentricNormal ( const double & local_origin_ecef_x,  const double & local_origin_ecef_y, const double & local_origin_ecef_z,
                const double & ecef_x_m,             const double & ecef_y_m,            const double & ecef_z_m,
                double & east_m,                     double & north_m,                   double & up_m );

        /**
         * @brief Convert Topocentric (local tangent) East, North, Up to Earth Centered Earth Fixed (ECEF)
         * @param local_origin_ecef_x   ECEF X Position of Topocentric coordinate system origin
         * @param local_origin_ecef_y   ECEF Y Position of Topocentric coordinate system origin
         * @param local_origin_ecef_z   ECEF Z Position of Topocentric coordinate system origin
         * @param east_m                Topocentric ENU X Position Input
         * @param north_m               Topocentric ENU Y Position Input
         * @param up_m                  Topocentric ENU Z Position Input
         * @param ecef_x_m              ECEF X Position Output
         * @param ecef_y_m              ECEF Y Position Output
         * @param ecef_z_m              ECEF Z Position Output
         */
        void convertTopocentricGeocentricNormalENU_ToECEF ( const double & local_origin_ecef_x,  const double & local_origin_ecef_y, const double & local_origin_ecef_z,
                const double & east_m,               const double & north_m,             const double & up_m,
                double & ecef_x_m,                   double & ecef_y_m,                  double & ecef_z_m
                                                          );


        /**
         * @brief Convert Earth Centered Earth Fixed (ECEF) to Topocentric (local geodetic tangent) East, North, Up
         * @param local_origin_ecef_x   ECEF X Position of Topocentric coordinate system origin
         * @param local_origin_ecef_y   ECEF Y Position of Topocentric coordinate system origin
         * @param local_origin_ecef_z   ECEF Z Position of Topocentric coordinate system origin
         * @param geodeticLatRad        Geodetic latitude angle to use for local tangent rotation
         * @param geodeticLonRad        Geodetic longitude angle to use for local tangent rotation
         * @param local_origin_ecef_z   ECEF Z Position of Topocentric coordinate system origin
         * @param ecef_x_m              ECEF X Position Input
         * @param ecef_y_m              ECEF Y Position Input
         * @param ecef_z_m              ECEF Z Position Input
         * @param east_m                Topocentric ENU X Position Output
         * @param north_m               Topocentric ENU Y Position Output
         * @param up_m                  Topocentric ENU Z Position Output
         */
        void convertECEF_ToTopocentricENU_GeodeticNormal ( const double & local_origin_ecef_x,  const double & local_origin_ecef_y, const double & local_origin_ecef_z,
                const double & geodeticLatRad,       const double & geodeticLonRad,
                const double & ecef_x_m,             const double & ecef_y_m,            const double & ecef_z_m,
                double & east_m,                     double & north_m,                   double & up_m );



        /**
         * @brief Convert Topocentric (local tangent) East, North, Up to Earth Centered Earth Fixed (ECEF)
         * @param local_origin_ecef_x   ECEF X Position of Topocentric coordinate system origin
         * @param local_origin_ecef_y   ECEF Y Position of Topocentric coordinate system origin
         * @param local_origin_ecef_z   ECEF Z Position of Topocentric coordinate system origin
         * @param geodeticLatRad        Geodetic latitude angle to use for local tangent rotation
         * @param geodeticLonRad        Geodetic longitude angle to use for local tangent rotation
         * @param east_m                Topocentric ENU X Position Input
         * @param north_m               Topocentric ENU Y Position Input
         * @param up_m                  Topocentric ENU Z Position Input
         * @param ecef_x_m              ECEF X Position Output
         * @param ecef_y_m              ECEF Y Position Output
         * @param ecef_z_m              ECEF Z Position Output
         */
        void convertTopocentricGeodeticNormalENU_ToECEF ( const double & local_origin_ecef_x,  const double & local_origin_ecef_y, const double & local_origin_ecef_z,
                const double & geodeticLatRad,       const double & geodeticLonRad,
                const double & east_m,               const double & north_m,             const double & up_m,
                double & ecef_x_m,                   double & ecef_y_m,                  double & ecef_z_m
                                                        );

        /**
         * @brief Populate the 3x3 Matrix & 3x1 Vector for Topocentric (geocentric local tangent) East, North, Up to Earth Centered Earth Fixed (ECEF)
         * @param local_origin_ecef_x    ECEF X Position of Topocentric coordinate system origin
         * @param local_origin_ecef_y    ECEF Y Position of Topocentric coordinate system origin
         * @param local_origin_ecef_z    ECEF Z Position of Topocentric coordinate system origin
         * @param XformRotationMatrix    Matrix reference to be populated, rotation from ECEF to local
         * @param XformTranslationVector Vector3 reference to be populated, subtracted vector to go from ECEF origin to local origin
         */
        void getECEF_ToTopocentricGeocentricNormalENU_Matricies ( const double & local_origin_ecef_x,  const double & local_origin_ecef_y, const double & local_origin_ecef_z,
                Matrix<double> & XformRotationMatrix, Vector3<double> & XformTranslationVector );

        /**
         * @brief Populate the 3x3 Matrix & 3x1 Vector for Earth Centered Earth Fixed (ECEF) to Topocentric (geocentric local tangent) East, North, Up
         * @param local_origin_ecef_x   ECEF X Position of Topocentric coordinate system origin
         * @param local_origin_ecef_y   ECEF Y Position of Topocentric coordinate system origin
         * @param local_origin_ecef_z   ECEF Z Position of Topocentric coordinate system origin
         * @param XformMatrix           Matrix reference to be populated
         */
        void getTopocentricGeocentricNormalENU_ToECEF_Matricies ( const double & local_origin_ecef_x,  const double & local_origin_ecef_y, const double & local_origin_ecef_z,
                Matrix<double> & XformRotationMatrix, Vector3<double> & XformTranslationVector );

        /**
         * @brief Populate the 3x3 Matrix & 3x1 Vector for Topocentric (geodetic local tangent) East, North, Up to Earth Centered Earth Fixed (ECEF)
         * @param local_origin_ecef_x    ECEF X Position of Topocentric coordinate system origin
         * @param local_origin_ecef_y    ECEF Y Position of Topocentric coordinate system origin
         * @param local_origin_ecef_z    ECEF Z Position of Topocentric coordinate system origin
         * @param geodeticLatRad         Geodetic latitude angle to use for local tangent rotation
         * @param geodeticLonRad         Geodetic longitude angle to use for local tangent rotation
         * @param XformRotationMatrix    Matrix reference to be populated, rotation from ECEF to local
         * @param XformTranslationVector Vector3 reference to be populated, subtracted vector to go from ECEF origin to local origin
         */
        void getECEF_ToTopocentricGeodeticNormalENU_Matricies ( const double & local_origin_ecef_x,  const double & local_origin_ecef_y, const double & local_origin_ecef_z,
                const double & geodeticLatRad,       const double & geodeticLonRad,
                Matrix<double> & XformRotationMatrix, Vector3<double> & XformTranslationVector );

        /**
         * @brief Populate the 3x3 Matrix & 3x1 Vector for Earth Centered Earth Fixed (ECEF) to Topocentric (geodetic local tangent) East, North, Up
         * @param local_origin_ecef_x   ECEF X Position of Topocentric coordinate system origin
         * @param local_origin_ecef_y   ECEF Y Position of Topocentric coordinate system origin
         * @param local_origin_ecef_z   ECEF Z Position of Topocentric coordinate system origin
         * @param geodeticLatRad        Geodetic latitude angle to use for local tangent rotation
         * @param geodeticLonRad        Geodetic longitude angle to use for local tangent rotation
         * @param XformRotationMatrix   Matrix reference to be populated
         * @param XformTranslationVector Vector3 reference to be populated, subtracted vector to go from local origin to ECEF origin
         */
        void getTopocentricGeodeticNormalENU_ToECEF_Matricies (
            const double & local_origin_ecef_x,    const double & local_origin_ecef_y, const double & local_origin_ecef_z,
            const double & geodeticLatRad,       const double & geodeticLonRad,
            Matrix<double> & XformRotationMatrix, Vector3<double> & XformTranslationVector );

        void convertGeodeticWGS84_LatLonAltToUTM(
                const double & latRad, const double & lonRad, const double & altMeters,
                double & NorthingMeters, double & EastingMeters, double & AltitudeMeters,
                unsigned short int & ZoneNumber
                                                );

        void convertUTM_ToGeodeticWGS84LatLonAlt(
                const double & NorthingMeters, const double & EastingMeters, const double & AltitudeMeters,
                const unsigned short int & ZoneNumber,
                double & latRad, double & lonRad, double & altMeters
                                                );

        void convertGeodeticWGS84_LatLonAltToGeocentricLatLonAlt( const double & latRad, const double & lonRad, const double & altMeters,
                                                                        double & GcLatRad,     double & GcLonRad,     double & GcAltMeters);

        void convertGeocentricLatLonAltToGeodeticWGS84LatLonAlt( const double & GcLatRad, const double & GcLonRad, const double & GcAltMeters,
                                                                       double & latRad,         double & lonRad,         double & altMeters);



        void convertCartesianToSpherical( const double & north_m, const double & east_m, const double & down_m,
                                           double & range_m, double & azimuthRad, double & elevationRad );

        void convertSphericalToCartesian( const double & range_m, const double & azimuthRad, const double & elevationRad,
                                          double & north_m, double & east_m, double & down_m );


        /**
        * @brief Convert Topocentric (local tangent) Yaw, Pitch, Roll angles to Earth Centered Earth Fixed (ECEF)
        * @param topo_yaw_rad      Topocentric Yaw Angle in radians (Angle about the Z axis. Usually called Psi)
        * @param topo_pitch_rad    Topocentric Pitch Angle in radians (Angle about the X axis. Usually called Theta)
        * @param topo_roll_rad     Topocentric Roll Angle in radians (Angle about the Y axis. Usually called Phi)
        * @param ref_latitude_rad  Geodetic latitude angle to use for local tangent rotation
        * @param ref_longitude_rad Geodetic longitude angle to use for local tangent rotation
        * @param ecef_psi_rad      ECEF Psi Angle Output in radians (Angle about the Z axis)
        * @param ecef_theta_rad    ECEF Theta Angle Output in radians (Angle about the Y axis)
        * @param ecef_phi_rad      ECEF Phi Angle Output in radians (Angle about the X axis)
        */
        void convertTopocentricYawPitchRoll_To_ECEFPsiThetaPhi ( const double & topo_yaw_rad, const double & topo_pitch_rad, const double & topo_roll_rad,
                                                                 const double & ref_latitude_rad, const double & ref_longitude_rad,
                                                                 double & ecef_psi_rad, double & ecef_theta_rad, double & ecef_phi_rad
                                                                 );

        /**
        * @brief Convert Earth Centered Earth Fixed (ECEF) Yaw, Pitch, Roll angles to Topocentric (local tangent)
        * @param ecef_psi_rad      ECEF Psi Angle in radians (Angle about the Z axis)
        * @param ecef_theta_rad    ECEF Theta Angle in radians (Angle about the Y axis)
        * @param ecef_phi_rad      ECEF Phi Angle in radians (Angle about the X axis)
        * @param ref_latitude_rad  Geodetic latitude angle to use for local tangent rotation
        * @param ref_longitude_rad Geodetic longitude angle to use for local tangent rotation
        * @param topo_yaw_rad      Topocentric Yaw Angle in radians (Angle about the Z axis. Usually called Psi)
        * @param topo_pitch_rad    Topocentric Pitch Angle in radians (Angle about the X axis. Usually called Theta)
        * @param topo_roll_rad     Topocentric Roll Angle in radians (Angle about the Y axis. Usually called Phi)
        */
        void convertECEFPsiThetaPhi_To_TopocentricYawPitchRoll ( const double & ecef_psi_rad, const double & ecef_theta_rad, const double & ecef_phi_rad,
                                                                 const double & ref_latitude_rad, const double & ref_longitude_rad,
                                                                 double & topo_yaw_rad, double & topo_pitch_rad, double & topo_roll_rad
                                                                 );

        /// =============================================== Alias Compiler Name Mappings to Support Other Languages/Sims ==================================
        /// CAUTION: These are aliases only.

        void convertgeodeticwgs84latlonalttoecef_ (
            const double & lat_rad, const double & lon_rad, const double & alt_m,
            double & ecef_x_out_m,  double & ecef_y_out_m , double & ecef_z_out_m ); // aliased to convertGeodeticWGS84LatLonAltToECEF in CoordinateTranslate.cpp
    }
}
#endif
