// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtScriptMatrix.hpp"

#include <sstream>

#include "UtCast.hpp"
#include "UtMatrix.hpp"

namespace // {anonymous}
{

void CheckRowIndex(const UtMatrixd& aMatrix, int aRow)
{
   if (aRow < 0 || ut::safe_cast<unsigned int>(aRow) >= aMatrix.GetRows())
   {
      std::stringstream err{};
      err << "Provided row index, " << aRow << ", must be in the range [0, " << aMatrix.GetRows() << ").";
      UT_SCRIPT_ABORT(err.str());
   }
}

void CheckColumnIndex(const UtMatrixd& aMatrix, int aCol)
{
   if (aCol < 0 || ut::safe_cast<unsigned int>(aCol) >= aMatrix.GetCols())
   {
      std::stringstream err{};
      err << "Provided column index, " << aCol << ", must be in the range [0, " << aMatrix.GetCols() << ").";
      UT_SCRIPT_ABORT(err.str());
   }
}

void CheckIndices(const UtMatrixd& aMatrix, int aRow, int aCol)
{
   std::stringstream err{};
   bool              abort{false};
   if (aRow < 0 || ut::safe_cast<unsigned int>(aRow) >= aMatrix.GetRows())
   {
      err << "Provided row index, " << aRow << ", must be in the range [0, " << aMatrix.GetRows() << ").";
      abort = true;
   }
   if (aCol < 0 || ut::safe_cast<unsigned int>(aCol) >= aMatrix.GetCols())
   {
      if (abort)
      {
         err << " ";
      }
      err << "Provided column index, " << aCol << ", must be in the range [0, " << aMatrix.GetCols() << ").";
      abort = true;
   }
   if (abort)
   {
      UT_SCRIPT_ABORT(err.str());
   }
}

void CheckSubmatrixSpec(const UtMatrixd& aMatrix, int aRowStart, int aRowEnd, int aColStart, int aColEnd)
{
   std::stringstream err{};
   bool              abort{false};
   if (aRowStart < 0 || ut::safe_cast<unsigned int>(aRowStart) >= aMatrix.GetRows())
   {
      err << "Provided inital row index, " << aRowStart << ", must be in the range [0, " << aMatrix.GetRows() << ").";
      abort = true;
   }
   if (aRowEnd < 0 || ut::safe_cast<unsigned int>(aRowEnd) > aMatrix.GetRows())
   {
      if (abort)
      {
         err << " ";
      }
      err << "Provided final row index, " << aRowEnd << ", must be in the range [0, " << aMatrix.GetRows() << "].";
      abort = true;
   }
   if (aRowEnd <= aRowStart)
   {
      if (abort)
      {
         err << " ";
      }
      err << "Provided final row index, " << aRowEnd << ", must be greater than the initial row index, " << aRowStart
          << ".";
      abort = true;
   }
   if (aColStart < 0 || ut::safe_cast<unsigned int>(aColStart) >= aMatrix.GetCols())
   {
      if (abort)
      {
         err << " ";
      }
      err << "Provided initial column index, " << aColStart << ", must be in the range [0, " << aMatrix.GetCols() << ").";
      abort = true;
   }
   if (aColEnd < 0 || ut::safe_cast<unsigned int>(aColEnd) > aMatrix.GetCols())
   {
      if (abort)
      {
         err << " ";
      }
      err << "Provided final column index, " << aColEnd << ", must be in the range [0, " << aMatrix.GetCols() << "].";
      abort = true;
   }
   if (aColEnd <= aColStart)
   {
      if (abort)
      {
         err << " ";
      }
      err << "Provided final column index, " << aColEnd << ", must be greater than the initial column index, "
          << aColStart << ".";
      abort = true;
   }
   if (abort)
   {
      UT_SCRIPT_ABORT(err.str());
   }
}

// NOTE: This does no bounds checking. This is assumed to happen at the call site.
void CopyFromTo(const UtMatrixd& aSource,
                unsigned int     aSourceRow,
                unsigned int     aSourceCol,
                unsigned int     aRowCount,
                unsigned int     aColCount,
                UtMatrixd&       aTarget,
                unsigned int     aTargetRow,
                unsigned int     aTargetCol)
{
   for (unsigned int i = 0; i < aRowCount; ++i)
   {
      for (unsigned int j = 0; j < aColCount; ++j)
      {
         aTarget(aTargetRow + i, aTargetCol + j) = aSource(aSourceRow + i, aSourceCol + j);
      }
   }
}

// NOTE: This does no bounds checking. This is assumed to happen at the call site.
void CopyDiagonal(const UtMatrixd& aDiagonal, unsigned int aDim, UtMatrixd& aTarget)
{
   for (unsigned int i = 0; i < aDim; ++i)
   {
      aTarget(i, i) = aDiagonal(i);
   }
}

} // namespace

namespace ut
{
namespace script
{


Matrix::Matrix(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("Matrix", aScriptTypesPtr)
{
   SetClassName("Matrix");

   mCloneable          = true;
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<ConstructDefault>("Construct"));
   AddStaticMethod(ut::make_unique<ConstructValue>("Construct"));
   AddStaticMethod(ut::make_unique<Identity>());
   AddMethod(ut::make_unique<Rows>());
   AddMethod(ut::make_unique<Columns>());
   AddMethod(ut::make_unique<Get>());
   AddMethod(ut::make_unique<Row>());
   AddMethod(ut::make_unique<Column>());
   AddMethod(ut::make_unique<Diagonal>());
   AddMethod(ut::make_unique<Submatrix>());
   AddMethod(ut::make_unique<Trace>());
   AddMethod(ut::make_unique<IsRow>());
   AddMethod(ut::make_unique<IsColumn>());
   AddMethod(ut::make_unique<IsPositiveDefinite>());
   AddMethod(ut::make_unique<Set>());
   AddMethod(ut::make_unique<SetRow>());
   AddMethod(ut::make_unique<SetColumn>());
   AddMethod(ut::make_unique<SetDiagonal>());
   AddMethod(ut::make_unique<SetSubmatrix>());
   AddMethod(ut::make_unique<Multiply>());
   AddMethod(ut::make_unique<Inverse>());
   AddMethod(ut::make_unique<PseudoInverse>());
   AddMethod(ut::make_unique<Transpose>());
   AddMethod(ut::make_unique<Add>());
   AddMethod(ut::make_unique<Subtract>());
   AddMethod(ut::make_unique<Scale>());
   AddMethod(ut::make_unique<CholeskyDecomposition>());
   AddMethod(ut::make_unique<SingularValueDecomposition>());
   AddMethod(ut::make_unique<Eigensystem>());

   // Do we want the binary operations to be static? Matrix mult, add, sub, etc..
   // If so, does that mean the others should be also (Inverse and so on?)
   // Or, provide both?
}

void Matrix::Destroy(void* aObjectPtr)
{
   delete static_cast<UtMatrixd*>(aObjectPtr);
}

void* Matrix::Clone(void* aObjectPtr)
{
   auto* argPtr    = static_cast<UtMatrixd*>(aObjectPtr);
   auto* retvalPtr = new UtMatrixd{argPtr->GetRows(), argPtr->GetCols()};
   CopyFromTo(*argPtr, 0, 0, argPtr->GetRows(), argPtr->GetCols(), *retvalPtr, 0, 0);
   return retvalPtr;
}

bool Matrix::EqualTo(void* aLHS_Ptr, void* aRHS_Ptr)
{
   auto* lhsMatrixPtr = static_cast<UtMatrixd*>(aLHS_Ptr);
   auto* rhsMatrixPtr = static_cast<UtMatrixd*>(aRHS_Ptr);
   return *lhsMatrixPtr == *rhsMatrixPtr;
}

std::string Matrix::ToString(void* aObjectPtr) const
{
   std::stringstream out{};
   auto*             matrixPtr = static_cast<UtMatrixd*>(aObjectPtr);
   matrixPtr->Stream(out);
   return out.str();
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, ConstructDefault, 2, "Matrix", "int, int")
{
   int nRows = aVarArgs[0].GetInt();
   if (nRows < 1)
   {
      UT_SCRIPT_ABORT("Number of rows must be greater than zero.");
   }

   int nCols = aVarArgs[1].GetInt();
   if (nCols < 1)
   {
      UT_SCRIPT_ABORT("Number of columns must be greater than zero.");
   }

   auto matPtr = ut::make_unique<UtMatrixd>(nRows, nCols);
   aReturnVal.SetPointer(new UtScriptRef(matPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, ConstructValue, 3, "Matrix", "int, int, double")
{
   int nRows = aVarArgs[0].GetInt();
   if (nRows < 1)
   {
      UT_SCRIPT_ABORT("Number of rows must be greater than zero.");
   }

   int nCols = aVarArgs[1].GetInt();
   if (nCols < 1)
   {
      UT_SCRIPT_ABORT("Number of columns must be greater than zero.");
   }

   double value = aVarArgs[2].GetDouble();

   auto matPtr = ut::make_unique<UtMatrixd>(nRows, nCols);
   matPtr->Fill(value);
   aReturnVal.SetPointer(new UtScriptRef(matPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Identity, 1, "Matrix", "int")
{
   int n = aVarArgs[0].GetInt();
   if (n < 1)
   {
      UT_SCRIPT_ABORT("Square matrix size must be greater than zero.");
   }

   auto matPtr = ut::make_unique<UtMatrixd>(n, n);
   matPtr->LoadIdentity();
   aReturnVal.SetPointer(new UtScriptRef(matPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Rows, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetRows());
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Columns, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetCols());
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Get, 2, "double", "int, int")
{
   int row = aVarArgs[0].GetInt();
   int col = aVarArgs[1].GetInt();
   CheckIndices(*aObjectPtr, row, col);
   aReturnVal.SetDouble((*aObjectPtr)(row, col));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Row, 1, "Matrix", "int")
{
   int row = aVarArgs[0].GetInt();
   CheckRowIndex(*aObjectPtr, row);
   auto rowPtr = ut::make_unique<UtMatrixd>(1, aObjectPtr->GetCols());
   CopyFromTo(*aObjectPtr, row, 0, 1, aObjectPtr->GetCols(), *rowPtr, 0, 0);
   aReturnVal.SetPointer(new UtScriptRef(rowPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Column, 1, "Matrix", "int")
{
   int col = aVarArgs[0].GetInt();
   CheckColumnIndex(*aObjectPtr, col);
   auto colPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), 1);
   CopyFromTo(*aObjectPtr, 0, col, aObjectPtr->GetRows(), 1, *colPtr, 0, 0);
   aReturnVal.SetPointer(new UtScriptRef(colPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Diagonal, 0, "Matrix", "")
{
   auto diagPtr = ut::make_unique<UtMatrixd>();
   aObjectPtr->GetDiagonal(*diagPtr);
   aReturnVal.SetPointer(new UtScriptRef(diagPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Submatrix, 4, "Matrix", "int, int, int, int")
{
   int rowStart = aVarArgs[0].GetInt();
   int rowEnd   = aVarArgs[1].GetInt();
   int colStart = aVarArgs[2].GetInt();
   int colEnd   = aVarArgs[3].GetInt();
   CheckSubmatrixSpec(*aObjectPtr, rowStart, rowEnd, colStart, colEnd);
   int  rowCount = rowEnd - rowStart;
   int  colCount = colEnd - colStart;
   auto subPtr   = ut::make_unique<UtMatrixd>(rowCount, colCount);
   CopyFromTo(*aObjectPtr, rowStart, colStart, rowCount, colCount, *subPtr, 0, 0);
   aReturnVal.SetPointer(new UtScriptRef(subPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Trace, 0, "double", "")
{
   if (aObjectPtr->GetRows() != aObjectPtr->GetCols())
   {
      UT_SCRIPT_ABORT("Matrix.Trace is only defined for a square matrix.");
   }
   aReturnVal.SetDouble(aObjectPtr->Trace());
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, IsRow, 0, "bool", "")
{
   // The matrix is a row vector if the number of rows is 1.
   aReturnVal.SetBool(aObjectPtr->GetRows() == 1);
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, IsColumn, 0, "bool", "")
{
   // The matrix is a column vector if the number of columns is 1.
   aReturnVal.SetBool(aObjectPtr->GetCols() == 1);
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, IsPositiveDefinite, 0, "bool", "")
{
   if (aObjectPtr->GetRows() != aObjectPtr->GetCols())
   {
      UT_SCRIPT_ABORT("Matrix.IsPositiveDefinite is only defined for a square matrix.");
   }
   aReturnVal.SetBool(aObjectPtr->IsPositiveDefinite());
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Set, 3, "void", "int, int, double")
{
   int row = aVarArgs[0].GetInt();
   int col = aVarArgs[1].GetInt();
   CheckIndices(*aObjectPtr, row, col);
   double value            = aVarArgs[2].GetDouble();
   (*aObjectPtr)(row, col) = value;
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, SetRow, 2, "void", "int, Matrix")
{
   int row = aVarArgs[0].GetInt();
   CheckRowIndex(*aObjectPtr, row);
   auto* matrixPtr = aVarArgs[1].GetPointer()->GetAppObject<UtMatrixd>();
   if (matrixPtr->GetRows() != 1)
   {
      UT_SCRIPT_ABORT("Input matrix must be a row matrix.");
   }
   if (matrixPtr->GetCols() != aObjectPtr->GetCols())
   {
      UT_SCRIPT_ABORT("Input matrix must have the same number of columns as this matrix.");
   }
   CopyFromTo(*matrixPtr, 0, 0, 1, aObjectPtr->GetCols(), *aObjectPtr, row, 0);
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, SetColumn, 2, "void", "int, Matrix")
{
   int col = aVarArgs[0].GetInt();
   CheckColumnIndex(*aObjectPtr, col);
   auto* matrixPtr = aVarArgs[1].GetPointer()->GetAppObject<UtMatrixd>();
   if (matrixPtr->GetCols() != 1)
   {
      UT_SCRIPT_ABORT("Input matrix must be a column matrix");
   }
   if (matrixPtr->GetRows() != aObjectPtr->GetRows())
   {
      UT_SCRIPT_ABORT("Input matrix must have the same number of rows as this matrix.");
   }
   CopyFromTo(*matrixPtr, 0, 0, aObjectPtr->GetRows(), 1, *aObjectPtr, 0, col);
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, SetDiagonal, 1, "void", "Matrix")
{
   unsigned int dim     = std::min(aObjectPtr->GetRows(), aObjectPtr->GetCols());
   auto*        diagPtr = aVarArgs[0].GetPointer()->GetAppObject<UtMatrixd>();
   if (diagPtr->GetRows() != dim)
   {
      std::stringstream err{};
      err << "Input matrix must have " << dim << " rows, but instead has " << diagPtr->GetRows() << ".";
      UT_SCRIPT_ABORT(err.str())
   }
   if (diagPtr->GetCols() != 1)
   {
      std::stringstream err{};
      err << "Input matrix must have 1 column, but instead has " << diagPtr->GetCols() << ".";
      UT_SCRIPT_ABORT(err.str())
   }
   CopyDiagonal(*diagPtr, dim, *aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, SetSubmatrix, 3, "void", "int, int, Matrix")
{
   int   rowStart  = aVarArgs[0].GetInt();
   int   colStart  = aVarArgs[1].GetInt();
   auto* matrixPtr = aVarArgs[2].GetPointer()->GetAppObject<UtMatrixd>();
   int   rowCount  = matrixPtr->GetRows();
   int   colCount  = matrixPtr->GetCols();
   int   rowEnd    = rowStart + rowCount;
   int   colEnd    = colStart + colCount;
   CheckSubmatrixSpec(*aObjectPtr, rowStart, rowEnd, colStart, colEnd);
   CopyFromTo(*matrixPtr, 0, 0, rowCount, colCount, *aObjectPtr, rowStart, colStart);
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Multiply, 1, "Matrix", "Matrix")
{
   auto* argPtr = aVarArgs[0].GetPointer()->GetAppObject<UtMatrixd>();
   if (aObjectPtr->GetCols() != argPtr->GetRows())
   {
      UT_SCRIPT_ABORT("Number of rows of input matrix must math the number of columns of this matrix.");
   }
   auto resultPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), argPtr->GetCols());
   resultPtr->Multiply(*aObjectPtr, *argPtr);
   aReturnVal.SetPointer(new UtScriptRef(resultPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Inverse, 0, "Matrix", "")
{
   if (aObjectPtr->GetRows() != aObjectPtr->GetCols())
   {
      UT_SCRIPT_ABORT("Matrix.Inverse is only defined for a square matrix.");
   }
   try
   {
      auto resultPtr = ut::make_unique<UtMatrixd>();
      resultPtr->Invert(*aObjectPtr);
      aReturnVal.SetPointer(new UtScriptRef(resultPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   catch (const UtMatrix_SingularMatrixError&)
   {
      UT_SCRIPT_ABORT("Matrix is singular.");
   }
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, PseudoInverse, 0, "Matrix", "")
{
   auto resultPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), aObjectPtr->GetCols());
   CopyFromTo(*aObjectPtr, 0, 0, aObjectPtr->GetRows(), aObjectPtr->GetCols(), *resultPtr, 0, 0);
   if (!resultPtr->PseudoInvert())
   {
      UT_SCRIPT_ABORT("Unable to compute PseudoInverse for this Matrix.");
   }
   aReturnVal.SetPointer(new UtScriptRef(resultPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Transpose, 0, "Matrix", "")
{
   auto resultPtr = ut::make_unique<UtMatrixd>();
   resultPtr->Transpose(*aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(resultPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Add, 1, "Matrix", "Matrix")
{
   auto* argPtr = aVarArgs[0].GetPointer()->GetAppObject<UtMatrixd>();
   if (argPtr->GetRows() != aObjectPtr->GetRows())
   {
      UT_SCRIPT_ABORT("Input matrix does not have the same number of rows as this matrix.");
   }
   if (argPtr->GetCols() != aObjectPtr->GetCols())
   {
      UT_SCRIPT_ABORT("Input matrix does not have the same number of columns as this matrix.");
   }
   auto resultPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), aObjectPtr->GetCols());
   resultPtr->Add(*aObjectPtr, *argPtr);
   aReturnVal.SetPointer(new UtScriptRef(resultPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Subtract, 1, "Matrix", "Matrix")
{
   auto* argPtr = aVarArgs[0].GetPointer()->GetAppObject<UtMatrixd>();
   if (argPtr->GetRows() != aObjectPtr->GetRows())
   {
      UT_SCRIPT_ABORT("Input matrix does not have the same number of rows as this matrix.");
   }
   if (argPtr->GetCols() != aObjectPtr->GetCols())
   {
      UT_SCRIPT_ABORT("Input matrix does not have the same number of columns as this matrix.");
   }
   auto resultPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), aObjectPtr->GetCols());
   resultPtr->Subtract(*aObjectPtr, *argPtr);
   aReturnVal.SetPointer(new UtScriptRef(resultPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Scale, 1, "Matrix", "double")
{
   double factor    = aVarArgs[0].GetDouble();
   auto   resultPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), aObjectPtr->GetCols());
   resultPtr->Multiply(*aObjectPtr, factor);
   aReturnVal.SetPointer(new UtScriptRef(resultPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, CholeskyDecomposition, 0, "Matrix", "")
{
   if (aObjectPtr->GetRows() != aObjectPtr->GetCols())
   {
      UT_SCRIPT_ABORT("Matrix.CholeskyDecomposition is only defined for square matrices.");
   }
   auto retvalPtr = ut::make_unique<UtMatrixd>();
   bool success;
   retvalPtr->CholeskyDecomposition(*aObjectPtr, success);
   if (!success)
   {
      UT_SCRIPT_ABORT("Unable to compute Cholesky decomposition");
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, SingularValueDecomposition, 0, "Array<Object>", "")
{
   // Result Matrices
   //  S (n x 1)
   auto sMatPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetCols(), 1);
   //  U (m x n)
   auto uMatPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), aObjectPtr->GetCols());
   //  V (n x n)
   auto vMatPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetCols(), aObjectPtr->GetCols());

   // Call the method
   int err = UtMatrixd::SingularValueDecomposition(*aObjectPtr, *sMatPtr, *uMatPtr, *vMatPtr);

   // Create the return value
   auto returnDataPtr = ut::make_unique<std::vector<UtScriptData>>();
   returnDataPtr->emplace_back(err);
   returnDataPtr->emplace_back(new UtScriptRef{sMatPtr.release(), aObjectClassPtr, UtScriptRef::cMANAGE});
   returnDataPtr->emplace_back(new UtScriptRef{uMatPtr.release(), aObjectClassPtr, UtScriptRef::cMANAGE});
   returnDataPtr->emplace_back(new UtScriptRef{vMatPtr.release(), aObjectClassPtr, UtScriptRef::cMANAGE});
   aReturnVal.SetPointer(new UtScriptRef{returnDataPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(Matrix, UtMatrixd, Eigensystem, 0, "Array<Matrix>", "")
{
   if (aObjectPtr->GetRows() != aObjectPtr->GetCols())
   {
      UT_SCRIPT_ABORT("Cannot compute eigen system for non-square matrices.");
   }

   // We do not modify this matrix, and instead return the eigenvectors in a new matrix.
   auto vectorsPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), aObjectPtr->GetCols());
   CopyFromTo(*aObjectPtr, 0, 0, aObjectPtr->GetRows(), aObjectPtr->GetCols(), *vectorsPtr, 0, 0);
   auto eigvalPtr = ut::make_unique<UtMatrixd>(aObjectPtr->GetRows(), 1);
   vectorsPtr->ReduceToEigenvectors(*eigvalPtr);

   // Create the return value
   auto returnDataPtr = ut::make_unique<std::vector<UtScriptData>>();
   returnDataPtr->emplace_back(new UtScriptRef{eigvalPtr.release(), aObjectClassPtr, UtScriptRef::cMANAGE});
   returnDataPtr->emplace_back(new UtScriptRef{vectorsPtr.release(), aObjectClassPtr, UtScriptRef::cMANAGE});
   aReturnVal.SetPointer(new UtScriptRef{returnDataPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

} // namespace script
} // namespace ut
