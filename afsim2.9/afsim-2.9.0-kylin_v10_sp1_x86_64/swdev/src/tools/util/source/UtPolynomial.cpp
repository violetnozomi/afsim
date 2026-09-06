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

#include "UtPolynomial.hpp"

#include <cstddef>

#include "UtMath.hpp"

// static
//! Solves a 1 degree polynomial.  Returns the number of solutions.
//! @param aCoefficients  The array of coefficients.
//! @param aSolutions     Filled with all solutions.
//! @note  Solves in the form C[0] + C[1]x = 0
int UtPolynomial::SolveLinear(const double aCoefficients[2], double aSolutions[1])
{
   int num = 0;
   if (aCoefficients[1] != 0.0)
   {
      num           = 1;
      aSolutions[0] = -aCoefficients[0] / aCoefficients[1];
   }
   return num;
}

// static
//! Solves a 2 degree polynomial.  Returns the number of solutions.
//! @param aCoefficients  The array of coefficients.
//! @param aSolutions     Filled with all solutions.
//! @note  Solves in the form C[0] + C[1]x + C[2]x^2 = 0
int UtPolynomial::SolveQuadric(const double aCoefficients[3], double aSolutions[2])
{
   if (aCoefficients[2] == 0.0)
   {
      return SolveLinear(aCoefficients, aSolutions);
   }

   /* normal form: x^2 + px + q = 0 */
   double p = aCoefficients[1] / (2 * aCoefficients[2]);
   double q = aCoefficients[0] / aCoefficients[2];

   double D = p * p - q;

   if (D == 0.0)
   {
      aSolutions[0] = -p;
      return 1;
   }
   else if (D < 0)
   {
      return 0;
   }

   double sqrt_D = sqrt(D);

   aSolutions[0] = sqrt_D - p;
   aSolutions[1] = -sqrt_D - p;
   return 2;
}

// static
//! Solves a 2 degree polynomial.  Returns true if there is a positive solution.
//! @param aCoefficients  The array of coefficients.
//! @param aResult        Set to the smallest positive solution if one is found.
//! @note  Solves in the form C[0] + C[1]x + C[2]x^2 = 0
// static
bool UtPolynomial::SolveQuadricMinPos(const double aCoefficients[3], double& aResult)
{
   bool   success(false);
   double results[2] = {-1.0, -1.0};
   int    count      = SolveQuadric(aCoefficients, results);
   if (count >= 1 && results[0] >= 0.0)
   {
      success = true;
      aResult = results[0];
      if (count == 2 && results[1] >= 0.0)
      {
         aResult = results[1];
      }
   }
   return success;
}

//! Solves a 3 degree polynomial.  Returns the number of solutions.
//! @param aCoefficients  The array of coefficients.
//! @param aSolutions     Filled with all solutions.
//! @note  Solves in the form C[0] + C[1]x + C[2]x^2 + C[3]x^3 = 0
//! @note  Wikipedia shows the math for Cardano's formula with some minor substitutions:
//! https://en.wikipedia.org/wiki/Cubic_function#Cardano.27s_method .
// static
int UtPolynomial::SolveCubic(const double aCoefficients[4], double aSolutions[3])
{
   if (aCoefficients[3] == 0.0)
   {
      return SolveQuadric(aCoefficients, aSolutions);
   }

   int num;

   /* normal form: x^3 + Ax^2 + Bx + C = 0 */

   double A = aCoefficients[2] / aCoefficients[3];
   double B = aCoefficients[1] / aCoefficients[3];
   double C = aCoefficients[0] / aCoefficients[3];

   /*  substitute x = y - A/3 to eliminate quadric term:
   x^3 +px + q = 0 */

   double sq_A = A * A;
   double p    = 1.0 / 3 * (-1.0 / 3 * sq_A + B);
   double q    = 1.0 / 2 * (2.0 / 27 * A * sq_A - 1.0 / 3 * A * B + C);

   /* use Cardano's formula */

   double cb_p = p * p * p;
   double D    = q * q + cb_p;

   if (D == 0.0)
   {
      if (q == 0.0) /* one triple solution */
      {
         aSolutions[0] = 0;
         num           = 1;
      }
      else /* one single and one double solution */
      {
         double u      = pow(-q, 1.0 / 3.0);
         aSolutions[0] = 2 * u;
         aSolutions[1] = -u;
         num           = 2;
      }
   }
   else if (D < 0) /* Casus irreducibilis: three real solutions */
   {
      double phi = 1.0 / 3 * acos(-q / sqrt(-cb_p));
      double t   = 2 * sqrt(-p);

      aSolutions[0] = t * cos(phi);
      aSolutions[1] = -t * cos(phi + UtMath::cPI / 3.0);
      aSolutions[2] = -t * cos(phi - UtMath::cPI / 3.0);
      num           = 3;
   }
   else /* one real solution */
   {
      double sqrt_D = sqrt(D);
      double u      = cbrt(-q + sqrt_D);
      double v      = cbrt(-q - sqrt_D);

      aSolutions[0] = u + v;
      num           = 1;
   }

   /* resubstitute */

   // sub = 1.0 / 3.0 * A;
   double sub = A / 3.0;

   for (int i = 0; i < num; ++i)
   {
      aSolutions[i] -= sub;
   }

   return num;
}

//! Solves a 4 degree polynomial.  Returns the number of solutions.
//! @param aCoefficients  The array of coefficients.
//! @param aSolutions     Filled with all solutions.
//! @note  Solves in the form C[0] + C[1]x + C[2]x^2 + C[3]x^3 + C[4]x^4 = 0
//! @note Algorithm follows Ferrari's solution
//! (e.g., see https://en.wikipedia.org/wiki/Quartic_function#Ferrari.27s_solution ).
int UtPolynomial::SolveQuartic(const double aCoefficients[5], double aSolutions[4])
{
   if (aCoefficients[4] == 0.0)
   {
      return SolveCubic(aCoefficients, aSolutions);
   }
   else if ((aCoefficients[1] == 0.0) && (aCoefficients[3] == 0.0))
   {
      int num = 0;

      // Bi-quadratic solution ax^4 + cx^2 + e = 0 (four roots).
      double coeffs[3] = {aCoefficients[0], aCoefficients[2], aCoefficients[4]};
      double solutions[2];
      size_t numQuadric = SolveQuadric(coeffs, solutions);
      if (numQuadric > 0)
      {
         for (size_t solutionNum = 0; solutionNum < numQuadric; ++solutionNum)
         {
            double solution = solutions[solutionNum];

            // Keep only real solutions
            if (solution > 0.0)
            {
               aSolutions[num]     = sqrt(solution);
               aSolutions[num + 1] = -aSolutions[num];
               num += 2;
            }
            else if (solution == 0.0)
            {
               aSolutions[num++] = 0.0;
            }
         }
      }
      return num;
   }

   // Otherwise, provide the general solution:
   double coeffs[4];

   /* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */

   double A = aCoefficients[3] / aCoefficients[4];
   double B = aCoefficients[2] / aCoefficients[4];
   double C = aCoefficients[1] / aCoefficients[4];
   double D = aCoefficients[0] / aCoefficients[4];

   /*  substitute x = y - A/4 to eliminate cubic term:
   x^4 + px^2 + qx + r = 0 */

   double sq_A = A * A;
   double p    = -3.0 / 8 * sq_A + B;
   double q    = 1.0 / 8 * sq_A * A - 1.0 / 2 * A * B + C;
   double r    = -3.0 / 256 * sq_A * sq_A + 1.0 / 16 * sq_A * B - 1.0 / 4 * A * C + D;

   int num = 0;
   if (r == 0.0)
   {
      /* no absolute term: y(y^3 + py + q) = 0 */

      coeffs[0] = q;
      coeffs[1] = p;
      coeffs[2] = 0;
      coeffs[3] = 1;

      num = SolveCubic(coeffs, aSolutions);

      aSolutions[num++] = 0;
   }
   else
   {
      /* y^4 + py^3 + qy + r = 0 (depressed quartic) */
      /* As the bi-quadratic case is handled above, q and m will be nonzero*/
      /* solve the resolvent cubic ... */
      /* m^3 + pm^2 + (1/4*p^2 - r)m - 1/8*q^2 = 0 */
      coeffs[0] = -0.125 * q * q;
      coeffs[1] = 0.25 * p * p - r;
      coeffs[2] = p;
      coeffs[3] = 1;

      SolveCubic(coeffs, aSolutions);

      /* ... and take the one real solution ... */
      double m    = aSolutions[0];
      double var1 = sqrt(2 * m);
      double var2 = q * sqrt(2.0 / m);
      double var3 = 2.0 * p + 2.0 * m;
      double var4 = -(var3 + var2);
      if (var4 != 0.0)
      {
         if (var4 > 0.0)
         {
            double var5       = sqrt(var4);
            aSolutions[num++] = (var1 + var5) / 2.0;
            aSolutions[num++] = (var1 - var5) / 2.0;
         }

         var4 = -(var3 - var2);
         if (var4 > 0.0)
         {
            double var5       = sqrt(var4);
            aSolutions[num++] = (-var1 + var5) / 2.0;
            aSolutions[num++] = (-var1 - var5) / 2.0;
         }
      }
      else // var4 == 0.0
      {
         aSolutions[num++] = var1 / 2.0;
         aSolutions[num++] = -var1 / 2.0;
      }
   }

   /* resubstitute */
   double sub = 1.0 / 4 * A;
   for (int i = 0; i < num; ++i)
   {
      aSolutions[i] -= sub;
   }

   return num;
}
