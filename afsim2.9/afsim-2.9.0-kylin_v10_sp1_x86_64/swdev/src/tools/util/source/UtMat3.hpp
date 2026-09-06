// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef UTMAT3_HPP
#define UTMAT3_HPP

#include "UtMath.hpp"
#include "UtVec3.hpp"

//! Static methods to treat 3x3 arrays as 3x3 matrices.
//!
//! At some point the VWC amended this to provide a UtMat3 object. The coding standards were not
//! adhered to when this was done.

template<class T>
class UtMat3
{
public:
   // default & copy constructors, operator= and destructor for UtMat3
   // Default constructor  Makes a UtMat3 set to the identity matrix.
   UtMat3()
   {
      val[0] = 1;
      val[1] = 0;
      val[2] = 0;
      val[3] = 0;
      val[4] = 1;
      val[5] = 0;
      val[6] = 0;
      val[7] = 0;
      val[8] = 1;
   }

   // Type constructor  Makes a UtMat3 from 3x3 arguments of type T
   UtMat3(const T input[3][3])
   {
      val[0] = input[0][0];
      val[1] = input[0][1];
      val[2] = input[0][2];
      val[3] = input[1][0];
      val[4] = input[1][1];
      val[5] = input[1][2];
      val[6] = input[2][0];
      val[7] = input[2][1];
      val[8] = input[2][2];
   }

   // Type constructor  Makes a UtMat3 from 9 arguments of type T
   UtMat3(const T input[9])
   {
      val[0] = input[0];
      val[1] = input[1];
      val[2] = input[2];
      val[3] = input[3];
      val[4] = input[4];
      val[5] = input[5];
      val[6] = input[6];
      val[7] = input[7];
      val[8] = input[8];
   }

   // Operator= assigns one UtMat3 to another
   UtMat3& operator=(const UtMat3& rhs)
   {
      if (this != &rhs)
      {
         val[0] = rhs.val[0];
         val[1] = rhs.val[1];
         val[2] = rhs.val[2];
         val[3] = rhs.val[3];
         val[4] = rhs.val[4];
         val[5] = rhs.val[5];
         val[6] = rhs.val[6];
         val[7] = rhs.val[7];
         val[8] = rhs.val[8];
      }
      return *this;
   }

   // Copy constructor  Makes one UtMat3 from another
   UtMat3(const UtMat3& rhs)
   {
      val[0] = rhs.val[0];
      val[1] = rhs.val[1];
      val[2] = rhs.val[2];
      val[3] = rhs.val[3];
      val[4] = rhs.val[4];
      val[5] = rhs.val[5];
      val[6] = rhs.val[6];
      val[7] = rhs.val[7];
      val[8] = rhs.val[8];
   }

   // Destructor   Destroys a UtMat3 (delete anything that was new'd)
   ~UtMat3() = default;

   // Construct matrix given phi, psi, theta in radians
   UtMat3(T phi_rad, T theta_rad, T psi_rad);

   // Construct  matrix given sinphi, cosphi, sintheta, costheta, sinpsi, and cospsi
   UtMat3(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA);

   // accessors
   T        GetValue(int j) const { return val[j]; }
   void     SetValue(int j, T value) { val[j] = value; }
   T*       GetFirstPtr() { return val; }
   const T* GetFirstPtr() const { return val; }
   T&       operator[](unsigned int j) { return val[j]; }
   T&       operator[](int j) { return val[j]; }
   T        operator[](unsigned int j) const { return val[j]; }
   T        operator[](int j) const { return val[j]; }

   // =================================================================================================
   //! Extract the rotation angles from a rotational transformation matrix.
   //!
   //! Extract the (yaw-pitch-roll style) Euler angles.
   //!
   //! If the matrix is the NED->ECS transformation then the return angles
   //! will be the heading, pitch and roll angles of the body relative to
   //! the local NED frame.
   //!
   //! If the matrix is the WCS->ECS transformation then the returned angles
   //! will be the angles needed for the orientation in the DIS PDU.
   //!
   //! The output values will be in radians in with the following ranges:
   //! \li -PI \<= Psi \<= PI
   //! \li -PI/2 \<= Theta \<= PI/2
   //! \li -PI \<= Phi \<= PI
   void EulerAngles(T& aPsi,   // [-pi, pi]
                    T& aTheta, // [-pi/2, pi/2]
                    T& aPhi)   // [-pi, pi]
   {
      // This algorithm is taken directly from:
      //
      // IEEE Standard for Distributed Interactive Simulation - Application Protocols
      // IEEE Std 1278.1-1995, Annex B, section B.1.6.1.5
      //
      // if aTransform[0][2] is exactly 1 or -1, the resulting value of theta will be
      // be -pi/2 or pi/2 respectively. Taking the cos(theta) would then result in a
      // value of zero and eventually a divide by zero. A more insidious problem arises,
      // however. Taking cos(asin(pi/2)) mathematically equals zero, but both GNU g++
      // Microsoft Visual C++ both return something that is not zero (about 1E-17).
      // When this is subsequently used in the recovery of the angles, one can get some
      // pretty violent fluctuations.
      //
      // If theta is +/- pi/2, we really can't solve for anything because psi and phi
      // cannot be uniquely determined. Take the matrix definition in B.1.6.1.4, and
      // set cos(theta)=0. Look at what's left, and you'll see there is no what to solve
      // for phi and psi unless you assume one of them. In this case we'll assume phi
      // (roll) is zero and solve for psi (yaw).

      // (JAJ) A tolerance was added because of numerical issues where compound transforms
      //       were involved. Recovering the NED angles during vertical rocket boosts was
      //       resulting in wild fluctuations.
      static const double cALMOST_ONE = 0.99999999999999;
      if ((val[2] >= cALMOST_ONE) || (val[2] <= -cALMOST_ONE))
      {
         aPsi = acos(min(max(val[4], -1.0), 1.0)); // [0, PI]
         if (val[3] > 0.0)
         {
            aPsi = -aPsi;
         }
         aTheta = UtMath::cPI_OVER_2; // theta has the opposite sign of aTransform[0][2]
         if (val[2] >= 1.0)
         {
            aTheta = -UtMath::cPI_OVER_2;
         }
         aPhi = 0.0;
      }
      else
      {
         aTheta          = asin(-val[2]);
         double cosTheta = cos(aTheta);
         aPsi            = acos(min(max(val[0] / cosTheta, -1.0), 1.0));
         if (val[1] < 0.0)
         {
            aPsi = -aPsi;
         }
         aPhi = acos(min(max(val[8] / cosTheta, -1.0), 1.0));
         if (val[5] < 0.0)
         {
            aPhi = -aPhi;
         }
      }
   }

   //  Return angle phi of the matrix
   T GetPhi() const { return static_cast<T>(atan2(val[5], val[8])); }

   //  Return angle theta of the matrix
   T GetTha() const { return static_cast<T>(-asinf(val[2])); }

   //  Return angle psi of the matrix
   T GetPsi() const { return static_cast<T>(atan2f(val[1], val[0])); }

   //  Return angle psi of the matrix that is 0 to 2 pi
   T GetPsi2Pi() const { return static_cast<T>(UtMath::NormalizeAngle0_TwoPi(GetPsi())); }

   //  Return angle phi of the matrix if it is transposed
   T GetPhiOfTransposed() const { return static_cast<T>(atan2(val[7], val[8])); }

   //  Return angle theta of the matrix if it is transposed
   T GetThaOfTransposed() const { return static_cast<T>(-asinf(val[6])); }

   //  Return angle psi of the matrix if it is transposed
   T GetPsiOfTransposed() const { return static_cast<T>(atan2f(val[3], val[0])); }

   //  Return angle psi between 0 and 2 pi of the matrix if it is transposed
   T GetPsi2PiOfTransposed() const { return static_cast<T>(UtMath::NormalizeAngle0_TwoPi(GetPsi2PiOfTransposed())); }

   void Build(T aAngle, T aAxis[3]);

   void Build(T phi_rad, T theta_rad, T psi_rad);

   void Build(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA);

   void BuildTranspose(T phi_rad, T theta_rad, T psi_rad);

   void BuildTranspose(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA);

   // Addition functions
   UtMat3<T> operator+=(const UtMat3<T>& rhs);
   UtMat3<T> operator+(const UtMat3<T>& rhs) const;
   UtMat3<T> operator+(T rhs) const;

   // Subtraction functions
   UtMat3<T> operator-=(const UtMat3<T>& rhs);
   UtMat3<T> operator-(const UtMat3<T>& rhs) const;
   UtMat3<T> operator-(T rhs) const;

   // Multiplication functions (more functions below)
   UtVec3<float>  operator*(const UtVec3<float>& rhs) const;
   UtVec3<double> operator*(const UtVec3<double>& rhs) const;
   UtMat3<T>      operator*=(const UtMat3<T>& rhs);
   UtMat3<T>      operator*(const UtMat3<T>& rhs) const;
   UtMat3<T>      operator*(T rhs) const;

   // Division functions
   UtMat3<T> operator/=(const UtMat3<T>& rhs);
   UtMat3<T> operator/(const UtMat3<T>& rhs) const;
   UtMat3<T> operator/(T rhs) const;

   // -----------------------------
   // More Multiplication Functions
   // -----------------------------
   UtVec3f Multiply(const UtVec3f& rhs) const;
   // Multiply a 3x1 float vector by "this" 3x3 matrix

   UtVec3d Multiply(const UtVec3d& rhs) const;
   // Multiply a 3x1 double vector by "this" 3x3 matrix

   UtMat3<T> Multiply(const UtMat3<T>& rhs) const;
   // Multiply a 3x3 matrix and by "this" 3x3 matrix

   UtVec3f TransposeMultiply(const UtVec3f& rhs) const;
   // Multiply a 3x1 float vector by "this" transposed matrix

   UtVec3d TransposeMultiply(const UtVec3d& rhs) const;
   // Multiply a 3x1 double vector by "this" 3x3 transposed matrix

   UtMat3<T> TransposeMultiply(const UtMat3<T>& rhs) const;
   // Multiply a 3x3 matrix and by "this" 3x3 transposed matrix

   void Multiply(UtVec3f& result, const UtVec3f& rhs) const;
   // Multiply a 3x1 float vector by "this" 3x3 matrix

   void Multiply(UtVec3d& result, const UtVec3d& rhs) const;
   // Multiply a 3x1 double vector by "this" 3x3 matrix

   void Multiply(UtMat3<T>& result, const UtMat3<T>& rhs) const;
   // Multiply a 3x3 matrix and by "this" 3x3 matrix

   void TransposeMultiply(UtVec3f& result, const UtVec3f& rhs) const;
   // Multiply a 3x1 float vector by "this" 3x3 transposed matrix

   void TransposeMultiply(UtVec3d& result, const UtVec3d& rhs) const;
   // Multiply a 3x1 double vector by "this"3x3 transposed matrix

   void TransposeMultiply(UtMat3<T>& result, const UtMat3<T>& rhs) const;
   // Multiply a 3x3 matrix and by "this" 3x3 transposed matrix

   // --------------
   // Misc Functions
   // --------------
   UtMat3<T> GetTranspose() const;
   // Get a transpose copy of the matrix

   void SetTranspose(const UtMat3<T>& rhs);
   // Set the matrix with a transpose of another matrix

   void Transpose();
   // Transpose the current matrix

   //! @name Static methods.
   //! The static methods provide the means to manage and use 3x3 arrays as 3x3 matrices.
   //@{

   //! Set A = I, the identity matrix
   static void Identity(T aA[3][3])
   {
      aA[0][0] = 1.0;
      aA[0][1] = 0.0;
      aA[0][2] = 0.0;

      aA[1][0] = 0.0;
      aA[1][1] = 1.0;
      aA[1][2] = 0.0;

      aA[2][0] = 0.0;
      aA[2][1] = 0.0;
      aA[2][2] = 1.0;
   }

   //! Set A = B where A and B are 3x3 matrices
   static void Set(T aA[3][3], const T aB[3][3])
   {
      aA[0][0] = aB[0][0];
      aA[0][1] = aB[0][1];
      aA[0][2] = aB[0][2];

      aA[1][0] = aB[1][0];
      aA[1][1] = aB[1][1];
      aA[1][2] = aB[1][2];

      aA[2][0] = aB[2][0];
      aA[2][1] = aB[2][1];
      aA[2][2] = aB[2][2];
   }

   //! Set all elements of A equal to the specified scalar, S.
   static void Set(T aA[3][3], T aS)
   {
      aA[0][0] = aS;
      aA[0][1] = aS;
      aA[0][2] = aS;

      aA[1][0] = aS;
      aA[1][1] = aS;
      aA[1][2] = aS;

      aA[2][0] = aS;
      aA[2][1] = aS;
      aA[2][2] = aS;
   }

   //! Set A = transpose(B) where A and B are 3x3 matrices
   //! @note A and B must not refer to the same matrix.
   static void Transpose(T aA[3][3], const T aB[3][3])
   {
      aA[0][0] = aB[0][0];
      aA[0][1] = aB[1][0];
      aA[0][2] = aB[2][0];

      aA[1][0] = aB[0][1];
      aA[1][1] = aB[1][1];
      aA[1][2] = aB[2][1];

      aA[2][0] = aB[0][2];
      aA[2][1] = aB[1][2];
      aA[2][2] = aB[2][2];
   }

   //! Compute A = B x C where A, B and C are 3x3 matrices
   //! @note A must not refer to the same matrix as B or C.
   static void Multiply(T aA[3][3], const T aB[3][3], const T aC[3][3])
   {
      aA[0][0] = (aB[0][0] * aC[0][0]) + (aB[0][1] * aC[1][0]) + (aB[0][2] * aC[2][0]);
      aA[0][1] = (aB[0][0] * aC[0][1]) + (aB[0][1] * aC[1][1]) + (aB[0][2] * aC[2][1]);
      aA[0][2] = (aB[0][0] * aC[0][2]) + (aB[0][1] * aC[1][2]) + (aB[0][2] * aC[2][2]);
      aA[1][0] = (aB[1][0] * aC[0][0]) + (aB[1][1] * aC[1][0]) + (aB[1][2] * aC[2][0]);
      aA[1][1] = (aB[1][0] * aC[0][1]) + (aB[1][1] * aC[1][1]) + (aB[1][2] * aC[2][1]);
      aA[1][2] = (aB[1][0] * aC[0][2]) + (aB[1][1] * aC[1][2]) + (aB[1][2] * aC[2][2]);
      aA[2][0] = (aB[2][0] * aC[0][0]) + (aB[2][1] * aC[1][0]) + (aB[2][2] * aC[2][0]);
      aA[2][1] = (aB[2][0] * aC[0][1]) + (aB[2][1] * aC[1][1]) + (aB[2][2] * aC[2][1]);
      aA[2][2] = (aB[2][0] * aC[0][2]) + (aB[2][1] * aC[1][2]) + (aB[2][2] * aC[2][2]);
   }

   //! Compute Y = A x X, where A is a 3x3 matrix and X and Y are 3x1 vectors.
   //! @note X and Y must not refer to the same matrix.
   static void Transform(T aY[3], const T aA[3][3], const T aX[3])
   {
      aY[0] = (aA[0][0] * aX[0]) + (aA[0][1] * aX[1]) + (aA[0][2] * aX[2]);
      aY[1] = (aA[1][0] * aX[0]) + (aA[1][1] * aX[1]) + (aA[1][2] * aX[2]);
      aY[2] = (aA[2][0] * aX[0]) + (aA[2][1] * aX[1]) + (aA[2][2] * aX[2]);
   }

   //! Compute Y = A(inverse) x X where A is a 3x3 matrix and X and Y are 3x1 vectors.
   //!
   //! Where A(inverse) is assumed to be equal to A(transpose)
   //!
   //! @note X and Y must not refer to the same matrix.
   static void InverseTransform(T aY[3], const T aA[3][3], const T aX[3])
   {
      aY[0] = (aA[0][0] * aX[0]) + (aA[1][0] * aX[1]) + (aA[2][0] * aX[2]);
      aY[1] = (aA[0][1] * aX[0]) + (aA[1][1] * aX[1]) + (aA[2][1] * aX[2]);
      aY[2] = (aA[0][2] * aX[0]) + (aA[1][2] * aX[1]) + (aA[2][2] * aX[2]);
   }

   //! Compute Y = A * X * A(inverse), where A is a 3x3 matrix (coordinate transform),
   //! and X and Y are 3x3 matrices.
   //! A(inverse) is assumed to be equal to A(transpose)
   //!
   //! @note X and Y must not refer to the same matrix.
   static void Transform(T aY[3][3], const T aA[3][3], const T aX[3][3])
   {
      double A_Transpose[3][3];
      Transpose(A_Transpose, aA);
      double tempResult[3][3];
      Multiply(tempResult, aX, A_Transpose);
      Multiply(aY, aA, tempResult);
   }

   //! Compute Y = A(inverse) * X * A, where A is a 3x3 matrix (coordinate transform),
   //! and X and Y are 3x3 matrices.
   //! A(inverse) is assumed to be equal to A(transpose)
   //!
   //! @note X and Y must not refer to the same matrix.
   static void InverseTransform(T aY[3][3], const T aA[3][3], const T aX[3][3])
   {
      double A_Transpose[3][3];
      Transpose(A_Transpose, aA);
      double tempResult[3][3];
      Multiply(tempResult, aX, aA);
      Multiply(aY, A_Transpose, tempResult);
   }

   //! Build a matrix that represents a rotation of a specified angle about a specified axis.
   //! @param aResult [output] The output matrix representing the rotation.
   //! @param aRotationAngle [input] The angle of rotation (radians).
   //! @param aRotationAxis  [input] The axis of rotation (this MUST be a unit vector).
   static void BuildRotationMatrix(double aResult[3][3], double aRotationAngle, const double aRotationAxis[3])
   {
      double ux     = aRotationAxis[0];
      double uy     = aRotationAxis[1];
      double uz     = aRotationAxis[2];
      double cosa   = cos(aRotationAngle);
      double cosan  = 1.0 - cosa;
      double sina   = sin(aRotationAngle);
      aResult[0][0] = cosa + ux * ux * cosan;
      aResult[0][1] = ux * uy * cosan - uz * sina;
      aResult[0][2] = ux * uz * cosan + uy * sina;
      aResult[1][0] = uy * ux * cosan + uz * sina;
      aResult[1][1] = cosa + uy * uy * cosan;
      aResult[1][2] = uy * uz * cosan - ux * sina;
      aResult[2][0] = uz * ux * cosan - uy * sina;
      aResult[2][1] = uz * uy * cosan + ux * sina;
      aResult[2][2] = cosa + uz * uz * cosan;
   }

   //! Compute the determinant.
   //! \param aMatrix - The matrix to have its determinant computed.
   //! \returns       - The determinant of the matrix.
   static double Determinant(double aMatrix[3][3])
   {
      return aMatrix[0][0] * aMatrix[1][1] * aMatrix[2][2] + aMatrix[0][1] * aMatrix[1][2] * aMatrix[2][0] +
             aMatrix[0][2] * aMatrix[1][0] * aMatrix[2][1] - aMatrix[2][0] * aMatrix[1][1] * aMatrix[0][2] -
             aMatrix[2][1] * aMatrix[1][2] * aMatrix[0][0] - aMatrix[2][2] * aMatrix[1][0] * aMatrix[0][1];
   }

   //@}

private:
   static const unsigned int vmax;
   T                         val[9];
};

template<class T>
UtMat3<T> operator+(T lhs, const UtMat3<T>& rhs);
template<class T>
UtMat3<T> operator-(T lhs, const UtMat3<T>& rhs);
template<class T>
UtMat3<T> operator*(T lhs, const UtMat3<T>& rhs);
template<class T>
UtMat3<T> operator/(T lhs, const UtMat3<T>& rhs);

using UtMat3f = UtMat3<float>;
using UtMat3d = UtMat3<double>;

/******************************************************************************/
/*                                                                            */
/*                           Static Variables                                 */
/*                                                                            */
/******************************************************************************/

template<class T>
const unsigned int UtMat3<T>::vmax = 8;

/******************************************************************************/
/*                                                                            */
/*        Constructor, Destructor, Copy Constructor, and operator=            */
/*                                                                            */
/******************************************************************************/

//  Construct matrix given phi, psi, theta in radians
template<class T>
UtMat3<T>::UtMat3(T phi_rad, T theta_rad, T psi_rad)
{
   Build(phi_rad, theta_rad, psi_rad);
}

//  Build matrix given sinphi, cosphi, sintheta, costheta, sinpsi, and cospsi
template<class T>
UtMat3<T>::UtMat3(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA)
{
   Build(sinphiA, cosphiA, sinthaA, costhaA, sinpsiA, cospsiA);
}

// ******************************************************************************
// Methods to build a rotation matrix.
// ******************************************************************************

//  Build matrix given phi, psi, theta in radians
template<class T>
void UtMat3<T>::Build(T phi_rad, T theta_rad, T psi_rad)
{
   const T sinphi = sinf(phi_rad);
   const T cosphi = cosf(phi_rad);
   const T sintha = sinf(theta_rad);
   const T costha = cosf(theta_rad);
   const T sinpsi = sinf(psi_rad);
   const T cospsi = cosf(psi_rad);

   T temp;

   val[0] = costha * cospsi;
   val[1] = costha * sinpsi;
   val[2] = -sintha;

   temp   = sinphi * sintha;
   val[3] = (temp * cospsi) - (cosphi * sinpsi);
   val[4] = (temp * sinpsi) + (cosphi * cospsi);
   val[5] = sinphi * costha;

   temp   = cosphi * sintha;
   val[6] = (temp * cospsi) + (sinphi * sinpsi);
   val[7] = (temp * sinpsi) - (sinphi * cospsi);
   val[8] = cosphi * costha;
}

//  Build matrix given sinphi, cosphi, sintheta, costheta, sinpsi, and cospsi
template<class T>
void UtMat3<T>::Build(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA)
{
   T temp;

   val[0] = costhaA * cospsiA;
   val[1] = costhaA * sinpsiA;
   val[2] = -sinthaA;

   temp   = sinphiA * sinthaA;
   val[3] = (temp * cospsiA) - (cosphiA * sinpsiA);
   val[4] = (temp * sinpsiA) + (cosphiA * cospsiA);
   val[5] = sinphiA * costhaA;

   temp   = cosphiA * sinthaA;
   val[6] = (temp * cospsiA) + (sinphiA * sinpsiA);
   val[7] = (temp * sinpsiA) - (sinphiA * cospsiA);
   val[8] = cosphiA * costhaA;
}

//  Build matrix given phi, psi, theta in radians
template<class T>
void UtMat3<T>::BuildTranspose(T phi_rad, T theta_rad, T psi_rad)
{
   const T sinphi = sinf(phi_rad);
   const T cosphi = cosf(phi_rad);
   const T sintha = sinf(theta_rad);
   const T costha = cosf(theta_rad);
   const T sinpsi = sinf(psi_rad);
   const T cospsi = cosf(psi_rad);

   T temp;

   val[0] = costha * cospsi;
   val[3] = costha * sinpsi;
   val[6] = -sintha;

   temp   = sinphi * sintha;
   val[1] = (temp * cospsi) - (cosphi * sinpsi);
   val[4] = (temp * sinpsi) + (cosphi * cospsi);
   val[7] = sinphi * costha;

   temp   = cosphi * sintha;
   val[2] = (temp * cospsi) + (sinphi * sinpsi);
   val[5] = (temp * sinpsi) - (sinphi * cospsi);
   val[8] = cosphi * costha;
}

//  Build matrix given sinphi, cosphi, sintheta, costheta, sinpsi, and cospsi
template<class T>
void UtMat3<T>::BuildTranspose(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA)
{
   T temp;

   val[0] = costhaA * cospsiA;
   val[3] = costhaA * sinpsiA;
   val[6] = -sinthaA;

   temp   = sinphiA * sinthaA;
   val[1] = (temp * cospsiA) - (cosphiA * sinpsiA);
   val[4] = (temp * sinpsiA) + (cosphiA * cospsiA);
   val[7] = sinphiA * costhaA;

   temp   = cosphiA * sinthaA;
   val[2] = (temp * cospsiA) + (sinphiA * sinpsiA);
   val[5] = (temp * sinpsiA) - (sinphiA * cospsiA);
   val[8] = cosphiA * costhaA;
}

/******************************************************************************/
/*                                                                            */
/*                      Addition Operations                                   */
/*                                                                            */
/******************************************************************************/

// operator+=  Adds a matrix to another matrix using a compound operator
template<class T>
UtMat3<T> UtMat3<T>::operator+=(const UtMat3<T>& rhs)
{
   for (int j = vmax; j >= 0; --j)
   {
      val[j] += rhs.val[j];
   }

   return *this;
}

// operator+  Adds a matrix to another matrix
template<class T>
UtMat3<T> UtMat3<T>::operator+(const UtMat3<T>& rhs) const
{
   UtMat3<T> result;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = val[j] + rhs.val[j];
   }

   return result;
}

// operator+  Adds a matrix to a scaler
template<class T>
UtMat3<T> UtMat3<T>::operator+(T rhs) const
{
   UtMat3<T> result;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = val[j] + rhs;
   }

   return result;
}

// operator+  Adds a scaler to a matrix (Not a class member function, but
// a global function)
template<class T>
UtMat3<T> operator+(T lhs, const UtMat3<T>& rhs)
{
   UtMat3<T> result;
   const int vmax = 8;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = lhs + rhs[j];
   }

   return result;
}

/******************************************************************************/
/*                                                                            */
/*                       Subtraction Operations                               */
/*                                                                            */
/******************************************************************************/

// operator-=  Subtracts a matrix from another matrix using a compound operator
template<class T>
UtMat3<T> UtMat3<T>::operator-=(const UtMat3<T>& rhs)
{
   for (int j = vmax; j >= 0; --j)
   {
      val[j] -= rhs.val[j];
   }

   return *this;
}

// operator-  Subtracts a matrix from another matrix
template<class T>
UtMat3<T> UtMat3<T>::operator-(const UtMat3<T>& rhs) const
{
   UtMat3<T> result;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = val[j] - rhs.val[j];
   }

   return result;
}

// operator-  Subtracts a scaler from a matrix
template<class T>
UtMat3<T> UtMat3<T>::operator-(T rhs) const
{
   UtMat3<T> result;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = val[j] - rhs;
   }

   return result;
}

// operator-  Subtracts a matrix from a scaler (Not a class member function, but
// a global function)
template<class T>
UtMat3<T> operator-(T lhs, const UtMat3<T>& rhs)
{
   UtMat3<T> result;
   const int vmax = 8;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = lhs - rhs[j];
   }

   return result;
}

/******************************************************************************/
/*                                                                            */
/*                     Multiplication Operations                              */
/*               (More multiplication functions are below)                    */
/*                                                                            */
/******************************************************************************/

// operator*=  Multiplies a matrix by another matrix using a compound operator
template<class T>
UtMat3<T> UtMat3<T>::operator*=(const UtMat3<T>& rhs)
{
   // Call the multiply
   T temp[9] = Multiply(rhs);

   // Call the assign
   *this = temp;

   return *this;
}

// Multiply a 3x3 matrix and another 3x3 matrix, yielding a 3x3 matrix
template<class T>
UtMat3<T> UtMat3<T>::operator*(const UtMat3<T>& rhs) const
{
   UtMat3<T> result = Multiply(rhs);

   return result;
}

// Multiply a 3x3 matrix by a 3x1 vector, yielding a 3x1 vector
template<class T>
UtVec3f UtMat3<T>::operator*(const UtVec3f& rhs) const
{
   UtVec3f result = Multiply(rhs);

   return result;
}

// Multiply a 3x3 matrix by a 3x1 vector, yielding a 3x1 vector
template<class T>
UtVec3d UtMat3<T>::operator*(const UtVec3d& rhs) const
{
   UtVec3d result = Multiply(rhs);

   return result;
}

// operator*  Multiplies a matrix by a scaler
template<class T>
UtMat3<T> UtMat3<T>::operator*(T rhs) const
{
   UtMat3<T> result;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = val[j] * rhs;
   }

   return result;
}

// operator*  Multiplies a scaler by a matrix (Not a class member function, but
// a global function)
template<class T>
UtMat3<T> operator*(T lhs, const UtMat3<T>& rhs)
{
   UtMat3<T> result;
   const int vmax = 8;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = lhs * rhs[j];
   }

   return result;
}

/******************************************************************************/
/*                                                                            */
/*                        Division Operations                                 */
/*                   (Only scalers make sense here)                           */
/*                                                                            */
/******************************************************************************/

// operator/  Divides a matrix by a scaler
template<class T>
UtMat3<T> UtMat3<T>::operator/(T rhs) const
{
   UtMat3<T> result;

   for (int j = vmax; j >= 0; --j)
   {
      result.val[j] = val[j] / rhs;
   }

   return result;
}

// operator/  Divides a scaler by a matrix (Not a class member function, but
// a global function)
template<class T>
UtMat3<T> operator/(T lhs, const UtMat3<T>& rhs)
{
   UtMat3<T> result;
   const int vmax = 8;

   for (int j = vmax; j >= 0; --j)
   {
      if (rhs[j] != 0.0F)
      {
         result.val[j] = lhs / rhs[j];
      }
   }

   return result;
}

/******************************************************************************/
/*                                                                            */
/*                        Multiplication Functions                            */
/*                                                                            */
/******************************************************************************/

// Multiply a 3x1 vector by "this" 3x3 matrix, yielding a 3x1 vector
template<class T>
UtVec3f UtMat3<T>::Multiply(const UtVec3f& rhs) const
{
   UtVec3f result;

   result[0] = (val[0] * rhs[0]) + (val[1] * rhs[1]) + (val[2] * rhs[2]);
   result[1] = (val[3] * rhs[0]) + (val[4] * rhs[1]) + (val[5] * rhs[2]);
   result[2] = (val[6] * rhs[0]) + (val[7] * rhs[1]) + (val[8] * rhs[2]);

   return result;
}

// Multiply a 3x1 double vector by "this" 3x3 matrix
template<class T>
UtVec3d UtMat3<T>::Multiply(const UtVec3d& rhs) const
{
   UtVec3d result;

   result[0] = (val[0] * rhs[0]) + (val[1] * rhs[1]) + (val[2] * rhs[2]);
   result[1] = (val[3] * rhs[0]) + (val[4] * rhs[1]) + (val[5] * rhs[2]);
   result[2] = (val[6] * rhs[0]) + (val[7] * rhs[1]) + (val[8] * rhs[2]);

   return result;
}

// Multiply a 3x3 matrix and by "this" 3x3 matrix
template<class T>
UtMat3<T> UtMat3<T>::Multiply(const UtMat3<T>& rhs) const
{
   UtMat3<T> result;

   result[0] = (val[0] * rhs.val[0]) + (val[1] * rhs.val[3]) + (val[2] * rhs.val[6]);
   result[1] = (val[0] * rhs.val[1]) + (val[1] * rhs.val[4]) + (val[2] * rhs.val[7]);
   result[2] = (val[0] * rhs.val[2]) + (val[1] * rhs.val[5]) + (val[2] * rhs.val[8]);
   result[3] = (val[3] * rhs.val[0]) + (val[4] * rhs.val[3]) + (val[5] * rhs.val[6]);
   result[4] = (val[3] * rhs.val[1]) + (val[4] * rhs.val[4]) + (val[5] * rhs.val[7]);
   result[5] = (val[3] * rhs.val[2]) + (val[4] * rhs.val[5]) + (val[5] * rhs.val[8]);
   result[6] = (val[6] * rhs.val[0]) + (val[7] * rhs.val[3]) + (val[8] * rhs.val[6]);
   result[7] = (val[6] * rhs.val[1]) + (val[7] * rhs.val[4]) + (val[8] * rhs.val[7]);
   result[8] = (val[6] * rhs.val[2]) + (val[7] * rhs.val[5]) + (val[8] * rhs.val[8]);

   return result;
}

// Multiply a 3x1 float vector by "this" 3x3 Transposed matrix
template<class T>
UtVec3f UtMat3<T>::TransposeMultiply(const UtVec3f& rhs) const
{
   // Get the transpose of this 3x3 matrix
   UtMat3<T> transformMat = GetTranspose();

   // Multiply the transposed matrix by the rhs
   UtVec3f result = transformMat.Multiply(rhs);

   return result;
}

// Multiply a 3x1 double vector by "this" Transposed matrix
template<class T>
UtVec3d UtMat3<T>::TransposeMultiply(const UtVec3d& rhs) const
{
   // Get the transpose of this 3x3 matrix
   UtMat3<T> transformMat = GetTranspose();

   // Multiply the transposed matrix by the rhs
   UtVec3d result = transformMat.Multiply(rhs);

   return result;
}

// Multiply a 3x3 matrix and by "this" 3x3 Transposed matrix
template<class T>
UtMat3<T> UtMat3<T>::TransposeMultiply(const UtMat3<T>& rhs) const
{
   // Get the transpose of this 3x3 matrix
   UtMat3<T> transformMat = GetTranspose();

   // Multiply the transposed matrix by the rhs
   UtMat3<T> result = transformMat.Multiply(rhs);

   return result;
}

// Multiply a 3x1 float vector by this 3x3 matrix, yielding a 3x1 float vector
template<class T>
void UtMat3<T>::Multiply(UtVec3f& result, const UtVec3f& rhs) const
{
   result[0] = (val[0] * rhs[0]) + (val[1] * rhs[1]) + (val[2] * rhs[2]);
   result[1] = (val[3] * rhs[0]) + (val[4] * rhs[1]) + (val[5] * rhs[2]);
   result[2] = (val[6] * rhs[0]) + (val[7] * rhs[1]) + (val[8] * rhs[2]);
}

// Multiply a 3x1 double vector by "this" 3x3 matrix
template<class T>
void UtMat3<T>::Multiply(UtVec3d& result, const UtVec3d& rhs) const
{
   result[0] = (val[0] * rhs[0]) + (val[1] * rhs[1]) + (val[2] * rhs[2]);
   result[1] = (val[3] * rhs[0]) + (val[4] * rhs[1]) + (val[5] * rhs[2]);
   result[2] = (val[6] * rhs[0]) + (val[7] * rhs[1]) + (val[8] * rhs[2]);
}

// Multiply a 3x3 matrix and by "this" 3x3 matrix
template<class T>
void UtMat3<T>::Multiply(UtMat3<T>& result, const UtMat3<T>& rhs) const
{
   result[0] = (val[0] * rhs.val[0]) + (val[1] * rhs.val[3]) + (val[2] * rhs.val[6]);
   result[1] = (val[0] * rhs.val[1]) + (val[1] * rhs.val[4]) + (val[2] * rhs.val[7]);
   result[2] = (val[0] * rhs.val[2]) + (val[1] * rhs.val[5]) + (val[2] * rhs.val[8]);
   result[3] = (val[3] * rhs.val[0]) + (val[4] * rhs.val[3]) + (val[5] * rhs.val[6]);
   result[4] = (val[3] * rhs.val[1]) + (val[4] * rhs.val[4]) + (val[5] * rhs.val[7]);
   result[5] = (val[3] * rhs.val[2]) + (val[4] * rhs.val[5]) + (val[5] * rhs.val[8]);
   result[6] = (val[6] * rhs.val[0]) + (val[7] * rhs.val[3]) + (val[8] * rhs.val[6]);
   result[7] = (val[6] * rhs.val[1]) + (val[7] * rhs.val[4]) + (val[8] * rhs.val[7]);
   result[8] = (val[6] * rhs.val[2]) + (val[7] * rhs.val[5]) + (val[8] * rhs.val[8]);
}

// Multiply a 3x1 float vector by this 3x3 Transposed matrix
template<class T>
void UtMat3<T>::TransposeMultiply(UtVec3f& result, const UtVec3f& rhs) const
{
   // Get the transpose of this 3x3 matrix
   UtMat3<T> transformMat = GetTranspose();

   // Multiply the transposed matrix by the rhs
   result = transformMat.Multiply(rhs);
}

// Multiply a 3x1 double vector by "this" 3x3 Transposed matrix
template<class T>
void UtMat3<T>::TransposeMultiply(UtVec3d& result, const UtVec3d& rhs) const
{
   // Get the transpose of this 3x3 matrix
   UtMat3<T> transformMat = GetTranspose();

   // Multiply the transposed matrix by the rhs
   result = transformMat.Multiply(rhs);
}

// Multiply a 3x3 matrix and by "this" 3x3 Transposed matrix
template<class T>
void UtMat3<T>::TransposeMultiply(UtMat3<T>& result, const UtMat3<T>& rhs) const
{
   // Get the transpose of this 3x3 matrix
   UtMat3<T> transformMat = GetTranspose();

   // Multiply the transposed matrix by the rhs
   result = transformMat.Multiply(rhs);
}

/******************************************************************************/
/*                                                                            */
/*                             Misc Functions                                 */
/*                                                                            */
/******************************************************************************/

// Get a Transpose copy of the matrix
template<class T>
UtMat3<T> UtMat3<T>::GetTranspose() const
{
   UtMat3<T> result;

   result[0] = val[0];
   result[1] = val[3];
   result[2] = val[6];
   result[3] = val[1];
   result[4] = val[4];
   result[5] = val[7];
   result[6] = val[2];
   result[7] = val[5];
   result[8] = val[8];

   return result;
}

// Set the matrix with a Transpose of another matrix
template<class T>
void UtMat3<T>::SetTranspose(const UtMat3<T>& rhs)
{
   *this = rhs.GetTranspose();
}

// Transpose the current matrix
template<class T>
void UtMat3<T>::Transpose()
{
   T temp;

   temp   = val[1];
   val[1] = val[3];
   val[3] = temp;

   temp   = val[2];
   val[2] = val[6];
   val[6] = temp;

   temp   = val[5];
   val[5] = val[7];
   val[7] = temp;
}

#endif
