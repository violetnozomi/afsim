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

#ifndef UTFORTMAT3_HPP
#define UTFORTMAT3_HPP

#include "UtVec3.hpp"

// At the current time this class provides static methods to treat 3x3 arrays
// as 3x3 matrices.  This class is specific to match FORTRAN matrix calculations

template<class T>
class UtFortMat3
{
public:
   // default & copy constructors, operator= and destructor for UtFortMat3
   // Default constructor  Makes a UtFortMat3 with no input
   UtFortMat3()
   {
      val[0] = 0.0;
      val[1] = 0.0;
      val[2] = 0.0;
      val[3] = 0.0;
      val[4] = 0.0;
      val[5] = 0.0;
      val[6] = 0.0;
      val[7] = 0.0;
      val[8] = 0.0;
   }

   // Type constructor  Makes a UtFortMat3 from 9 arguments of type T
   UtFortMat3(const T input[9])
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

   // Operator= assigns one UtFortMat3 to another
   UtFortMat3& operator=(const UtFortMat3& rhs)
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

   // Copy constructor  Makes one UtFortMat3 from another
   UtFortMat3(const UtFortMat3& rhs)
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

   // Destructor   Destroys a UtFortMat3 (delete anything that was new'd)
   ~UtFortMat3() {}

   // Construct matrix given phi, psi, theta in radians
   UtFortMat3(T phi_rad, T theta_rad, T psi_rad);

   // Construct  matrix given sinphi, cosphi, sintheta, costheta, sinpsi, and cospsi
   UtFortMat3(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA);

   // accessors
   T        GetValue(int j) const { return val[j]; }
   void     SetValue(int j, T value) { val[j] = value; }
   T*       GetFirstPtr() { return val; }
   const T* GetFirstPtr() const { return val; }
   T&       operator[](unsigned int j) { return val[j]; }
   T&       operator[](int j) { return val[j]; }
   T        operator[](unsigned int j) const { return val[j]; }
   T        operator[](int j) const { return val[j]; }

   //  Return angle phi of the matrix
   T GetPhi() const { return static_cast<T>(atan2(val[5], val[8])); }

   //  Return angle theta of the matrix
   T GetTha() const { return static_cast<T>(-asinf(val[2])); }

   //  Return angle psi of the matrix
   T GetPsi() const { return static_cast<T>(atan2f(val[1], val[0])); }

   //  Return angle psi of the matrix that is 0 to 2 pie
   T GetPsi2Pie() const
   {
      T temp = GetPsi();
      if (temp < 0.0)
         return temp + 6.2831853; // 6.2831853F = 2 * Pie
      else
         return temp;
   }

   //  Return angle phi of the matrix if it is transposed
   T GetPhiOfTransposed() const { return static_cast<T>(atan2(val[7], val[8])); }

   //  Return angle theta of the matrix if it is transposed
   T GetThaOfTransposed() const { return static_cast<T>(-asinf(val[6])); }

   //  Return angle psi of the matrix if it is transposed
   T GetPsiOfTransposed() const { return static_cast<T>(atan2f(val[3], val[0])); }

   //  Return angle psi between 0 and 2 pie of the matrix if it is transposed
   T GetPsi2PieOfTransposed() const
   {
      T temp = GetPsiOfTransposed();
      if (temp < 0.0)
         return temp + 6.2831853; // 6.2831853 = 2 * Pie
      else
         return temp;
   }

   void Build(T phi_rad, T theta_rad, T psi_rad);

   void Build(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA);

   void BuildTranspose(T phi_rad, T theta_rad, T psi_rad);

   void BuildTranspose(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA);

   // Addition functions
   UtFortMat3<T> operator+=(const UtFortMat3<T>& rhs);
   UtFortMat3<T> operator+(const UtFortMat3<T>& rhs) const;
   UtFortMat3<T> operator+(T rhs) const;

   // Subtraction functions
   UtFortMat3<T> operator-=(const UtFortMat3<T>& rhs);
   UtFortMat3<T> operator-(const UtFortMat3<T>& rhs) const;
   UtFortMat3<T> operator-(T rhs) const;

   // Multiplication functions (more functions below)
   UtVec3<float>  operator*(UtVec3<float> rhs) const;
   UtVec3<double> operator*(UtVec3<double> rhs) const;
   UtFortMat3<T>  operator*=(UtFortMat3<T> rhs);
   UtFortMat3<T>  operator*(UtFortMat3<T> rhs) const;
   UtFortMat3<T>  operator*(T rhs) const;

   // Division functions
   UtFortMat3<T> operator/=(const UtFortMat3<T>& rhs);
   UtFortMat3<T> operator/(const UtFortMat3<T>& rhs) const;
   UtFortMat3<T> operator/(T rhs) const;

   // -----------------------------
   // More Mulitplication Functions
   // -----------------------------
   UtVec3f Multiply(UtVec3f rhs) const;
   // Multiply a 3x1 float vector by "this" 3x3 matrix

   UtVec3d Multiply(UtVec3d rhs) const;
   // Multiply a 3x1 double vector by "this" 3x3 matrix

   UtFortMat3<T> Multiply(UtFortMat3<T> rhs) const;
   // Multiply a 3x3 matrix and by "this" 3x3 matrix

   UtVec3f TransposeMultiply(UtVec3f rhs) const;
   // Multiply a 3x1 float vector by "this" transposed matrix

   UtVec3d TransposeMultiply(UtVec3d rhs) const;
   // Multiply a 3x1 double vector by "this" 3x3 transposed matrix

   UtFortMat3<T> TransposeMultiply(UtFortMat3<T> rhs) const;
   // Multiply a 3x3 matrix and by "this" 3x3 transposed matrix

   void Multiply(UtVec3f& result, UtVec3f rhs) const;
   // Multiply a 3x1 float vector by "this" 3x3 matrix

   void Multiply(UtVec3d& result, UtVec3d rhs) const;
   // Multiply a 3x1 double vector by "this" 3x3 matrix

   void Multiply(UtFortMat3<T>& result, UtFortMat3<T> rhs) const;
   // Multiply a 3x3 matrix and by "this" 3x3 matrix

   void TransposeMultiply(UtVec3f& result, UtVec3f rhs) const;
   // Multiply a 3x1 float vector by "this" 3x3 transposed matrix

   void TransposeMultiply(UtVec3d& result, UtVec3d rhs) const;
   // Multiply a 3x1 double vector by "this"3x3 transposed matrix

   void TransposeMultiply(UtFortMat3<T>& result, UtFortMat3<T> rhs) const;
   // Multiply a 3x3 matrix and by "this" 3x3 transposed matrix

   // --------------
   // Misc Functions
   // --------------
   UtFortMat3<T> GetTranspose() const;
   // Get a transpose copy of the matrix

   void SetTranspose(const UtFortMat3<T>& rhs);
   // Set the matrix with a transpose of another matrix

   void Transpose();
   // Transpose the current matrix

   // Set A = I, the identity matrix

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
   //
   // NOTE: A and B must not refer to the same matrix.

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
   //!
   //! NOTE: A must not refer to the same matrix as B or C.

   static void Multiply(T aA[3][3], const T aB[3][3], const T aC[3][3])
   {
      for (int i = 0; i < 3; ++i)
      {
         for (int j = 0; j < 3; ++j)
         {
            aA[i][j] = (aB[i][0] * aC[0][j]) + (aB[i][1] * aC[1][j]) + (aB[i][2] * aC[2][j]);
         }
      }
   }

   //! Compute Y = A x X, where A is a 3x3 matrix and X and Y are 3x1 vectors.
   //!
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

private:
   static const unsigned int vmax;
   T                         val[9];
};


template<class T>
UtFortMat3<T> operator+(T lhs, const UtFortMat3<T>& rhs);
template<class T>
UtFortMat3<T> operator-(T lhs, const UtFortMat3<T>& rhs);
template<class T>
UtFortMat3<T> operator*(T lhs, const UtFortMat3<T>& rhs);
template<class T>
UtFortMat3<T> operator/(T lhs, const UtFortMat3<T>& rhs);

typedef UtFortMat3<float>  UtFortMat3f;
typedef UtFortMat3<double> UtFortMat3d;


/******************************************************************************/
/*                                                                            */
/*                           Static Variables                                 */
/*                                                                            */
/******************************************************************************/

template<class T>
const unsigned int UtFortMat3<T>::vmax = 8;

/******************************************************************************/
/*                                                                            */
/*        Constructors, Destructors, Copy Contructor, and operator=           */
/*                                                                            */
/******************************************************************************/


//  Construct matrix given phi, psi, theta in radians
template<class T>
UtFortMat3<T>::UtFortMat3(T phi_rad, T theta_rad, T psi_rad)
{
   Build(phi_rad, theta_rad, psi_rad);
}


//  Build matrix given sinphi, cosphi, sintheta, costheta, sinpsi, and cospsi
template<class T>
UtFortMat3<T>::UtFortMat3(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA)
{
   Build(sinphiA, cosphiA, sinthaA, costhaA, sinpsiA, cospsiA);
}


/******************************************************************************/
/*                                                                            */
/*                        Accessor Functions                                   */
/*                                                                            */
/******************************************************************************/


//  Build matrix given phi, psi, theta in radians
template<class T>
void UtFortMat3<T>::Build(T phi_rad, T theta_rad, T psi_rad)
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
   val[3] = temp * cospsi - cosphi * sinpsi;
   val[4] = temp * sinpsi + cosphi * cospsi;
   val[5] = sinphi * costha;

   temp   = cosphi * sintha;
   val[6] = temp * cospsi + sinphi * sinpsi;
   val[7] = temp * sinpsi - sinphi * cospsi;
   val[8] = cosphi * costha;

   return;
}

//  Build matrix given sinphi, cosphi, sintheta, costheta, sinpsi, and cospsi
template<class T>
void UtFortMat3<T>::Build(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA)
{
   T temp;

   val[0] = costhaA * cospsiA;
   val[1] = costhaA * sinpsiA;
   val[2] = -sinthaA;

   temp   = sinphiA * sinthaA;
   val[3] = temp * cospsiA - cosphiA * sinpsiA;
   val[4] = temp * sinpsiA + cosphiA * cospsiA;
   val[5] = sinphiA * costhaA;

   temp   = cosphiA * sinthaA;
   val[6] = temp * cospsiA + sinphiA * sinpsiA;
   val[7] = temp * sinpsiA - sinphiA * cospsiA;
   val[8] = cosphiA * costhaA;

   return;
}

//  Build matrix given phi, psi, theta in radians
template<class T>
void UtFortMat3<T>::BuildTranspose(T phi_rad, T theta_rad, T psi_rad)
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
   val[1] = temp * cospsi - cosphi * sinpsi;
   val[4] = temp * sinpsi + cosphi * cospsi;
   val[7] = sinphi * costha;

   temp   = cosphi * sintha;
   val[2] = temp * cospsi + sinphi * sinpsi;
   val[5] = temp * sinpsi - sinphi * cospsi;
   val[8] = cosphi * costha;

   return;
}

//  Build matrix given sinphi, cosphi, sintheta, costheta, sinpsi, and cospsi
template<class T>
void UtFortMat3<T>::BuildTranspose(T sinphiA, T cosphiA, T sinthaA, T costhaA, T sinpsiA, T cospsiA)
{
   T temp;

   val[0] = costhaA * cospsiA;
   val[3] = costhaA * sinpsiA;
   val[6] = -sinthaA;

   temp   = sinphiA * sinthaA;
   val[1] = temp * cospsiA - cosphiA * sinpsiA;
   val[4] = temp * sinpsiA + cosphiA * cospsiA;
   val[7] = sinphiA * costhaA;

   temp   = cosphiA * sinthaA;
   val[2] = temp * cospsiA + sinphiA * sinpsiA;
   val[5] = temp * sinpsiA - sinphiA * cospsiA;
   val[8] = cosphiA * costhaA;

   return;
}

/******************************************************************************/
/*                                                                            */
/*                      Addition Operations                                   */
/*                                                                            */
/******************************************************************************/


// operator+=  Adds a matrix to another matrix using a compound operator
template<class T>
UtFortMat3<T> UtFortMat3<T>::operator+=(const UtFortMat3<T>& rhs)
{
   for (int j = vmax; j >= 0; --j)
      val[j] += rhs.val[j];

   return *this;
}


// operator+  Adds a matrix to another matrix
template<class T>
UtFortMat3<T> UtFortMat3<T>::operator+(const UtFortMat3<T>& rhs) const
{
   UtFortMat3<T> result;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = val[j] + rhs.val[j];

   return result;
}


// operator+  Adds a matrix to a scaler
template<class T>
UtFortMat3<T> UtFortMat3<T>::operator+(T rhs) const
{
   UtFortMat3<T> result;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = val[j] + rhs;

   return result;
}


// operator+  Adds a scaler to a matrix (Not a class member function, but
// a global function)
template<class T>
UtFortMat3<T> operator+(T lhs, const UtFortMat3<T>& rhs)
{
   UtFortMat3<T> result;
   const int     vmax = 8;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = lhs + rhs[j];

   return result;
}


/******************************************************************************/
/*                                                                            */
/*                       Subtraction Operations                               */
/*                                                                            */
/******************************************************************************/


// operator-=  Subtracts a matrix from another matrix using a compound operator
template<class T>
UtFortMat3<T> UtFortMat3<T>::operator-=(const UtFortMat3<T>& rhs)
{
   for (int j = vmax; j >= 0; --j)
      val[j] -= rhs.val[j];

   return *this;
}


// operator-  Subtracts a matrix from another matrix
template<class T>
UtFortMat3<T> UtFortMat3<T>::operator-(const UtFortMat3<T>& rhs) const
{
   UtFortMat3<T> result;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = val[j] - rhs.val[j];

   return result;
}


// operator-  Subtracts a scaler from a matrix
template<class T>
UtFortMat3<T> UtFortMat3<T>::operator-(T rhs) const
{
   UtFortMat3<T> result;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = val[j] - rhs;

   return result;
}


// operator-  Subtracts a matrix from a scaler (Not a class member function, but
// a global function)
template<class T>
UtFortMat3<T> operator-(T lhs, const UtFortMat3<T>& rhs)
{
   UtFortMat3<T> result;
   const int     vmax = 8;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = lhs - rhs[j];

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
UtFortMat3<T> UtFortMat3<T>::operator*=(UtFortMat3<T> rhs)
{
   T temp[9];

   temp[0] = val[0] * rhs.val[0] + val[3] * rhs.val[1] + val[6] * rhs.val[2];

   temp[1] = val[1] * rhs.val[0] + val[4] * rhs.val[1] + val[7] * rhs.val[2];

   temp[2] = val[2] * rhs.val[0] + val[5] * rhs.val[1] + val[8] * rhs.val[2];

   temp[3] = val[0] * rhs.val[3] + val[3] * rhs.val[4] + val[6] * rhs.val[5];

   temp[4] = val[1] * rhs.val[3] + val[4] * rhs.val[4] + val[7] * rhs.val[5];

   temp[5] = val[2] * rhs.val[3] + val[5] * rhs.val[4] + val[8] * rhs.val[5];

   temp[6] = val[0] * rhs.val[6] + val[3] * rhs.val[7] + val[6] * rhs.val[8];

   temp[7] = val[1] * rhs.val[6] + val[4] * rhs.val[7] + val[7] * rhs.val[8];

   temp[8] = val[2] * rhs.val[6] + val[5] * rhs.val[7] + val[8] * rhs.val[8];

   for (int j = vmax; j >= 0; --j)
      val[j] = temp[j];

   return *this;
}

// Multiply a 3x3 matrix and another 3x3 matrix, yielding a 3x3 matrix
template<class T>
UtFortMat3<T> UtFortMat3<T>::operator*(UtFortMat3<T> rhs) const
{
   UtFortMat3<T> result;

   result[0] = val[0] * rhs.val[0] + val[3] * rhs.val[1] + val[6] * rhs.val[2];

   result[1] = val[1] * rhs.val[0] + val[4] * rhs.val[1] + val[7] * rhs.val[2];

   result[2] = val[2] * rhs.val[0] + val[5] * rhs.val[1] + val[8] * rhs.val[2];

   result[3] = val[0] * rhs.val[3] + val[3] * rhs.val[4] + val[6] * rhs.val[5];

   result[4] = val[1] * rhs.val[3] + val[4] * rhs.val[4] + val[7] * rhs.val[5];

   result[5] = val[2] * rhs.val[3] + val[5] * rhs.val[4] + val[8] * rhs.val[5];

   result[6] = val[0] * rhs.val[6] + val[3] * rhs.val[7] + val[6] * rhs.val[8];

   result[7] = val[1] * rhs.val[6] + val[4] * rhs.val[7] + val[7] * rhs.val[8];

   result[8] = val[2] * rhs.val[6] + val[5] * rhs.val[7] + val[8] * rhs.val[8];

   return result;
}


// Multiply a 3x3 matrix by a 3x1 vector, yielding a 3x1 vector
template<class T>
UtVec3f UtFortMat3<T>::operator*(UtVec3f rhs) const
{
   UtVec3f result;

   result[0] = val[0] * rhs[0] + val[3] * rhs[1] + val[6] * rhs[2];

   result[1] = val[1] * rhs[0] + val[4] * rhs[1] + val[7] * rhs[2];

   result[2] = val[2] * rhs[0] + val[5] * rhs[1] + val[8] * rhs[2];

   return result;
}

// Multiply a 3x3 matrix by a 3x1 vector, yielding a 3x1 vector
template<class T>
UtVec3d UtFortMat3<T>::operator*(UtVec3d rhs) const
{
   UtVec3d result;

   result[0] = val[0] * rhs[0] + val[3] * rhs[1] + val[6] * rhs[2];

   result[1] = val[1] * rhs[0] + val[4] * rhs[1] + val[7] * rhs[2];

   result[2] = val[2] * rhs[0] + val[5] * rhs[1] + val[8] * rhs[2];

   return result;
}


// operator*  Multiplies a matrix by a scaler
template<class T>
UtFortMat3<T> UtFortMat3<T>::operator*(T rhs) const
{
   UtFortMat3<T> result;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = val[j] * rhs;

   return result;
}


// operator*  Multiplies a scaler by a matrix (Not a class member function, but
// a global function)
template<class T>
UtFortMat3<T> operator*(T lhs, const UtFortMat3<T>& rhs)
{
   UtFortMat3<T> result;
   const int     vmax = 8;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = lhs * rhs[j];

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
UtFortMat3<T> UtFortMat3<T>::operator/(T rhs) const
{
   UtFortMat3<T> result;

   for (int j = vmax; j >= 0; --j)
      result.val[j] = val[j] / rhs;

   return result;
}


// operator/  Divides a scaler by a matrix (Not a class member function, but
// a global function)
template<class T>
UtFortMat3<T> operator/(T lhs, const UtFortMat3<T>& rhs)
{
   UtFortMat3<T> result;
   const int     vmax = 8;

   for (int j = vmax; j >= 0; --j)
   {
      if (rhs[j] != 0.0F)
         result.val[j] = lhs / rhs[j];
   }

   return result;
}


/******************************************************************************/
/*                                                                            */
/*                        Mulitplication Functions                            */
/*                                                                            */
/******************************************************************************/


// Multiply a 3x1 vector by "this" 3x3 matrix, yielding a 3x1 vector
template<class T>
UtVec3f UtFortMat3<T>::Multiply(UtVec3f rhs) const
{
   UtVec3f result;

   result[0] = val[0] * rhs[0] + val[3] * rhs[1] + val[6] * rhs[2];

   result[1] = val[1] * rhs[0] + val[4] * rhs[1] + val[7] * rhs[2];

   result[2] = val[2] * rhs[0] + val[5] * rhs[1] + val[8] * rhs[2];

   return result;
}


// Multiply a 3x1 double vector by "this" 3x3 matrix
template<class T>
UtVec3d UtFortMat3<T>::Multiply(UtVec3d rhs) const
{
   UtVec3d result;

   result[0] = val[0] * rhs[0] + val[3] * rhs[1] + val[6] * rhs[2];

   result[1] = val[1] * rhs[0] + val[4] * rhs[1] + val[7] * rhs[2];

   result[2] = val[2] * rhs[0] + val[5] * rhs[1] + val[8] * rhs[2];

   return result;
}


// Multiply a 3x3 matrix and by "this" 3x3 matrix
template<class T>
UtFortMat3<T> UtFortMat3<T>::Multiply(UtFortMat3<T> rhs) const
{
   UtFortMat3<T> result;

   result.val[0] = val[0] * rhs.val[0] + val[3] * rhs.val[1] + val[6] * rhs.val[2];

   result.val[1] = val[1] * rhs.val[0] + val[4] * rhs.val[1] + val[7] * rhs.val[2];

   result.val[2] = val[2] * rhs.val[0] + val[5] * rhs.val[1] + val[8] * rhs.val[2];

   result.val[3] = val[0] * rhs.val[3] + val[3] * rhs.val[4] + val[6] * rhs.val[5];

   result.val[4] = val[1] * rhs.val[3] + val[4] * rhs.val[4] + val[7] * rhs.val[5];

   result.val[5] = val[2] * rhs.val[3] + val[5] * rhs.val[4] + val[8] * rhs.val[5];

   result.val[6] = val[0] * rhs.val[6] + val[3] * rhs.val[7] + val[6] * rhs.val[8];

   result.val[7] = val[1] * rhs.val[6] + val[4] * rhs.val[7] + val[7] * rhs.val[8];

   result.val[8] = val[2] * rhs.val[6] + val[5] * rhs.val[7] + val[8] * rhs.val[8];

   return result;
}

// Multiply a 3x1 float vector by "this" 3x3 Transposed matrix
template<class T>
UtVec3f UtFortMat3<T>::TransposeMultiply(UtVec3f rhs) const
{
   UtVec3f result;

   result[0] = val[0] * rhs[0] + val[1] * rhs[1] + val[2] * rhs[2];

   result[1] = val[3] * rhs[0] + val[4] * rhs[1] + val[5] * rhs[2];

   result[2] = val[6] * rhs[0] + val[7] * rhs[1] + val[8] * rhs[2];

   return result;
}


// Multiply a 3x1 double vector by "this" Transposed matrix
template<class T>
UtVec3d UtFortMat3<T>::TransposeMultiply(UtVec3d rhs) const
{
   UtVec3d result;

   result[0] = val[0] * rhs[0] + val[1] * rhs[1] + val[2] * rhs[2];

   result[1] = val[3] * rhs[0] + val[4] * rhs[1] + val[5] * rhs[2];

   result[2] = val[6] * rhs[0] + val[7] * rhs[1] + val[8] * rhs[2];

   return result;
}

// Multiply a 3x3 matrix and by "this" 3x3 Transposed matrix
template<class T>
UtFortMat3<T> UtFortMat3<T>::TransposeMultiply(UtFortMat3<T> rhs) const
{
   UtFortMat3<T> result;

   result.val[0] = val[0] * rhs.val[0] + val[1] * rhs.val[1] + val[2] * rhs.val[2];

   result.val[1] = val[3] * rhs.val[0] + val[4] * rhs.val[1] + val[5] * rhs.val[2];

   result.val[2] = val[6] * rhs.val[0] + val[7] * rhs.val[1] + val[8] * rhs.val[2];

   result.val[3] = val[0] * rhs.val[3] + val[1] * rhs.val[4] + val[2] * rhs.val[5];

   result.val[4] = val[3] * rhs.val[3] + val[4] * rhs.val[4] + val[5] * rhs.val[5];

   result.val[5] = val[6] * rhs.val[3] + val[7] * rhs.val[4] + val[8] * rhs.val[5];

   result.val[6] = val[0] * rhs.val[6] + val[1] * rhs.val[7] + val[2] * rhs.val[8];

   result.val[7] = val[3] * rhs.val[6] + val[4] * rhs.val[7] + val[5] * rhs.val[8];

   result.val[8] = val[6] * rhs.val[6] + val[7] * rhs.val[7] + val[8] * rhs.val[8];

   return result;
}


// Multiply a 3x1 float vector by this 3x3 matrix, yielding a 3x1 float vector
template<class T>
void UtFortMat3<T>::Multiply(UtVec3f& result, UtVec3f rhs) const
{
   result[0] = val[0] * rhs[0] + val[3] * rhs[1] + val[6] * rhs[2];

   result[1] = val[1] * rhs[0] + val[4] * rhs[1] + val[7] * rhs[2];

   result[2] = val[2] * rhs[0] + val[5] * rhs[1] + val[8] * rhs[2];

   return;
}


// Multiply a 3x1 double vector by "this" 3x3 matrix
template<class T>
void UtFortMat3<T>::Multiply(UtVec3d& result, UtVec3d rhs) const
{
   result[0] = val[0] * rhs[0] + val[3] * rhs[1] + val[6] * rhs[2];

   result[1] = val[1] * rhs[0] + val[4] * rhs[1] + val[7] * rhs[2];

   result[2] = val[2] * rhs[0] + val[5] * rhs[1] + val[8] * rhs[2];

   return;
}


// Multiply a 3x3 matrix and by "this" 3x3 matrix
template<class T>
void UtFortMat3<T>::Multiply(UtFortMat3<T>& result, UtFortMat3<T> rhs) const
{
   result.val[0] = val[0] * rhs.val[0] + val[3] * rhs.val[1] + val[6] * rhs.val[2];

   result.val[1] = val[1] * rhs.val[0] + val[4] * rhs.val[1] + val[7] * rhs.val[2];

   result.val[2] = val[2] * rhs.val[0] + val[5] * rhs.val[1] + val[8] * rhs.val[2];

   result.val[3] = val[0] * rhs.val[3] + val[3] * rhs.val[4] + val[6] * rhs.val[5];

   result.val[4] = val[1] * rhs.val[3] + val[4] * rhs.val[4] + val[7] * rhs.val[5];

   result.val[5] = val[2] * rhs.val[3] + val[5] * rhs.val[4] + val[8] * rhs.val[5];

   result.val[6] = val[0] * rhs.val[6] + val[3] * rhs.val[7] + val[6] * rhs.val[8];

   result.val[7] = val[1] * rhs.val[6] + val[4] * rhs.val[7] + val[7] * rhs.val[8];

   result.val[8] = val[2] * rhs.val[6] + val[5] * rhs.val[7] + val[8] * rhs.val[8];

   return;
}


// Multiply a 3x1 float vector by this 3x3 Transposed matrix
template<class T>
void UtFortMat3<T>::TransposeMultiply(UtVec3f& result, UtVec3f rhs) const
{
   result[0] = val[0] * rhs[0] + val[1] * rhs[1] + val[2] * rhs[2];

   result[1] = val[3] * rhs[0] + val[4] * rhs[1] + val[5] * rhs[2];

   result[2] = val[6] * rhs[0] + val[7] * rhs[1] + val[8] * rhs[2];

   return;
}


// Multiply a 3x1 double vector by "this" 3x3 Transposed matrix
template<class T>
void UtFortMat3<T>::TransposeMultiply(UtVec3d& result, UtVec3d rhs) const
{
   result[0] = val[0] * rhs[0] + val[1] * rhs[1] + val[2] * rhs[2];

   result[1] = val[3] * rhs[0] + val[4] * rhs[1] + val[5] * rhs[2];

   result[2] = val[6] * rhs[0] + val[7] * rhs[1] + val[8] * rhs[2];

   return;
}


// Multiply a 3x3 matrix and by "this" 3x3 Transposed matrix
template<class T>
void UtFortMat3<T>::TransposeMultiply(UtFortMat3<T>& result, UtFortMat3<T> rhs) const
{
   result.val[0] = val[0] * rhs.val[0] + val[1] * rhs.val[1] + val[2] * rhs.val[2];

   result.val[1] = val[3] * rhs.val[0] + val[4] * rhs.val[1] + val[5] * rhs.val[2];

   result.val[2] = val[6] * rhs.val[0] + val[7] * rhs.val[1] + val[8] * rhs.val[2];

   result.val[3] = val[0] * rhs.val[3] + val[1] * rhs.val[4] + val[2] * rhs.val[5];

   result.val[4] = val[3] * rhs.val[3] + val[4] * rhs.val[4] + val[5] * rhs.val[5];

   result.val[5] = val[6] * rhs.val[3] + val[7] * rhs.val[4] + val[8] * rhs.val[5];

   result.val[6] = val[0] * rhs.val[6] + val[1] * rhs.val[7] + val[2] * rhs.val[8];

   result.val[7] = val[3] * rhs.val[6] + val[4] * rhs.val[7] + val[5] * rhs.val[8];

   result.val[8] = val[6] * rhs.val[6] + val[7] * rhs.val[7] + val[8] * rhs.val[8];

   return;
}


/******************************************************************************/
/*                                                                            */
/*                             Misc Functions                                 */
/*                                                                            */
/******************************************************************************/


// Get a Transpose copy of the matrix
template<class T>
UtFortMat3<T> UtFortMat3<T>::GetTranspose() const
{
   UtFortMat3<T> result;

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
void UtFortMat3<T>::SetTranspose(const UtFortMat3<T>& rhs)
{
   val[0] = rhs.val[0];
   val[1] = rhs.val[3];
   val[2] = rhs.val[6];
   val[3] = rhs.val[1];
   val[4] = rhs.val[4];
   val[5] = rhs.val[7];
   val[6] = rhs.val[2];
   val[7] = rhs.val[5];
   val[8] = rhs.val[8];
}


// Transpose the current matrix
template<class T>
void UtFortMat3<T>::Transpose()
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
