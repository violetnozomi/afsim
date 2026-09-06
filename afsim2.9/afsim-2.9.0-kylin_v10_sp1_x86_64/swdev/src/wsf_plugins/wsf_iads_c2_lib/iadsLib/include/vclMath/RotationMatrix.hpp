// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file RotationMatrix.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *  @brief Three Dimensional Rotation Matrix Class\n
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_ROTMATRIX_HPP
#define VCLMATH_ROTMATRIX_HPP 1

//#define VCLMATH_ROTMATRIX_DEBUG 1

#include <iostream>
#include <cmath>
#include <vclMath/Constants.hpp>
#include <vclMath/Matrix.hpp>
#include <vclMath/TransformMatrix4.hpp>

namespace vm
{
		/**
	 *  \brief Three Dimensional Euler Rotation Matrix Class
		 */
	template < typename T > class RotationMatrix : protected Matrix<T>
	{
		private :
			T m_angleInRadians;
		protected :

		public :
			RotationMatrix();
			Matrix<T>::display;
			Matrix<T>::transpose;
			Matrix<T>::inverse;
			Matrix<T>::operator*;
			Matrix<T>::multVector3x3;
			Matrix<T>::multMatrix3x3;

			/*! \brief Overloaded = operator
			 *  \param A Pointer to Matrix to be copied into
			 */
			void operator= ( const RotationMatrix < T > &A );
			void operator= ( const Matrix < T > &A );

			/*!
			 *    \brief Construct a rotation matrix from any rotation sequence (radians)
			 *    \param _firstRotationAxis integer 1, 2, or 3 specifying which axis is the first axis to rotate about
			 *    \param _secondRotationAxis integer 1, 2, or 3 specifying which axis is the second axis to rotate about
			 *    \param _thirdRotationAxis integer 1, 2, or 3 specifying which axis is the third axis to rotate about
			 *    \param _firstRotationAngleRad T rotation angle (rad) following right hand rule for first rotation
			 *    \param _secondRotationAngleRad T rotation angle (rad) following right hand rule for second rotation
			 *    \param _thirdRotationAngleRad T rotation angle (rad) following right hand rule for third rotation
			 */
			void rotateRad (
			    unsigned int _firstRotationAxis, T _firstRotationAngleRad,
			    unsigned int _secondRotationAxis, T _secondRotationAngleRad,
			    unsigned int _thirdRotationAxis, T _thirdRotationAngleRad
			);



			/*!
			 *    \brief Construct a rotation matrix from any rotation sequence (degrees)
			 *    \param _firstRotationAxis integer 1, 2, or 3 specifying which axis is the first axis to rotate about
			 *    \param _secondRotationAxis integer 1, 2, or 3 specifying which axis is the second axis to rotate about
			 *    \param _thirdRotationAxis integer 1, 2, or 3 specifying which axis is the third axis to rotate about
			 *    \param _firstRotationAngleDeg T rotation angle (deg) following right hand rule for first rotation
			 *    \param _secondRotationAngleDeg T rotation angle (deg) following right hand rule for second rotation
			 *    \param _thirdRotationAngleDeg T rotation angle (deg) following right hand rule for third rotation
			 */
			void rotateDeg (
			    unsigned int _firstRotationAxis, T _firstRotationAngleDeg,
			    unsigned int _secondRotationAxis, T _secondRotationAngleDeg,
			    unsigned int _thirdRotationAxis, T _thirdRotationAngleDeg
			);
	};

	template < typename T >
	RotationMatrix<T>::RotationMatrix()
	{
		this->resize ( 3,3 );		// will always be a 3x3
		this->makeIdentity();		// start off with no rotation
	}

	template < typename T >
	void RotationMatrix < T >::operator= ( const RotationMatrix < T > &A )
	{
		this->m_Vector = A.m_Vector;
	}

	template < typename T >
	void RotationMatrix < T >::operator= ( const Matrix < T > &A )
	{
		this->m_Vector[0] = A ( 1,1 );
		this->m_Vector[1] = A ( 1,2 );
		this->m_Vector[2] = A ( 1,3 );
		this->m_Vector[3] = A ( 2,1 );
		this->m_Vector[4] = A ( 2,2 );
		this->m_Vector[5] = A ( 2,3 );
		this->m_Vector[6] = A ( 3,1 );
		this->m_Vector[7] = A ( 3,2 );
		this->m_Vector[8] = A ( 3,3 );
	}

	template < typename T >
	inline void RotationMatrix<T>::rotateRad (
	    unsigned int _firstRotationAxis, T _firstRotationAngleRad,
	    unsigned int _secondRotationAxis, T _secondRotationAngleRad,
	    unsigned int _thirdRotationAxis, T _thirdRotationAngleRad
	)
	{
		/*
			reference: 	Space Vehicle Dynamics & Control
						AIAA Education Series
						Author: Bong Wie, Arizona State University Prof.
		*/
		T _angle[3] = {_firstRotationAngleRad,_secondRotationAngleRad,_thirdRotationAngleRad};

		unsigned int _axis[3] = {_firstRotationAxis,_secondRotationAxis,_thirdRotationAxis};

		vm::RotationMatrix<T> _R, _C;

		_R.m_Vector[0] = 1.0;
		_R.m_Vector[4] = 1.0;
		_R.m_Vector[8] = 1.0;

		// for each rotation
		for ( int k = 2; k > -1; --k )
		{
			//determine which axis the rotation is about (1,2,3) (x,y,z)
			switch ( _axis[k] )
			{
				case 1:
					_C.makeIdentity();
					_C.m_Vector[4] = std::cos ( _angle[k] );
					_C.m_Vector[5] = std::sin ( _angle[k] );
					_C.m_Vector[7] =-std::sin ( _angle[k] );
					_C.m_Vector[8] = std::cos ( _angle[k] );
#ifdef VCLMATH_ROTMATRIX_DEBUG
					std::cout << "Using angle " << _angle[k];
					_C.display ( "Rotation about axis 1" );
#endif
					break;
				case 2:
					_C.makeIdentity();
					_C.m_Vector[0] = std::cos ( _angle[k] );
					_C.m_Vector[2] =-std::sin ( _angle[k] );
					_C.m_Vector[6] = std::sin ( _angle[k] );
					_C.m_Vector[8] = std::cos ( _angle[k] );
#ifdef VCLMATH_ROTMATRIX_DEBUG
					std::cout << "Using angle " << _angle[k];
					_C.display ( "Rotation about axis 2" );
#endif
					break;
				case 3:
					_C.makeIdentity();
					_C.m_Vector[0] = std::cos ( _angle[k] );
					_C.m_Vector[1] = std::sin ( _angle[k] );
					_C.m_Vector[3] =-std::sin ( _angle[k] );
					_C.m_Vector[4] = std::cos ( _angle[k] );
#ifdef VCLMATH_ROTMATRIX_DEBUG
					std::cout << "Using angle " << _angle[k];
					_C.display ( "Rotation about axis 3" );
#endif
					break;
				default:
					std::cout << "vm::RotationMatrix Error, axis of rotation must be 1, 2, or 3!\n";
					exit ( -1 );
			}
			_R = _R.multMatrix3x3 ( _C );
		}

		this->m_Vector[0] = _R.m_Vector[0];
		this->m_Vector[1] = _R.m_Vector[1];
		this->m_Vector[2] = _R.m_Vector[2];
		this->m_Vector[3] = _R.m_Vector[3];
		this->m_Vector[4] = _R.m_Vector[4];
		this->m_Vector[5] = _R.m_Vector[5];
		this->m_Vector[6] = _R.m_Vector[6];
		this->m_Vector[7] = _R.m_Vector[7];
		this->m_Vector[8] = _R.m_Vector[8];
	}

	template < typename T >
	inline void RotationMatrix<T>::rotateDeg (
	    unsigned int _firstRotationAxis, T _firstRotationAngleDeg,
	    unsigned int _secondRotationAxis, T _secondRotationAngleDeg,
	    unsigned int _thirdRotationAxis, T _thirdRotationAngleDeg
	)
	{
		this->rotateRad (
		    _firstRotationAxis, _firstRotationAngleDeg * vm::DEGREES_TO_RADIANS,
		    _secondRotationAxis, _secondRotationAngleDeg * vm::DEGREES_TO_RADIANS,
		    _thirdRotationAxis, _thirdRotationAngleDeg * vm::DEGREES_TO_RADIANS );
	}

}
#endif
