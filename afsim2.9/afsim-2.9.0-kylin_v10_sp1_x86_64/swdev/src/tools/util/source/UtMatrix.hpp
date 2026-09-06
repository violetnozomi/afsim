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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTMATRIX_HPP
#define UTMATRIX_HPP

#include "ut_export.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "UtException.hpp"
#include "UtMath.hpp"

//#define UTMATRIX_CHECK_BOUNDS

class UtMatrix_IncompatibleSizeError : public UtException
{
public:
   UtMatrix_IncompatibleSizeError();
};
class UtMatrix_OutOfBoundsError : public UtException
{
public:
   UtMatrix_OutOfBoundsError();
};
class UtMatrix_SingularMatrixError : public UtException
{
public:
   UtMatrix_SingularMatrixError();
};
class UtMatrix_NonConvergenceError : public UtException
{
public:
   UtMatrix_NonConvergenceError();
};

//! A generalized matrix class of m by n elements.
//! Supported typedefs are UtMatrixf for single precision floating point values and
//! UtMatrixd for double-precision floating point values.
template<typename T>
class UtMatrix
{
public:
   using IncompatibleSizeError = UtMatrix_IncompatibleSizeError;
   using OutOfBoundsError      = UtMatrix_OutOfBoundsError;
   using SingularMatrixError   = UtMatrix_SingularMatrixError;
   using NonConvergenceError   = UtMatrix_NonConvergenceError;

   //! Construct a matrix of the specified size.
   //! @param aRows The number of rows in the matrix.
   //! @param aCols The number of columns in the matrix.
   //! @note The default number of columns is 1, which indicates a vector (e.g., UtMatrixd myVec(3) ).
   explicit UtMatrix(unsigned int aRows, unsigned int aCols = 1)
      : mMatrix(aRows * aCols, static_cast<T>(0.0))
      , mRows(aRows)
      , mCols(aCols)
   {
   }

   //! Default constructor.  If this constructor is used,
   //! the SetSize() method MUST be called prior to actually
   //! setting any data in the matrix.
   UtMatrix()
      : mMatrix()
      , mRows(0)
      , mCols(0)
   {
   }

   //! Destructor.
   virtual ~UtMatrix() = default;

   //! Return the element at the specified row and column (const version).
   const T& operator()(unsigned int aRow, unsigned int aCol) const
   {
#ifdef UTMATRIX_CHECK_BOUNDS
      if ((aRow >= mRows) || (aCol >= mCols))
      {
         throw OutOfBoundsError();
      }
#endif
      return mMatrix[(aRow * mCols) + aCol];
   }

   //! Return a reference to the element at the specified row and column.
   T& operator()(unsigned int aRow, unsigned int aCol)
   {
#ifdef UTMATRIX_CHECK_BOUNDS
      if ((aRow >= mRows) || (aCol >= mCols))
      {
         throw OutOfBoundsError();
      }
#endif
      return mMatrix[(aRow * mCols) + aCol];
   }

   //! Return the element at the specified row (column 0);
   //! used for single-dimensional vectors.
   T& operator()(unsigned int aRow)
   {
#ifdef UTMATRIX_CHECK_BOUNDS
      if (aRow >= mRows)
      {
         throw OutOfBoundsError();
      }
#endif
      return mMatrix[aRow];
   }

   //! Return the element at the specified row (column 0);
   //! used for single-dimensional vectors
   //! (const version).
   const T& operator()(unsigned int aRow) const
   {
#ifdef UTMATRIX_CHECK_BOUNDS
      if (aRow >= mRows)
      {
         throw OutOfBoundsError();
      }
#endif
      return mMatrix[aRow];
   }

   //! Return the number of rows in the matrix.
   unsigned int GetRows() const { return mRows; }

   //! Return the number of columns in the matrix.
   unsigned int GetCols() const { return mCols; }

   //! Return the total number of elements in the matrix
   //! (useful for single-dimensional arrays).
   unsigned int GetSize() const { return (mRows * mCols); }

   //! Return whether the matrix is "null"; i.e., has 0 rows and columns.
   bool IsNull() const { return ((mRows == 0) && (mCols == 0)); }

   void LoadIdentity();

   void Fill(const T& aVal);

   void Fill(const T& aVal, unsigned int aBeginRow, unsigned int aBeginCol, unsigned int aNumRows, unsigned int aNumCols);

   void FillRow(unsigned int aRow, const T& aVal);

   void FillColumn(unsigned int aCol, const T& aVal);

   void FillDiagonal(const T& aVal);

   void GetDiagonal(UtMatrix<T>& aDiagonal) const;

   const T& Minimum() const;

   const T& Maximum() const;

   UtMatrix<T>& Add(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs);

   UtMatrix<T>& Add(const UtMatrix<T>& aLhs, const T& aRhs);

   UtMatrix<T>& Subtract(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs);

   UtMatrix<T>& Subtract(const UtMatrix<T>& aLhs, const T& aRhs);

   UtMatrix<T>& Multiply(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs);

   UtMatrix<T>& Multiply(const UtMatrix<T>& aLhs, const T& aRhs);

   UtMatrix<T>& MultiplyArray(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs);

   UtMatrix<T>& Divide(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs);

   UtMatrix<T>& Divide(const UtMatrix<T>& aLhs, const T& aRhs);

   UtMatrix<T>& DivideArray(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs);

   UtMatrix<T>& MultiplyDiagonal(const UtMatrix<T>& aLhs, const T& aRhs, unsigned aStartIndex, unsigned aEndIndex);

   UtMatrix<T>& MultiplyDiagonal(const UtMatrix<T>& aLhs, const T& aRhs);

   UtMatrix<T>& Power(const UtMatrix<T>& aLhs, const T& aPower);

   UtMatrix<T>& Absolute(const UtMatrix<T>& aMatrix);

   UtMatrix<T>& CholeskyDecomposition(const UtMatrix<T>& aMatrix, bool& aSuccess);

   UtMatrix<T>& Transpose(const UtMatrix<T>& aSrc);

   UtMatrix<T>& Invert(const UtMatrix<T>& aSrc);

   UtMatrix<T>& Invert2x2(const UtMatrix<T>& aSrc);

   UtMatrix<T>& PseudoInvert(const UtMatrix<T>& aMatrix);

   bool IsPositiveDefinite() const;

   T Trace() const;

   void Invert();

   void Invert2x2();

   bool PseudoInvert();

   void Symmetrize();

   void CopyFrom(const T* aVector);

   void CopyFrom(const T* aVector, unsigned aNumElements, unsigned aStartElement = 0);

   void CopyFrom(const UtMatrix<T>& aSrc, unsigned aBeginRow, unsigned aBeginCol, unsigned aSizeRows, unsigned aSizeCols);

   void Crop(const UtMatrix<T>& aSrc, unsigned aBeginRow, unsigned aBeginCol, unsigned aSizeRows, unsigned aSizeCols);

   void CopyTo(T* aVector);

   void CopyTo(T* aVector, unsigned aNumElements, unsigned aStartElement = 0) const;

   void SetDiagonal(T* aVector);

   void SetSize(unsigned int aRows, unsigned int aCols = 1);

   void Resize(unsigned int aRows, unsigned int aCols);

   void SwapIndices(unsigned int aSwap1, unsigned int aSwap2);

   UtMatrix<T> operator+(const UtMatrix<T>& aRhs) const
   {
      UtMatrix<T> output(mRows, mCols);
      return output.Add(*this, aRhs);
   }

   UtMatrix<T> operator+(const T& aRhs) const
   {
      UtMatrix<T> output(mRows, mCols);
      return output.Add(*this, aRhs);
   }

   friend UtMatrix<T> operator+(const T& aLhs, const UtMatrix<T>& aRhs) { return aRhs.operator+(aLhs); }

   UtMatrix<T>& operator+=(const UtMatrix<T>& aRhs) { return Add(*this, aRhs); }

   UtMatrix<T> operator-(const UtMatrix<T>& aRhs) const
   {
      UtMatrix<T> output(mRows, mCols);
      return output.Subtract(*this, aRhs);
   }

   UtMatrix<T> operator-(const T& aRhs) const
   {
      UtMatrix<T> output(mRows, mCols);
      return output.Subtract(*this, aRhs);
   }

   friend UtMatrix<T> operator-(const T& aLhs, const UtMatrix<T>& aRhs) { return aRhs.operator-(aLhs); }

   UtMatrix<T>& operator-=(const UtMatrix<T>& aRhs) { return Subtract(*this, aRhs); }

   UtMatrix<T> operator*(const UtMatrix<T>& aRhs) const
   {
      UtMatrix<T> output(mRows, aRhs.mCols);
      return output.Multiply(*this, aRhs);
   }

   UtMatrix<T> operator*(const T& aRhs) const
   {
      UtMatrix<T> output(mRows, mCols);
      return output.Multiply(*this, aRhs);
   }

   friend UtMatrix<T> operator*(const T& aLhs, const UtMatrix<T>& aRhs) { return aRhs.operator*(aLhs); }

   UtMatrix<T>& operator*=(const T& aRhs) { return Multiply(*this, aRhs); }

   bool operator==(const UtMatrix<T>& aRHS) const;
   bool operator!=(const UtMatrix<T>& aRHS) const;

   template<typename Type>
   friend std::ostream& operator<<(std::ostream& aOut, const UtMatrix<Type>& aMatrix);

   void Stream(std::ostream& aOut, bool aOutputLineContinuation = false) const;

   //! For XIO (de)serialization.
   template<typename AR>
   void Serialize(AR& aAr)
   {
      aAr& mMatrix& mRows& mCols;
   }

   //! Reduce 'this' matrix to a set of eigenvectors in the columns.
   //! @note Only use this version if you are only interested in the eigenvectors and not the eigenvalues.
   UtMatrix<T>& ReduceToEigenvectors()
   {
      UtMatrix<T> diagonalElements;
      UtMatrix<T> offDiagonalElements;
      ReduceToTridiagonal(diagonalElements, offDiagonalElements);
      ReduceToEigenvectorsFromTridiagonal(diagonalElements, offDiagonalElements);
      return (*this);
   }

   //! Reduce 'this' matrix to a set of eigenvectors in the columns, and return the eigenvalues in a parameter
   //! @param aEignvalues  A vector into which the eignvalues will be placed.
   //! @return This reduced, modified matrix.
   UtMatrix<T>& ReduceToEigenvectors(UtMatrix<T>& aEigenvalues)
   {
      UtMatrix<T> offDiagonalElements;
      ReduceToTridiagonal(aEigenvalues, offDiagonalElements);
      ReduceToEigenvectorsFromTridiagonal(aEigenvalues, offDiagonalElements);
      return (*this);
   }

   double GetConditionNumber() const;

   static int SingularValueDecomposition(const UtMatrix<T>& A, UtMatrix<T>& S, UtMatrix<T>& U, UtMatrix<T>& V);

   //!@note The following method is used in the ReduceToEigenvectors methods, above,
   //! but probably has limited utility on its own.
   UtMatrix<T>& ReduceToTridiagonal(UtMatrix<T>& aDiagonalElements, UtMatrix<T>& aOffDiagonalElements);

   //!@note The following method is used in the ReduceToEigenvectors methods, above,
   //! but probably has limited utility on its own.
   const UtMatrix<T>& ReduceToEigenvectorsFromTridiagonal(UtMatrix<T>& aDiagonalElements,
                                                          UtMatrix<T>& aOffDiagonalElements); // const;

   //! Reduce 'this' matrix to a set of eigenvectors in the columns.
   //! @note Deprecated.  Please use ReduceToEigenvectors (small "V").
   UtMatrix<T>& ReduceToEigenVectors() { return ReduceToEigenvectors(); }

   //! Reduce 'this' matrix to a set of eigenvectors in the columns, and rerun the eigenvalues in a parameter
   //! @param aEignvalues  A vector into which the eignvalues will be placed.
   //! @param aOffDiagonalElements A working matrix.
   //! @return This reduced, modified matrix.
   //! @note Deprecated. Please use the single parameter version instead.
   UtMatrix<T>& ReduceToEigenVectors(UtMatrix<T>& aEigenvalues, UtMatrix<T>& aOffDiagonalElements)
   {
      ReduceToTridiagonal(aEigenvalues, aOffDiagonalElements);
      ReduceToEigenvectorsFromTridiagonal(aEigenvalues, aOffDiagonalElements);
      return (*this);
   }

protected:
private:
   std::vector<T> mMatrix;
   unsigned int   mRows;
   unsigned int   mCols;
};

template<typename T>
inline void UtMatrix<T>::LoadIdentity()
{
#if !defined(NDEBUG)
   if (mRows != mCols)
   {
      throw IncompatibleSizeError();
   }
#endif
   size_t n = mMatrix.size();
   for (unsigned int i = 0; i < n; ++i)
   {
      mMatrix[i] = static_cast<T>(0.0);
   }
   for (unsigned int j = 0; j < mRows; ++j)
   {
      mMatrix[j * mCols + j] = static_cast<T>(1.0);
   }
}

//! Compute this = Matrix + Matrix
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side matrix.
//! @returns *this.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Add(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs)
{
#if !defined(NDEBUG)
   if ((aLhs.mRows != aRhs.mRows) || (aLhs.mCols != aRhs.mCols))
   {
      throw IncompatibleSizeError();
   }
#endif
   SetSize(aLhs.mRows, aLhs.mCols);

   size_t n = aLhs.mMatrix.size();
   for (size_t i = 0; i < n; ++i)
   {
      mMatrix[i] = aLhs.mMatrix[i] + aRhs.mMatrix[i];
   }
   return *this;
}

//! Compute this = Matrix + Scalar
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side constant.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Add(const UtMatrix<T>& aLhs, const T& aRhs)
{
   SetSize(aLhs.mRows, aLhs.mCols);

   size_t n = aLhs.mMatrix.size();
   for (size_t i = 0; i < n; ++i)
   {
      mMatrix[i] = aLhs.mMatrix[i] + aRhs;
   }
   return *this;
}

//! Compute this = Matrix - Matrix
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side matrix.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Subtract(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs)
{
#if !defined(NDEBUG)
   if ((aLhs.mRows != aRhs.mRows) || (aLhs.mCols != aRhs.mCols))
   {
      throw IncompatibleSizeError();
   }
#endif
   SetSize(aLhs.mRows, aLhs.mCols);

   size_t n = aLhs.mMatrix.size();
   for (size_t i = 0; i < n; ++i)
   {
      mMatrix[i] = aLhs.mMatrix[i] - aRhs.mMatrix[i];
   }
   return *this;
}

//! Computes this = Matrix + Scalar.
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side constant.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Subtract(const UtMatrix<T>& aLhs, const T& aRhs)
{
   SetSize(aLhs.mRows, aLhs.mCols);

   size_t n = aLhs.mMatrix.size();
   for (size_t i = 0; i < n; ++i)
   {
      mMatrix[i] = aLhs.mMatrix[i] - aRhs;
   }
   return *this;
}

//! Multiplies the two provided matrices.
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side matrix.
//! @note The same matrix cannot be used both for the result and one of the parameters.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Multiply(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs)
{
#if !defined(NDEBUG)
   if ((aLhs.mCols != aRhs.mRows))
   {
      throw IncompatibleSizeError();
   }
#endif
   SetSize(aLhs.mRows, aRhs.mCols);

   for (unsigned int i = 0; i < aLhs.mRows; ++i)
   {
      for (unsigned int j = 0; j < aRhs.mCols; ++j)
      {
         T sum = 0.0;
         for (unsigned int k = 0; k < aRhs.mRows; ++k)
         {
            sum += aLhs(i, k) * aRhs(k, j);
         }
         (*this)(i, j) = sum;
      }
   }
   return *this;
}

//! Compute this = Matrix * constant.
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side constant.
//! @note This multiply can be used with the same matrix for the result and the argument.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Multiply(const UtMatrix<T>& aLhs, const T& aRhs)
{
   SetSize(aLhs.mRows, aLhs.mCols);

   size_t n = aLhs.mMatrix.size();
   for (unsigned int i = 0; i < n; ++i)
   {
      mMatrix[i] = aLhs.mMatrix[i] * aRhs;
   }
   return *this;
}

//! Perform an array-based multiplication, (multiply every element of one array times
//! every element of the other.
//! @param aLhs    The left-hand side array.
//! @param aRhs    The right-hand side array.
//! @note The arrays must be the same size.
template<typename T>
UtMatrix<T>& UtMatrix<T>::MultiplyArray(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs)
{
#if !defined(NDEBUG)
   if ((aLhs.mRows != aRhs.mRows) || (aLhs.mCols != aRhs.mCols))
   {
      throw IncompatibleSizeError();
   }
#endif
   SetSize(aLhs.mRows, aLhs.mCols);

   for (unsigned int i = 0; i < mRows; ++i)
   {
      for (unsigned int j = 0; j < mCols; ++j)
      {
         (*this)(i, j) = aLhs(i, j) * aRhs(i, j);
      }
   }
   return *this;
}

//! "Divides" the two provided matrices (aLhs * Inv(aRhs))
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side matrix.
//! @note The same matrix cannot be used both for the result and one of the parameters.
//! @note Matrix 'divide' is nonstandard notation
template<typename T>
UtMatrix<T>& UtMatrix<T>::Divide(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs)
{
   UtMatrix<T> aRhsInverse(aRhs);
   aRhsInverse.Invert();
   return Multiply(aLhs, aRhsInverse);
}

//! Compute this = Matrix / constant.
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side constant.
//! @note This multiply can be used with the same matrix for the result and the argument.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Divide(const UtMatrix<T>& aLhs, const T& aRhs)
{
   SetSize(aLhs.mRows, aLhs.mCols);

   size_t n = aLhs.mMatrix.size();
   for (size_t i = 0; i < n; ++i)
   {
      mMatrix[i] = aLhs.mMatrix[i] / aRhs;
   }
   return *this;
}

//! Perform an array-based division, (every element of one array divided by
//! every element of the other.
//! @param aLhs    The left-hand side array (numerator).
//! @param aRhs    The right-hand side array (denominator).
//! @note The arrays must be the same size.
template<typename T>
UtMatrix<T>& UtMatrix<T>::DivideArray(const UtMatrix<T>& aLhs, const UtMatrix<T>& aRhs)
{
#if !defined(NDEBUG)
   if ((aLhs.mRows != aRhs.mRows) || (aLhs.mCols != aRhs.mCols))
   {
      throw IncompatibleSizeError();
   }
#endif
   SetSize(aLhs.mRows, aLhs.mCols);

   for (unsigned int i = 0; i < mRows; ++i)
   {
      for (unsigned int j = 0; j < mCols; ++j)
      {
         (*this)(i, j) = aLhs(i, j) / aRhs(i, j);
      }
   }
   return *this;
}

//! Multiply only diagonal terms by a constant value.
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side constant.
//! @note MultiplyDiagonal can be used with the same matrix for the result and the argument.
template<typename T>
UtMatrix<T>& UtMatrix<T>::MultiplyDiagonal(const UtMatrix<T>& aLhs, const T& aRhs)
{
   return MultiplyDiagonal(aLhs, aRhs, 0, aLhs.mRows);
}

//! Multiply only diagonal terms by a constant value.
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side constant.
//! @param aStartIndex (optional) The start index, along the diagonal, to use
//!  (for example, aStartIndex of 2 indicates the matrix index (2,2) ).
//! @param aEndIndex (optional) The end index, along the diagonal, to use
//!  (for example, aStartIndex of 2 indicates the matrix index (2,2) ).
//! @note MultiplyDiagonal can be used with the same matrix for the result and the argument.
template<typename T>
UtMatrix<T>& UtMatrix<T>::MultiplyDiagonal(const UtMatrix<T>& aLhs, const T& aRhs, unsigned aStartIndex, unsigned aEndIndex)
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if (aLhs.mRows != aLhs.mCols)
   {
      throw IncompatibleSizeError();
   }
   if (aEndIndex > aLhs.mRows)
   {
      throw OutOfBoundsError();
   }
#endif

   SetSize(aLhs.mRows, aLhs.mCols);

   for (unsigned int i = aStartIndex; i < aEndIndex; ++i)
   {
      unsigned int index = i * aLhs.mRows + i;
      mMatrix[index]     = aLhs.mMatrix[index] * aRhs;
   }
   return *this;
}

//! Compute this = Matrix * constant.
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side constant.
//! @note This multiply can be used with the same matrix for the result and the argument.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Power(const UtMatrix<T>& aLhs, const T& aPower)
{
   SetSize(aLhs.mRows, aLhs.mCols);

   size_t n = aLhs.mMatrix.size();
   for (unsigned int i = 0; i < n; ++i)
   {
      mMatrix[i] = std::pow(aLhs.mMatrix[i], aPower);
   }
   return *this;
}

//! Compute this = Matrix * constant.
//! @param aLhs    The left-hand side matrix.
//! @param aRhs    The right-hand side constant.
//! @note This multiply can be used with the same matrix for the result and the argument.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Absolute(const UtMatrix<T>& aMatrix)
{
   SetSize(aMatrix.mRows, aMatrix.mCols);

   size_t n = aMatrix.mMatrix.size();
   for (unsigned int i = 0; i < n; ++i)
   {
      mMatrix[i] = std::abs(aMatrix.mMatrix[i]);
   }
   return *this;
}

//! Given a positive definite symmetric matrix
//!                                               T
//! Construct the Cholesky decomposition A = L * L
//! Cholesky factor L is returned in the lower triangle
template<typename T>
UtMatrix<T>& UtMatrix<T>::CholeskyDecomposition(const UtMatrix<T>& aSrc, bool& aSuccess)
{
   // Set the default return value
   aSuccess = true;

   // Quick return if possible
   if ((aSrc.GetRows() == 0) || (aSrc.GetCols() == 0) || (aSrc.GetRows() != aSrc.GetCols()))
   {
      aSuccess = false;
      return *this;
   }

   // Set the size of this matrix based on input
   SetSize(aSrc.GetRows(), aSrc.GetCols());

   (*this).CopyFrom(aSrc, 0, 0, aSrc.GetRows(), aSrc.GetCols());

   // Used as a limit in the "for" loops
   int n(aSrc.GetRows());

   for (int i = 0; i < n; ++i)
   {
      for (int j = i; j < n; ++j)
      {
         double sum((*this)(i, j));
         for (int k = i - 1; k >= 0; --k)
         {
            sum -= (*this)(i, k) * (*this)(j, k);
         }
         if (i == j)
         {
            if (sum <= 0.0)
            {
               aSuccess = false;
               return *this;
            }
            (*this)(j, i) = (T)sqrt(sum);
         }
         else
         {
            (*this)(j, i) = (T)(sum / (*this)(i, i));
         }
      }
   }

   // Clearing upper since solution is in lower
   for (int i = 0; i < n; ++i)
   {
      for (int j = 0; j < n; ++j)
      {
         if (j > i)
         {
            (*this)(i, j) = 0.0;
         }
      }
   }

   return *this;
}

//! Compute this = transpose(Matrix);
template<typename T>
UtMatrix<T>& UtMatrix<T>::Transpose(const UtMatrix<T>& aSrc)
{
   SetSize(aSrc.mCols, aSrc.mRows);

   for (unsigned int row = 0; row < aSrc.mRows; ++row)
   {
      for (unsigned int col = 0; col < aSrc.mCols; ++col)
      {
         (*this)(col, row) = aSrc(row, col);
      }
   }
   return *this;
}

//! Return the trace (sum of the diagonal terms) of the matrix.
//! The matrix must be square.
template<typename T>
T UtMatrix<T>::Trace() const
{
   // Make sure the matrix is square.
   if (mRows != mCols)
   {
      throw IncompatibleSizeError();
   }
   unsigned int i;
   T            trace = 0.0;
   for (i = 0; i < mRows; ++i)
   {
      trace += mMatrix[(i * mRows) + i];
   }
   return trace;
}

//! Enforce the symmetry of a symmetric matrix
template<typename T>
void UtMatrix<T>::Symmetrize()
{
   // Make sure the matrix is square.
   if (mRows != mCols)
   {
      throw IncompatibleSizeError();
   }
   for (unsigned i = 0; i < mRows; ++i)
   {
      for (unsigned j = 0; j < mCols; ++j)
      {
         (*this)(j, i) = (*this)(i, j);
      }
   }
   // todo check for positive definite
}

//! Compute the inverse of the matrix and assign in place.
//! The matrix must be square.
template<typename T>
void UtMatrix<T>::Invert()
{
   // Make sure the matrix is square.
   if (mRows != mCols)
   {
      throw IncompatibleSizeError();
   }

   // Copy the source matrix to the work area.
   UtMatrix<T> curMat(*this);
   LoadIdentity();

   unsigned int i;
   unsigned int j;
   unsigned int k;
   for (k = 0; k < mCols; ++k)
   {
      // Find the pivot value
      unsigned int pivotRow       = 0;
      T            pivotValue     = 0.0;
      T            tempPivotValue = 0.0;
      for (i = k; i < mRows; ++i)
      {
         T x = curMat(i, k);
         if (std::abs(x) > std::abs(tempPivotValue))
         {
            pivotRow       = i;
            pivotValue     = x;
            tempPivotValue = x;
         }
      }

      if (pivotValue == 0.0)
      {
         throw SingularMatrixError();
      }

      if (pivotRow == k)
      {
         for (j = k; j < mCols; ++j)
         {
            curMat(k, j) /= pivotValue;
         }
         for (j = 0; j < mCols; ++j)
         {
            (*this)(k, j) /= pivotValue;
         }
      }
      else
      {
         for (j = k; j < mCols; ++j)
         {
            T x                 = curMat(k, j);
            curMat(k, j)        = curMat(pivotRow, j) / pivotValue;
            curMat(pivotRow, j) = x;
         }
         for (j = 0; j < mCols; ++j)
         {
            T x                  = (*this)(k, j);
            (*this)(k, j)        = (*this)(pivotRow, j) / pivotValue;
            (*this)(pivotRow, j) = x;
         }
      }

      for (i = 0; i < mRows; ++i)
      {
         if (i != k)
         {
            for (j = k + 1; j < mCols; ++j)
            {
               curMat(i, j) -= curMat(i, k) * curMat(k, j);
            }
            for (j = 0; j < mCols; ++j)
            {
               (*this)(i, j) -= curMat(i, k) * (*this)(k, j);
            }
         }
      }
   }
}

//! Calculate the Pseudo-Inverse, or
//! V * S^(-1)  * Ut, computed using Singular Value Decomposition.
//! Matrix to be inverted has M rows and N cols, where M>=N.
//! Note that this method can return a solution for an overdetermined
//! set of equations (the matrix does not have to be square).
template<typename T>
bool UtMatrix<T>::PseudoInvert()
{
   unsigned    m = GetRows();
   unsigned    n = GetCols();
   UtMatrix<T> V(n, n);
   UtMatrix<T> S(m);
   UtMatrix<T> U(m, n);
   int         error = SingularValueDecomposition(*this, S, U, V);
   bool        ok    = (error == 0);

   if (ok)
   {
      UtMatrix<T> Sinv_Ut;
      Sinv_Ut.Transpose(U);

      // S only has diagonal elements, so the inverse consists of the inverse of those elements
      // To multiply by S^-1 multiply all row elements "i" of Ut by the inverse of S(i)
      for (unsigned i = 0; i < n; ++i)
      {
         T sigma    = S(i);
         T sigmaInv = 0.0;
         if (sigma != 0.0)
         {
            sigmaInv = (T)1.0 / sigma;
         }
         for (unsigned j = 0; j < m; ++j)
         {
            Sinv_Ut(i, j) *= sigmaInv;
         }
      }

      // Finally multiply by V and store the result in "this."
      Multiply(V, Sinv_Ut);
   }

   return ok;
}

//! Compute The pseudo-inverse of a given matrix and assign it to "this."
//! The matrix need not be square, but the number of rows must be
//! greater than the number of columns (overdetermined solution).
//! "This" is also returned in a reference but need not be used. Example:
//! \code
//! UtMatrixd mat1(2,2), mat2(2,2)
//! UtMatrixd result;
//! result.PseudoInvert(mat1 + mat2);  // "result" contains the pseudo-inverse of mat1+mat2.
//! \endcode
//! @param aMatrix The matrix to be inverted.
//! @note In the rare case that a complete solution is not found, no exception
//! will be thrown.  That is because in some cases the solution is still
//! "good enough."  In order to have more control, use the
//! UtMatrix::SingularValueDecomposition routine directly.
template<typename T>
UtMatrix<T>& UtMatrix<T>::PseudoInvert(const UtMatrix<T>& aMatrix)
{
   *this = aMatrix;
   PseudoInvert();
   return *this;
}

//! Compute the inverse of a 2x2 matrix and assign in place.
//! The matrix must be a 2x2 square array.
template<typename T>
void UtMatrix<T>::Invert2x2()
{
   // Make sure the matrix is square.
   if (!((mRows == 2) && (mCols == 2)))
   {
      throw IncompatibleSizeError();
   }
   T a           = mMatrix[0];
   T b           = mMatrix[1];
   T c           = mMatrix[2];
   T d           = mMatrix[3];
   T scaleFactor = (a * d - b * c);
   if (scaleFactor == 0.0)
   {
      throw SingularMatrixError();
   }
   else
   {
      mMatrix[0] = d / scaleFactor;
      mMatrix[1] = -b / scaleFactor;
      mMatrix[2] = -c / scaleFactor;
      mMatrix[3] = a / scaleFactor;
   }
}

//! Compute The inverse of a given matrix and assign it to "this."
//! The matrix sizes of "this" and the parameter must be identical,
//! and the matrix must be square.
//! "This" is also returned in a reference but need not be used. Example:
//! \code
//! UtMatrixd mat1(2,2), mat2(2,2)
//! UtMatrixd result;
//! result.Invert(mat1 + mat2);  // "result" contains the inverse of mat1+mat2.
//! \endcode
//! @param aSrc The matrix to be inverted.
//! Algorithm from GlMatrix (Boeing).
template<typename T>
UtMatrix<T>& UtMatrix<T>::Invert(const UtMatrix<T>& aSrc)
{
   *this = aSrc;
   Invert();
   return *this;
}

//! Compute The inverse of a given matrix and assign it to "this."
//! The matrix sizes of "this" and the parameter must be identical,
//! and the matrix must be 2x2.
//! "This" is also returned in a reference but need not be used. Example:
//! \code
//! UtMatrixd mat1(2,2), mat2(2,2)
//! UtMatrixd result;
//! result.Invert(mat1 + mat2);  // "result" contains the inverse of mat1+mat2.
//! \endcode
//! @param aSrc The matrix to be inverted.
template<typename T>
UtMatrix<T>& UtMatrix<T>::Invert2x2(const UtMatrix<T>& aSrc)
{
   *this = aSrc;
   Invert2x2();
   return *this;
}

//! Determines if the provided matrix is not equal to the current matrix.
//! @param The right-hand side matrix.
//! @return Returns true if not equal.
template<typename T>
bool UtMatrix<T>::operator!=(const UtMatrix<T>& aRhs) const
{
   return !(*this == aRhs);
}

//! Determines if the provided matrix is equal to the current matrix.
//! @param The right-hand side matrix.
//! @return Returns true if equal.
template<typename T>
bool UtMatrix<T>::operator==(const UtMatrix<T>& aRhs) const
{
   static const double epsilon = 1.0e-10;

   if ((mRows != aRhs.mRows) || (mCols != aRhs.mCols))
   {
      return false;
   }

   bool   same = true;
   size_t n    = mMatrix.size();
   for (size_t i = 0; i < n; ++i)
   {
      if (std::abs(mMatrix[i] - aRhs.mMatrix[i]) > epsilon)
      {
         same = false;
         break;
      }
   }
   return same;
}

//! A human-readable output method.
template<typename T>
void UtMatrix<T>::Stream(std::ostream& aOut,
                         bool          aOutputLineContinuation) const // = false
{
   aOut << "  { ";
   std::streamsize width     = aOut.width();
   std::streamsize precision = aOut.precision();
   for (unsigned i = 0; i < mRows; ++i)
   {
      for (unsigned j = 0; j < mCols; ++j)
      {
         aOut.width(10);
         aOut.precision(25);
         aOut << mMatrix[(i * mCols) + j] << " ";
      }
      if (i != (mRows - 1))
      {
         if (aOutputLineContinuation)
         {
            aOut << " \\";
         }
         aOut << "\n    ";
      }
      else
      {
         aOut << "  } ";
      }
   }
   aOut.width(width);
   aOut.precision(precision);
}

// friend
template<typename Type>
std::ostream& operator<<(std::ostream& aOut, const UtMatrix<Type>& aMatrix)
{
   aMatrix.Stream(aOut);
   return aOut;
}

//! Fill all elements in the matrix with a constant value.
template<typename T>
void UtMatrix<T>::Fill(const T& aVal)
{
   for (unsigned i = 0; i < mRows; ++i)
   {
      for (unsigned j = 0; j < mCols; ++j)
      {
         (*this)(i, j) = aVal;
      }
   }
}

//! Generalized Fill method.
//!@param aVal The value with which to fill the specified elements.
//!@param aBeginRow The starting row to fill (zero indexed).
//!@param aBeingCol The starting colum to fill (zero indexed).
//!@param aSizeRows The number of rows to fill.
//!@param aSizeCols The number of colums to fill.
template<typename T>
void UtMatrix<T>::Fill(const T& aVal, unsigned int aBeginRow, unsigned int aBeginCol, unsigned int aSizeRows, unsigned int aSizeCols)
{
   unsigned endRow = aBeginRow + aSizeRows - 1;
   unsigned endCol = aBeginCol + aSizeCols - 1;
   for (unsigned i = aBeginRow; i <= endRow; ++i)
   {
      for (unsigned j = aBeginCol; j <= endCol; ++j)
      {
         mMatrix[i * mCols + j] = aVal;
      }
   }
}

//! Fill all diagonal elements with a specified value.
template<typename T>
void UtMatrix<T>::FillDiagonal(const T& aVal)
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if (mRows != mCols)
   {
      throw IncompatibleSizeError();
   }
#endif
   for (unsigned i = 0; i < mRows; ++i)
   {
      (*this)(i, i) = aVal;
   }
}

//! Return the diagonal of the matrix as an nx1 column matrix,
//! where "n" is the number of rows.
//!@note This is equivalent to the Matlab "diag" function for
//! a matrix along the main diagonal.
template<typename T>
void UtMatrix<T>::GetDiagonal(UtMatrix<T>& aDiagonal) const
{
   unsigned rows = (mRows < mCols) ? mRows : mCols;
   aDiagonal.SetSize(rows, 1);
   for (unsigned i = 0; i < rows; ++i)
   {
      aDiagonal(i, 0) = (*this)(i, i);
   }
}

//! Find the minimum value in the matrix.
template<typename T>
const T& UtMatrix<T>::Minimum() const
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if (mRows == 0)
   {
      throw OutOfBoundsError();
   }
#endif
   unsigned minRow = 0;
   unsigned minCol = 0;
   for (unsigned row = 0; row < mRows; ++row)
   {
      for (unsigned col = 0; col < mCols; ++col)
      {
         if ((*this)(row, col) < (*this)(minRow, minCol))
         {
            minRow = row;
            minCol = col;
         }
      }
   }
   return (*this)(minRow, minCol);
}

//! Find the maximum value in the matrix.
template<typename T>
const T& UtMatrix<T>::Maximum() const
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if (mRows == 0)
   {
      throw OutOfBoundsError();
   }
#endif
   unsigned maxRow = 0;
   unsigned maxCol = 0;
   for (unsigned row = 0; row < mRows; ++row)
   {
      for (unsigned col = 0; col < mCols; ++col)
      {
         if ((*this)(row, col) > (*this)(maxRow, maxCol))
         {
            maxRow = row;
            maxCol = col;
         }
      }
   }
   return (*this)(maxRow, maxCol);
}

//! Fill all elements in a row with a specified value.
template<typename T>
void UtMatrix<T>::FillRow(unsigned int aRow, const T& aVal)
{
   for (unsigned i = 0; i < mCols; ++i)
   {
      (*this)(aRow, i) = aVal;
   }
}

//! Fill all elements in a column with a specified value.
template<typename T>
void UtMatrix<T>::FillColumn(unsigned int aCol, const T& aVal)
{
   for (unsigned i = 0; i < mRows; ++i)
   {
      (*this)(i, aCol) = aVal;
   }
}

//! Copy a subset of one matrix to another.
//!
template<typename T>
void UtMatrix<T>::CopyFrom(const UtMatrix<T>& aSrc, unsigned aBeginRow, unsigned aBeginCol, unsigned aSizeRows, unsigned aSizeCols)
{
   unsigned endRow = aBeginRow + aSizeRows - 1;
   unsigned endCol = aBeginCol + aSizeCols - 1;
#ifdef UTMATRIX_CHECK_BOUNDS
   if ((endRow >= aSrc.mRows) || (endCol >= aSrc.mCols) || (mRows < aSizeRows) || (mCols < aSizeCols))
   {
      throw IncompatibleSizeError();
   }
#endif
   for (unsigned i = aBeginRow; i <= endRow; ++i)
   {
      for (unsigned j = aBeginCol; j <= endCol; ++j)
      {
         mMatrix[i * mCols + j] = aSrc.mMatrix[i * aSrc.mCols + j];
      }
   }
}

//! Copy elements from a single-dimensional array into a UtMatrix object.
//! @note The user must verify that the array sizes are compatible.
template<typename T>
void UtMatrix<T>::CopyFrom(const T* aVector)
{
   for (unsigned i = 0; i < mMatrix.size(); ++i)
   {
      mMatrix[i] = aVector[i];
   }
}

//! Copy "aNumElements" of a UtMatrix object, starting with "aStartElement,"
//! from a single-dimensional array.
template<typename T>
void UtMatrix<T>::CopyFrom(const T* aVector, unsigned aNumElements, unsigned aStartElement)
{
   unsigned endElement = aStartElement + aNumElements - 1;
#ifdef UTMATRIX_CHECK_BOUNDS
   if (endElement >= mMatrix.size())
   {
      throw IncompatibleSizeError();
   }
#endif
   for (unsigned i = aStartElement; i <= endElement; ++i)
   {
      mMatrix[i] = aVector[i - aStartElement];
   }
}

//! Copy "aNumElements" of a UtMatrix object, starting with "aStartElement,"
//! to a single-dimensional array.
//! @note The user must verify that the array sizes are compatible.
template<typename T>
void UtMatrix<T>::CopyTo(T* aVector, unsigned aNumElements, unsigned aStartElement) const
{
   unsigned endElement = aStartElement + aNumElements - 1;
#ifdef UTMATRIX_CHECK_BOUNDS
   if (endElement >= mMatrix.size())
   {
      throw IncompatibleSizeError();
   }
#endif
   for (unsigned i = aStartElement; i <= endElement; ++i)
   {
      aVector[i - aStartElement] = mMatrix[i];
   }
}

//! Copy elements from a UtMatrix object into a single dimensional array.
//! @note The user must verify that the array sizes are compatible.
template<typename T>
void UtMatrix<T>::CopyTo(T* aVector)
{
   for (unsigned i = 0; i < mMatrix.size(); ++i)
   {
      aVector[i] = mMatrix[i];
   }
}

//! Set the diagonal elements of a UtMatrix object from a single-dimensional array.
//! @note The user must verify that the array sizes are compatible.
template<typename T>
void UtMatrix<T>::SetDiagonal(T* aVector)
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if (mRows != mCols)
   {
      throw IncompatibleSizeError();
   }
#endif
   for (unsigned i = 0; i < mRows; ++i)
   {
      mMatrix[i * mRows + i] = aVector[i];
   }
}

//! Set the size of the matrix.  Use this method after default
//! construction, or when the contents don't need to be preserved.
//!@param aRows The new number of rows in the matrix.
//!@param aCols The new number of columns in the matrix.
template<typename T>
void UtMatrix<T>::SetSize(unsigned int aRows, unsigned int aCols)
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if ((aRows == 0) && (aCols == 0))
   {
      throw IncompatibleSizeError();
   }
#endif
   if ((mRows != aRows) || (mCols != aCols))
   {
      mRows = aRows;
      mCols = aCols;
      mMatrix.resize(mRows * mCols);
   }
}

//! Resize the matrix.  This method is similar to SetSize but
//! preserves existing data (either all data for a larger matrix,
//! or existing data up to the new size, for a smaller matrix).
//!@param aRows The new number of rows in the matrix.
//!@param aCols The new number of columns in the matrix.
template<typename T>
void UtMatrix<T>::Resize(unsigned int aRows, unsigned int aCols)
{
   // Expand the state covariance to accommodate the existing local track covariance.
   UtMatrix<T> temp(mRows, mCols);
   temp = *this; // It is necessary to copy the existing matrix because the SetSize call invalidates the data.
   unsigned int rows = mRows;
   unsigned int cols = mCols;
   SetSize(aRows, aCols);

   if (mCols > cols)
   {
      if (rows > 0)
      {
         // Because the underlying implementation is a vector, and we access the data linearly,
         // we must clear the upper diagonal (which had already been used).
         Fill(T(), 0, cols, rows, (mCols - cols));
      }
   }
   else
   {
      cols = mCols;
   }
   if (rows > mRows)
   {
      rows = mRows;
   }
   if ((rows > 0) && (cols > 0))
   {
      CopyFrom(temp, 0, 0, rows, cols);
   }
}

//! Resize the matrix, then populate it with a subset of a matrix of the
//! same size or larger.  This method is similar to the general CopyFrom
//! command, but whereas CopyFrom copies a subset of a matrix in a set
//! range of indicies to a matrix of the same size, this method also trims the
//! matrix and only uses the subset.  The result is stored in "this."
//!@param aRows The new number of rows in the matrix.
//!@param aCols The new number of columns in the matrix.
//!@param aSrc The matrix from which the data will be copied.
//!@param aBeginRow The starting row from which elements are copied from aSrc.
//!@param aBeginCol The starting column from which elements are copied form aSrc.
template<typename T>
void UtMatrix<T>::Crop(const UtMatrix<T>& aSrc, unsigned aBeginRow, unsigned aBeginCol, unsigned aRows, unsigned aCols)
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if ((aSrc.mRows < (aRows + aBeginRow)) || (aSrc.mCols < (aCols + aBeginCol)))
   {
      throw IncompatibleSizeError();
   }
#endif
   SetSize(aRows, aCols);
   for (unsigned row = 0; row < aRows; ++row)
   {
      for (unsigned col = 0; col < aCols; ++col)
      {
         mMatrix[row * aCols + col] = aSrc.mMatrix[(row + aBeginRow) * aSrc.mCols + (col + aBeginCol)];
      }
   }
}

//! Reduce "this" matrix to tridiagonal form.
//! This algorithm is the Householder method,
//! and the implementation is the "tred2" routine, as found in
//! Numerical Recipes (11.2)
template<typename T>
UtMatrix<T>& UtMatrix<T>::ReduceToTridiagonal(UtMatrix<T>& aDiagonalElements, UtMatrix<T>& aOffDiagonalElements)
{
   int          n = mRows;
   UtMatrix<T>& V = *this;

#ifdef UTMATRIX_CHECK_BOUNDS
   if (mRows != mCols)
   {
      throw IncompatibleSizeError();
   }
#endif

   aDiagonalElements.SetSize(n);
   aOffDiagonalElements.SetSize(n);
   UtMatrix<T>& d = aDiagonalElements;
   UtMatrix<T>& e = aOffDiagonalElements;

   int k;
   int i;
   int j;
   T   h;
   T   hh;
   T   f;
   T   g;

   //  This is derived from the Algol procedures tred2 by
   //  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
   //  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
   //  Fortran subroutine in EISPACK.

   for (j = 0; j < n; ++j)
   {
      d(j) = V(n - 1, j);
   }

   // Householder reduction to tridiagonal form.

   for (i = n - 1; i > 0; --i)
   {
      // Scale to avoid under/overflow.

      T scale = 0.0;
      h       = 0.0;
      for (k = 0; k < i; ++k)
      {
         scale = scale + std::abs(d(k));
      }

      if (scale == 0.0)
      {
         e(i) = d(i - 1);
         for (j = 0; j < i; ++j)
         {
            d(j)    = V(i - 1, j);
            V(i, j) = 0.0;
            V(j, i) = 0.0;
         }
      }
      else
      {
         // Generate Householder vector.

         for (k = 0; k < i; ++k)
         {
            d(k) /= scale;
            h += d(k) * d(k);
         }
         f = d(i - 1);
         g = sqrt(h);
         if (f > 0)
         {
            g = -g;
         }
         e(i)     = scale * g;
         h        = h - f * g;
         d(i - 1) = f - g;
         for (j = 0; j < i; ++j)
         {
            e(j) = 0.0;
         }

         // Apply similarity transformation to remaining columns.

         for (j = 0; j < i; ++j)
         {
            f       = d(j);
            V(j, i) = f;
            g       = e(j) + V(j, j) * f;
            for (k = j + 1; k <= i - 1; ++k)
            {
               g += V(k, j) * d(k);
               e(k) += V(k, j) * f;
            }
            e(j) = g;
         }
         f = 0.0;
         for (j = 0; j < i; ++j)
         {
            e(j) /= h;
            f += e(j) * d(j);
         }
         hh = f / (h + h);
         for (j = 0; j < i; ++j)
         {
            e(j) -= hh * d(j);
         }
         for (j = 0; j < i; ++j)
         {
            f = d(j);
            g = e(j);
            for (k = j; k <= i - 1; ++k)
            {
               V(k, j) -= (f * e(k) + g * d(k));
            }
            d(j)    = V(i - 1, j);
            V(i, j) = 0.0;
         }
      }
      d(i) = h;
   }

   // Accumulate transformations.

   for (i = 0; i < n - 1; ++i)
   {
      V(n - 1, i) = V(i, i);
      V(i, i)     = 1.0;
      h           = d(i + 1);
      if (h != 0.0)
      {
         for (k = 0; k <= i; ++k)
         {
            d(k) = V(k, i + 1) / h;
         }

         for (j = 0; j <= i; ++j)
         {
            g = 0.0;
            for (k = 0; k <= i; ++k)
            {
               g += V(k, i + 1) * V(k, j);
            }

            for (k = 0; k <= i; ++k)
            {
               V(k, j) -= g * d(k);
            }
         }
      }
      for (k = 0; k <= i; ++k)
      {
         V(k, i + 1) = 0.0;
      }
   }
   for (j = 0; j < n; ++j)
   {
      d(j)        = V(n - 1, j);
      V(n - 1, j) = 0.0;
   }
   V(n - 1, n - 1) = 1.0;
   e(0)            = 0.0;

   return (*this);
}

//! QL algorithm with implicit shifts, to determine the
//! eigenvalues of a real,symmetric, tridiagonal matrix, or of a real,
//! symmetric matrix previously reduced by tred2. d is a vector of length
//! np. On input, its first n elements are the diagonal elements of the
//! tridiagonal matrix. On output, it returns the eigenvalues. The vector
//! e inputs the subdiagonal elements of the tridiagonal matrix, with e(1)
//!  arbitrary. On output e is destroyed. If routine
//! cannot converge within prescribed iterations, NonConvergenceError
//! is thrown.
template<typename T>
const UtMatrix<T>& UtMatrix<T>::ReduceToEigenvectorsFromTridiagonal(UtMatrix<T>& aDiagonalElements,
                                                                    UtMatrix<T>& aOffDiagonalElements) // const
{
#ifdef UTMATRIX_CHECK_BOUNDS
   {
      int          n = mRows;
      UtMatrix<T>& d = aDiagonalElements;
      UtMatrix<T>& e = aOffDiagonalElements;
      UtMatrix<T>& z = *this;
      if (mRows != mCols)
      {
         throw IncompatibleSizeError();
      }
      else if ((d.mRows != mRows) || (e.mRows != mRows))
      {
         throw IncompatibleSizeError();
      }
   }
#endif
   int n = mRows;

   UtMatrix& d = aDiagonalElements;
   UtMatrix& e = aOffDiagonalElements;
   UtMatrix& V = *this;

   //  This is derived from the Algol procedures tql2, by
   //  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
   //  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
   //  Fortran subroutine in EISPACK.

   int b;
   int k;
   int i;
   int j;
   T   h;
   T   g;
   T   f;
   T   tst1;
   T   p;
   T   r;
   T   dl1;
   T   c;
   T   c2;
   T   c3;
   T   el1;
   T   s;
   T   s2;

   for (i = 1; i < n; ++i)
   {
      e(i - 1) = e(i);
   }
   e(n - 1) = 0.0;

   f    = 0.0;
   tst1 = 0.0;

   // double eps = pow(2.0,-52.0);
   static const double eps = 2.3E-12;

   for (b = 0; b < n; ++b)
   {
      // Find small subdiagonal element
      T tst2 = std::abs(d(b)) + std::abs(e(b));
      // tst1 = max(tst1,);
      tst1  = tst1 > tst2 ? tst1 : tst2;
      int m = b;
      while (m < n)
      {
         if (std::abs(e(m)) <= eps * tst1)
         {
            break;
         }
         m++;
      }

      // If m == l, d(l) is an eigenvalue,
      // otherwise, iterate.

      if (m > b)
      {
         do
         {
            // Compute implicit shift

            g = d(b);
            p = (d(b + 1) - g) / (2.0F * e(b));
            // r = Maths.hypot(p,1.0);
            r = sqrt(p * p + 1.0F);
            if (p < 0)
            {
               r = -r;
            }
            d(b)     = e(b) / (p + r);
            d(b + 1) = e(b) * (p + r);
            dl1      = d(b + 1);
            h        = g - d(b);
            for (i = b + 2; i < n; ++i)
            {
               d(i) -= h;
            }
            f = f + h;

            // Implicit QL transformation.

            p = d(m);

            c   = 1.0;
            c2  = c;
            c3  = c;
            el1 = e(b + 1);
            s   = 0.0;
            s2  = 0.0;
            for (i = m - 1; i >= b; --i)
            {
               c3 = c2;
               c2 = c;
               s2 = s;
               g  = c * e(i);
               h  = c * p;
               // r = Maths.hypot(p,e(i));
               r        = sqrt(p * p + e(i) * e(i));
               e(i + 1) = s * r;
               s        = e(i) / r;
               c        = p / r;
               p        = c * d(i) - s * g;
               d(i + 1) = h + s * (c * g + s * d(i));

               // Accumulate transformation.

               for (k = 0; k < n; ++k)
               {
                  h           = V(k, i + 1);
                  V(k, i + 1) = s * V(k, i) + c * h;
                  V(k, i)     = c * V(k, i) - s * h;
               }
            }
            p    = -s * s2 * c3 * el1 * e(b) / dl1;
            e(b) = s * p;
            d(b) = c * p;

            // Check for convergence.

         } while (std::abs(e(b)) > eps * tst1);
      }
      d(b) = d(b) + f;
      e(b) = 0.0;
   }

   // Sort eigenvalues and corresponding vectors.

   for (i = 0; i < n - 1; ++i)
   {
      k = i;
      p = d(i);
      for (j = i + 1; j < n; ++j)
      {
         if (d(j) < p)
         {
            k = j;
            p = d(j);
         }
      }
      if (k != i)
      {
         d(k) = d(i);
         d(i) = p;
         for (j = 0; j < n; ++j)
         {
            p       = V(j, i);
            V(j, i) = V(j, k);
            V(j, k) = p;
         }
      }
   }

   return (*this);
}

//! "compute the condition number of a symmetric real valued matrix using
//! the QL algorithm;
//! first, assuming that the input vector a is at least symmetric, we;
//! compute the maximum eigenvalue of "a" using the power method for;
//! symmetric matrices;
//! find the eigenvalues of a symmetric matrix using the householder
//! also to get a tridiagonal matrix and then the QL algorithm
template<typename T>
double UtMatrix<T>::GetConditionNumber() const
{
   double             cond = 0.0;
   const UtMatrix<T>& a    = *this;
   UtMatrix<T>        copy(a);
   UtMatrix           xv;
   bool               err = false;
   try
   {
      copy.ReduceToEigenvectors(xv);
   }
   catch (NonConvergenceError&)
   {
      //*ql algorithm could not converge, assume poorly conditioned matrix
      err  = true;
      cond = 1.0e+200;
   }
   if (!err)
   {
      //*find the maximum and minimum absolute values of the eigenvalues
      double   eigmax = std::abs(xv(0));
      double   eigmin = std::abs(xv(0));
      unsigned n      = xv.GetRows();
      for (unsigned i = 1; i < n; ++i)
      {
         if (std::abs(xv(i)) > eigmax)
         {
            eigmax = std::abs(xv(i));
         }
         if (std::abs(xv(i)) < eigmin)
         {
            eigmin = std::abs(xv(i));
         }
      }
      //*guard against division by zero
      if (eigmin <= 0.0)
      {
         cond = 1.0e+200;
      }
      else
      {
         cond = eigmax / eigmin;
      }
   }
   return cond;
}

//! This method swaps all elements, in 'this', of one index with
//! all elements with another index (for example (3,2)->(2,3)).
template<typename T>
void UtMatrix<T>::SwapIndices(unsigned int aSwap1, unsigned int aSwap2)
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if ((aSwap1 >= mRows) || (aSwap1 >= mCols) || (aSwap2 >= mRows) || (aSwap2 >= mCols))
   {
      throw OutOfBoundsError();
   }
#endif

   // Straightforward, but not optimal:
   // Simply replace the indices:
   // First Create a duplicate.
   UtMatrix<T>& mat = *this;
   UtMatrix<T>  temp(mat);
   for (unsigned row = 0; row < mRows; ++row)
   {
      unsigned newRow = row;
      if (newRow == aSwap1)
      {
         newRow = aSwap2;
      }
      else if (newRow == aSwap2)
      {
         newRow = aSwap1;
      }

      for (unsigned col = 0; col < mCols; ++col)
      {
         unsigned newCol = col;
         if (newCol == aSwap1)
         {
            newCol = aSwap2;
         }
         else if (newCol == aSwap2)
         {
            newCol = aSwap1;
         }

         // Finally:
         mat(newRow, newCol) = temp(row, col);
      }
   }
}

template<typename T>
bool UtMatrix<T>::IsPositiveDefinite() const
{
#ifdef UTMATRIX_CHECK_BOUNDS
   if (mRows != mCols)
   {
      throw IncompatibleSizeError();
   }
#endif
   UtMatrix<T>        copy(*this);
   unsigned           n = mRows;
   UtMatrix<T>        xv(n);
   UtMatrix<T>        yv(n);
   UtMatrix<T>        b(n, n);
   const UtMatrix<T>& a      = *this;
   double             eigmin = 0.0;
   bool               posdef = true;

   for (unsigned i = 0; i < (n - 1); ++i)
   {
      for (unsigned j = (i + 1); j < n; ++j)
      {
         if (a(i, j) != a(j, i))
         {
            return false;
         }
      }
   }
   copy.ReduceToTridiagonal(xv, yv);
   try
   {
      copy.ReduceToEigenvectorsFromTridiagonal(xv, yv);
   }
   catch (NonConvergenceError&)
   {
      posdef = false;
   }
   if (posdef)
   {
      eigmin = xv(0);
      for (unsigned i = 1; i < n; ++i)
      {
         if (xv(i) < eigmin)
         {
            eigmin = xv(i);
         }
      }
   }
   posdef = (eigmin > 0.0);
   return posdef;
}

//! Support method for SingularValueDecomposition, below.
//! finds dsqrt(a**2+b**2) without overflow or destructive underflow.
template<typename T>
T PYTHAG(T a, T b)
{
   T p;
   T r;
   T s;
   T t;
   T u;

   p = std::max(std::abs(a), std::abs(b));
   if (p != 0.0)
   {
      auto root_r           = std::min(std::abs(a), std::abs(b)) / p;
      r                     = root_r * root_r;
      static bool keepGoing = true;
      while (keepGoing)
      {
         t = (T)(4.0 + r);
         if (t == 4.0)
         {
            break;
         }
         s = r / t;
         u = (T)(1.0 + 2.0 * s);
         p = u * p;
         r = (s * s) / (u * u) * r;
      };
   }
   return p;
}

//!     Singular Value Decomposition Algorithm.
//!     ref. num. math. 14, 403-420(1970) by golub and reinsch.
//!     handbook for auto. comp., vol ii-linear algebra, 134-151(1971).
//!
//!     this subroutine determines the singular value decomposition
//!          t
//!     a=usv  of a real m by n rectangular matrix.  householder
//!     bidiagonalization and a variant of the qr algorithm are used.
//!
//!     on input
//!
//!        @param a contains the rectangular input matrix to be decomposed.
//!
//!     on output
//!
//!        a is unaltered (unless overwritten by u or v).
//!
//!        @param w contains the n (non-negative) singular values of a (the
//!          diagonal elements of s).  they are unordered.  if an
//!          error exit is made, the singular values should be correct
//!          for indices ierr+1,ierr+2,...,n.
//!
//!        @param u contains the matrix u (orthogonal column vectors) of the
//!          decomposition. u may coincide with a.
//!          if an error exit is made, the columns of u corresponding
//!          to indices of correct singular values should be correct.
//!
//!        @param v contains the matrix v (orthogonal) of the decomposition
//!          v may also coincide with a if u is not needed.  if an error
//!          exit is made, the columns of v corresponding to indices of
//!          correct singular values should be correct.
//!
//!        @return Zero for normal return,
//!          k          if the k-th singular value has not been
//!                     determined after 30 iterations.
template<typename T>
int UtMatrix<T>::SingularValueDecomposition(const UtMatrix<T>& a, UtMatrix<T>& w, UtMatrix<T>& u, UtMatrix<T>& v)
{
   int         i;
   int         j;
   int         k;
   int         l = 0;
   int         i1;
   int         k1;
   int         l1 = 0;
   int         mn;
   int         its;
   int         ierr = 0;
   T           c;
   T           f;
   T           g;
   T           h;
   T           s;
   T           x;
   T           y;
   T           z;
   T           tst1;
   T           tst2;
   T           scale;
   int         m = a.GetRows();
   int         n = a.GetCols();
   UtMatrix<T> rv1(n); // work matrix

   for (i = 0; i < m; ++i)
   {
      for (j = 0; j < n; ++j)
      {
         u(i, j) = a(i, j);
      }
   }
   //     .......... householder reduction to bidiagonal form ..........
   g     = 0.0;
   scale = 0.0;
   x     = 0.0;

   for (i = 0; i < n; ++i)
   {
      l      = i + 1;
      rv1(i) = scale * g;
      g      = 0.0;
      s      = 0.0;
      scale  = 0.0;

      if (i <= (m - 1))
      {
         for (k = i; k < m; ++k)
         {
            scale = scale + std::abs(u(k, i));
         }

         if (scale != 0.0)
         {
            for (k = i; k < m; ++k)
            {
               u(k, i) = u(k, i) / scale;
               s       = s + u(k, i) * u(k, i);
            }

            f       = u(i, i);
            g       = -std::copysign(sqrt(s), f);
            h       = f * g - s;
            u(i, i) = f - g;

            if (i < (n - 1))
            {
               for (j = l; j < n; ++j)
               {
                  s = 0.0;

                  for (k = i; k < m; ++k)
                  {
                     s = s + u(k, i) * u(k, j);
                  }

                  f = s / h;

                  for (k = i; k < m; ++k)
                  {
                     u(k, j) = u(k, j) + f * u(k, i);
                  }
               }
            }
            for (k = i; k < m; ++k)
            {
               u(k, i) = scale * u(k, i);
            }
         }
      }

      w(i)  = scale * g;
      g     = 0.0;
      s     = 0.0;
      scale = 0.0;

      if ((i <= (m - 1)) && (i != (n - 1)))
      {
         for (k = l; k < n; ++k)
         {
            scale = scale + std::abs(u(i, k));
         }

         if (scale != 0.0)
         {
            for (k = l; k < n; ++k)
            {
               u(i, k) = u(i, k) / scale;
               s       = s + u(i, k) * u(i, k);
            }

            f       = u(i, l);
            g       = -std::copysign(sqrt(s), f);
            h       = f * g - s;
            u(i, l) = f - g;

            for (k = l; k < n; ++k)
            {
               rv1(k) = u(i, k) / h;
            }

            if (i != (m - 1))
            {
               for (j = l; j < m; ++j)
               {
                  s = 0.0;

                  for (k = l; k < n; ++k)
                  {
                     s = s + u(j, k) * u(i, k);
                  }

                  for (k = l; k < n; ++k)
                  {
                     u(j, k) = u(j, k) + s * rv1(k);
                  }
               }
            }
            for (k = l; k < n; ++k)
            {
               u(i, k) = scale * u(i, k);
            }
         }
      }
      x = std::max(x, std::abs(w(i)) + std::abs(rv1(i)));
   }
   //     .......... accumulation of right-hand transformations ..........
   for (i = (n - 1); i >= 0; --i)
   {
      if (i < (n - 1))
      {
         if (g != 0.0)
         {
            for (j = l; j < n; ++j)
            {
               //     .......... double division avoids possible underflow ..........
               v(j, i) = (u(i, j) / u(i, l)) / g;
            }

            for (j = l; j < n; ++j)
            {
               s = 0.0;
               for (k = l; k < n; ++k)
               {
                  s = s + u(i, k) * v(k, j);
               }
               for (k = l; k < n; ++k)
               {
                  v(k, j) = v(k, j) + s * v(k, i);
               }
            }
         }
         for (j = l; j < n; ++j)
         {
            v(i, j) = 0.0;
            v(j, i) = 0.0;
         }
      }
      v(i, i) = 1.0;
      g       = rv1(i);
      l       = i;
   }
   //     .......... accumulation of left-hand transformations ..........
   //     ..........for i=min(m,n) step -1 until 1 do -- ..........
   mn = n;
   if (m < n)
   {
      mn = m;
   }

   for (i = (mn - 1); i >= 0; --i)
   {
      l = i + 1;
      g = w(i);

      if (i < (n - 1))
      {
         for (j = l; j < n; ++j)
         {
            u(i, j) = 0.0;
         }
      }

      if (g != 0.0)
      {
         if (i < (mn - 1))
         {
            for (j = l; j < n; ++j)
            {
               s = 0.0;

               for (k = l; k < m; ++k)
               {
                  s = s + u(k, i) * u(k, j);
               }
               //     .......... double division avoids possible underflow ..........
               f = (s / u(i, i)) / g;

               for (k = i; k < m; ++k)
               {
                  u(k, j) = u(k, j) + f * u(k, i);
               }
            }
         }

         for (j = i; j < m; ++j)
         {
            u(j, i) = u(j, i) / g;
         }
      }
      else // g == 0.0
      {
         for (j = i; j < m; ++j)
         {
            u(j, i) = 0.0;
         }
      }

      u(i, i) = u(i, i) + (T)1.0;
   }
   //     .......... diagonalization of the bidiagonal form ..........
   tst1 = x;
   //     .......... for k=n step -1 until 1 do -- ..........
   for (k = (n - 1); k >= 0; --k)
   {
      k1  = k - 1;
      its = 0;
      //     .......... test for splitting.
      //                for l=k step -1 until 1 do -- ..........
      bool cancelRv1 = false;
   _520:
      for (l = k; l >= 0; --l)
      {
         l1   = l - 1;
         tst2 = tst1 + std::abs(rv1(l));
         if (tst2 == tst1)
         {
            break;
         }
         //     .......... rv1(1) is always zero, so there is no exit
         //                through the bottom of the loop ..........
         // mjm The index l1 is -1 on the last pass.
         // This is also in the original algorithm so
         // it is not a translation error.
         // I am placing a guard around it.
         if (l1 >= 0)
         {
            tst2 = tst1 + fabs(w(l1));
            if (tst2 == tst1)
            {
               cancelRv1 = true;
               break;
            }
         }
      }

      // mjm add this if-check to disambiguate the situation
      if (cancelRv1)
      {
         //     .......... cancellation of rv1(l) if l greater than 1 ..........
         c = 0.0;
         s = 1.0;

         for (i = l; i <= k; ++i)
         {
            f      = s * rv1(i);
            rv1(i) = c * rv1(i);
            tst2   = tst1 + std::abs(f);

            if (tst2 == tst1)
            {
               break;
            }
            g    = w(i);
            h    = PYTHAG(f, g);
            w(i) = h;
            c    = g / h;
            s    = -f / h;

            for (j = 0; j < m; ++j)
            {
               y        = u(j, l1);
               z        = u(j, i);
               u(j, l1) = y * c + z * s;
               u(j, i)  = -y * s + z * c;
            }
         }
      }
      //     .......... test for convergence ..........
      z = w(k);

      if (l != k)
      {
         //     .......... shift from bottom 2 by 2 minor ..........
         if (its == 30)
         {
            //     .......... set error -- no convergence to a
            //                singular value after 30 iterations ..........
            ierr = k;
            return ierr;
         }
         its = its + 1;
         x   = w(l);
         y   = w(k1);
         g   = rv1(k1);
         h   = rv1(k);
         f   = (T)(0.5) * (((g + z) / h) * ((g - z) / y) + y / h - h / y);
         g   = PYTHAG(f, (T)1.0);
         f   = x - (z / x) * z + (h / x) * (y / (f + std::copysign(g, f)) - h);
         //     .......... next qr transformation ..........
         c = 1.0;
         s = 1.0;

         for (i1 = l; i1 <= k1; ++i1)
         {
            i       = i1 + 1;
            g       = rv1(i);
            y       = w(i);
            h       = s * g;
            g       = c * g;
            z       = PYTHAG(f, h);
            rv1(i1) = z;
            c       = f / z;
            s       = h / z;
            f       = x * c + g * s;
            g       = -x * s + g * c;
            h       = y * s;
            y       = y * c;

            for (j = 0; j < n; ++j)
            {
               x        = v(j, i1);
               z        = v(j, i);
               v(j, i1) = x * c + z * s;
               v(j, i)  = -x * s + z * c;
            }

            z     = PYTHAG(f, h);
            w(i1) = z;
            //     .......... rotation can be arbitrary if z is zero ..........

            if (z != 0.0)
            {
               c = f / z;
               s = h / z;
            }
            f = c * g + s * y;
            x = -s * g + c * y;

            for (j = 0; j < m; ++j)
            {
               y        = u(j, i1);
               z        = u(j, i);
               u(j, i1) = y * c + z * s;
               u(j, i)  = -y * s + z * c;
            }
         }

         rv1(l) = 0.0;
         rv1(k) = f;
         w(k)   = x;
         goto _520; // Can't easily get rid of this goto (mjm).
      }
      //     .......... convergence ..........

      if (z < 0.0)
      {
         //     .......... w(k) is made non-negative ..........
         w(k) = -z;

         for (j = 0; j < n; ++j)
         {
            v(j, k) = -v(j, k);
         }
      }
   }

   return ierr;
}

using UtMatrixf = UtMatrix<float>;
using UtMatrixd = UtMatrix<double>;

#if !defined(sgi) && !defined(__APPLE__)
// Explicitly instantiate UtMatrix for float and double.
template class UT_EXPORT UtMatrix<float>;
template class UT_EXPORT UtMatrix<double>;
#endif

#endif
