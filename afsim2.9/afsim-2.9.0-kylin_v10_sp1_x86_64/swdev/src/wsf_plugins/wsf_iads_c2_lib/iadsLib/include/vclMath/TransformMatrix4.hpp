// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file TransformMatrix4.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *  @brief Three Dimensional Rotation Matrix Class\n
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_TRANSFORMMATRIX4_HPP
#define VCLMATH_TRANSFORMMATRIX4_HPP 1

//#define VCLMATH_TRANSFORMMATRIX4_DEBUG 1

#include <iostream>
#include <cmath>
#include <vclMath/Constants.hpp>
#include <vclMath/Matrix.hpp>
#include <vclMath/RotationMatrix.hpp>



/*
	1,1		1,2		1,3		1,4		0	1	2	3
	2,1		2,2		2,3		2,4		4	5	6	7
	3,1		3,2		3,3		3,4		8	9	10	11
	4,1		4,2		4,3		4,4		12	13	14	15

*/

namespace vm
{
			/**
	 *  \brief Four Dimensional Computer Graphics Matrix Class
			 */
	template < typename T > class TransformMatrix4 : protected Matrix<T>
	{
		private :
			T m_angleInRadians;

		protected :

		public :
			TransformMatrix4();
			Matrix<T>::display;
			Matrix<T>::transpose;
			Matrix<T>::inverse;
			Matrix<T>::operator*;
			Matrix<T>::multVector4x4;
			Matrix<T>::multMatrix4x4;

			/*! \brief Overloaded = operator
			 *  \param A Pointer to Matrix to be copied into
			 */
			void operator= ( const TransformMatrix4 < T > &A );
			void operator= ( const Matrix < T > &A );

			/*!
			 *    \brief Construct a rotation matrix from any rotation sequence (radians)
			 *    \param _firstRotationAxis integer 1, 2, or 3 specifying which axis is the first axis to rotate about
			 *    \param _secondRotationAxis integer 1, 2, or 3 specifying which axis is the second axis to rotate about
			 *    \param _thirdRotationAxis integer 1, 2, or 3 specifying which axis is the third axis to rotate about
			 *    \param _firstRotationAngleRad T rotation angle (rad) following right hand rule for first rotation
			 *    \param _firstRotationAngleRad T rotation angle (rad) following right hand rule for second rotation
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
			 *    \param _firstRotationAngleDeg T rotation angle (deg) following right hand rule for second rotation
			 *    \param _thirdRotationAngleDeg T rotation angle (deg) following right hand rule for third rotation
			 */
			void rotateDeg (
			    unsigned int _firstRotationAxis, T _firstRotationAngleDeg,
			    unsigned int _secondRotationAxis, T _secondRotationAngleDeg,
			    unsigned int _thirdRotationAxis, T _thirdRotationAngleDeg
			);

			/*!
			 *    \brief Specify the right most column of the 4x4 transform matrix
			 *    \param v Vector4 vector containing four elements
			 */
			void setRightColumn (const Vector4<T> v)
			{
				this->m_Vector[3]  = v[0];
				this->m_Vector[7]  = v[1];
				this->m_Vector[11] = v[2];
				this->m_Vector[15] = v[3];
			}

	};

	template < typename T >
	TransformMatrix4<T>::TransformMatrix4()
	{
		this->resize ( 4,4 );		// will always be a 4x4
		this->makeIdentity();		// start off with no rotation
	}

	template < typename T >
	void TransformMatrix4 < T >::operator= ( const TransformMatrix4 < T > &A )
	{
		this->m_Vector = A.m_Vector;
	}

	template < typename T >
	void TransformMatrix4 < T >::operator= ( const Matrix < T > &A )
	{
		this->m_Vector[0] = A.m_Vector[0];
		this->m_Vector[1] = A.m_Vector[1];
		this->m_Vector[2] = A.m_Vector[2];
		this->m_Vector[3] = A.m_Vector[3];
		this->m_Vector[4] = A.m_Vector[4];
		this->m_Vector[5] = A.m_Vector[5];
		this->m_Vector[6] = A.m_Vector[6];
		this->m_Vector[7] = A.m_Vector[7];
		this->m_Vector[8] = A.m_Vector[8];
		this->m_Vector[9] = A.m_Vector[9];
		this->m_Vector[10] = A.m_Vector[10];
		this->m_Vector[11] = A.m_Vector[11];
		this->m_Vector[12] = A.m_Vector[12];
		this->m_Vector[13] = A.m_Vector[13];
		this->m_Vector[14] = A.m_Vector[14];
		this->m_Vector[15] = A.m_Vector[15];
	}

	template < typename T >
	inline void TransformMatrix4<T>::rotateRad (
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
		std::vector <T> _C ( 9 ),_I ( 9 );

		// CREATE A TEMPORARY IDENTITY MATRIX
		_I[0] = 1.0;_I[1] = 0.0;_I[2] = 0.0;
		_I[3] = 0.0;_I[4] = 1.0;_I[5] = 0.0;
		_I[6] = 0.0;_I[7] = 0.0;_I[8] = 1.0;

		Matrix<T> _R ( _I,3,3 );

		// for each rotation
		for ( int k = 2; k > -1; --k )
		{
			//determine which axis the rotation is about (1,2,3) (x,y,z)
			_C = _I;
			switch ( _axis[k] )
			{
				case 1:
					_C[4] = std::cos ( _angle[k] );
					_C[5] = std::sin ( _angle[k] );
					_C[7] =-std::sin ( _angle[k] );
					_C[8] = std::cos ( _angle[k] );

					break;
				case 2:
					_C[0] = std::cos ( _angle[k] );
					_C[2] =-std::sin ( _angle[k] );
					_C[6] = std::sin ( _angle[k] );
					_C[8] = std::cos ( _angle[k] );

					break;
				case 3:
					_C[0] = std::cos ( _angle[k] );
					_C[1] = std::sin ( _angle[k] );
					_C[3] =-std::sin ( _angle[k] );
					_C[4] = std::cos ( _angle[k] );

					break;
				default:
					std::cout << "vm::RotationMatrix Error, axis of rotation must be 1, 2, or 3!\n";
					exit ( -1 );
			}
			Matrix<T> _CM ( _C,3,3 );
			_R = _R.multMatrix3x3 ( _CM );
		}

		this->m_Vector[0] = _R ( 1,1 );
		this->m_Vector[1] = _R ( 1,2 );
		this->m_Vector[2] = _R ( 1,3 );
		this->m_Vector[4] = _R ( 2,1 );
		this->m_Vector[5] = _R ( 2,2 );
		this->m_Vector[6] = _R ( 2,3 );
		this->m_Vector[8] = _R ( 3,1 );
		this->m_Vector[9] = _R ( 3,2 );
		this->m_Vector[10] = _R ( 3,3 );
	}

	template < typename T >
	inline void TransformMatrix4<T>::rotateDeg (
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
#endif  // header include protection
