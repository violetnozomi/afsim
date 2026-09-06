// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file RotationMatrixYPR.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *  @brief Three Dimensional Rotation Matrix Class\n
 **  Classification: UNCLASSIFIED
 */

 #ifndef VCLMATH_ROTMATRIXYPR_HPP
#define VCLMATH_ROTMATRIXYPR_HPP 1

//#define VCLMATH_ROTMATRIX_DEBUG 1

#include <iostream>
#include <cmath>
#include <vclMath/Constants.hpp>
#include <vclMath/Matrix.hpp>

namespace vm
{
	/**
	*  \brief Three Dimensional Euler Rotation Matrix Class
	 */
	template < typename T > class RotationMatrixYPR : protected Matrix<T>
	{
		private :
			T m_angleInRadians;
		protected :

		public :
			RotationMatrixYPR();
			RotationMatrixYPR ( double* yaw, double* pitch, double* roll );
			void updateRotation ( double* yaw, double* pitch, double* roll );
			Matrix<T>::display;
			Matrix<T>::transpose;
			Matrix<T>::inverse;
			Matrix<T>::operator*;
			Matrix<T>::multVector3x3;
			Matrix<T>::multMatrix3x3;
	};

	template < typename T >
	RotationMatrixYPR<T>::RotationMatrixYPR()
	{
		this->resize ( 3,3 );		// will always be a 3x3
		this->makeIdentity();		// start off with no rotation
	}

	template < typename T >
	RotationMatrixYPR<T>::RotationMatrixYPR ( double* yaw, double* pitch, double* roll )
	{
		this->resize ( 3,3 );		// will always be a 3x3
		this->updateRotation ( yaw,pitch,roll );
	}

	template < typename T >
	void RotationMatrixYPR<T>::updateRotation( double* yaw, double* pitch, double* roll )
	{
		double cp, cy, cr, sp, sy, sr;
		cp = cos ( *pitch );
		cy = cos ( *yaw );
		cr = cos ( *roll );
		sp = sin ( *pitch );
		sy = sin ( *yaw );
		sr = sin ( *roll );

		this->m_Vector[0] = cp * cy;
		this->m_Vector[1] = cp * sy;
		this->m_Vector[2] =-sp;
		this->m_Vector[3] = -cr * sy + cy * sp * sr;
		this->m_Vector[4] = cr * cy + sp * sr * sy;
		this->m_Vector[5] = cp * sr;
		this->m_Vector[6] = sr * sy + cr * cy * sp;
		this->m_Vector[7] = cr * sp * sy - cy * sr;
		this->m_Vector[8] = cp * cr;
	}


}
#endif
