// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file Geometry.hpp
 *  @author Ken Sewell AFRL/RYZW \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 (Sewell)
 *  @brief Template for basic geometric functions.
 *  Classification: UNCLASSIFIED
 */

#include <cmath>
#include <vector>
#include <vclMath/Vector3.hpp>

#ifndef VCLMATH_GEOMETRY_HPP
#define VCLMATH_GEOMETRY_HPP 1

namespace vm
{
    /**
     * @brief Calculates the projection of point P onto line AB.
     * @param P Point to project.
     * @param A First point defining line.
     * @param B Second point defining line.
     * @returns Point P projected onto line AB.
     */
    template<typename T>
    Vector3<T> projectPointToLine( const Vector3<T> &P,
               const Vector3<T> &A,
               const Vector3<T> &B )
    {
   Vector3<T> AP( P - A );
   Vector3<T> AB( B - A );
   T r = AP.dot( AB ) / std::pow( AB.norm(), 2.0 );

   AB *= r;
   AB += A;

   return AB;
    }

    /**
     * @brief Calculates the projection of point P onto line segment AB.
     * @param P Point to project.
     * @param A First point defining line.
     * @param B Second point defining line.
     * @returns Point P projected onto line segment AB.
     */
    template<typename T>
    Vector3<T> projectPointToLineSegment( const Vector3<T> &P,
                 const Vector3<T> &A,
                 const Vector3<T> &B )
    {
   Vector3<T> AP( P - A );
   Vector3<T> AB( B - A );
   T r = AP.dot( AB ) / std::pow( AB.norm(), 2.0 );

   if( 0.0 >= r )
   {
       return A;
   }
   else if( 1.0 <= r )
   {
       return B;
   }

   AB *= r;
   AB += A;

   return AB;
    }

    /**
     * @brief Calculates the distance squared from point to line.
     * @param P Point to calculate distance from.
     * @param L1 First point defining line.
     * @param L2 Second point defining line.
     * @returns Distance squared from Point P to Line L1L2.
     */
    template<typename T>
    T distanceFromPointToLineSquared( const Vector3<T> &P,
                  const Vector3<T> &L1,
                  const Vector3<T> &L2
   )
    {
   Vector3<T> QS( P - L1 );
   Vector3<T> SV( L2 - L1 );

   double distance = std::pow( QS.norm(), 2.0 );
   distance -= std::pow( QS.dot( SV ), 2.0 )/std::pow( SV.norm(), 2.0 );

   return distance;
    }

    /**
     * @brief Calculates the distance squared from point to line.
     * @param Px X coord of point to calculate distance from.
     * @param Py Y coord of point to calculate distance from.
     * @param Pz Z coord of point to calculate distance from.
     * @param Lx1 X coord of first point defining line.
     * @param Ly1 Y coord of first point defining line.
     * @param Lz1 Z coord of first point defining line.
     * @param Lx2 X coord of second point defining line.
     * @param Ly2 Y coord of second point defining line.
     * @param Lz2 Z coord of second point defining line.
     * @returns Distance squared from Point P to Line L1L2.
     */
    template<typename T>
    T distanceFromPointToLineSquared( const T &Px,
                  const T &Py,
                  const T &Pz,
                  const T &Lx1,
                  const T &Ly1,
                  const T &Lz1,
                  const T &Lx2,
                  const T &Ly2,
                  const T &Lz2
   )
    {
   Vector3<T> QS( Px - Lx1,  Py - Ly1, Pz - Lz1 );
   Vector3<T> SV( Lx2 - Lx1, Ly2 - Ly1, Lz2 - Lz1 );

   double distance = std::pow( QS.norm(), 2.0 );
   distance -= std::pow( QS.dot( SV ), 2.0 )/std::pow( SV.norm(), 2.0 );

   return distance;
    }

    /**
     * @brief Calculates the distance from point to line.
     * @param P Point to calculate distance from.
     * @param L1 First point defining line.
     * @param L2 Second point defining line.
     * @returns Distance from Point P to Line L1L2.
     */
    template<typename T>
    T distanceFromPointToLine( const Vector3<T> &P,
                const Vector3<T> &L1,
                const Vector3<T> &L2
   )
    {
   Vector3<T> QS( P - L1 );
   Vector3<T> SV( L2 - L1 );

   double distance = std::pow( QS.norm(), 2.0 );
   distance -= std::pow( QS.dot( SV ), 2.0 )/std::pow( SV.norm(), 2.0 );

   return std::sqrt( distance );
    }

    /**
     * @brief Calculates the distance from point to line.
     * @param Px X coord of point to calculate distance from.
     * @param Py Y coord of point to calculate distance from.
     * @param Pz Z coord of point to calculate distance from.
     * @param Lx1 X coord of first point defining line.
     * @param Ly1 Y coord of first point defining line.
     * @param Lz1 Z coord of first point defining line.
     * @param Lx2 X coord of second point defining line.
     * @param Ly2 Y coord of second point defining line.
     * @param Lz2 Z coord of second point defining line.
     * @returns Distance from Point P to Line L1L2.
     */
    template<typename T>
    T distanceFromPointToLine( const T &Px,
                const T &Py,
                const T &Pz,
                const T &Lx1,
                const T &Ly1,
                const T &Lz1,
                const T &Lx2,
                const T &Ly2,
                const T &Lz2
   )
    {
   Vector3<T> QS( Px - Lx1,  Py - Ly1, Pz - Lz1 );
   Vector3<T> SV( Lx2 - Lx1, Ly2 - Ly1, Lz2 - Lz1 );

   double distance = std::pow( QS.norm(), 2.0 );
   distance -= std::pow( QS.dot( SV ), 2.0 )/std::pow( SV.norm(), 2.0 );

   return std::sqrt( distance );
    }

    template<typename T>
    bool doesLineIntersectSphere( const Vector3<T> &L1,
              const Vector3<T> &L2,
              const Vector3<T> &C,
              const double &R )
    {
   /*
     True if distance from sphere center squared is less than
     or equal to the radius of the sphere squared.
   */
   return ( distanceFromPointToLineSquared( C, L1, L2 ) <= (R*R) );
    }

    template<typename T>
    bool doesLineIntersectSphere( const T &Lx1,
              const T &Ly1,
              const T &Lz1,
              const T &Lx2,
              const T &Ly2,
              const T &Lz2,
              const T &Cx,
              const T &Cy,
              const T &Cz,
              const double &R )
    {
   /*
     True if distance from sphere center squared is less than
     or equal to the radius of the sphere squared.
   */
   return ( distanceFromPointToLineSquared(
           Cx, Cy, Cz,
           Lx1, Ly1, Lz1,
           Lx2, Ly2, Lz2 ) <= ( R * R ) );
    }

    /**
     * @brief Returns true if point is inside polygon.
     * @param x X coord of point to test.
     * @param y Y coord of point to test.
     * @param vertX X coords of the polygon vertices.
     * @param vertY Y coords of the polygon vertices.
     * @returns True if point is inside polygon.
     */
    template<typename T>
    bool isPointInPolygon( const T &x,
            const T &y,
            const std::vector< T > &vertX,
            const std::vector< T > &vertY )
    {
   /*
     True if distance from sphere center squared is less than
     or equal to the radius of the sphere squared.
   */
   if( ( 0 == vertX.size() ) || ( vertX.size() != vertY.size() ) )
   {
       return false;
   }

   size_t nvert = vertX.size();
   bool isIn = false;
   for( size_t i = 0, j = nvert-1; i < nvert;  j = i++ )
   {
       if( (( vertY[i] > y ) != ( vertY[j] > y ) )
      && ( x < ( vertX[j]-vertX[i]) * (y-vertY[i]) / (vertY[j] - vertY[i]) + vertX[i]) )
       {
      isIn = !isIn;
       }
   }

   return isIn;

    }

    /**
     * @brief Returns true if two 2D lines intersect.
     * @param X1 X coord of line 1 point 1.
     * @param Y1 Y coord of line 1 point 1.
     * @param X2 X coord of line 1 point 2.
     * @param Y2 Y coord of line 1 point 2.
     * @param X3 X coord of line 2 point 3.
     * @param Y3 Y coord of line 2 point 3.
     * @param X4 X coord of line 2 point 4.
     * @param Y4 Y coord of line 2 point 4.
     * @param Xi X coords of the line intersection.
     * @param Yi Y coords of the line intersection.
     * @returns True if lines intersect.
     */
    template<typename T>
    bool intersectLines( const T &X1,
          const T &Y1,
          const T &X2,
          const T &Y2,
          const T &X3,
          const T &Y3,
          const T &X4,
          const T &Y4,
          T &Xi,
          T &Yi)
    {

   T d = (Y4 - Y3) * (X2 - X1) - (X4 - X3) * (Y2-Y1);
   // If d is 0, then lines are parallel.
   if( std::fabs( d ) < 0.000001 )
   {
       return false;
   }

   T ua = ((X4 - X3) * (Y1 - Y3) - (Y4 - Y3) * (X1 - X3)) / d;

   Xi = X1 + ua * ( X2 - X1 );
   Yi = Y1 + ua * ( Y2 - Y1 );

   return true;
    }

    /**
     * @brief Returns true if two 2D line segments intersect.
     * @param X1 X coord of line segment 1 point 1.
     * @param Y1 Y coord of line segment 1 point 1.
     * @param X2 X coord of line segment 1 point 2.
     * @param Y2 Y coord of line segment 1 point 2.
     * @param X3 X coord of line segment 2 point 3.
     * @param Y3 Y coord of line segment 2 point 3.
     * @param X4 X coord of line segment 2 point 4.
     * @param Y4 Y coord of line segment 2 point 4.
     * @param Xi X coords of the line intersection.
     * @param Yi Y coords of the line intersection.
     * @returns True if lines intersect.
     */
    template<typename T>
    bool intersectLineSegments( const T &X1,
            const T &Y1,
            const T &X2,
            const T &Y2,
            const T &X3,
            const T &Y3,
            const T &X4,
            const T &Y4,
            T &Xi,
            T &Yi)
    {

   T d = (Y4 - Y3) * (X2 - X1) - (X4 - X3) * (Y2-Y1);

   // If d is 0, then lines are parallel.
   if( std::fabs( d ) < 0.000001 )
   {
       return false;
   }

   T ua = ((X4 - X3) * (Y1 - Y3) - (Y4 - Y3) * (X1 - X3)) / d;
   T ub = ((X2 - X1) * (Y1 - Y3) - (Y2 - Y1) * (X1 - X3)) / d;

   // ua and ub both must be between 0 and 1
   if( ( ua <= 1.0 ) && ( ua >= 0.0 )
       && ( ub <= 1.0 ) && ( ub >= 0.0 ) )
   {
       Xi = X1 + ua * ( X2 - X1 );
       Yi = Y1 + ua * ( Y2 - Y1 );
       return true;
   }
   else
   {
       return false;
   }
    }

    /**
     * @brief Returns true if a 2D line intersects with a 2D line segment.
     * @param X1 X coord of line 1 point 1.
     * @param Y1 Y coord of line 1 point 1.
     * @param X2 X coord of line 1 point 2.
     * @param Y2 Y coord of line 1 point 2.
     * @param X3 X coord of line segment 2 point 3.
     * @param Y3 Y coord of line segment 2 point 3.
     * @param X4 X coord of line segment 2 point 4.
     * @param Y4 Y coord of line segment 2 point 4.
     * @param Xi X coords of the line-segment intersection.
     * @param Yi Y coords of the line-segment intersection.
     * @returns True if line intersects segment intersect.
     */
    template<typename T>
    bool intersectLineWithSegment( const T &X1,
               const T &Y1,
               const T &X2,
               const T &Y2,
               const T &X3,
               const T &Y3,
               const T &X4,
               const T &Y4,
               T &Xi,
               T &Yi)
    {

   T d = (Y4 - Y3) * (X2 - X1) - (X4 - X3) * (Y2-Y1);

   // If d is 0, then lines are parallel.
   if( std::fabs( d ) < 0.000001 )
   {
       return false;
   }

   T ub = ((X2 - X1) * (Y1 - Y3) - (Y2 - Y1) * (X1 - X3)) / d;

   // ub must be between 0 and 1 for it to contain the intersection.
   if( ( ub <= 1.0 ) && ( ub >= 0.0 ) )
   {
       Xi = X3 + ub * ( X4 - X3 );
       Yi = Y3 + ub * ( Y4 - Y3 );
       return true;
   }
   else
   {
       return false;
   }
    }


}

#endif
