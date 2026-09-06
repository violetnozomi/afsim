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
#include "UtMatrix.hpp"

UtMatrix_NonConvergenceError::UtMatrix_NonConvergenceError()
   : UtException("UtMatrix: Iterative solution not converging error.")
{
}

UtMatrix_SingularMatrixError::UtMatrix_SingularMatrixError()
   : UtException("UtMatrix: Singular matrix error.")
{
}

UtMatrix_OutOfBoundsError::UtMatrix_OutOfBoundsError()
   : UtException("UtMatrix: Index out of bounds error.")
{
}

UtMatrix_IncompatibleSizeError::UtMatrix_IncompatibleSizeError()
   : UtException("UtMatrix: Incompatible size error.")
{
}

//#define TEST_UTMATRIX
#ifdef TEST_UTMATRIX
int main()
{
   // unit test of SVD
   /*

      double Araw[3][2] = { {1.0, 2.0},
                            {3.0, 4.0},
                            {5.0, 6.0} };
      UtMatrixd A(3, 2);
      for (int i=0; i<3; ++i)
      {
         for (int j=0; j<2; ++j)
         {
            A(i,j) = Araw[i][j];
         }
      }
      UtMatrixd  U(3, 3);
      UtMatrixd VT(2, 2);
      UtMatrixd  S(2);
   */
   double    Araw[3][3] = {{1.0, 0.0, 5.0}, {-10.0, 2.0, -10.0}, {4.0, 1.0, 0.0}};
   UtMatrixd A(3, 3);
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         A(i, j) = Araw[i][j];
      }
   }
   UtMatrixd U(3, 3);
   UtMatrixd VT(3, 3);
   UtMatrixd S(3);

   UtMatrixd::SingularValueDecomposition(A, S, U, VT);

   // Test of the standard invert algorithm
   double    Braw[2][2] = {{0.0, 0.5}, {1.0, 0.0}};
   UtMatrixd B(2, 2);
   for (int i = 0; i < 2; ++i)
   {
      for (int j = 0; j < 2; ++j)
      {
         B(i, j) = Braw[i][j];
      }
   }

   UtMatrixd Binv = B;
   Binv.Invert();

   UtMatrixd ident = B * Binv;

   // Now try a singular matrix.
   try
   {
      UtMatrixd sm(2, 2);
      sm(0, 0) = 0.0;
      sm(0, 1) = 5.0;
      sm(1, 0) = 0.0;
      sm(0, 1) = -90.0;
      sm.Invert();
   }
   catch (...)
   {
      ut::log::error() << "Singular...";
   }
   return 0;
}
#endif
