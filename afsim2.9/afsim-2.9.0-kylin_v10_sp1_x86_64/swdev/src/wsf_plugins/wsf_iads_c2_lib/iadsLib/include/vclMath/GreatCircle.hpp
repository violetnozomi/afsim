// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file GreatCircle.hpp
 *  @author Ken Sewell AFRL/RYZW \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *          937-656-4899 x3004 (Sewell)
 *  @brief Template for Great circle calculations.
 *  Classification: UNCLASSIFIED
 */

#include <vclMath/Constants.hpp>
#include <vclMath/GeodeticToECEF.hpp>
#include <vclMath/Vector3.hpp>

#ifndef VCLMATH_GREATCIRCLE_HPP
#define VCLMATH_GREATCIRCLE_HPP 1

namespace vm
{
    /**
     *  @brief Great circle calculation routines.
     */
    template<typename T> class GreatCircle
    {
    public:
	/**
	 * @brief Calculates difference in longitude in Radians.
	 * @param lon1 Longitude in radians.
	 * @param lon2 Longitude in radians.
	 * @returns Distance in radians.
	 */
	static T calculateDeltaLongitudeInRad(
	    const T &lon1,
	    const T &lon2 );

	/**
	 * @brief Calculates great circle distance between two points.
	 * @param lat1 Point 1 latitude in radians.
	 * @param lon1 Point 1 longitude in radians.
	 * @param lat2 Point 2 latitude in radians.
	 * @param lon2 Point 2 longitude in radians.
	 * @returns Great circle distance in radians.
	 */
	static T calculateDistanceInRad( const T &lat1,
					 const T &lon1,
					 const T &lat2,
					 const T &lon2
	    );

	/**
	 * @brief Calculates great circle distance between two points.
	 * @param lat1 Point 1 latitude in decimal degrees.
	 * @param lon1 Point 1 longitude in decimal degrees.
	 * @param lat2 Point 2 latitude in decimal degrees.
	 * @param lon2 Point 2 longitude in decimal degrees.
	 * @returns Great circle distance in decimal degrees.
	 */
	static T calculateDistanceInDeg( const T &lat1,
					 const T &lon1,
					 const T &lat2,
					 const T &lon2
	    );

	/**
	 * @brief Calculates great circle distance between two points.
	 * @param lat1 Point 1 latitude in decimal degrees.
	 * @param lon1 Point 1 longitude in decimal degrees.
	 * @param lat2 Point 2 latitude in decimal degrees.
	 * @param lon2 Point 2 longitude in decimal degrees.
	 * @returns Great circle distance in nautical miles.
	 */
	static T calculateDistanceInNM( const T &lat1,
					const T &lon1,
					const T &lat2,
					const T &lon2
	    );

	/**
	 * @brief Calculates great circle distance between two points.
	 * @param lat1 Point 1 latitude in decimal degrees.
	 * @param lon1 Point 1 longitude in decimal degrees.
	 * @param lat2 Point 2 latitude in decimal degrees.
	 * @param lon2 Point 2 longitude in decimal degrees.
	 * @returns Great circle distance in meters.
	 */
	static T calculateDistanceInMeters( const T &lat1,
					    const T &lon1,
					    const T &lat2,
					    const T &lon2
	    );

	/**
	   @brief Calculate shortest great-circle distance from point to
	   arc segment. If arc ends are closer then return that distance.
	   @param pointLat Geodetic latitude of point in radians.
	   @param pointLon Geodetic longitude of point in radians.
	   @param arcLat1 Geodetic latitude of arc start point in radians.
	   @param arcLon1 Geodetic longitude of arc start point in radians.
	   @param arcLat2 Geodetic latitude of arc end point in radians.
	   @param arcLon3 Geodetic longitude of arc end point in radians.
	   @returns Great circle distance in arc radians.
	*/
	static T calculateDistanceFromPointToArcInRad(
	    const T &pointLat,
	    const T &pointLon,
	    const T &arcLat1,
	    const T &arcLon1,
	    const T &arcLat2,
	    const T &arcLon2
	    );

	/**
	   @brief Calculate shortest great-circle distance from point to
	   arc segment.
	   @param pointLat Geodetic latitude of point in radians.
	   @param pointLon Geodetic longitude of point in radians.
	   @param arcLat1 Geodetic latitude of arc start point in radians.
	   @param arcLon1 Geodetic longitude of arc start point in radians.
	   @param arcLat2 Geodetic latitude of arc end point in radians.
	   @param arcLon3 Geodetic longitude of arc end point in radians.
	   @returns Great circle distance in arc radians.
	*/
        static T calculateDistanceFromPointToGreatCircleInRad(
            const T &pointLatC,
            const T &pointLonC,
            const T &arcLatA,
            const T &arcLonA,
            const T &arcLatB,
            const T &arcLonB
            );

	/**
	   @brief Calculate shortest great-circle distance from point to
	   arc segment. If arc ends are closer then return that distance.
	   @param pointLat Geodetic latitude of point in degrees.
	   @param pointLon Geodetic longitude of point in degrees.
	   @param arcLat1 Geodetic latitude of arc start point in degrees.
	   @param arcLon1 Geodetic longitude of arc start point in degrees.
	   @param arcLat2 Geodetic latitude of arc end point in degrees.
	   @param arcLon3 Geodetic longitude of arc end point in degrees.
	   @returns Great circle distance in arc degrees.
	*/
	static T calculateDistanceFromPointToArcInDeg(
	    const T &pointLat,
	    const T &pointLon,
	    const T &arcLat1,
	    const T &arcLon1,
	    const T &arcLat2,
	    const T &arcLon2
	    );

	/**
	   @brief Calculate shortest great-circle distance from point to
	   arc segment.
	   @param pointLat Geodetic latitude of point in degrees.
	   @param pointLon Geodetic longitude of point in degrees.
	   @param arcLat1 Geodetic latitude of arc start point in degrees.
	   @param arcLon1 Geodetic longitude of arc start point in degrees.
	   @param arcLat2 Geodetic latitude of arc end point in degrees.
	   @param arcLon3 Geodetic longitude of arc end point in degrees.
	   @returns Great circle distance in arc degrees.
	*/
        static T calculateDistanceFromPointToGreatCircleInDeg(
            const T &pointLatC,
            const T &pointLonC,
            const T &arcLatA,
            const T &arcLonA,
            const T &arcLatB,
            const T &arcLonB
            );

	/**
	   @brief Project point C onto great circle defined by points A,B.
	   @param pointC Point on unit sphere surface to project.
	   @param pointA First point on unit sphere surface defining
	   great circle.
	   @param pointB Second point on unit sphere surface defining
	   great circle.
	   @returns Projection of point C onto great circle AB.
	*/
	static Vector3<T> projectPointToGreatCircleOnUnitSphere(
	    const Vector3<T> &pointC,
	    const Vector3<T> &pointA,
	    const Vector3<T> &pointB
	    );

	/**
	   @brief Calculate great-circle bearing from point 1 to point 2.
	   @param lat1 Geodetic latitude of point in radians.
	   @param lon1 Geodetic longitude of point in radians.
	   @param lat2 Geodetic latitude of point in radians.
	   @param lon2 Geodetic longitude of point in radians.
	   @returns Great circle distance in arc radians.
	*/
	static T calculateBearingInRadians( const T &lat1,
					    const T &lon1,
					    const T &lat2,
					    const T &lon2
	    );

	/**
	   @brief Calculate great-circle bearing from point 1 to point 2.
	   @param lat1 Geodetic latitude of point in degrees.
	   @param lon1 Geodetic longitude of point in degrees.
	   @param lat2 Geodetic latitude of point in degrees.
	   @param lon2 Geodetic longitude of point in degrees.
	   @returns Great circle distance in arc degrees.
	*/
	static T calculateBearingInDegrees( const T &lat1,
					    const T &lon1,
					    const T &lat2,
					    const T &lon2
	    );

    /**
       @brief Calculate new point given start point, bearing and distance traveled.
       @brief Uses a spherical representation of the earth with radius WGS84_MEAN_RADIUS_OF_SEMI_AXES.
       @param pointLat Geodetic latitude of point in radians.
       @param pointLon Geodetic longitude of point in radians.
       @param bearing Great Circle bearing in radians.
       @param distanceMeters distance traveled in meters.
       @param outPointLat Geodetic latitude of new point in radians.
       @param lon2 Geodetic longitude of new point in radians.
       @returns void
    */
    static void calculatePointFromPointBearingDistanceInRadians (
        const T &pointLat,
        const T &pointLon,
        const T &bearing,
        const T &distanceMeters,
        T &outPointLat,
        T &outPointLon
        );

    /**
       @brief Calculate new point given start point, bearing and distance traveled.
       @brief Uses a spherical representation of the earth with radius WGS84_MEAN_RADIUS_OF_SEMI_AXES.
       @param pointLat Geodetic latitude of point in degrees.
       @param pointLon Geodetic longitude of point in degrees.
       @param bearing Great Circle bearing in degrees.
       @param distanceMeters distance traveled in meters.
       @param outPointLat Geodetic latitude of new point in degrees.
       @param lon2 Geodetic longitude of new point in degrees.
       @returns void
    */
    static void calculatePointFromPointBearingDistanceInDegrees (
        const T &pointLat,
        const T &pointLon,
        const T &bearing,
        const T &distanceMeters,
        T &outPointLat,
        T &outPointLon
        );

    /**
        @brief Calculate the point between 2 points where an entity should be if it is percentDone moving between the 2 points.
        @param lat1 Geodetic latitude of point1 in degrees.
        @param lon1 Geodetic longitude of point1 in degrees.
        @param lat2 Geodetic latitude of point2 in degrees.
        @param lon2 Geodetic longitude of point2 in degrees.
        @param percentDone Number between 0.0 and 1.0 indicating how far along the entity is between point1 and point2.
        @param outLat Geodetic latitude of output point in degrees.
        @param outLat Geodetic longitude of output point in degrees.
        @param outBearing Bearing between point1 and point2 in degrees.
    */
    static void calculatePointFromPointsPercentDoneInDegrees (
        const T &lat1,
        const T &lon1,
        const T &lat2,
        const T &lon2,
        const T &percentDone,
        T &outLat,
        T &outLon,
        T &outBearing
        );

    protected:

    private:

    };

    template<typename T>
    T GreatCircle<T>::calculateDeltaLongitudeInRad(
	const T &lon1,
	const T &lon2
	)
    {
	T dlon = lon2 - lon1;

	if( dlon < -vm::PI )
	{
	    dlon += ( vm::PI * 2.0 );
	}
	else if ( dlon > vm::PI )
	{
	    dlon -= ( vm::PI * 2.0 );
	}

	return dlon;
    }


    template<typename T>
    T GreatCircle<T>::calculateDistanceInRad(
	const T &lat1,
	const T &lon1,
	const T &lat2,
	const T &lon2
	)
    {
	T dlon = calculateDeltaLongitudeInRad( lon1, lon2 );

	// American Practical Navigator, Vol I - Bowditch
	// Algorithm from page 1258
        // Page 332 in pdf
	T distance = acos( ( sin( lat1 ) * sin( lat2 ) )
			   + (cos( lat1 ) * cos( lat2 ) * cos( dlon ) )
	    );

	return( distance );
    }

    template<typename T>
    T GreatCircle<T>::calculateDistanceInDeg(
	const T &lat1,
	const T &lon1,
	const T &lat2,
	const T &lon2
	)
    {
	T distance = calculateDistanceInRad( lat1 * vm::DEGREES_TO_RADIANS,
					     lon1 * vm::DEGREES_TO_RADIANS,
					     lat2 * vm::DEGREES_TO_RADIANS,
					     lon2 * vm::DEGREES_TO_RADIANS
	    );
	distance *= vm::RADIANS_TO_DEGREES;

	return( distance );
    }

    template<typename T>
    T GreatCircle<T>::calculateDistanceInNM(
	const T &lat1,
	const T &lon1,
	const T &lat2,
	const T &lon2
	)
    {
	T distance = calculateDistanceInRad( lat1 * vm::DEGREES_TO_RADIANS,
					     lon1 * vm::DEGREES_TO_RADIANS,
					     lat2 * vm::DEGREES_TO_RADIANS,
					     lon2 * vm::DEGREES_TO_RADIANS
	    );
	distance *= vm::RADIANS_TO_DEGREES * vm::ARCDEGREES_TO_NM;

	return( distance );
    }

    template<typename T>
    T GreatCircle<T>::calculateDistanceInMeters(
	const T &lat1,
	const T &lon1,
	const T &lat2,
	const T &lon2
	)
    {
	T distance = calculateDistanceInRad( lat1 * vm::DEGREES_TO_RADIANS,
					     lon1 * vm::DEGREES_TO_RADIANS,
					     lat2 * vm::DEGREES_TO_RADIANS,
					     lon2 * vm::DEGREES_TO_RADIANS
	    );
	distance *= vm::RADIANS_TO_DEGREES * vm::ARCDEGREES_TO_METERS;

	return( distance );
    }


    template<typename T>
    T GreatCircle<T>::calculateDistanceFromPointToArcInDeg(
	const T &pointLatC,
	const T &pointLonC,
	const T &arcLatA,
	const T &arcLonA,
	const T &arcLatB,
	const T &arcLonB
	)
    {
	return( calculateDistanceFromPointToArcInRad(
		    pointLatC * vm::DEGREES_TO_RADIANS,
		    pointLonC * vm::DEGREES_TO_RADIANS,
		    arcLatA * vm::DEGREES_TO_RADIANS,
		    arcLonA * vm::DEGREES_TO_RADIANS,
		    arcLatB * vm::DEGREES_TO_RADIANS,
		    arcLonB * vm::DEGREES_TO_RADIANS
		    ) * vm::RADIANS_TO_DEGREES );
    }

    template<typename T>
    T GreatCircle<T>::calculateDistanceFromPointToGreatCircleInDeg(
	const T &pointLatC,
	const T &pointLonC,
	const T &arcLatA,
	const T &arcLonA,
	const T &arcLatB,
	const T &arcLonB
	)
    {
	return( calculateDistanceFromPointToGreatCircleInRad(
		    pointLatC * vm::DEGREES_TO_RADIANS,
		    pointLonC * vm::DEGREES_TO_RADIANS,
		    arcLatA * vm::DEGREES_TO_RADIANS,
		    arcLonA * vm::DEGREES_TO_RADIANS,
		    arcLatB * vm::DEGREES_TO_RADIANS,
		    arcLonB * vm::DEGREES_TO_RADIANS
		    ) * vm::RADIANS_TO_DEGREES );
    }

    template<typename T>
    Vector3<T> GreatCircle<T>::projectPointToGreatCircleOnUnitSphere(
	const Vector3<T> &pointC,
	const Vector3<T> &pointA,
	const Vector3<T> &pointB
	)
    {
#if 0
	std::cout << "Vectors from sphere center to Points A,B,C" << std::endl;
	std::cout << "Vector A: " << std::fixed << pointA << std::endl;
	std::cout << "Vector B: " << std::fixed << pointB << std::endl;
	std::cout << "Vector C: " << std::fixed << pointC << std::endl;
#endif

	// Cross AxB to generate normal to the plane containing great circle AB.
	Vector3<T> N( pointA.cross( pointB ) );

	// N is 0 if points A and B are the same.
	if( 0.0 == N.normSquared() )
	{
	    // If arc is a point, then projected point is A and/or B.
	    return pointA;
	}

	try{
	    N.normalize();
	}
	catch(...)
	{
	    throw;
	}

	// Cross NxC to generate vector perpendicular to both N and C.
	Vector3<T> N2( N.cross( pointC ) );
	try{
	    N2.normalize();
	}
	catch(...)
	{
	    throw;
	}

	// (N2xC)xN generates original vector C, projected onto great circle AB.
	Vector3<T> N3( N2.cross( N ) );
	try{
	    N3.normalize();
	}
	catch(...)
	{
	    throw;
	}

#if 0
	std::cout << "Vector AxB: " << std::fixed << N << std::endl;
	std::cout << "Vector (AxB)xC: " << std::fixed << N2 << std::endl;
	std::cout << "Vector ((AxB)xC)x(AxB): " << std::fixed << N3
		  << std::endl;
	std::cout << std::endl;
#endif
	return N3;
    }

    template<typename T>
    T GreatCircle<T>::calculateDistanceFromPointToArcInRad(
	const T &pointLatC,
	const T &pointLonC,
	const T &arcLatA,
	const T &arcLonA,
	const T &arcLatB,
	const T &arcLonB
	)
    {
	// Vector from center of sphere to point C.
	Vector3<T> C;
	geodeticToECEF( pointLatC, pointLonC, 0.0, C[0], C[1], C[2] );
	C.normalize();

	// Vector from center of earth to point A.
	Vector3<T> A;
	geodeticToECEF( arcLatA, arcLonA, 0.0, A[0], A[1], A[2] );
	A.normalize();

	// Vector from center of earth to point B.
	Vector3<T> B;
	geodeticToECEF( arcLatB, arcLonB, 0.0, B[0], B[1], B[2] );
	B.normalize();

	Vector3<T> Cproj( projectPointToGreatCircleOnUnitSphere( C, A, B ) );

	T angleAC = acos( A.dot( Cproj ) );
	T angleBC = acos( B.dot( Cproj ) );
	T angleAB = acos( A.dot( B ) );

#if 0
	std::cout << "Angle AB: " << (angleAB * vm::RADIANS_TO_DEGREES)
		  << std::endl;
	std::cout << "Angle AC: " << (angleAC * vm::RADIANS_TO_DEGREES)
		  << std::endl;
	std::cout << "Angle BC: " << (angleBC * vm::RADIANS_TO_DEGREES)
		  << std::endl;
	std::cout << std::endl;
#endif
	if( angleAC < angleAB && angleBC < angleAB )
	{
	    //std::cout << "Point is closer to arc AB" << std::endl;
	    T dotProduct( std::max( -1.0, std::min( 1.0, C.dot( Cproj ) ) ) );
	    return( acos( dotProduct ) );
	}
	else if( angleAC < angleBC )
	{
	    //std::cout << "Point is closer to A." << std::endl;
	    return calculateDistanceInRad( arcLatA, arcLonA,
					   pointLatC, pointLonC );
	}
	else
	{
	    //std::cout << "Point is closer to B." << std::endl;
	    return calculateDistanceInRad( arcLatB, arcLonB,
					   pointLatC, pointLonC );
	}
    }

    template<typename T>
    T GreatCircle<T>::calculateDistanceFromPointToGreatCircleInRad(
	const T &pointLatC,
	const T &pointLonC,
	const T &arcLatA,
	const T &arcLonA,
	const T &arcLatB,
	const T &arcLonB
	)
    {
	// Vector from center of sphere to point C.
	Vector3<T> C;
	geodeticToECEF( pointLatC, pointLonC, 0.0, C[0], C[1], C[2] );
	C.normalize();

	// Vector from center of earth to point A.
	Vector3<T> A;
	geodeticToECEF( arcLatA, arcLonA, 0.0, A[0], A[1], A[2] );
	A.normalize();

	// Vector from center of earth to point B.
	Vector3<T> B;
	geodeticToECEF( arcLatB, arcLonB, 0.0, B[0], B[1], B[2] );
	B.normalize();

	Vector3<T> Cproj( projectPointToGreatCircleOnUnitSphere( C, A, B ) );

        T dotProduct( std::max( -1.0, std::min( 1.0, C.dot( Cproj ) ) ) );
        return( acos( dotProduct ) );
    }

    template<typename T>
    T GreatCircle<T>::calculateBearingInRadians(
	const T &lat1,
	const T &lon1,
	const T &lat2,
	const T &lon2
	)
    {
	T dlon = calculateDeltaLongitudeInRad( lon1, lon2 );

	// American Practical Navigator, Vol I - Bowditch
	// Algorithm from page 1258
	T course = atan2( sin( dlon ),
			  ( cos( lat1 ) * tan( lat2 ) )
			  - ( sin( lat1 ) * cos( dlon ) )
	    );

	return course;
    }

    template<typename T>
    T GreatCircle<T>::calculateBearingInDegrees(
	const T &lat1,
	const T &lon1,
	const T &lat2,
	const T &lon2
	)
    {
	T course = calculateBearingInRadians(
	    lat1 * vm::DEGREES_TO_RADIANS,
	    lon1 * vm::DEGREES_TO_RADIANS,
	    lat2 * vm::DEGREES_TO_RADIANS,
	    lon2 * vm::DEGREES_TO_RADIANS
	    );
	course *= vm::RADIANS_TO_DEGREES;

	return course;
    }

    template<typename T>
    void GreatCircle<T>::calculatePointFromPointBearingDistanceInRadians (
        const T &pointLat,
        const T &pointLon,
        const T &bearing,
        const T &distanceMeters,
        T &outPointLat,
        T &outPointLon
        )
    {
        T angularDistance = distanceMeters / WGS84_MEAN_RADIUS_OF_SEMI_AXES;
        outPointLat = asin ( sin (pointLat) * cos (angularDistance) +
            cos (pointLat) * sin (angularDistance) * cos (bearing) );
        outPointLon = pointLon + atan2 (sin (bearing) * sin (angularDistance) * cos (pointLat),
            cos (angularDistance) - sin (pointLat) * sin (outPointLat));
    }

    template<typename T>
    void GreatCircle<T>::calculatePointFromPointBearingDistanceInDegrees (
        const T &pointLat,
        const T &pointLon,
        const T &bearing,
        const T &distanceMeters,
        T &outPointLat,
        T &outPointLon
        )
    {
        calculatePointFromPointBearingDistanceInRadians (pointLat * vm::DEGREES_TO_RADIANS,
            pointLon * vm::DEGREES_TO_RADIANS,
            bearing * vm::DEGREES_TO_RADIANS,
            distanceMeters,
            outPointLat, outPointLon);
        outPointLat = outPointLat * vm::RADIANS_TO_DEGREES;
        outPointLon = outPointLon * vm::RADIANS_TO_DEGREES;
    }

    template<typename T>
    void GreatCircle<T>::calculatePointFromPointsPercentDoneInDegrees (
        const T &lat1,
        const T &lon1,
        const T &lat2,
        const T &lon2,
        const T &percentDone,
        T &outLat,
        T &outLon,
        T &outBearing
        )
    {
        T totalDistanceMeters = calculateDistanceInMeters (lat1, lon1, lat2, lon2);
        T distanceMeters = totalDistanceMeters * percentDone;
        outBearing = calculateBearingInDegrees (lat1, lon1, lat2, lon2);
        calculatePointFromPointBearingDistanceInDegrees (lat1, lon1, outBearing, distanceMeters, outLat, outLon);
    }


}

#endif
