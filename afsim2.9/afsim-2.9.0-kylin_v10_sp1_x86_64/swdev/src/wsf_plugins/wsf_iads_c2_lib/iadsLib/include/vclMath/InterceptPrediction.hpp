// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file InterceptPrediction.hpp
 *  @author Ken Sewell AFRL/RYZW \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 (Sewell)
 *  @brief Functions for intercept calculations.
 *  Classification: UNCLASSIFIED
 */

#include <cmath>
#include <vclMath/Vector3.hpp>

#ifndef VCLMATH_INTERCEPTPREDICTION_HPP
#define VCLMATH_INTERCEPTPREDICTION_HPP 1

namespace vm
{
    /**
     * @brief Calculates predicted intercept point.
     * @param targetPosition Current position of target.
     * @param targetVelocity Current velocity vector of target.
     * @param shooterPosition Current position of shooter.
     * @param averageVelocity Average velocity for projectile launched.
     * @param interceptTime Used to return time of first intercept.
     * @param interceptPosition Used to return position of first intercept.
     * @param epsilon Value which less than is considered zero.
     * @returns true if intercept is possible, false otherwise.
     */
    bool predictIntercept( const vm::Vector3<double> &targetPosition,
			   const vm::Vector3<double> &targetVelocity,
			   const vm::Vector3<double> &shooterPosition,
			   const double &averageVelocity,
			   double &interceptTime,
			   vm::Vector3<double> &interceptPosition,
			   const double &epsilon = 0.00001 )
    {
	vm::Vector3<double> siteToTarget( targetPosition - shooterPosition );

	// Solve the following quadratic equation for time to intercept:
	// (targetSpeed**2 - avgvel**2)*ti**2 + 2.0*rdv*ti + distanceSquared = 0
	double a = targetVelocity.normSquared()
	    - averageVelocity * averageVelocity;

	// Calculate dot product of target range vector and velocity
	// vector (m**2/s)
	double b = 2.0 * siteToTarget.dot( targetVelocity );

	// Calculate square of target range to site (m**2)
	double c = siteToTarget.normSquared();

	if( fabs( a ) > epsilon )
	{
	    // Calculate discriminant.
	    double urad = b*b - 4.0 * a * c;
	    if( urad < 0.0 )
	    {
		// Discriminant is less than zero, no real roots.
		return false;
	    }
	    else
	    {
		double srad = std::sqrt( urad );
		double s1 = ( -b + srad ) / ( 2.0 * a );
		double s2 = ( -b - srad ) / ( 2.0 * a );
		if( (s1 > 0.0) && (s2 > 0.0) )
		{
		    interceptTime = std::min( s1, s2 );
		}
		else if( s1 > 0.0 )
		{
		    interceptTime = s1;
		}
		else if( s2 > 0.0 )
		{
		    interceptTime = s2;
		}
		else
		{
		    return false;
		}
	    }
	}
	else
	{
	    if( fabs( b ) > epsilon )
	    {
		interceptTime = -c/b;
	    }
	    else
	    {
		return false;
	    }

	    if( interceptTime < 0.0 )
	    {
		return false;
	    }
	}

	interceptPosition = targetPosition + targetVelocity * interceptTime;

	return true;
    }
}

#endif
