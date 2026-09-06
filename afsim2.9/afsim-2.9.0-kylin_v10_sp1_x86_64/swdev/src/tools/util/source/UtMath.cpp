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

#include "UtMath.hpp"

#include <cfloat>
#include <climits>
#include <cmath>
#include <vector>

namespace UtMath
{

double Lerp(const double& aValue, const double& aValueMin, const double& aValueMax, const double& aRangeMin, const double& aRangeMax)
{
   return aRangeMin + ((aValue - aValueMin) * (aRangeMax - aRangeMin)) / (aValueMax - aValueMin);
}

double TempKtoF(double aTempK)
{
   return aTempK * cDEG_F_PER_DEG_K + cABS_ZERO_IN_F;
}

double TempFtoK(double aTempF)
{
   // From UtInput class:
   // aValue = (aValue - 32) * cDEG_K_PER_DEG_F + cZERO_C_IN_K;
   return (aTempF - cZERO_C_IN_F) * cDEG_K_PER_DEG_F + cZERO_C_IN_K;
}

//-------------------------------------------------------------------------------------------------
//! Returns a bool denoting that aAngle fits in between
//! the other two.  (It resolves all the wrap-around issues.)
bool AngleIsBetween(double aAngle, double aLowerLimit, double aUpperLimit)
{
   bool limitsAreEqual = (aLowerLimit == aUpperLimit);

   // This is trickier than it sounds.
   // To eliminate all wrap-around issues:
   // 1.  Force both aLowerLimit and aAngle to be
   //     small non-negative values.
   aLowerLimit = fmod(aLowerLimit, cTWO_PI);
   if (aLowerLimit < 0.0)
   {
      aLowerLimit += cTWO_PI;
   }
   aAngle = fmod(aAngle, cTWO_PI);
   if (aAngle < 0.0)
   {
      aAngle += cTWO_PI;
   }

   // 2.  Force aUpperLimit to be GREATER than aLowerLimit.
   // (This assumes the caller knows what he is doing, and
   //  DOES get the upper and lower limits in the right order!)
   aUpperLimit = fmod(aUpperLimit, cTWO_PI);
   if (aUpperLimit < 0.0)
   {
      aUpperLimit += cTWO_PI;
   }
   if (aUpperLimit < aLowerLimit)
   {
      aUpperLimit += cTWO_PI;
   }
   if (aAngle < aLowerLimit)
   {
      aAngle += cTWO_PI;
   }

   // If the two limits normalized to the same value but were not originally the same value, then we have a full circle.
   if ((aLowerLimit == aUpperLimit) && !limitsAreEqual)
   {
      return true;
   }

   return ((aAngle >= aLowerLimit) && (aAngle <= aUpperLimit));
}

//-------------------------------------------------------------------------------------------------
//! Returns a bool denoting that aAngle is less than
//! abs(aTolerance).  If abs(Tolerance) is greater than PI,
//! the absolute value of the angle nearest zero is assumed.
bool AngleWithinTolerance(double aAngle, double aTolerance)
{
   // First make sure that Tolerance is a "small" angle:
   double tol = fabs(fmod(aTolerance, cTWO_PI));
   return (fabs(fmod(aAngle, cTWO_PI)) < tol);
}

//-------------------------------------------------------------------------------------------------
//! Convert a linearly scaled value to a decibel scaled value.
//! @param aLinearValue Linear value to be converted.
//! @return Decibel scaled value = 10.0 * log(aValue);
double SafeLinearToDB(double aLinearValue)
{
   double linearValue = (aLinearValue > 0.0) ? aLinearValue : DBL_MIN;
   return 10.0 * log10(linearValue);
}

int Round(float aValue)
{
   return static_cast<int>((aValue > 0.0) ? (aValue + 0.5) : (aValue - 0.5));
}

long long Round64(double aValue)
{
   return (long long)((aValue > 0.0) ? (aValue + 0.5) : (aValue - 0.5));
}

double LinearToDB(double aLinearValue)
{
   return 10.0 * log10(aLinearValue);
}

double DB_ToLinear(double aDB_Value)
{
   return pow(10.0, 0.1 * aDB_Value);
}

double NormalizeAngleDeg(double aAngle, double aMinAngle)
{
   double angle = fmod(aAngle, 360.0);
   // For aMinAngle within -360.0 to 360.0 this will loop a max of 1 time.
   // For numbers outside that range, it may loop more often.
   while (angle < aMinAngle)
   {
      angle += 360.0;
   }
   while (angle >= aMinAngle + 360.0)
   {
      angle -= 360.0;
   }
   return angle;
}

double NormalizeSinCosMinusOne_One(double aSinCos)
{
   if (aSinCos < -1.0)
   {
      aSinCos = -1.0;
   }
   else if (aSinCos > 1.0)
   {
      aSinCos = 1.0;
   }
   return aSinCos;
}

double NormalizeAngle0_360(double aAngle)
{
   return NormalizeAngleDeg(aAngle, 0.0);
}

double NormalizeAngleMinus180_180(double aAngle)
{
   return NormalizeAngleDeg(aAngle, -180.0);
}

double NormalizeAngleRad(double aAngle, double aMinAngle)
{
   double angle = fmod(aAngle, cTWO_PI);
   // For aMinAngle within -cTWO_PI to cTWO_PI this will loop a max of 1 time.
   // For numbers outside that range, it may loop more often.
   while (angle < aMinAngle)
   {
      angle += cTWO_PI;
   }
   while (angle >= aMinAngle + cTWO_PI)
   {
      angle -= cTWO_PI;
   }
   return angle;
}

double NormalizeAngle0_TwoPi(double aAngle)
{
   return NormalizeAngleRad(aAngle, 0.0);
}

double NormalizeAngleMinusPi_Pi(double aAngle)
{
   return NormalizeAngleRad(aAngle, -cPI);
}

void NormalizeAzElAnglesRad(double& aAz, double& aEl)
{
   // Normalize both aAz and aEl to -cPI to cPI
   aAz = NormalizeAngleRad(aAz, -cPI);
   aEl = NormalizeAngleRad(aEl, -cPI);
   // Normalize el to [-PI/2, PI/2], if el is outside this range,
   //  then the aAz sign has to be changed (add cPI, and normalize)
   if (aEl < -cPI_OVER_2)
   {
      // aEl = -cPI_OVER_2 - (aEl - -cPI_OVER_2) = -aEl - cPI;
      aEl = -aEl - cPI;
      aAz = UtMath::NormalizeAngleRad(aAz + UtMath::cPI, -UtMath::cPI);
   }
   if (aEl > cPI_OVER_2)
   {
      // aEl = cPI_OVER_2 - ( aEl - cPI_OVER_2) = -aEl + cPI
      aEl = -aEl + cPI;
      aAz = UtMath::NormalizeAngleRad(aAz - UtMath::cPI, -UtMath::cPI);
   }
}

double AngleBisector(double aAngle1, double aAngle2)
{
   double bisector = aAngle1 + (aAngle2 - aAngle1) / 2.0;
   return NormalizeAngleMinusPi_Pi(bisector);
}

double AngleDifference(double aAngle1, double aAngle2)
{
   return fabs(NormalizeAngleMinusPi_Pi(aAngle2 - aAngle1));
}

double NextAfter(double aValue, double aTowards)
{
#ifdef _WIN32
   return _nextafter(aValue, aTowards);
#else
   return nextafter(aValue, aTowards);
#endif
}

//! Compute the value of the error function.
//! @note This routine was a simple translation of the routine presented in the following referenced
//! solution. Please see the implementation for the copyright statement for this code. The implementation
//! assumes that the compiler uses an IEEE double. If this is not the case, this routine may likely
//! fail.
//! @ref https://math.stackexchange.com/questions/263216/error-function-erf-with-better-precision
double ErrorFunction(double aValue)
{
   /*
   Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
   *
   * Developed at SunPro, a Sun Microsystems, Inc. business.
   * Permission to use, copy, modify, and distribute this
   * software is freely granted, provided that this notice
   * is preserved.
   */

   static const double tiny = 1e-300;
   static const double one  = 1.00000000000000000000e+00; /* 0x3FF00000, 0x00000000 */
   static const double erx  = 8.45062911510467529297e-01; /* 0x3FEB0AC1, 0x60000000 */
   /*
    * Coefficients for approximation to erf on [0,0.84375]
    */
   static const double efx  = 1.28379167095512586316e-01;  /* 0x3FC06EBA, 0x8214DB69 */
   static const double efx8 = 1.02703333676410069053e+00;  /* 0x3FF06EBA, 0x8214DB69 */
   static const double pp0  = 1.28379167095512558561e-01;  /* 0x3FC06EBA, 0x8214DB68 */
   static const double pp1  = -3.25042107247001499370e-01; /* 0xBFD4CD7D, 0x691CB913 */
   static const double pp2  = -2.84817495755985104766e-02; /* 0xBF9D2A51, 0xDBD7194F */
   static const double pp3  = -5.77027029648944159157e-03; /* 0xBF77A291, 0x236668E4 */
   static const double pp4  = -2.37630166566501626084e-05; /* 0xBEF8EAD6, 0x120016AC */
   static const double qq1  = 3.97917223959155352819e-01;  /* 0x3FD97779, 0xCDDADC09 */
   static const double qq2  = 6.50222499887672944485e-02;  /* 0x3FB0A54C, 0x5536CEBA */
   static const double qq3  = 5.08130628187576562776e-03;  /* 0x3F74D022, 0xC4D36B0F */
   static const double qq4  = 1.32494738004321644526e-04;  /* 0x3F215DC9, 0x221C1A10 */
   static const double qq5  = -3.96022827877536812320e-06; /* 0xBED09C43, 0x42A26120 */
   /*
    * Coefficients for approximation to erf in [0.84375,1.25]
    */
   static const double pa0 = -2.36211856075265944077e-03; /* 0xBF6359B8, 0xBEF77538 */
   static const double pa1 = 4.14856118683748331666e-01;  /* 0x3FDA8D00, 0xAD92B34D */
   static const double pa2 = -3.72207876035701323847e-01; /* 0xBFD7D240, 0xFBB8C3F1 */
   static const double pa3 = 3.18346619901161753674e-01;  /* 0x3FD45FCA, 0x805120E4 */
   static const double pa4 = -1.10894694282396677476e-01; /* 0xBFBC6398, 0x3D3E28EC */
   static const double pa5 = 3.54783043256182359371e-02;  /* 0x3FA22A36, 0x599795EB */
   static const double pa6 = -2.16637559486879084300e-03; /* 0xBF61BF38, 0x0A96073F */
   static const double qa1 = 1.06420880400844228286e-01;  /* 0x3FBB3E66, 0x18EEE323 */
   static const double qa2 = 5.40397917702171048937e-01;  /* 0x3FE14AF0, 0x92EB6F33 */
   static const double qa3 = 7.18286544141962662868e-02;  /* 0x3FB2635C, 0xD99FE9A7 */
   static const double qa4 = 1.26171219808761642112e-01;  /* 0x3FC02660, 0xE763351F */
   static const double qa5 = 1.36370839120290507362e-02;  /* 0x3F8BEDC2, 0x6B51DD1C */
   static const double qa6 = 1.19844998467991074170e-02;  /* 0x3F888B54, 0x5735151D */
   /*
    * Coefficients for approximation to erfc in [1.25,1/0.35]
    */
   static const double ra0 = -9.86494403484714822705e-03; /* 0xBF843412, 0x600D6435 */
   static const double ra1 = -6.93858572707181764372e-01; /* 0xBFE63416, 0xE4BA7360 */
   static const double ra2 = -1.05586262253232909814e+01; /* 0xC0251E04, 0x41B0E726 */
   static const double ra3 = -6.23753324503260060396e+01; /* 0xC04F300A, 0xE4CBA38D */
   static const double ra4 = -1.62396669462573470355e+02; /* 0xC0644CB1, 0x84282266 */
   static const double ra5 = -1.84605092906711035994e+02; /* 0xC067135C, 0xEBCCABB2 */
   static const double ra6 = -8.12874355063065934246e+01; /* 0xC0545265, 0x57E4D2F2 */
   static const double ra7 = -9.81432934416914548592e+00; /* 0xC023A0EF, 0xC69AC25C */
   static const double sa1 = 1.96512716674392571292e+01;  /* 0x4033A6B9, 0xBD707687 */
   static const double sa2 = 1.37657754143519042600e+02;  /* 0x4061350C, 0x526AE721 */
   static const double sa3 = 4.34565877475229228821e+02;  /* 0x407B290D, 0xD58A1A71 */
   static const double sa4 = 6.45387271733267880336e+02;  /* 0x40842B19, 0x21EC2868 */
   static const double sa5 = 4.29008140027567833386e+02;  /* 0x407AD021, 0x57700314 */
   static const double sa6 = 1.08635005541779435134e+02;  /* 0x405B28A3, 0xEE48AE2C */
   static const double sa7 = 6.57024977031928170135e+00;  /* 0x401A47EF, 0x8E484A93 */
   static const double sa8 = -6.04244152148580987438e-02; /* 0xBFAEEFF2, 0xEE749A62 */
   /*
    * Coefficients for approximation to erfc in [1/.35,28]
    */
   static const double rb0 = -9.86494292470009928597e-03; /* 0xBF843412, 0x39E86F4A */
   static const double rb1 = -7.99283237680523006574e-01; /* 0xBFE993BA, 0x70C285DE */
   static const double rb2 = -1.77579549177547519889e+01; /* 0xC031C209, 0x555F995A */
   static const double rb3 = -1.60636384855821916062e+02; /* 0xC064145D, 0x43C5ED98 */
   static const double rb4 = -6.37566443368389627722e+02; /* 0xC083EC88, 0x1375F228 */
   static const double rb5 = -1.02509513161107724954e+03; /* 0xC0900461, 0x6A2E5992 */
   static const double rb6 = -4.83519191608651397019e+02; /* 0xC07E384E, 0x9BDC383F */
   static const double sb1 = 3.03380607434824582924e+01;  /* 0x403E568B, 0x261D5190 */
   static const double sb2 = 3.25792512996573918826e+02;  /* 0x40745CAE, 0x221B9F0A */
   static const double sb3 = 1.53672958608443695994e+03;  /* 0x409802EB, 0x189D5118 */
   static const double sb4 = 3.19985821950859553908e+03;  /* 0x40A8FFB7, 0x688C246A */
   static const double sb5 = 2.55305040643316442583e+03;  /* 0x40A3F219, 0xCEDF3BE6 */
   static const double sb6 = 4.74528541206955367215e+02;  /* 0x407DA874, 0xE79FE763 */
   static const double sb7 = -2.24409524465858183362e+01; /* 0xC03670E2, 0x42712D62 */

   int    n0;
   int    hx;
   int    ix;
   double R;
   double S;
   double s;
   double z;
   double r;
   n0 = ((*(int*)&one) >> 29) ^ 1;
   hx = *(n0 + (int*)&aValue);
   ix = hx & 0x7fffffff;

   if (ix >= 0x7ff00000)
   { /* erf(nan)=nan */
      int i = ((unsigned)hx >> 31) << 1;
      return (double)(1 - i) + one / aValue; /* erf(+-inf)=+-1 */
   }

   if (ix < 0x3feb0000)
   { /* |aValue|<0.84375 */
      if (ix < 0x3e300000)
      { /* |aValue|<2**-28 */
         if (ix < 0x00800000)
         {
            return 0.125 * (8.0 * aValue + efx8 * aValue); /*avoid underflow */
         }
         return aValue + efx * aValue;
      }
      z        = aValue * aValue;
      r        = pp0 + z * (pp1 + z * (pp2 + z * (pp3 + z * pp4)));
      s        = one + z * (qq1 + z * (qq2 + z * (qq3 + z * (qq4 + z * qq5))));
      double y = r / s;
      return aValue + aValue * y;
   }

   if (ix < 0x3ff40000)
   { /* 0.84375 <= |aValue| < 1.25 */
      s        = fabs(aValue) - one;
      double P = pa0 + s * (pa1 + s * (pa2 + s * (pa3 + s * (pa4 + s * (pa5 + s * pa6)))));
      double Q = one + s * (qa1 + s * (qa2 + s * (qa3 + s * (qa4 + s * (qa5 + s * qa6)))));
      if (hx >= 0)
      {
         return erx + P / Q;
      }
      else
      {
         return -erx - P / Q;
      }
   }

   if (ix >= 0x40180000)
   { /* inf>|aValue|>=6 */
      if (hx >= 0)
      {
         return one - tiny;
      }
      else
      {
         return tiny - one;
      }
   }

   aValue = fabs(aValue);
   s      = one / (aValue * aValue);
   if (ix < 0x4006DB6E)
   { /* |aValue| < 1/0.35 */
      R = ra0 + s * (ra1 + s * (ra2 + s * (ra3 + s * (ra4 + s * (ra5 + s * (ra6 + s * ra7))))));
      S = one + s * (sa1 + s * (sa2 + s * (sa3 + s * (sa4 + s * (sa5 + s * (sa6 + s * (sa7 + s * sa8)))))));
   }
   else
   { /* |aValue| >= 1/0.35 */
      R = rb0 + s * (rb1 + s * (rb2 + s * (rb3 + s * (rb4 + s * (rb5 + s * rb6)))));
      S = one + s * (sb1 + s * (sb2 + s * (sb3 + s * (sb4 + s * (sb5 + s * (sb6 + s * sb7))))));
   }

   z                    = aValue;
   *(1 - n0 + (int*)&z) = 0;
   r                    = exp(-z * z - 0.5625) * exp((z - aValue) * (z + aValue) + R / S);

   if (hx >= 0)
   {
      return one - r / aValue;
   }
   else
   {
      return r / aValue - one;
   }
}

}; // namespace UtMath
