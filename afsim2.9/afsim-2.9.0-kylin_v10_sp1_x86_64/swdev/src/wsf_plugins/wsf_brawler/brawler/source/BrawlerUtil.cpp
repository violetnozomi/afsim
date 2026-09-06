// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "BrawlerUtil.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <math.h>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBuffer.hpp"
#include "UtLog.hpp"

namespace BRAWLER
{

double amin1(const double a, const double b)
{
   return std::min(a, b);
}

double amax1(const double a, const double b)
{
   return std::max(a, b);
}

double amax1(const double a, const double b, const double c)
{
   return std::max(a, std::max(b, c));
}

int sign(const int a, const int b)
{
   return ((b >= 0) ? (std::abs(a)) : (-std::abs(a)));
}

double sign(const double a, const double b)
{
   return ((b >= 0) ? (std::abs(a)) : (-std::abs(a)));
}

bool btest(const int var, const int pos)
{
   return std::bitset<sizeof(int)>(var).test(pos);
}

bool lbit(const int word, const int n)
{
   return btest(word, n - 1);
}

double arccos(const double a)
{
   return std::acos(a);
}

double arcsin(const double a)
{
   return std::asin(a);
}

double avnorm(const UtVec3d& x)
{
   return std::max(std::abs(x[0]), std::max(std::abs(x[1]), std::abs(x[2])));
}

double border(const double z, const double z0)
{
   double border = 0;
   if (z <= 0)
   {
      double t = (z / z0 - 1.0);
      border   = 1.0 / (1.0 + (t * t));
   }
   else
   {
      double u = (z / z0 + 1.0) * (z / z0 + 1.0);
      border   = u / (1 + u);
   }
   return border;
}

double cauchy(const double z, const double z0)
{
   double t = (z / z0);
   return 1.0 / (1.0 + (t * t));
}

double reward(const double z, const double z0)
{
   double usq = (z / z0) * (z / z0);
   return (1. - usq) / (1. + usq);
}

double dist(const UtVec3d& a, const UtVec3d& b)
{
   return (a - b).Magnitude();
}

double energy(const UtVec3d& pos, const UtVec3d& vel)
{
   return -pos[3 - 1] + vel.DotProduct(vel) / (2.0 * grav);
}

double egy(double z, double s)
{
   return (-z + s * s / (2.0 * grav));
}

UtFortMat3d makecs(const UtVec3d& xVec, const UtVec3d& zVec)
{
   UtFortMat3d coordSystem;
   UtVec3d     bx;
   UtVec3d     by;
   UtVec3d     bz;

   // Normalize xVec, store in bx
   xVec.Normalize(bx);
   // Cross product of zVec and Normalized xVec, stored in by
   UtVec3d::CrossProduct(by.GetData(), zVec.GetData(), bx.GetData());
   // Normalize by and evaluate the magnitude of original by
   if (by.Normalize() == 0.0)
   {
      // If the magnitude of by is zero, replace by with cross of bx and unit Z vector
      UtVec3d unitZ(0.0, 0.0, 1.0);
      UtVec3d::CrossProduct(by.GetData(), unitZ.GetData(), bx.GetData());
      by.Normalize();
   }
   // cross product of bx and by stored in bz
   UtVec3d::CrossProduct(bz.GetData(), bx.GetData(), by.GetData());

   coordSystem[0] = bx.Get(0); // b(1,1) = bx(1)
   coordSystem[3] = bx.Get(1); // b(1,2) = bx(2)
   coordSystem[6] = bx.Get(2); // b(1,3) = bx(3)
   coordSystem[1] = by.Get(0); // b(2,1) = by(1)
   coordSystem[4] = by.Get(1); // b(2,2) = by(2)
   coordSystem[7] = by.Get(2); // b(2,3) = by(3)
   coordSystem[2] = bz.Get(0); // b(3,1) = bz(1)
   coordSystem[5] = bz.Get(1); // b(3,2) = bz(2)
   coordSystem[8] = bz.Get(2); // b(3,3) = bz(3)
   return coordSystem;
}

UtFortMat3d makcsi(const UtVec3d& xVec, const UtVec3d& zVec)
{
   UtFortMat3d coordSystem;
   UtVec3d     bx;
   UtVec3d     by;
   UtVec3d     bz;

   // Normalize xVec, store in bx
   xVec.Normalize(bx);
   // Cross product of zVec and Normalized xVec, stored in by
   UtVec3d::CrossProduct(by.GetData(), zVec.GetData(), bx.GetData());
   // Normalize by and evaluate the magnitude of original by
   if (by.Normalize() == 0.0)
   {
      // If the magnitude of by is zero, replace by with cross of unit Z vector and bx
      UtVec3d unitZ(0.0, 0.0, 1.0);
      UtVec3d::CrossProduct(by.GetData(), bx.GetData(), unitZ.GetData()); // This line only difference from makecs()
      by.Normalize();
   }
   // cross product of bx and by stored in bz
   UtVec3d::CrossProduct(bz.GetData(), bx.GetData(), by.GetData());

   coordSystem[0] = bx.Get(0); // b(1,1) = bx(1)
   coordSystem[3] = bx.Get(1); // b(1,2) = bx(2)
   coordSystem[6] = bx.Get(2); // b(1,3) = bx(3)
   coordSystem[1] = by.Get(0); // b(2,1) = by(1)
   coordSystem[4] = by.Get(1); // b(2,2) = by(2)
   coordSystem[7] = by.Get(2); // b(2,3) = by(3)
   coordSystem[2] = bz.Get(0); // b(3,1) = bz(1)
   coordSystem[5] = bz.Get(1); // b(3,2) = bz(2)
   coordSystem[8] = bz.Get(2); // b(3,3) = bz(3)
   return coordSystem;
}

UtFortMat3d makex(const UtVec3d& cxin)
{
   UtVec3d unitZ(0.0, 0.0, 1.0);
   return makecs(cxin, unitZ);
}

// TODO - verify please
UtFortMat3d makeh(const UtVec3d& cxin)
{
   UtFortMat3d c;
   // C     --local declarations
   double temp = 0;
   // C*ENDDEC
   temp = double(cxin[0]) * cxin[0] + double(cxin[1]) * cxin[1];
   temp = sqrt(temp);
   c[0] = cxin[0] / double(temp); // c[0][0] = cxin[0]/double(temp);
   c[3] = cxin[1] / double(temp); // c[0][1] = cxin[1]/double(temp);
   c[6] = 0.0;                    // c[0][2] = 0.0;
   c[1] = -c[3];                  // c[1][0] = -c[0][1];
   c[4] = c[0];                   // c[1][1] =  c[0][0];
   c[7] = 0.0;                    // c[1][2] = 0.0;
   c[2] = 0.0;                    // c[2][0] = 0.0;
   c[5] = 0.0;                    // c[2][1] = 0.0;
   c[8] = 1.0;                    // c[2][2] = 1.0;
   return c;
}
double headeg(UtVec3d v)
{
   double headeg;
   headeg = atan2(v[0], -v[1]) / rad;
   if (headeg < 0)
   {
      headeg = headeg + 360;
   }
   return headeg;
}
double ramp(const double xlo, const double xval, const double xhi)
{
   double ramp = (xval - xlo) / (xhi - xlo);
   if (ramp <= 0.0)
   {
      ramp = 0.0;
   }
   if (ramp >= 1.0)
   {
      ramp = 1.0;
   }
   return ramp;
}

void vsplit(const UtVec3d& v, const UtVec3d& x, UtVec3d& vl, UtVec3d& vt)
{
   // rescale const = DOT(V,X)/XMAG(X)**2
   double rescaleConst = (x[0] * v[0] + x[1] * v[1] + x[2] * v[2]) / (x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
   vl[0]               = x[0] * rescaleConst;
   vt[0]               = v[0] - vl[0];
   vl[1]               = x[1] * rescaleConst;
   vt[1]               = v[1] - vl[1];
   vl[2]               = x[2] * rescaleConst;
   vt[2]               = v[2] - vl[2];
}

double xlimit(const double xreq, const double xmax, const double xmin)
{
   double xlimit = xreq;
   if (xreq > xmax)
   {
      xlimit = xmax;
   }
   else if (xreq < xmin)
   {
      xlimit = xmin;
   }
   return xlimit;
}

UtFortMat3d bodyax(const UtFortMat3d& rwe, // returns rbe
                   double             alpha)
{
   UtFortMat3d rbe;
   // UtFortMat3d rwb;
   UtFortMat3d rwbt;
   // call getrwb(alpha,rwb);
   // call mtrans(rwb,rwbt);
   // we construct body->wind matrix and transpose it in same step
   // so intermediate rwb matrix not needed
   rwbt.BuildTranspose(0.0, 1.0, sin(alpha), cos(alpha), 0.0, 1.0);
   // call mlt333(rwbt,rwe,rbe);
   rbe = rwbt * rwe;
   return rbe;
}

UtFortMat3d NEDToECSRotation(UtEntity& ref)
{
   UtFortMat3d r;
   double      t[3][3];
   ref.GetNEDToECSTransform(t);
   r[0] = t[0][0];
   r[1] = t[1][0];
   r[2] = t[2][0];
   r[3] = t[0][1];
   r[4] = t[1][1];
   r[5] = t[2][1];
   r[6] = t[0][2];
   r[7] = t[1][2];
   r[8] = t[2][2];
   return r;
}

UtFortMat3d mrotx(UtFortMat3d a, double chi)
{
   double      cc, sc;
   UtFortMat3d rx;
   cc    = std::cos(chi);
   sc    = std::sin(chi);
   rx[4] = cc;
   rx[8] = cc;
   rx[5] = -sc;
   rx[7] = sc;
   // Replaces mlt333
   return (rx.Multiply(a));
}

// from atmos.f  calculating DR
double AtmosphereDensityRatio(double altitude)
{
   double alt1 = 36089.24;
   double alt2 = 65616.80;
   double alt3 = 104987.00;
   double alt4 = 154199.00;
   double alt5 = 167323.00;
   // double alt6 = 232940.00;
   double c1  = 6.87535e-6;
   double c2  = 5.2561;
   double c3  = -4.80634e-5;
   double c6  = 5.4864e-04;
   double c7  = 353.97;
   double c8  = -34.1743;
   double c9  = -13.20114;
   double c10 = 11.20114;

   double h   = std::max(altitude, 1.0);
   double dr  = 0; // density ratio
   double fac = 0;

   if (h <= alt1)
   {
      // C        --TROPOSPHERE CALCULATION
      fac = 1.0 - c1 * h;
      dr  = pow(fac, (c2 - 1.0));
   }
   else if (h <= alt2)
   {
      // C        --STRATOSPHERE CALCULATION
      h   = h - alt1;
      fac = exp(c3 * h);
      dr  = 0.29708 * fac;
   }
   else if (h <= alt3)
   {
      // C        --MESOSPHERE CALCULATION
      fac = (c6 * h + c7) * 0.0025643;
      dr  = 0.071867 * pow(fac, (c8 - 1.0));
   }
   else if (h <= alt4)
   {
      dr = pow((0.857003 + h / 190115.), (c9));
   }
   else if (h <= alt5)
   {
      // C        --Thermoshpere
      fac = exp((h - alt4) / -25992.);
      dr  = 0.00116533 * fac;
   }
   else
   {
      // C        --CHECK TO SEE IF THE LIMITS OF THE ALGORITHM HAVE BEEN EXCEEDED
      dr = pow((0.79899 - h / 606330.), (c10));
   }
   return dr;
}

// from atmos.f  calculating VS
double SpeedOfSound(double altitude)
{
   double vs = 0;
   double t  = 0;
   // C     --local declarations
   double h   = 0;
   double fac = 0;
   double tr  = 0;
   double c1  = 6.87535e-6;
   // double c2 = 5.2561;
   double c3 = -4.80634e-5;
   double c6 = 5.4864e-04;
   double c7 = 353.97;
   // double c8 = -34.1743;
   // double c9 = -13.20114;
   // double c10 = 11.20114;
   double alt1 = 36089.24;
   double alt2 = 65616.8;
   double alt3 = 104987.0;
   double alt4 = 154199.0;
   double alt5 = 167323.0;
   double alt6 = 232940.0;
   bool   warn = true;
   // save warn //TODO - does this make "warn" a static variable???
   // C*ENDDEC
   h = altitude;
   if (h < 0.0)
   {
      // if (h < -5000.0)     //par.fi, trkr_neg_alt = 5000.0
      //{
      //    nabort("h < -trkr_neg_alt");
      // }
      // else
      {
         h = 1.0;
      }
   }
   if (h <= alt1)
   {
      // C        --TROPOSPHERE CALCULATION
      fac = 1.0 - c1 * h;
      tr  = fac;
   }
   else if (h <= alt2)
   {
      // C        --STRATOSPHERE CALCULATION
      h   = h - alt1;
      fac = exp(c3 * h);
      tr  = 0.75187;
   }
   else if (h <= alt3)
   {
      // C        --MESOSPHERE CALCULATION
      fac = (c6 * h + c7) * 0.0025643;
      tr  = fac * 0.751865;
   }
   else if (h <= alt4)
   {
      tr = 0.482561 + h / 337634.0;
   }
   else if (h <= alt5)
   {
      // C        --Thermoshpere
      tr  = 0.939268;
      fac = exp((h - alt4) / -25992.0);
   }
   else
   {
      // C        --CHECK TO SEE IF THE LIMITS OF THE ALGORITHM HAVE BEEN EXCEEDED
      if (h > alt6 && warn)
      {
         // write(*,*)'   ************** '
         // write(*,*)'ATMOS... EXCEEDED DEFINITION OF ATMOSPHERE AT ',h
         // write(*,*)'   no further warnings will be printed'
         // write(*,*)'   ************** '
         warn = false;
      }
      tr = 1.434843 - h / 337634.0;
   }
   // C     --Compute Atmosphere Terms
   t  = 518.688 * tr; // pcon.fi  tsl = 518.688
   vs = 49.020576 * sqrt(t);
   return vs;
}


// double WsfBrawlerPlatform::ecmsig(UtFortMat3d m, double r6[6][6], int mode, double r)
// double WsfBrawlerPlatform::LOSangleSigma(double M[3][3], double R6[6][6], double R)
double LOSangleSigma(UtFortMat3d M, UtMatrixd R6, double R)
{
   // M[3][3];   //given earth to LOS rotation matrix
   // R6[6][6];  //given cross correlation (covariance matrix?)
   // R;         //given range for getting angle error
   // int R6c = R6.GetCols();
   double ecmvar[2] = {0, 0};
   for (int k = 1; k < 3; ++k)
   {
      for (int j = 0; j < 3; ++j)
      {
         for (int i = 0; i < 3; ++i)
         {
            // ecmvar[k-1] += M[k][i] * M[k][j] * R6[i][j];
            ecmvar[k - 1] += M[k * 3 + i] * M[k * 3 + j] * R6(i, j);
         }
      }
   }
   if (ecmvar[0] <= 0.0)
   {
      // assert(ecmvar[0] > -5.0);  //else negative variance
      if (ecmvar[0] <= -5.0)
      {
         nabort("negative variance");
      }
      ecmvar[0] = 1.0;
   }
   // C        --ECMVAR now contains the y and z covar along los.
   double sigma = sqrt(std::max(ecmvar[0], ecmvar[1]));
   sigma        = atan2(sigma, R);
   return sigma;
}

void nabort(const std::string& str)
{
   ut::log::error() << str;
   throw std::runtime_error(str);
}

double param1(int i, double xx, double* aray)
{
   return (aray[i - 1] + xx * (aray[i + 1 - 1] - aray[i - 1]));
}

double tbl1(double x, double* z, double* xval, int nxval)
{
   double p = 0;
   int    i = 0;
   // C*ENDDEC
   if (nxval <= 0)
   {
      nabort("TBL1...nxval <= 0");
   }
   srch(xval, nxval, x, i, p);
   // C     --TRUNCATE p TO AVOID UNDEFINED ERRORS (13 MAY 86)
   if (i != 0)
   {
      if (std::abs(p) < 1.e-5)
      {
         p = 0.0;
      }
      else if (std::abs(p - 1.) < 1.e-5)
      {
         p = 1.0;
      }
      return (z[i - 1] + p * (z[i + 1 - 1] - z[i - 1]));
   }
   else if (x < xval[0])
   {
      return z[0];
   }
   else
   {
      return z[nxval - 1];
   }
}

double tbl1b(double x, double* z, int nx, double xmin, double xmax)
{
   // NOTE all z[n] index values are reduced by 1 to account for C++
   // array index starting at zero vs. Fortran starting at one
   double tbl1bValue = 0.0;
   double p          = (x - xmin) * static_cast<double>(nx - 1) / (xmax - xmin) + 1.0;
   int    i          = static_cast<int>(p);
   if (i >= nx)
   {
      tbl1bValue = z[nx - 1];
   }
   else if (i < 0)
   {
      tbl1bValue = z[0];
   }
   else
   {
      p          = p - static_cast<double>(i);
      tbl1bValue = z[i - 1] + p * (z[i + 1 - 1] - z[i - 1]);
   }

   return tbl1bValue;
}

double tbl2(double x, double y, double* z, double* xval, int nxval, double* yval, int nyval)
{
   // integer nxval,nyval
   // real x,y,z(nxval,nyval),xval(nxval),yval(nyval)

   double p  = 0;
   double q  = 0;
   double z1 = 0;
   double z2 = 0;
   int    i  = 0;
   int    j  = 0;
   int    i1 = 0;
   int    j1 = 0;
   // C*ENDDEC
   if (nxval == 1)
   {
      i  = 1;
      i1 = 1;
      p  = 0.0;
   }
   else
   {
      srch(xval, nxval, x, i, p);
      if (i == 0)
      {
         if (x < xval[0])
         {
            i = 1;
            p = 0.0;
         }
         else
         {
            i = nxval - 1;
            p = 1.0;
         }
      }
      else
      {
         // C           --TRUNCATE p TO AVOID UNDEFINED ERRORS
         if (std::abs(p) < 1.e-5)
         {
            p = 0.0;
         }
         else if (std::abs(p - 1.) < 1.e-5)
         {
            p = 1.0;
         }
      }
      i1 = i + 1;
   }
   if (nyval == 1)
   {
      j  = 1;
      j1 = 1;
      q  = 0.0;
   }
   else
   {
      srch(yval, nyval, y, j, q);
      if (j == 0)
      {
         if (y < yval[0])
         {
            j = 1;
            q = 0.0;
         }
         else
         {
            j = nyval - 1;
            q = 1.0;
         }
      }
      else
      {
         // C           --TRUNCATE q TO AVOID UNDEFINED ERRORS
         if (std::abs(q) < 1.e-5)
         {
            q = 0.0;
         }
         else if (std::abs(q - 1.0) < 1.e-5)
         {
            q = 1.0;
         }
      }
      j1 = j + 1;
   }
   z1 = z[(j - 1) * nxval + (i - 1)] + p * (z[(j - 1) * nxval + (i1 - 1)] - z[(j - 1) * nxval + (i - 1)]);
   z2 = z[(j1 - 1) * nxval + (i - 1)] + p * (z[(j1 - 1) * nxval + (i1 - 1)] - z[(j1 - 1) * nxval + (i - 1)]);
   return (z1 + q * (z2 - z1));
}

double tbl2e(double x, double y, double* z, double* xval, int nxval, double* yval, int nyval)
{
   // integer nxval,nyval
   // real x,y,z(nxval,nyval),xval(nxval),yval(nyval)
   double p  = 0;
   double q  = 0;
   double z1 = 0;
   double z2 = 0;
   int    i  = 0;
   int    j  = 0;
   int    i1 = 0;
   int    j1 = 0;
   // C*ENDDEC
   if (nxval == 1)
   {
      i  = 1;
      p  = 0.0;
      i1 = 1;
   }
   else
   {
      srch(xval, nxval, x, i, p);
      // C        -X DIMENSION
      if (i == 0)
      {
         if (x < xval[0])
         {
            i = 1;
            p = (x - xval[0]) / (xval[1] - xval[0]);
         }
         else
         {
            i = nxval - 1;
            p = (x - xval[i - 1]) / (xval[nxval - 1] - xval[i - 1]);
         }
      }
      // C        --TRUNCATE p TO AVOID UNDEFINED ERRORS
      if (std::abs(p) < 1.0e-5)
      {
         p = 0.0;
      }
      else if (std::abs(p - 1.0) < 1.0e-5)
      {
         p = 1.0;
      }
      i1 = i + 1;
   }
   // C     --Y DIMENSION
   if (nyval == 1)
   {
      j  = 1;
      q  = 0.0;
      j1 = 1;
   }
   else
   {
      srch(yval, nyval, y, j, q);
      if (j == 0)
      {
         if (y < yval[0])
         {
            j = 1;
            q = (y - yval[0]) / (yval[1] - yval[0]);
         }
         else
         {
            j = nyval - 1;
            q = (y - yval[j - 1]) / (yval[nyval - 1] - yval[j - 1]);
         }
      }
      // C        --TRUNCATE q TO AVOID UNDEFINED ERRORS
      if (std::abs(q) < 1.0e-5)
      {
         q = 0.0;
      }
      else if (std::abs(q - 1.) < 1.e-5)
      {
         q = 1.0;
      }
      j1 = j + 1;
   }
   // C
   z1 = z[(j - 1) * nxval + (i - 1)] + p * (z[(j - 1) * nxval + (i1 - 1)] - z[(j - 1) * nxval + (i - 1)]);
   z2 = z[(j1 - 1) * nxval + (i - 1)] + p * (z[(j1 - 1) * nxval + (i1 - 1)] - z[(j1 - 1) * nxval + (i - 1)]);
   return z1 + q * (z2 - z1);
}

void invtb2(double* tbl, double z, int nr, int ir, double pr, bool revdir, int nc, int& ic, double& pc)
{
   int    ir1    = 0;
   int    j      = 0;
   int    firstc = 0;
   int    startp = 0;
   int    endp   = 0;
   int    incr   = 0;
   double thigh  = 0;
   double tlow   = 0;
   // double temp;
   // C*ENDDEC
   if (revdir)
   {
      firstc = nc - 1;
      startp = nc - 1;
      endp   = 1;
      incr   = -1;
   }
   else
   {
      firstc = 1;
      startp = 2;
      endp   = nc - 1; // LBM - can not allow search to go past end of array
      incr   = 1;
   }
   ir1 = ir + 1;

   // helper macro for double array access
   // notice the swapped indicies:  tbl[c*R+r]
#define tbl(r, c) (tbl[(c - 1) * nr + r - 1])
   thigh = tbl(ir, firstc + 1) + pr * (tbl(ir1, firstc + 1) - tbl(ir, firstc + 1));
   tlow  = tbl(ir, firstc) + pr * (tbl(ir1, firstc) - tbl(ir, firstc));
   if (!revdir)
   {
      if (z <= tlow)
      {
         if (tlow < thigh)
         {
            // C              --SOLUTION IS OUTSIDE OF TABLE (ASSUMING MONOTONIC
            // C              --INCREASING TABLE)
            // C              --FOLLOWING GIVES NEAREST TABLE VALUES:
            ic = 1;
            pc = 0.0;
            return;
         }
         else
         {
            // C              --TABLE VALUES DECREASE AS INDEX INCREASES
            // goto 300
            // C        --THIS SECTION FOR MONOTONICALLY DECREASING TABLE:
            // 300      continue
            // do 400 j=startp,endp,incr
            for (j = startp; j <= endp; j += incr)
            {
               if (z >= thigh)
               {
                  ic = j - 1;
                  pc = (z - tlow) / (thigh - tlow);
                  return;
               }
               else
               {
                  tlow  = thigh;
                  thigh = tbl(ir, j + 1) + pr * (tbl(ir1, j + 1) - tbl(ir, j + 1));
                  if (thigh >= tlow)
                  {
                     ic = j;
                     pc = 0.0;
                     return;
                  }
               }
            } // 400      continue

            // C        --ASSERT: Z < SMALLEST TABLE VALUE; USE NEAREST POINT:
            ic = nc - 1;
            pc = 1.0;
            return;
         }
      }
      else
      {
         if (tlow > thigh)
         {
            // C              --SOLUTION IS OUTSIDE OF (MONOTICIALLY DECREASING)
            // C              --TABLE.  USE NEAREST VALUES:
            ic = 1;
            pc = 0.0;
            return;
         }
         else
         {
            // C              --TABLE VALUES INCREASE WITH COLUMN INDEX:
            // goto 100
            // C        --THIS SECTION FOR MONOTONICALLY INCREASING TABLE:
            // 100      continue
            // do 200 j=startp,endp,incr
            for (j = startp; j <= endp; j += incr)
            {
               if (z <= thigh)
               {
                  ic = j - 1;
                  pc = (z - tlow) / (thigh - tlow);
                  return;
               }
               else
               {
                  tlow  = thigh;
                  thigh = tbl(ir, j + 1) + pr * (tbl(ir1, j + 1) - tbl(ir, j + 1));
                  if (thigh <= tlow)
                  {
                     // c                 --HAVE GONE PAST THE MAXIMUM:
                     ic = j;
                     pc = 0.0;
                     return;
                  }
               }
            } // 200      continue

            // C        --ASSERT: Z > LARGEST TABLE VALUE; USE NEAREST POINT:
            ic = nc - 1;
            pc = 1.0;
            return;
         }
      }
   }
   else
   {
      if (z <= thigh)
      {
         if (tlow > thigh)
         {
            // C              --SOLUTION IS OUTSIDE OF TABLE (ASSUMING MONOTONIC
            // C              --DECREASING TABLE)
            // C              --FOLLOWING GIVES NEAREST TABLE VALUES:
            ic = startp;
            pc = 1.0;
            return;
         }
         else
         {
            // C              --TABLE VALUES INCREASE AS INDEX DECREASES
            // goto 700
            // C        --THIS SECTION FOR MONOTONICALLY DECREASING TABLE:
            // 700      continue
            // do 800 j=startp,endp,incr
            for (j = startp; j <= endp; j += incr)
            {
               if (z <= tlow)
               {
                  ic = j;
                  pc = (z - tlow) / (thigh - tlow);
                  return;
               }
               else
               {
                  thigh = tlow;
                  tlow  = tbl(ir, j) + pr * (tbl(ir1, j) - tbl(ir, j));
                  if (thigh >= tlow)
                  {
                     // c                 --HAVE GONE PAST THE MAXIMUM:
                     ic = j;
                     pc = 0.0;
                     return;
                  }
               }
            } // 800      continue

            // C        --ASSERT: Z < SMALLEST TABLE VALUE; USE NEAREST POINT:
            ic = 1;
            pc = 0.0;
            return;
         }
      }
      else
      {
         if (tlow < thigh)
         {
            // C              --SOLUTION IS OUTSIDE OF (MONOTICIALLY INCREASING)
            // C              --TABLE.  USE NEAREST VALUES:
            ic = startp;
            pc = 1.0;
            return;
         }
         else
         {
            // C              --TABLE VALUES DECREASES AS INDEX DECREASES:
            // goto 500
            // C        --THIS SECTION FOR MONOTONICALLY INCREASING TABLE:
            // 500      continue
            // do 600 j=startp,endp,incr
            for (j = startp; j <= endp; j += incr)
            {
               if (z >= tlow)
               {
                  ic = j;
                  pc = (z - tlow) / (thigh - tlow);
                  return;
               }
               else
               {
                  thigh = tlow;
                  tlow  = tbl(ir, j) + pr * (tbl(ir1, j) - tbl(ir, j));
                  if (thigh <= tlow)
                  {
                     // c                 --HAVE GONE PAST THE MINIMUM:
                     ic = j;
                     pc = 0.0;
                     return;
                  }
               }
            } // 600      continue

            // C        --ASSERT: Z > LARGEST TABLE VALUE; USE NEAREST POINT:
            ic = 1;
            pc = 0.0;
            return;
         }
      }
   }
}

void srch(double* x, int nx, double xint, int& ilow, double& p)
{
   int i     = 0;
   int ihigh = 0;
   int mx    = 0;
   // C*ENDDEC
   if (nx <= 0)
   {
      nabort("SRCH...nx <= 0");
   }
   mx = nx;
   // 5     continue
   ilow  = 1;
   ihigh = mx;
   if (xint < x[ilow - 1] || xint > x[ihigh - 1])
   {
      ilow = 0;
      return;
   }

   while (true)
   {
      // 10    continue
      i = (ihigh + ilow) / 2;
      if ((xint - x[i - 1]) < 0)
      {
         // go to 20
         // 20    continue
         ihigh = i;
         if (ihigh == ilow + 1)
         {
            p = (xint - x[ilow - 1]) / (x[ihigh - 1] - x[ilow - 1]);
            return;
         }
         // go to 10;
      }
      else if ((xint - x[i - 1]) == 0)
      {
         // go to 30
         // 30    continue
         ilow = i;
         p    = 0.0;
         return;
      }
      else // (xint-x[i-1]) > 0
      {
         // go to 40
         // 40    continue
         ilow = i;
         if (ihigh == ilow + 1)
         {
            p = (xint - x[ilow - 1]) / (x[ihigh - 1] - x[ilow - 1]);
            return;
         }
         // go to 10;
      }
   }
}

bool maxcnt(int& count, bool zprnt, int mcount, const std::string& subnam)
{
   // COUNT is compared to MCOUNT and if COUNT is greater than MCOUNT
   //  and ZPRNT is true, then call NABORT.  If ZPRNT is false, then set
   //  ZPRNT to true, reset COUNT to zero, and return MAXCNT as true.

   // C  ---local declarations:
   // C*ENDDEC
   // C
   // C     --INCREMENT COUNT BY ONE
   count = count + 1;
   // C     --CHECK FOR COUNT GREATER THAN MCOUNT
   if (count > mcount)
   {
      // if (zprnt)
      //{
      //    //tmpstr = 'MAXCNT...See IOUT file - infinite loop discovered in 'subnam;
      //    //call nabort(tmpstr);
      //    nabort("MAXCNT... infinite loop discovered");
      // }
      // else
      //{
      //    zprnt = true;
      //    count = 0;
      // }
      return true;
   }
   else
   {
      // C        --SET MAXCNT TO DEFAULT .FALSE.
      return false;
   }
}

// C#ABSTRACT RETURNS DESIRED X - DOT FOR A RATE - COMMANDED CONTROL SYSTEM
// C#PURPOSE  RETURNS DESIRED X - DOT FOR A RATE - COMMANDED SECOND - ORDER
// C  CONTROL SYSTEM THAT PLACES LIMITS ON COMMANDED RATES.
// C#AUTHOR   KERCHNER
// C#TYPE     AIRCRAFT AERODYNAMICS
// C#PARAMETER DESCRIPTIONS
// CIN  XDES   REAL - DESIRED VALUE OF VARIABLE 'X'
// CIN  X      REAL - PRESENT VALUE OF X
// CIN  XDOT   REAL - TIME DERIVATIVE OF X
// CIN  XDOTMX REAL - MAXIMUM ALLOWED XDOT
// CIN  TAU    REAL - SYSTEM CHARACTERISTIC TIME CONSTANT
// COUT DSXDOT REAL - "OPTIMAL" COMMAND XDOT
// COUT DT     REAL - TIME BEFORE WHICH DSXDOT(OR DXDOT2) SHOULD BE CALLED
// C           AGAIN TO GIVE REASONABLE APPROXIMATION TO CONTINUOUS CALLING.
// C#TECHNICAL DESCRIPTION
// C    DERIVATION DETAILS ARE TO BE FOUND IN APPENDIX C OF THE ANALYST
// C  MANUAL TO BRAWLER.IT IS BASED ON A RATE -
// C  COMMANDED SYSTEM WITH FIRST - ORDER LAG TO THE COMMANDED RATE WITH
// C  TIME CONSTANT TAU.CONTINUOUS APPLICATION OF THE FORMULA WILL
// C  LEAD TO A SLIGHTLY UNDER - DAMPED RELAXATION OF X TO XDES.
// C     DXDOT2 IS LIKE DSXDOT, BUT ALSO RETURNS A MAX RECONSIDERATION TIME.
// C  CALLS AT THIS INTERVAL OR LESS WILL APPROXIMATE CONTINUOUS CALLING.
// C#VARIABLE DESCRIPTIONS
// C  CHI     REAL - USED WHEN NEAR CONVERGENCE TO ALLOW LONGER RECONSIDERATION
// C         TIMES, I.E., NEARER TO TAU.
// C         COMPARED TO XDOT, CHI IS NEAR 1
// C  XC      REAL - THE DISTANCE X - CRITICAL AT WHICH THE DAMPED OSCILLATOR EQN
// C         WILL UNLIMIT, " I.E., RETURN A
// C####
double dsxdot(double xdes, double x, double xdot, double xdotmx, double tau)
{
   // C**   dsxdot = 11.8468*(xdes - x) / tau - 3.8676*xdot
   // C     --The above eqn is modified to the one below because the derivation of the
   // C     --original gains assumed continuous update of the output variable.In
   // C     --fact, the system is not stable if the refresh interval is longer than
   // C--0.4*tau.The revised gains yield stability if the interval is 0.8*tau.
   // C     --There appears to be little loss in terms of achieving speedy response.

   double dsxdot_out = 5.019 * (xdes - x) / tau - 2.168 * xdot;
   if (std::abs(dsxdot_out) > xdotmx)
   {
      dsxdot_out = sign(xdotmx, dsxdot_out);
   }

   return dsxdot_out;
}

UtVec3d vorth(const UtVec3d& a, const UtVec3d& b)
{
   double  x    = (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / (b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
   UtVec3d temp = b;
   temp.Multiply(x);
   UtVec3d c = a - temp;
   return c;
}

UtVec3d rotz(double phi, const UtVec3d& vin)
{
   UtVec3d vout;
   double  temp = 0;
   double  cphi = cos(phi);
   double  sphi = sin(phi);
   temp         = cphi * vin[1 - 1] + sphi * vin[2 - 1];
   vout[2 - 1]  = -sphi * vin[1 - 1] + cphi * vin[2 - 1];
   vout[1 - 1]  = temp;
   vout[3 - 1]  = vin[3 - 1];
   return vout;
}


// C#ABSTRACT COMPUTES AIMPOINT FROM ATTACKER AND TARGET POSITION
// C#PURPOSE  COMPUTES AIMPOINT FROM ATTACKER AND TARGET POSITION
// C#TYPE     MISSILE
// C#PARAMETER DESCRIPTIONS
// CIN  XA     ARRAY DIM(3) OF (REAL) - ATTACKER POSITION
// CIN  SA     REAL - ATTACKER SPEED
// CIN  XT     ARRAY DIM(3) OF (REAL) - TARGET POSITION
// CIN  VT     ARRAY DIM(3) OF (REAL) - TARGET VELOCITY
// CIN  ST     REAL - TARGET SPEED
// COUT AIMP   ARRAY DIM(3) OF (REAL) - AIMPOINT SOLUTION
// COUT LSOLN  LOG - .TRUE. IF SOLUTION EXISTS
// C#TECHNICAL DESCRIPTION
// C PROJECTS LOCATION OF TARGET AT MISSILE IMPACT.  ANGLE BETWEEN
// C TARGET LINE-OF-SIGHT AND AIMPOINT LINE-OF-SIGHT IS COMPUTED
// C USING ANGLE OFF AND AN INTERCEPT SPEED OF SA.
// C NO SOLUTION EXISTS IF THE SIN OF THIS ANGLE > 1.0.
// C THE ANGLE DESIRED IS ALWAYS ACUTE, SINCE THE OBTUSE
// C SOLUTION IMPLIES THAT THE TARGET HAS ALREADY PASSED BY.
// C#VARIABLE DESCRIPTIONS
// C  R       ARRAY DIM(3) OF (REAL) - TARGET TO ATTACKER RANGE VECTOR
// C  RMAG    REAL - MAGNITUDE OF R
// C  SING    REAL - SIN OF ANGLE BETWEEN LOS TO TARGET AND LOS TO AI
// C  DTIME   REAL - TIME TO IMPACT
// C####
bool AimPoint(const UtVec3d& xa, double sa, const UtVec3d& xt, const UtVec3d& vt, double st, UtVec3d& aimp)
{
   UtVec3d r;
   bool    lsoln = false;
   double  cosd  = 0;
   double  cosg  = 0;
   double  dtime = 0;
   double  rmag  = 0;
   double  sind  = 0;
   double  sing  = 0;
   // C*ENDDEC
   // C     --TARGET TO ATKR RANGE
   r    = xt - xa;
   rmag = r.Magnitude();
   if (st <= eps)
   {
      // C        --Target is stationary.  The target position is the aim point
      lsoln = true;
      aimp  = xt;
   }
   else
   {
      // C        --D = PI - TGT ASPECT
      if (rmag == 0.0)
      {
         // call nabort('AIMPT... rmag = 0');
      }
      cosd = -vt.DotProduct(r) / (st * rmag);
      if (cosd > 1.0)
      {
         cosd = 1.0;
      }
      else if (cosd < -1.0)
      {
         cosd = -1.0;
      }
      sind = sqrt(1.0 - pow(cosd, 2));
      // C        --ANGLE BTWN LOS TO TGT AND LOS TO AIMP
      sing  = sind * st / sa;
      lsoln = sing <= 1.0;
      if (!lsoln)
      {
         return lsoln;
      }

      // C        --ANGLE G IS ALWAYS ACUTE:
      cosg = sqrt(1.0 - pow(sing, 2));
      if (cosg * sa + cosd * st <= eps)
      {
         lsoln = false;
      }
      else
      {
         dtime = rmag / (cosg * sa + cosd * st);
         if (dtime >= eps)
         {
            // call vecinc(xt,dtime,vt,aimp);   //vecinc(aa,b,bb,cc) = CC = AA + B*BB.
            aimp = xt + dtime * vt;
         }
         else
         {
            lsoln = false;
         }
      }
   }
   // C
   return lsoln;
}


// C#ABSTRACT GENERATES A DESIRED VELOCITY TO INTERCEPT
// C#PURPOSE  USED BY SPECIAL TACTICS INTERP ROUTINES WHEN AN INTERCEPT VECTOR
// C  IS DESIRED.
// C#TYPE     FLIGHT TACTICS
// C#PARAMETER DESCRIPTIONS
// CIN  XA     3-VEC - ATTACKER POSITION
// CIN  SA     REAL - ATTACKER SPEED
// CIN  XT     3-VEC - TARGET POSITION
// CIN  VT     REAL - TARGET VELOCITY
// CIN  CLOSMN REAL - COSINE OF MAX LOS TO TARGET
// CIN  ANGINT REAL - DESIRED LOS ANGLE TO INTRCPT PT
// CIN  HDES   REAL - DESIRED ALTITUDE UNTIL 30 DEG LIMIT EXCEEDED
// COUT DESDIR 3-VEC - DESIRED VELOCITY DIRECTION (UNIT VECTOR)
// COUT SDES   REAL - DESIRED SPEED
// C#TECHNICAL DESCRIPTION
// C     WHEN AIMPOINT EXISTS (GIVEN ATTACKER SPEED SDES) A VECTOR IN
// C  THAT DIRECTION IS ATTEMPTED. OTHERWISE A POINT 100 SECONDS IN
// C  FRONT OF THE TARGET IS USED. IF THE LOS ANGLE TO THE TARGET WILL
// C  EXCEED THAT GIVEN BY CLOSMN THEN A DIRECTION IN THE PLANE
// C  CONTAINING THE LOS AND THE ORIGINAL DESIRED DIRECTION IS USED,
// C  SUCH THAT THE LOS TO THE TARGET HAS COSINE CLOSMN.  IF THE
// C  ELEVATION ANGLE DESIRED DOES NOT EXCEED 30 DEGRESS IT IS SET TO
// C  ZERO. THAT IS, THE INTERCEPT IS AT THE ALTITUDE SPECIFIED BY XA AS
// C  LONG AS POSSIBLE.
// C
// C  COMPUTATION OF SDES USES BVRMCH VARIABLE IN /MIND2/.
// C  THIS IS NO GOOD FOR CALLS TO INTCPT THAT ARE NOT ASSUMING THAT THE
// C  ATTCKER IS ONESELF, SO THE ENTRY POINT ICPT2, FREE OF THIS ASSUMPTION
// C  AND USING THE OLD LOGIC, IS PROVIDED.  ICPT2 IS CALLED ONLY BY
// C  PRJATK AND PRJMIC.
// C#VARIABLE DESCRIPTIONS
// C  XLOS    3-VEC - LINE OF SIGHT VECTOR TO TARGET
// C  VS      REAL - SPEED OF SOUND
// C  AIMP    3-VEC - INTERCEPT POSITION
// C  LAIMP   LOG - TRUE IF AIMP EXISTS
// C  DXI     3-VEC - WORK VECTOR, USUALLY POINTING TO AIMP FROM ATTACKER
// C####
void intcpt(const UtVec3d& xa,
            double         sa,
            const UtVec3d& xt,
            const UtVec3d& vt,
            double         closmn,
            double         angint,
            double         hdes,
            UtVec3d&       desdir,
            double&        sdes)
{
   // C     --local declarations
   UtVec3d xlos, x_tgt, x_atkr;
   double  vs    = 0;
   double  range = 0;
   // C*ENDDEC
   // if (xa[3-1] >= trkr_neg_alt)
   if (xa[3 - 1] >= 5000) // trkr_neg_alt = 5000 //unclassified par.fi
   {
      nabort("intcpt...attacker more than /par/trkr_neg_alt feet below ground");
   }
   x_atkr = xa;
   if (x_atkr[3 - 1] >= 0.0)
   {
      x_atkr[3 - 1] = -1.0;
   }
   // if (xt[3-1] >= trkr_neg_alt)
   if (xt[3 - 1] >= 5000) // trkr_neg_alt = 5000 //unclassified par.fi
   {
      nabort("intcpt...tgt more than /par/trkr_neg_alt feet below ground");
   }
   x_tgt = xt;
   if (x_tgt[3 - 1] >= 0.0)
   {
      x_tgt[3 - 1] = -1.0;
   }
   // C
   vs    = SpeedOfSound(-xa[3 - 1]);
   range = (xt - xa).Magnitude();
   // double bvrmch = 0.90;   //LBM - assume subsonic cruise for BVR approach, for now
   // sdes = vs*bvrmch;
   // LBM - do not use mind variable bvrmch for speed, for now
   sdes = amax1(sa, vs * (1. + 0.2 * ramp(10.0, range * ftnmi, 5.0)));

   xlos  = x_tgt - x_atkr;
   range = xlos.Magnitude();
   intcpt_inner(x_atkr, sa, x_tgt, vt, closmn, angint, hdes, vs, xlos, range, desdir, sdes);
   return;
}

void intcpt_inner(const UtVec3d& x_atkr,
                  double         sa,
                  const UtVec3d& x_tgt,
                  const UtVec3d& vt,
                  double         closmn,
                  double         angint,
                  double         hdes,
                  double         vs,
                  const UtVec3d& xlos,
                  double         range,
                  UtVec3d&       desdir,
                  double&        sdes)
{
   // C     --local declarations
   UtVec3d aimp;
   UtVec3d dxi;
   double  st     = 0;
   double  sindiv = 0.50;
   double  csep   = 0;
   double  coslos = 0;
   double  cos30  = 0.866;
   bool    laimp  = false;
   // C*ENDDEC
   st = vt.Magnitude();
   // C     --MODIFY FOR POOR CLOSURE:
   if (vt.DotProduct(xlos) > 0.0)
   {
      sdes = amax1(sa, sdes, st + 100.0);
   }

   laimp = AimPoint(x_atkr, sdes, x_tgt, vt, st, aimp);
   if (laimp)
   {
      // C        --HEAD FOR AIMPT
      dxi = aimp - x_atkr;
      dxi.Normalize();
   }
   else
   {
      // C        --LEAD TGT BY 100 SECONDS
      dxi = xlos + 100.0 * vt;
      dxi.Normalize();
   }

   // C     --ROTATE TO DES LOS, IF DESIRED
   if (angint != 0.0)
   {
      dxi = rotz(-angint, dxi);
   }

   csep = dxi.DotProduct(xlos) / range;
   if (csep <= closmn)
   {
      // C        --NEED TO ADJUST FOR EXCESSIVE LOS ANGLE TO INTERCEPT PT
      dxi = vorth(dxi, xlos);
      // call vmake(sqrt(1.0-pow(closmn,2)),dxi,dxi);
      dxi = dxi.GetNormal() * sqrt(1.0 - pow(closmn, 2));
      // call vecinc(dxi,closmn/range,xlos,dxi);
      dxi = dxi + (closmn / range) * xlos;
   }

   // C     --ADJUST VERTICAL COMPONENT TO ACHIEVE hdes
   // C     --UNLESS VERTICAL COMPONENT OF LOS is > 30 deg.
   coslos = sqrt(xlos[0] * xlos[0] + xlos[1] * xlos[1]) / xlos.Magnitude();
   if (coslos > cos30)
   {
      // call unitv2(dxi,dxi);
      double mag2 = sqrt(dxi[0] * dxi[0] + dxi[1] * dxi[1]);
      dxi[0] /= mag2;
      dxi[1] /= mag2;
      ///////////////////////
      dxi[3 - 1] = sindiv * xlimit((-hdes - x_atkr[3 - 1]) / 5000.0, 1.0, -1.0);
      // call vmult2(sqrt(1.0 - pow(dxi[3-1],2)),dxi,dxi);
      double mult = sqrt(1.0 - pow(dxi[3 - 1], 2));
      dxi[0] *= mult;
      dxi[1] *= mult;
      ///////////////////////////////////////////////////
   }
   desdir = dxi;
   return;
}


bool TryReadCommand(UtInput& aInput, std::string& aCommand)
{
   bool ret = aInput.TryReadCommand(aCommand);
   if (ret)
   {
      size_t pos = aCommand.find_first_of('!');
      if (pos != std::string::npos)
      {
         // input found a brawler comment ('!') in the command just read
         if (pos == 0)
         {
            // the command is the start of a comment, so ignore the rest of the line
            aInput.GetCurrentInput()->Ignore(99999, '\n');
            return TryReadCommand(aInput, aCommand); // try again
         }
         else
         {
            // the comment starts after the command
            // it happens to be joined to the command in this case
            aCommand = aCommand.substr(0, pos);
            aInput.GetCurrentInput()->Ignore(99999, '\n');
         }
      }
   }
   return ret;
}


// from UtInput.cpp
bool InputIsValid(std::istringstream& aIn)
{
   bool isValid = false;
   if (aIn)
   {
      // make sure there is nothing but trailing white space
      isValid = true;
      int nextChar;
      while ((nextChar = aIn.get()) != EOF)
      {
         if (!isspace(nextChar))
         {
            isValid = false;
            break;
         }
      }
   }
   return isValid;
}


void ReadValue(UtInput& aInput, int& aValue)
{
   std::string strValue;
   aInput.ReadValue(strValue);

   size_t pos = strValue.find_first_of('!');
   if (pos != std::string::npos)
   {
      // input found a brawler comment ('!') in the string just read
      if (pos == 0)
      {
         throw UtInput::BadValue(aInput);
      }
      else
      {
         // the comment starts after the value
         // it happens to be joined to the value in this case
         strValue = strValue.substr(0, pos);
         aInput.GetCurrentInput()->Ignore(99999, '\n');
      }
   }

   std::istringstream iss(strValue);
   iss >> aValue;
   if (!InputIsValid(iss))
   {
      throw UtInput::BadValue(aInput);
   }
}


void ReadValue(UtInput& aInput, double& aValue)
{
   std::string strValue;
   aInput.ReadValue(strValue);

   size_t pos = strValue.find_first_of('!');
   if (pos != std::string::npos)
   {
      // input found a brawler comment ('!') in the string just read
      if (pos == 0)
      {
         throw UtInput::BadValue(aInput);
      }
      else
      {
         // the comment starts after the value
         // it happens to be joined to the value in this case
         strValue = strValue.substr(0, pos);
         aInput.GetCurrentInput()->Ignore(99999, '\n');
      }
   }

   std::istringstream iss(strValue);
   iss >> aValue;
   if (!InputIsValid(iss))
   {
      throw UtInput::BadValue(aInput);
   }
}


} // namespace BRAWLER
