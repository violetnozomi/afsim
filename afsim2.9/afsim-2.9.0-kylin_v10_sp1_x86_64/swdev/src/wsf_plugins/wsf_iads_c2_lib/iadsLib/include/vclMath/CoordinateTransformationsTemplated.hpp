// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
*  @file CoordinateTransformationsTemplated.hpp
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

#ifndef VCLMATH_COORDINATETRANSFORMATIONS_TEMPLATED_HPP
#define VCLMATH_COORDINATETRANSFORMATIONS_TEMPLATED_HPP 1

#include <cmath>
#include <vclMath/Constants.hpp>
#include <vclMath/Matrix.hpp>
#include <vclMath/Vector3.hpp>

namespace vm
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
template< typename T >
void convertGeodeticWGS84LatLonAltToECEF ( const T & lat_rad, const T & lon_rad, const T & alt_m,
        T & ecef_x_out_m,  T & ecef_y_out_m , T & ecef_z_out_m )
{
    // Compute frequently used arguments
    T cos_lat ( cos ( lat_rad ) ),
    sin_lat ( sin ( lat_rad ) );

    // Compute local Earth radius based upon WGS-84 datum
    T Re = vm::WGS84_SEMI_MAJOR / sqrt ( 1.0 - vm::WGS84_FIRST_ECCENTRICITY_SQUARED * sin_lat * sin_lat );

    // Compute and return ECEF
    ecef_x_out_m = ( Re + alt_m ) * cos_lat * cos ( lon_rad );
    ecef_y_out_m = ( Re + alt_m ) * cos_lat * sin ( lon_rad );
    ecef_z_out_m = ( Re * ( 1.0 - vm::WGS84_FIRST_ECCENTRICITY_SQUARED ) + alt_m ) * sin_lat;
}

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
template < typename T >
void convertECEF_ToGeodeticWGS84LatLonAlt ( const T & x_m, const T & y_m, const T & z_m,
        T & wgs84_lat_out_rad,  T & wgs84_lon_out_rad , T & wgs84_alt_out_m )
{
    T z_m2 ( z_m * z_m );

    T r ( sqrt ( x_m * x_m + y_m * y_m ) );
    T r2 ( r * r );
    T E2 ( vm::WGS84_SEMI_MAJOR2 - vm::WGS84_SEMI_MINOR2 );
    T F ( 54.0 * vm::WGS84_SEMI_MINOR2 * z_m2 );
    T G ( r2 + ( 1.0-vm::WGS84_FIRST_ECCENTRICITY_SQUARED ) *z_m2 - vm::WGS84_FIRST_ECCENTRICITY_SQUARED * E2 );
    T C ( vm::WGS84_FIRST_ECCENTRICITY_SQUARED * vm::WGS84_FIRST_ECCENTRICITY_SQUARED * F * r2 / ( G*G*G ) );
    T S ( pow ( ( 1.0+C + sqrt ( C*C + 2.0*C ) ),1.0/3.0 ) );
    T P ( F / ( 3.0*pow ( S+1.0/S+1.0,2.0 ) *G*G ) );
    T Q ( sqrt ( 1.0 + 2.0*vm::WGS84_FIRST_ECCENTRICITY_SQUARED * vm::WGS84_FIRST_ECCENTRICITY_SQUARED * P ) );
    T r0 ( - ( P*vm::WGS84_FIRST_ECCENTRICITY_SQUARED*r ) / ( 1.0+Q ) + sqrt ( 0.5 * vm::WGS84_SEMI_MAJOR2 * ( 1.0+1.0/Q ) - P* ( 1.0-vm::WGS84_FIRST_ECCENTRICITY_SQUARED ) *z_m2 / ( Q* ( 1.0+Q ) ) - 0.5 * P * r2 ) );
    T U ( sqrt ( pow ( r-vm::WGS84_FIRST_ECCENTRICITY_SQUARED*r0,2.0 ) +z_m2 ) );
    T V ( sqrt ( pow ( r-vm::WGS84_FIRST_ECCENTRICITY_SQUARED*r0,2.0 ) + ( 1.0-vm::WGS84_FIRST_ECCENTRICITY_SQUARED ) *z_m2 ) );
    T Z0 ( ( vm::WGS84_SEMI_MINOR2 * z_m ) / ( vm::WGS84_SEMI_MAJOR * V ) );

    wgs84_alt_out_m = U * ( 1.0 - vm::WGS84_SEMI_MINOR2 / ( vm::WGS84_SEMI_MAJOR * V ) );
    wgs84_lat_out_rad = atan ( ( z_m+vm::WGS84_SECOND_ECCENTRICITY_SQUARED*Z0 ) / ( r ) );
    wgs84_lon_out_rad = atan2 ( y_m,x_m );
}

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
template < typename T >
void convertLatLonAltToTransverseMercatorProjectionENU (
    const T & lat_rad,     const T & lon_rad,     const T & alt_m,               // Input
    const T & lat_ref_rad, const T & lon_ref_rad, const T & datumEarthRadius,    // Reference geodetic data
    T & east_m,            T & north_m,           T & up_m )                    // ENU Mercator Output
{
    // compute the change in longitude from the reference
    T delta_lambda ( lon_rad - lon_ref_rad );

    T B ( cos ( lat_rad ) * sin ( delta_lambda ) );
    east_m  = 0.5 * datumEarthRadius * log ( ( 1.0 + B ) / ( 1.0 - B ) );
    north_m = datumEarthRadius * ( atan2 ( tan ( lat_rad ),cos ( delta_lambda ) ) - lat_ref_rad );
    up_m    = alt_m;
}

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
template < typename T >
void convertTransverseMercatorProjectionENUToLatLonAlt (
    const T & east_m,      const T & north_m,     const T & up_m,                // Input TM ENU
    const T & lat_ref_rad, const T & lon_ref_rad, const T & datumEarthRadius,    // Reference geodetic data
    T & lat_rad,           T & lon_rad,           T & alt_m )                   // Geodetic output
{
    T D ( north_m / datumEarthRadius + lat_ref_rad );
    lat_rad = asin ( sin ( D ) / cosh ( east_m/datumEarthRadius ) );
    lon_rad = lon_ref_rad + atan2 ( sinh ( east_m/datumEarthRadius ),cos ( D ) );
    alt_m   = up_m;
}

/**
* @brief Convert Cartesian East, North, Up (ENU) to Cartesian North, East, Down (NED)
* @param e_in   Input X Axis, East
* @param n_in   Input Y Axis, North
* @param u_in   Input Z Axis, Up
* @param n_out  Output X Axis, North
* @param e_out  Output Y Axis, East
* @param d_out  Output Z Axis, Down
*/
template < typename T >
void convertENU_ToNED ( const T & e_in, const T & n_in, const T & u_in,
                        T & n_out, T & e_out, T & d_out )
{
    n_out = n_in;
    e_out = e_in;
    d_out = -u_in;
}

/**
* @brief Convert Cartesian East, North, Up (ENU) to Cartesian North, East, Down (NED)
* @param n_in  Input X Axis, North
* @param e_in  Input Y Axis, East
* @param d_in  Input Z Axis, Down
* @param e_out Output X Axis, East
* @param n_out Output Y Axis, North
* @param u_out Output Z Axis, Up
*/
template < typename T >
void convertNED_ToENU ( const T & n_in, const T & e_in, const T & d_in,
                        T & e_out, T & n_out, T & u_out )
{
    e_out = e_in;
    n_out = n_in;
    u_out = -d_in;
}

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
template < typename T >
void convertECEF_ToTopocentricENU_GeodeticNormal ( const T & local_origin_ecef_x,  const T & local_origin_ecef_y, const T & local_origin_ecef_z,
        const T & geodeticLatRad,       const T & geodeticLonRad,
        const T & ecef_x_m,             const T & ecef_y_m,            const T & ecef_z_m,
        T & east_m,                     T & north_m,                   T & up_m )
{
    T sin_phip ( sin ( geodeticLatRad ) ),
    cos_phip ( cos ( geodeticLatRad ) ),
    sin_lambda ( sin ( geodeticLonRad ) ),
    cos_lambda ( cos ( geodeticLonRad ) );

    T ecef_rel_x ( ecef_x_m - local_origin_ecef_x );
    T ecef_rel_y ( ecef_y_m - local_origin_ecef_y );
    T ecef_rel_z ( ecef_z_m - local_origin_ecef_z );

    // ** From MATLAB **
    // xform_ecef_to_enu = [ ...
    // -sin(lambda)             cos(lambda)             0;
    // -sin(phip)*cos(lambda)  -sin(phip)*sin(lambda)   cos(phip);
    //  cos(phip)*cos(lambda)   cos(phip)*sin(lambda)   sin(phip) ];


    east_m  =          -sin_lambda*ecef_rel_x +           cos_lambda*ecef_rel_y;
    north_m = -sin_phip*cos_lambda*ecef_rel_x + -sin_phip*sin_lambda*ecef_rel_y  + cos_phip*ecef_rel_z;
    up_m    =  cos_phip*cos_lambda*ecef_rel_x +  cos_phip*sin_lambda*ecef_rel_y  + sin_phip*ecef_rel_z;
}

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
template < typename T >
void convertECEF_ToTopocentricENU_GeocentricNormal ( const T & local_origin_ecef_x,  const T & local_origin_ecef_y, const T & local_origin_ecef_z,
        const T & ecef_x_m,             const T & ecef_y_m,            const T & ecef_z_m,
        T & east_m,                     T & north_m,                   T & up_m )
{
    // compute the geocentric latitude/longitude angles to pass to
    T phip ( atan2 ( local_origin_ecef_z, sqrt ( local_origin_ecef_y*local_origin_ecef_y + local_origin_ecef_x*local_origin_ecef_x ) ) );
    T lambda ( atan2 ( local_origin_ecef_y,local_origin_ecef_x ) );

    vm::convertECEF_ToTopocentricENU_GeodeticNormal<T> (
        local_origin_ecef_x,  local_origin_ecef_y, local_origin_ecef_z,
        phip,                 lambda,
        ecef_x_m,             ecef_y_m,            ecef_z_m,
        east_m,               north_m,             up_m
    );

}

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
template < typename T >
void convertTopocentricGeodeticNormalENU_ToECEF ( const T & local_origin_ecef_x,  const T & local_origin_ecef_y, const T & local_origin_ecef_z,
        const T & geodeticLatRad,       const T & geodeticLonRad,
        const T & east_m,               const T & north_m,             const T & up_m,
        T & ecef_x_m,                   T & ecef_y_m,                  T & ecef_z_m
                                                )
{
    T sin_phip ( sin ( geodeticLatRad ) ),
    cos_phip ( cos ( geodeticLatRad ) ),
    sin_lambda ( sin ( geodeticLonRad ) ),
    cos_lambda ( cos ( geodeticLonRad ) );

    // ** From MATLAB **
    // xform_ecef_to_enu' = [ ...
    // -sin(lambda)             -sin(phip)*cos(lambda)   cos(phip)*cos(lambda);
    //  cos(lambda)             -sin(phip)*sin(lambda)   cos(phip)*sin(lambda);
    //  0                        cos(phip)               sin(phip)               ];

    ecef_x_m = local_origin_ecef_x + -sin_lambda*east_m + -sin_phip*cos_lambda*north_m + cos_phip*cos_lambda*up_m;
    ecef_y_m = local_origin_ecef_y +  cos_lambda*east_m + -sin_phip*sin_lambda*north_m + cos_phip*sin_lambda*up_m;
    ecef_z_m = local_origin_ecef_z +                                  cos_phip*north_m +            sin_phip*up_m;
}

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
template < typename T >
void convertTopocentricGeocentricNormalENU_ToECEF ( const T & local_origin_ecef_x,  const T & local_origin_ecef_y, const T & local_origin_ecef_z,
        const T & east_m,               const T & north_m,             const T & up_m,
        T & ecef_x_m,                   T & ecef_y_m,                  T & ecef_z_m
                                                  )
{
    // compute topocentric normal angles
    T phip ( atan2 ( local_origin_ecef_z, sqrt ( local_origin_ecef_y*local_origin_ecef_y + local_origin_ecef_x*local_origin_ecef_x ) ) );
    T lambda ( atan2 ( local_origin_ecef_y,local_origin_ecef_x ) );

    vm::convertTopocentricGeodeticNormalENU_ToECEF<T> (
        local_origin_ecef_x,  local_origin_ecef_y, local_origin_ecef_z,
        phip,                 lambda,
        east_m,               north_m,             up_m,
        ecef_x_m,             ecef_y_m,            ecef_z_m
    );
}

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
template < typename T >
void getECEF_ToTopocentricGeodeticNormalENU_Matricies ( const T & local_origin_ecef_x,  const T & local_origin_ecef_y, const T & local_origin_ecef_z,
        const T & geodeticLatRad,       const T & geodeticLonRad,
        Matrix<T> & XformRotationMatrix, Vector3<T> & XformTranslationVector )
{
    T sin_phip ( sin ( geodeticLatRad ) ),
    cos_phip ( cos ( geodeticLatRad ) ),
    sin_lambda ( sin ( geodeticLonRad ) ),
    cos_lambda ( cos ( geodeticLonRad ) );

    // Complete transform from ECEF to GeocentricNormal/Topocentric ENU involves a translation and rotation

    // Compute the rotation matrix from ECEF to GeocentricNormal/Topocentric ENU
    XformRotationMatrix.resize ( 3,3 );

    XformRotationMatrix.m_Vector[0] = -sin_lambda;
    XformRotationMatrix.m_Vector[3] = -sin_phip*cos_lambda;
    XformRotationMatrix.m_Vector[6] = cos_phip*cos_lambda;
    XformRotationMatrix.m_Vector[1] =  cos_lambda;
    XformRotationMatrix.m_Vector[4] = -sin_phip*sin_lambda;
    XformRotationMatrix.m_Vector[7] = cos_phip*sin_lambda;
    XformRotationMatrix.m_Vector[2] =         0.0;
    XformRotationMatrix.m_Vector[5] =             cos_phip;
    XformRotationMatrix.m_Vector[8] =            sin_phip;

    // Compute the translation vector from ECEF to GeocentricNormal/Topocentric ENU
    Vector3<T> ECEF_Ref ( local_origin_ecef_x, local_origin_ecef_y, local_origin_ecef_z );
    XformTranslationVector = XformRotationMatrix.multVector3x3 ( ECEF_Ref );
}

/**
* @brief Populate the 3x3 Matrix & 3x1 Vector for Topocentric (geocentric local tangent) East, North, Up to Earth Centered Earth Fixed (ECEF)
* @param local_origin_ecef_x    ECEF X Position of Topocentric coordinate system origin
* @param local_origin_ecef_y    ECEF Y Position of Topocentric coordinate system origin
* @param local_origin_ecef_z    ECEF Z Position of Topocentric coordinate system origin
* @param XformRotationMatrix    Matrix reference to be populated, rotation from ECEF to local
* @param XformTranslationVector Vector3 reference to be populated, subtracted vector to go from ECEF origin to local origin
*/
template < typename T >
void getECEF_ToTopocentricGeocentricNormalENU_Matricies ( const T & local_origin_ecef_x,  const T & local_origin_ecef_y, const T & local_origin_ecef_z,
        Matrix<T> & XformRotationMatrix, Vector3<T> & XformTranslationVector )
{
    // compute topocentric normal angles
    T phip ( atan2 ( local_origin_ecef_z, sqrt ( local_origin_ecef_y*local_origin_ecef_y + local_origin_ecef_x*local_origin_ecef_x ) ) );
    T lambda ( atan2 ( local_origin_ecef_y,local_origin_ecef_x ) );

    vm::getECEF_ToTopocentricGeodeticNormalENU_Matricies<T> (
        local_origin_ecef_x,    local_origin_ecef_y,    local_origin_ecef_z,
        phip,                   lambda,
        XformRotationMatrix,    XformTranslationVector
    );
}

/**
* @brief Populate the 3x3 Matrix & 3x1 Vector for Earth Centered Earth Fixed (ECEF) to Topocentric (geocentric local tangent) East, North, Up
* @param local_origin_ecef_x   ECEF X Position of Topocentric coordinate system origin
* @param local_origin_ecef_y   ECEF Y Position of Topocentric coordinate system origin
* @param local_origin_ecef_z   ECEF Z Position of Topocentric coordinate system origin
* @param XformMatrix           Matrix reference to be populated
*/
template < typename T >
void getTopocentricGeocentricNormalENU_ToECEF_Matricies ( const T & local_origin_ecef_x,  const T & local_origin_ecef_y, const T & local_origin_ecef_z,
        Matrix<T> & XformRotationMatrix, Vector3<T> & XformTranslationVector )
{
    vm::getECEF_ToTopocentricGeocentricNormalENU_Matricies<T> ( local_origin_ecef_x, local_origin_ecef_y, local_origin_ecef_z, XformRotationMatrix, XformTranslationVector );
    XformRotationMatrix    = XformRotationMatrix.transpose();
    XformTranslationVector = XformTranslationVector * -1.0;
}

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
template < typename T >
void getTopocentricGeodeticNormalENU_ToECEF_Matricies (
    const T & local_origin_ecef_x,    const T & local_origin_ecef_y, const T & local_origin_ecef_z,
    const T & geodeticLatRad,       const T & geodeticLonRad,
    Matrix<T> & XformRotationMatrix, Vector3<T> & XformTranslationVector )
{
    vm::getECEF_ToTopocentricGeodeticNormalENU_Matricies<T> (
        local_origin_ecef_x, local_origin_ecef_y, local_origin_ecef_z,
        geodeticLatRad,      geodeticLonRad,
        XformRotationMatrix, XformTranslationVector );
    XformRotationMatrix    = XformRotationMatrix.transpose();
    XformTranslationVector = XformTranslationVector * -1.0;
}

template < typename T >
void convertGeodeticWGS84_LatLonAltToUTM(
    const T & latRad, const T & lonRad, const T & altMeters,
    T & NorthingMeters, T & EastingMeters, T & AltitudeMeters,
    unsigned short int & ZoneNumber
)
{

    // longitude modification to lie between -/+ pi
    T lonRadMod ( lonRad + vm::PI - static_cast<int> ( ( lonRad+vm::PI ) /vm::TWOPI ) *vm::TWOPI - vm::PI );

    unsigned short int zoneNumber ( static_cast<unsigned short int> ( ( lonRadMod + vm::PI ) / ( 6.0*vm::DEGREES_TO_RADIANS ) ) + 1 );
    T lonRadOrigin ( static_cast<T> ( zoneNumber-1 ) * ( 6.0*vm::DEGREES_TO_RADIANS ) - vm::PI + 3.0 * vm::DEGREES_TO_RADIANS );
    T p ( lonRadMod - lonRadOrigin );
    //printf("lonRadMod = %18.5f, origin (deg) = %18.5f, p = %18.5f\n",lonRadMod,lonRadOrigin * vm::RADIANS_TO_DEGREES, p);
    const T & e2 ( vm::WGS84_FIRST_ECCENTRICITY_SQUARED );
    const T & e4 ( e2 * e2 );
    const T & e6 ( e2 * e2 * e2 );

    const T & ep2 ( vm::WGS84_SECOND_ECCENTRICITY_SQUARED );
    const T & ep4 ( ep2 * ep2 );

    const T & a ( vm::WGS84_SEMI_MAJOR );
    T M (
        a * (
            + ( 1.0   - e2/4.0    - 3.0  * e4/64.0   -  5.0 * e6/256.0 ) * latRad
            - ( 3.0   * e2/8.0    + 3.0  * e4/32.0   + 45.0 * e6/1024.0 ) * std::sin ( 2.0 * latRad )
            + ( 15.0  * e4/256.0  + 45.0 * e6/1024.0 ) * std::sin ( 4.0 * latRad )
            - ( -35.0 * e6/3072.0 ) * std::sin ( 6.0 * latRad )
        )
    );
    //T nu ( a * (1.0 - e2) / pow(1.0 - e2 * sin(latRad) * sin(latRad) ,3.0/2.0) );
    T nu ( a / sqrt ( 1.0 - e2 * std::sin ( latRad ) * std::sin ( latRad ) ) );
    T K1 ( M * vm::USGS_UTM_K0 );
    T K2 ( vm::USGS_UTM_K0 * nu * std::sin ( 2.0 * latRad ) / 4.0 );
    T K3 (
        ( vm::USGS_UTM_K0 * nu * std::sin ( latRad ) * std::pow ( float( std::cos ( latRad ) ), 3.0f ) / 24.0f ) *
        ( 5.0 - std::pow ( float( std::tan ( latRad ) ), 2.0f ) + 9.0 * ep2 * std::cos ( latRad ) * std::cos ( latRad ) + 4.0 * ep4 * std::pow ( float( std::cos ( latRad ) ), 4.0f ) )
    );
    T K4 ( vm::USGS_UTM_K0 * nu * std::cos ( latRad ) );
    T K5 ( ( vm::USGS_UTM_K0 * nu * std::pow ( float( std::cos ( latRad ) ), 3.0f ) /6.0 ) * ( 1.0 - std::pow ( float( std::tan ( latRad ) ), 2.0f ) + ep2 * std::cos ( latRad ) * std::cos ( latRad ) ) );

    NorthingMeters = K1 + K2 * std::pow ( float( p ), 2.0f ) + K3 * std::pow ( float( p ), 4.0f );
    EastingMeters  = K4 * p + K5 * std::pow ( float( p ), 3.0f ) + 500000.0;    // add 500000 because computed easting is from central meridian
    AltitudeMeters = altMeters;
    ZoneNumber = zoneNumber;
    //printf("Zone: %d\nNorthing: %18.5f\nEasting: %18.5f\nAltitude: %18.5f\n",zoneNumber,NorthingMeters, EastingMeters, AltitudeMeters);
}

template < typename T >
void convertUTM_ToGeodeticWGS84LatLonAlt(
    const T & NorthingMeters, const T & EastingMeters, const T & AltitudeMeters,
    const unsigned short int & ZoneNumber,
    T & latRad, T & lonRad, T & altMeters
)
{
    T EastingMod ( EastingMeters - 500000.0 );
    T lonRadOrigin ( static_cast<T> ( ZoneNumber-1 ) * ( 6.0*vm::DEGREES_TO_RADIANS ) - vm::PI + 3.0 * vm::DEGREES_TO_RADIANS );

    const T & e2 ( vm::WGS84_FIRST_ECCENTRICITY_SQUARED );
    const T & e4 ( e2 * e2 );
    const T & e6 ( e2 * e2 * e2 );

    const T & ep2 ( vm::WGS84_SECOND_ECCENTRICITY_SQUARED );

    const T & a ( vm::WGS84_SEMI_MAJOR );

    T M ( NorthingMeters / vm::USGS_UTM_K0 );

    T mu (
        M / ( a * ( 1.0 - e2/4.0 - 3.0 * e4/64.0 - 5.0 * e6 / 256.0 ) )
    );

    T temp ( sqrt ( 1.0-e2 ) );
    T e_1 ( ( 1.0 - temp ) / ( 1.0 + temp ) );

    T J1 ( 3.0    *          e_1 / 2.0  - 27.0 * pow ( e_1,3.0 ) / 32.0 );
    T J2 ( 21.0   * pow ( e_1,2.0 ) / 16.0 - 55.0 * pow ( e_1,4.0 ) / 32.0 );
    T J3 ( 151.0  * pow ( e_1,3.0 ) / 96.0 );
    T J4 ( 1097.0 * pow ( e_1,4.0 ) / 512.0 );

    T fp (
        mu + J1 * sin ( 2.0*mu ) + J2 * sin ( 4.0*mu ) + J3 * sin ( 6.0*mu ) + J4 * sin ( 8.0*mu )
    );

    T C1 ( ep2 * cos ( fp ) * cos ( fp ) );
    T T1 ( tan ( fp ) * tan ( fp ) );
    temp = 1.0 - e2*sin ( fp ) *sin ( fp );
    T R1 ( a / ( pow ( temp,1.5 ) ) );
    T N1 ( a / sqrt ( temp ) );
    T D ( EastingMod / ( N1 * vm::USGS_UTM_K0 ) );

    T Q1 ( N1 * tan ( fp ) / R1 );
    T Q2 ( D * D / 2.0 );
    T Q3 ( ( 5.0 +  3.0 * T1 +  10.0 * C1                  - 4.0 * C1 * C1 - 9.0   * ep2 ) * pow ( D,4.0 ) / 24.0 );
    T Q4 ( ( 61.0 + 90.0 * T1 + 298.0 * C1 + 45.0 * T1 * T1 - 3.0 * C1 * C1 - 252.0 * ep2 ) * pow ( D,6.0 ) / 720.0 );
    T & Q5 ( D );
    T Q6 ( ( 1.0 + 2.0 * T1 + C1 ) * pow ( D,3.0 ) / 6.0 );
    T Q7 ( ( 5.0 - 2.0*C1 + 28.0 * T1 - 3.0 * C1 * C1 + 8.0 * ep2 + 24.0 * T1 * T1 ) * pow ( D,5.0 ) / 120.0 );

    latRad = fp - Q1 * ( Q2 - Q3 + Q4 );
    lonRad = lonRadOrigin + ( Q5 - Q6 + Q7 ) / cos ( fp );
    altMeters = AltitudeMeters;
}

template < typename T >
void convertGeodeticWGS84_LatLonAltToGeocentricLatLonAlt( const T & latRad, const T & lonRad, const T & altMeters,
        T & GcLatRad,     T & GcLonRad,     T & GcAltMeters)
{
    // first convert geodetic values to ECEF
    T tX, tY, tZ;
    vm::convertGeodeticWGS84LatLonAltToECEF<T> (
        latRad, lonRad, altMeters,
        tX, tY, tZ
    );

    // then convert ECEF to geocentric values
    GcLatRad =  atan2 ( tZ, sqrt ( tY*tY + tX*tX ) );
    GcLonRad =  atan2 ( tY,tX );
    GcAltMeters =  sqrt( tX * tX + tY * tY + tZ * tZ ) - vm::WGS84_MEAN_RADIUS_OF_SEMI_AXES; // altitude above sphere
}

template < typename T >
void convertGeocentricLatLonAltToGeodeticWGS84LatLonAlt( const T & GcLatRad, const T & GcLonRad, const T & GcAltMeters,
        T & latRad,         T & lonRad,         T & altMeters)
{
    // first convert geocentric values to ECEF
    T R ( vm::WGS84_MEAN_RADIUS_OF_SEMI_AXES + GcAltMeters );
    T tX( R * cos(GcLatRad) * cos(GcLonRad) ),tY( R * cos(GcLatRad) * sin(GcLonRad) ),tZ( R * sin(GcLatRad) );

    // then convert ECEF to geodetic values
    vm::convertECEF_ToGeodeticWGS84LatLonAlt<T> ( tX, tY, tZ, latRad, lonRad, altMeters );

}

template < typename T >
void convertCartesianToSpherical( const T & north_m, const T & east_m, const T & down_m,
                                  T & range_m, T & azimuthRad, T & elevationRad )
{
    range_m = sqrt ( pow ( north_m, 2.0 ) + pow ( east_m, 2.0 ) + pow ( down_m, 2.0 ) );
    azimuthRad = atan2 ( east_m, north_m );
    elevationRad = asin ( - down_m / range_m );
}

template < typename T >
void convertSphericalToCartesian( const T & range_m, const T & azimuthRad, const T & elevationRad,
                                  T & north_m, T & east_m, T & down_m )
{
    north_m = range_m * cos ( elevationRad ) * cos ( azimuthRad );
    east_m = range_m * cos ( elevationRad ) * sin ( azimuthRad );
    down_m = - range_m * sin ( elevationRad );
}


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
template < typename T >
void convertTopocentricYawPitchRoll_To_ECEFPsiThetaPhi ( const T & topo_yaw_rad, const T & topo_pitch_rad, const T & topo_roll_rad,
        const T & ref_latitude_rad, const T & ref_longitude_rad,
        T & ecef_psi_rad, T & ecef_theta_rad, T & ecef_phi_rad
                                                       )
{
    T sinTopoYaw   ( std::sin( topo_yaw_rad ) );
    T cosTopoYaw   ( std::cos( topo_yaw_rad ) );
    T sinTopoPitch ( std::sin( topo_pitch_rad ) );
    T cosTopoPitch ( std::cos( topo_pitch_rad ) );
    T sinTopoRoll  ( std::sin( topo_roll_rad ) );
    T cosTopoRoll  ( std::cos( topo_roll_rad ) );

    T sin_ref_latitude ( std::sin ( ref_latitude_rad ) );
    T cos_ref_latitude ( std::cos ( ref_latitude_rad ) );
    T sin_ref_longitude ( std::sin ( ref_longitude_rad ) );
    T cos_ref_longitude ( std::cos ( ref_longitude_rad ) );

    // calculate geocentric theta (pitch)
    ecef_theta_rad = std::asin( -cos_ref_latitude * cosTopoYaw * cosTopoPitch - sin_ref_latitude * sinTopoPitch );

    // calculate geocentric psi (yaw)
    // note:  both of these first calculated vales are actually
    //        multiplied by cos(gc_theta) in the real matrix
    T A_sub_12 ( cos_ref_longitude * sinTopoYaw * cosTopoPitch
                 - sin_ref_latitude * sin_ref_longitude * cosTopoYaw * cosTopoPitch
                 + cos_ref_latitude * sin_ref_longitude * sinTopoPitch );

    T A_sub_11 ( -sin_ref_longitude * sinTopoYaw * cosTopoPitch
                 - sin_ref_latitude * cos_ref_longitude * cosTopoYaw * cosTopoPitch
                 + cos_ref_latitude * cos_ref_longitude * sinTopoPitch );

    // ECEF yaw
    ecef_psi_rad = std::atan2( A_sub_12, A_sub_11 );

    // calculate geocentric phi (roll)
    // note:  both of these first calculated vales are actually
    //        multiplied by cos(gc_theta) in the real matrix
    T A_sub_23 ( cos_ref_latitude * (-sinTopoYaw * cosTopoRoll + cosTopoYaw * sinTopoPitch * sinTopoRoll )
                 - sin_ref_latitude * cosTopoPitch * sinTopoRoll );

    T A_sub_33 ( cos_ref_latitude * ( sinTopoYaw * sinTopoRoll + cosTopoYaw * sinTopoPitch * cosTopoRoll )
                 - sin_ref_latitude * cosTopoPitch * cosTopoRoll );

    // ECEF roll
    ecef_phi_rad = std::atan2( A_sub_23, A_sub_33 );
}

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
template < typename T >
void convertECEFPsiThetaPhi_To_TopocentricYawPitchRoll ( const T & ecef_psi_rad, const T & ecef_theta_rad, const T & ecef_phi_rad,
        const T & ref_latitude_rad, const T & ref_longitude_rad,
        T & topo_yaw_rad, T & topo_pitch_rad, T & topo_roll_rad
                                                       )
{
    T sinECEFYaw   ( std::sin( ecef_psi_rad ) );
    T cosECEFYaw   ( std::cos( ecef_psi_rad ) );
    T sinECEFPitch ( std::sin( ecef_theta_rad ) );
    T cosECEFPitch ( std::cos( ecef_theta_rad ) );
    T sinECEFRoll  ( std::sin( ecef_phi_rad ) );
    T cosECEFRoll  ( std::cos( ecef_phi_rad ) );

    T sin_ref_latitude ( std::sin ( ref_latitude_rad ) );
    T cos_ref_latitude ( std::cos ( ref_latitude_rad ) );
    T sin_ref_longitude ( std::sin ( ref_longitude_rad ) );
    T cos_ref_longitude ( std::cos ( ref_longitude_rad ) );

    // calculate local theta (pitch)
    topo_pitch_rad = std::asin ( cos_ref_latitude * cos_ref_longitude * cosECEFPitch * cosECEFYaw +
                                 cos_ref_latitude * sin_ref_longitude * cosECEFPitch * sinECEFYaw -
                                 sin_ref_latitude * sinECEFPitch );

    // calculate local psi (yaw)
    T B_sub_11 ( -sin_ref_longitude * cosECEFPitch * cosECEFYaw
                 + cos_ref_longitude * cosECEFPitch * sinECEFYaw );
    T B_sub_12 ( -sin_ref_latitude * cos_ref_longitude * cosECEFPitch * cosECEFYaw
                 - sin_ref_latitude * sin_ref_longitude * cosECEFPitch * sinECEFYaw
                 - cos_ref_latitude * sinECEFPitch );

    // the simulator's yaw
    topo_yaw_rad = std::atan2( B_sub_11, B_sub_12 );

    // calculate local phi (roll)
    // note:  both of these first calculated vales are actually
    //        multiplied by cos(local_theta) in the real matrix
    T B_sub_23 ( cos_ref_latitude * cos_ref_longitude * (-cosECEFRoll*sinECEFYaw+sinECEFRoll*sinECEFPitch*cosECEFYaw)     +
                 cos_ref_latitude * sin_ref_longitude * ( cosECEFRoll*cosECEFYaw + sinECEFRoll*sinECEFPitch*sinECEFYaw)   +
                 sin_ref_latitude * ( sinECEFRoll*cosECEFPitch) );

    T B_sub_33 ( cos_ref_latitude * cos_ref_longitude * ( sinECEFRoll*sinECEFYaw + cosECEFRoll*sinECEFPitch*cosECEFYaw)   +
                 cos_ref_latitude * sin_ref_longitude * (-sinECEFRoll*cosECEFYaw + cosECEFRoll*sinECEFPitch*sinECEFYaw)   +
                 sin_ref_latitude * ( cosECEFRoll*cosECEFPitch) );

    topo_roll_rad = std::atan2( -B_sub_23, -B_sub_33 );
}

}

#endif
