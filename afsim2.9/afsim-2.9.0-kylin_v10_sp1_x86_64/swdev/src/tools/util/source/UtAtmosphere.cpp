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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "UtAtmosphere.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace
{
// To make the table contents clearer, we give symbolic names to the array indices:

const int ALTITUDE = 0; // The (only) independent variable.

const int TEMPERATURE  = 1;  // Values for Temperature
const int PRESSURE     = 2;  // Values for Pressure
const int DENSITY      = 3;  // Values for Density
const int SONIC_SPEED  = 4;  // Values for Sonic Velocity
const int REL_HUMIDITY = 5;  // Values for relative humidity
const int TEMP_INTERP  = 6;  // Values for Interpolants for Temperature
const int PRESS_INTERP = 7;  // Values for Interpolants for Pressure
const int DENS_INTERP  = 8;  // Values for Interpolants for Density
const int SSPD_INTERP  = 9;  // Values for Interpolants for Sonic Velocity
const int RH_INTERP    = 10; // Values for Interpolants for Relative Humidity

const int MS_210_ALT    = 0;
const int MS_210_TF_HOT = 1;
const int MS_210_TF_TRP = 2;
const int MS_210_TF_CLD = 3;
const int MS_210_TF_PLR = 4;

const int INDEX_NOT_SET = -1;

double sInterp(double aXtarget, double aX0, double aX1, double aY0, double aY1)
{
   double interp = (aXtarget - aX0) / (aX1 - aX0);
   return aY0 + interp * (aY1 - aY0);
}

inline bool Bridges(double aValue, double aX0, double aX1)
{
   // Put the two Xs in ascending order first:
   double largest  = (aX0 >= aX1) ? aX0 : aX1;
   double smallest = (aX0 <= aX1) ? aX0 : aX1;
   return aValue >= smallest && aValue <= largest;
}
} // namespace

const double UtAtmosphere::cGMR                = 34.163195 / 1000;
const double UtAtmosphere::MIN_ALT             = 0.0;
const double UtAtmosphere::ALT_INCR            = 1000.0; // Meters.
const double UtAtmosphere::cGAMMA              = 1.401;
const double UtAtmosphere::cSSL_SONIC_VELOCITY = 340.3; // meters per second, Standard Sea Level

// Air Specific Weight under Standard Sea Level Conditions, kg / m^3:
// static
double UtAtmosphere::cSSL_AIR_DENSITY()
{
   return UtEarth::cSSL_AIR_DENSITY;
} // kg / m^3

// Atmospheric pressure at Standard Sea Level
// static
double UtAtmosphere::cSSL_PRESSURE()
{
   return UtEarth::cSSL_PRESSURE;
} // N/m^2 or Pa

// Atmospheric temperature at Standard Sea Level
// static
double UtAtmosphere::cSSL_TEMP()
{
   return UtEarth::cSSL_TEMP;
} // K


// ==============================================================
// Note:  Sonic velocity = k*sqrt(theta), where theta is the
// absolute temperature ratio from sea level.  So the table
// lookup is actually sqrt theta, rather than sonic velocity...

/* = {
Table is created on the fly now at initialization, based on
minimum altitude, altitude increment, and number of altitudes
(which are defined as constants or enumerations)

   // Air Density Ratio and Sonic Velocity Tables:
   //    ALT     DENSITY  ROOT_THETA  INTERPOLANTS
   {  -500.0,    1.0490,    1.0056,   0.0,   0.0 },
   {     0.0,    1.0000,    1.0000,   0.0,   0.0 },
   {   500.0,    0.9528,    0.9944,   0.0,   0.0 },
   {  1000.0,    0.9074,    0.9887,   0.0,   0.0 },
   {  1500.0,    0.8637,    0.9830,   0.0,   0.0 },
   {  2000.0,    0.8216,    0.9772,   0.0,   0.0 },
   {  2500.0,    0.7811,    0.9714,   0.0,   0.0 },
   {  3000.0,    0.7421,    0.9656,   0.0,   0.0 },
   {  3500.0,    0.7047,    0.9597,   0.0,   0.0 },
   {  4000.0,    0.6687,    0.9538,   0.0,   0.0 },
   {  4500.0,    0.6341,    0.9479,   0.0,   0.0 },
   {  5000.0,    0.6009,    0.9419,   0.0,   0.0 },
   {  5500.0,    0.5691,    0.9359,   0.0,   0.0 },
   {  6000.0,    0.5385,    0.9299,   0.0,   0.0 },
   {  6500.0,    0.5093,    0.9238,   0.0,   0.0 },
   {  7000.0,    0.4812,    0.9177,   0.0,   0.0 },
   {  7500.0,    0.4544,    0.9115,   0.0,   0.0 },
   {  8000.0,    0.4287,    0.9053,   0.0,   0.0 },
   {  8500.0,    0.4042,    0.8991,   0.0,   0.0 },
   {  9000.0,    0.3087,    0.8928,   0.0,   0.0 },
   {  9500.0,    0.3583,    0.8864,   0.0,   0.0 },
   { 10000.0,    0.3369,    0.8800,   0.0,   0.0 },
   { 10500.0,    0.3165,    0.8736,   0.0,   0.0 },
   { 11000.0,    0.2971,    0.8671,   0.0,   0.0 },
   { 11500.0,    0.2745,    0.8671,   0.0,   0.0 },
   { 12000.0,    0.2537,    0.8671,   0.0,   0.0 },
   { 12500.0,    0.2345,    0.8671,   0.0,   0.0 },
   { 13000.0,    0.2167,    0.8671,   0.0,   0.0 },
   { 13500.0,    0.2003,    0.8671,   0.0,   0.0 },
   { 14000.0,    0.1851,    0.8671,   0.0,   0.0 },
   { 14500.0,    0.1711,    0.8671,   0.0,   0.0 },
   { 15000.0,    0.1581,    0.8671,   0.0,   0.0 },
   { 15500.0,    0.1461,    0.8671,   0.0,   0.0 },
   { 16000.0,    0.1350,    0.8671,   0.0,   0.0 },
   { 16500.0,    0.1248,    0.8671,   0.0,   0.0 },
   { 17000.0,    0.1153,    0.8671,   0.0,   0.0 },
   { 17500.0,    0.1066,    0.8671,   0.0,   0.0 },
   { 18000.0,    0.0985,    0.8671,   0.0,   0.0 },
   { 18500.0,    0.09104,   0.8671,   0.0,   0.0 },
   { 19000.0,    0.08413,   0.8671,   0.0,   0.0 },
   { 19500.0,    0.07776,   0.8671,   0.0,   0.0 },
   { 20000.0,    0.07186,   0.8671,   0.0,   0.0 },
   { 25000.0,    0.03222,   0.8771,   0.0,   0.0 },
   { 30000.0,    0.01470,   0.8869,   0.0,   0.0 },
   { 32000.0,    0.01080,   0.8908,   0.0,   0.0 },
   { 35000.0,    0.006705,  0.9070,   0.0,   0.0 },
   { 40000.0,    0.003144,  0.9334,   0.0,   0.0 },
   { 45000.0,    0.001536,  0.9591,   0.0,   0.0 },
   { 47000.0,    0.001165,  0.9692,   0.0,   0.0 },
   { 50000.0,    0.000798,  0.9692,   0.0,   0.0 },
   { 52000.0,    0.0006199, 0.9692,   0.0,   0.0 },
   { 55000.0,    0.0004323, 0.9584,   0.0,   0.0 },
   { 60000.0,    0.0002327, 0.9401,   0.0,   0.0 },
   { 61000.0,    0.0002050, 0.9364,   0.0,   0.0 }
};*/

// ========================== Data to Define the Non-Standard Altitudes =====
// static
// This is Pressure Altitude in Feet, and Temperatures in Degrees K:
const double UtAtmosphereData::sMS_210A[NUM_210_ALTS][NUM_210_COLS] = {
   // NON-STANDARD ATMOSPHERE TEMPERATURES:
   // Source www.pdas.com/milstd210.html.  (Via separate spreadsheet, translated from kft to ft, Rankine to Kelvin.)
   // (Note:  These tables only extended upward to 100 kft.  Above that altitude, temperature was linearly interpolated
   //  to merge with the standard atmosphere at the 41000 m thermocline breakpoint.  Above, all temperatures match standard.
   // PresAltFt, HotK, TropicalK, ColdK, PolarK
   {0, 312.61, 305.28, 222.06, 246.67},      {1000, 310.50, 303.11, 229.56, 248.33},
   {2000, 308.39, 300.94, 237.06, 250.06},   {3000, 306.22, 298.78, 244.67, 251.72},
   {4000, 304.06, 296.67, 247.06, 251.94},   {5000, 301.89, 294.50, 247.06, 251.67},
   {6000, 299.72, 292.33, 247.06, 251.33},   {7000, 297.50, 290.17, 247.06, 251.06},
   {8000, 295.28, 288.00, 247.06, 250.72},   {9000, 293.06, 285.89, 247.06, 250.44},
   {10000, 290.89, 283.72, 247.06, 250.00},  {11000, 288.83, 281.56, 246.61, 248.44},
   {12000, 286.72, 279.39, 244.78, 246.83},  {13000, 284.61, 277.28, 242.94, 245.28},
   {14000, 282.50, 275.11, 241.11, 243.72},  {15000, 280.33, 272.94, 239.22, 242.17},
   {16000, 278.17, 270.83, 237.39, 240.56},  {17000, 276.00, 268.67, 235.50, 239.00},
   {18000, 273.78, 266.50, 233.61, 237.44},  {19000, 271.61, 264.33, 231.67, 235.83},
   {20000, 269.56, 262.22, 229.78, 234.28},  {21000, 267.50, 260.06, 227.83, 232.67},
   {22000, 265.39, 257.89, 225.83, 231.11},  {23000, 263.33, 255.78, 223.89, 229.50},
   {24000, 261.22, 253.61, 221.89, 227.94},  {25000, 259.11, 251.50, 219.89, 226.33},
   {26000, 257.00, 249.33, 217.89, 224.72},  {27000, 254.83, 247.17, 215.89, 223.17},
   {28000, 252.67, 245.39, 213.83, 221.56},  {29000, 250.56, 242.89, 211.72, 219.94},
   {30000, 248.56, 240.78, 209.67, 218.33},  {31000, 246.56, 238.61, 208.17, 218.06},
   {32000, 244.56, 236.44, 208.17, 217.89},  {33000, 242.50, 234.33, 208.17, 217.78},
   {34000, 240.50, 232.17, 208.17, 217.61},  {35000, 238.67, 230.06, 208.17, 217.44},
   {36000, 236.83, 227.89, 208.17, 217.33},  {37000, 235.00, 225.78, 208.17, 217.17},
   {38000, 233.11, 223.67, 208.17, 217.06},  {39000, 231.22, 221.56, 208.17, 216.89},
   {40000, 230.50, 219.50, 208.17, 216.72},  {41000, 230.78, 217.44, 208.17, 216.61},
   {42000, 231.00, 215.39, 208.17, 216.44},  {43000, 231.22, 213.39, 206.39, 216.33},
   {44000, 231.44, 211.39, 203.56, 216.17},  {45000, 231.72, 209.39, 200.61, 216.00},
   {46000, 232.00, 207.44, 197.67, 215.89},  {47000, 232.22, 205.50, 194.67, 215.72},
   {48000, 232.50, 203.61, 191.67, 215.61},  {49000, 232.78, 201.67, 189.17, 215.44},
   {50000, 233.06, 199.78, 187.11, 215.28},  {51000, 233.22, 197.94, 185.94, 215.17},
   {52000, 233.33, 196.06, 185.94, 215.00},  {53000, 233.44, 194.22, 185.94, 214.89},
   {54000, 233.56, 193.67, 185.94, 214.72},  {55000, 233.67, 194.83, 185.94, 214.56},
   {56000, 233.72, 196.06, 185.94, 214.44},  {57000, 233.83, 197.28, 185.94, 214.28},
   {58000, 233.94, 198.44, 185.94, 214.17},  {60000, 234.17, 200.94, 185.94, 213.89},
   {62000, 234.39, 203.39, 187.56, 213.56},  {64000, 234.61, 205.94, 190.94, 213.28},
   {66000, 234.78, 208.50, 194.06, 213.00},  {68000, 235.33, 211.06, 196.89, 212.72},
   {70000, 236.11, 213.44, 199.56, 212.44},  {72000, 236.89, 214.89, 202.00, 212.17},
   {74000, 237.67, 216.33, 203.00, 211.89},  {76000, 238.44, 217.83, 202.72, 211.61},
   {78000, 239.22, 219.28, 202.44, 211.28},  {80000, 240.00, 220.78, 202.11, 211.00},
   {82000, 240.89, 222.28, 201.72, 210.72},  {84000, 241.72, 223.78, 201.28, 210.44},
   {86000, 242.61, 225.28, 200.83, 210.17},  {88000, 243.56, 226.78, 200.44, 210.17},
   {90000, 244.39, 228.28, 200.00, 210.17},  {92000, 245.22, 229.78, 199.56, 210.17},
   {94000, 246.06, 231.28, 199.11, 210.17},  {96000, 247.00, 232.72, 198.67, 210.17},
   {98000, 247.94, 234.22, 198.17, 210.17},  {100000, 248.94, 235.72, 197.67, 210.17},
   {101000, 249.34, 236.36, 199.02, 211.29}, {102000, 249.74, 237.01, 200.36, 212.40},
   {103000, 250.14, 237.65, 201.71, 213.52}, {104000, 250.54, 238.30, 203.06, 214.63},
   {105000, 250.94, 238.94, 204.40, 215.75}, {106000, 251.34, 239.59, 205.75, 216.87},
   {107000, 251.74, 240.23, 207.10, 217.98}, {108000, 252.14, 240.88, 208.44, 219.10},
   {109000, 252.55, 241.52, 209.79, 220.21}, {110000, 252.95, 242.16, 211.14, 221.33},
   {111000, 253.35, 242.81, 212.48, 222.44}, {112000, 253.75, 243.45, 213.83, 223.56},
   {113000, 254.15, 244.10, 215.17, 224.68}, {114000, 254.55, 244.74, 216.52, 225.79},
   {115000, 254.95, 245.39, 217.87, 226.91}, {116000, 255.35, 246.03, 219.21, 228.02},
   {117000, 255.75, 246.68, 220.56, 229.14}, {118000, 256.15, 247.32, 221.91, 230.26},
   {119000, 256.55, 247.96, 223.25, 231.37}, {120000, 256.95, 248.61, 224.60, 232.49},
   {121000, 257.35, 249.25, 225.95, 233.60}, {122000, 257.75, 249.90, 227.29, 234.72},
   {123000, 258.15, 250.54, 228.64, 235.84}, {124000, 258.55, 251.19, 229.99, 236.95},
   {125000, 258.95, 251.83, 231.33, 238.07}, {126000, 259.35, 252.48, 232.68, 239.18},
   {127000, 259.76, 253.12, 234.03, 240.30}, {128000, 260.16, 253.77, 235.37, 241.41},
   {129000, 260.56, 254.41, 236.72, 242.53}, {130000, 260.96, 255.05, 238.07, 243.65},
   {131000, 261.36, 255.70, 239.41, 244.76}, {132000, 261.76, 256.34, 240.76, 245.88},
   {133000, 262.16, 256.99, 242.10, 246.99}, {134000, 262.56, 257.63, 243.45, 248.11},
   {135000, 262.96, 258.28, 244.80, 249.23}, {136000, 263.36, 258.92, 246.14, 250.34},
   {137000, 263.76, 259.57, 247.49, 251.46}, {138000, 264.16, 260.21, 248.84, 252.57},
   {139000, 264.56, 260.85, 250.18, 253.69}, {140000, 264.96, 261.50, 251.53, 254.81},
   {141000, 265.36, 262.14, 252.88, 255.92}, {142000, 265.76, 262.79, 254.22, 257.04},
   {143000, 266.16, 263.43, 255.57, 258.15}, {144000, 266.56, 264.08, 256.92, 259.27},
   {145000, 266.97, 264.72, 258.26, 260.38}, {146000, 267.37, 265.37, 259.61, 261.50},
   {147000, 267.77, 266.01, 260.96, 262.62}, {148000, 268.17, 266.65, 262.30, 263.73},
   {149000, 268.57, 267.30, 263.65, 264.85}, {150000, 268.97, 267.94, 265.00, 265.96},
   {151000, 269.37, 268.59, 266.34, 267.08}, {152000, 269.77, 269.23, 267.69, 268.20},
   {153000, 270.17, 269.88, 269.03, 269.31}, {154000, 270.57, 270.52, 270.38, 270.43},
   {155000, 270.65, 270.65, 270.65, 270.65}, {156000, 270.65, 270.65, 270.65, 270.65},
   {157000, 270.65, 270.65, 270.65, 270.65}, {158000, 270.65, 270.65, 270.65, 270.65},
   {159000, 270.65, 270.65, 270.65, 270.65}, {160000, 270.65, 270.65, 270.65, 270.65},
   {161000, 270.65, 270.65, 270.65, 270.65}, {162000, 270.65, 270.65, 270.65, 270.65},
   {163000, 270.65, 270.65, 270.65, 270.65}, {164000, 270.65, 270.65, 270.65, 270.65},
   {165000, 270.65, 270.65, 270.65, 270.65}, {166000, 270.65, 270.65, 270.65, 270.65},
   {167000, 270.65, 270.65, 270.65, 270.65}, {168000, 270.07, 270.07, 270.07, 270.07},
   {169000, 269.22, 269.22, 269.22, 269.22}, {170000, 268.37, 268.37, 268.37, 268.37},
   {171000, 267.51, 267.51, 267.51, 267.51}, {172000, 266.66, 266.66, 266.66, 266.66},
   {173000, 265.80, 265.80, 265.80, 265.80}, {174000, 264.95, 264.95, 264.95, 264.95},
   {175000, 264.10, 264.10, 264.10, 264.10}, {176000, 263.24, 263.24, 263.24, 263.24},
   {177000, 262.39, 262.39, 262.39, 262.39}, {178000, 261.54, 261.54, 261.54, 261.54},
   {179000, 260.68, 260.68, 260.68, 260.68}, {180000, 259.83, 259.83, 259.83, 259.83},
   {181000, 258.98, 258.98, 258.98, 258.98}, {182000, 258.12, 258.12, 258.12, 258.12},
   {183000, 257.27, 257.27, 257.27, 257.27}, {184000, 256.42, 256.42, 256.42, 256.42},
   {185000, 255.56, 255.56, 255.56, 255.56}, {186000, 254.71, 254.71, 254.71, 254.71},
   {187000, 253.86, 253.86, 253.86, 253.86}, {188000, 253.00, 253.00, 253.00, 253.00},
   {189000, 252.15, 252.15, 252.15, 252.15}, {190000, 251.30, 251.30, 251.30, 251.30},
   {191000, 250.44, 250.44, 250.44, 250.44}, {192000, 249.59, 249.59, 249.59, 249.59},
   {193000, 248.74, 248.74, 248.74, 248.74}, {194000, 247.88, 247.88, 247.88, 247.88},
   {195000, 247.03, 247.03, 247.03, 247.03}, {196000, 246.18, 246.18, 246.18, 246.18},
   {197000, 245.32, 245.32, 245.32, 245.32}, {198000, 244.47, 244.47, 244.47, 244.47},
   {199000, 243.62, 243.62, 243.62, 243.62}, {200000, 242.76, 242.76, 242.76, 242.76},
   {201000, 241.91, 241.91, 241.91, 241.91}};

// ==========================================================================
void UtAtmosphereData::FillTables()
{
   for (int iAtmIndex = 0; iAtmIndex < NUM_ATMOSPHERES; ++iAtmIndex)
   {
      FillTable(iAtmIndex);
   }
   CalcInterpolants();
}

// ==========================================================================
void UtAtmosphereData::FillTable(int aAtmIndex)
{
   // Fill in lookup table using private functions that use
   // empirical formulas
   for (int iAlt = 0; iAlt < NUM_ALTS_STD; ++iAlt)
   {
      bool isNonStd = (aAtmIndex > 0);

      double altM = UtAtmosphere::MIN_ALT + iAlt * UtAtmosphere::ALT_INCR;

      double stdTemp    = CalcStdTemperature(altM);
      double nonStdTemp = stdTemp;
      if (isNonStd)
      {
         nonStdTemp = CalcNonStdTemperature(altM, aAtmIndex);
      }

      // All atmospheres (whether standard or not) use the same pressure profile,
      // a profile that is based on the STANDARD TEMPERATURE PROFILE.
      double stdPressure   = CalcPressure(altM, stdTemp);
      double stdDensity    = CalcDensity(altM, stdTemp, stdPressure);
      double nonStdDensity = stdDensity;

      if (isNonStd)
      {
         nonStdDensity = CalcDensity(altM, nonStdTemp, stdPressure);
      }

      mTable[aAtmIndex][iAlt][ALTITUDE]    = altM;
      mTable[aAtmIndex][iAlt][TEMPERATURE] = (isNonStd) ? nonStdTemp : stdTemp;
      mTable[aAtmIndex][iAlt][PRESSURE]    = stdPressure;
      mTable[aAtmIndex][iAlt][DENSITY]     = (isNonStd) ? nonStdDensity : stdDensity;

      // NOTE / TO-DO - Modification of the temperature to be non-standard MAY adversely
      //                affect this calculation, but was not accounted for.  No guarantees.
      mTable[aAtmIndex][iAlt][REL_HUMIDITY] = CalcRelHumidity(altM, stdTemp, stdDensity);

      // Note that this value is based on _sea_level_standard.  This calculation should still be okay at non-standard.
      mTable[aAtmIndex][iAlt][SONIC_SPEED] =
         UtAtmosphere::cSSL_SONIC_VELOCITY * sqrt(mTable[aAtmIndex][iAlt][TEMPERATURE] / UtEarth::cSSL_TEMP);
   }

   for (int iAlt = NUM_ALTS_STD; iAlt < NUM_ALTS_MAX; ++iAlt)
   {
      mTable[aAtmIndex][iAlt][ALTITUDE]     = mTable[aAtmIndex][iAlt - 1][ALTITUDE] + 1.0;
      mTable[aAtmIndex][iAlt][TEMPERATURE]  = 0.0;
      mTable[aAtmIndex][iAlt][PRESSURE]     = 0.0;
      mTable[aAtmIndex][iAlt][DENSITY]      = 0.0;
      mTable[aAtmIndex][iAlt][REL_HUMIDITY] = 0.0;
      mTable[aAtmIndex][iAlt][SONIC_SPEED]  = mTable[aAtmIndex][iAlt - 1][SONIC_SPEED];
   }
}

// ==========================================================================
void UtAtmosphereData::CalcInterpolants()
{
   for (int iAtm = 0; iAtm < NUM_ATMOSPHERES; ++iAtm)
   {
      for (int iAlt = 0; iAlt < NUM_ALTS_MAX - 1; ++iAlt)
      {
         // Calculate interpolants for Temperature.
         mTable[iAtm][iAlt][TEMP_INTERP] = (mTable[iAtm][iAlt + 1][TEMPERATURE] - mTable[iAtm][iAlt][TEMPERATURE]) /
                                           (mTable[iAtm][iAlt + 1][ALTITUDE] - mTable[iAtm][iAlt][ALTITUDE]);
         // Calculate interpolants for Pressure.
         mTable[iAtm][iAlt][PRESS_INTERP] = (mTable[iAtm][iAlt + 1][PRESSURE] - mTable[iAtm][iAlt][PRESSURE]) /
                                            (mTable[iAtm][iAlt + 1][ALTITUDE] - mTable[iAtm][iAlt][ALTITUDE]);
         // Calculate interpolants for Density.
         mTable[iAtm][iAlt][DENS_INTERP] = (mTable[iAtm][iAlt + 1][DENSITY] - mTable[iAtm][iAlt][DENSITY]) /
                                           (mTable[iAtm][iAlt + 1][ALTITUDE] - mTable[iAtm][iAlt][ALTITUDE]);
         // Calculate interpolants for Sonic Velocity.
         mTable[iAtm][iAlt][SSPD_INTERP] = (mTable[iAtm][iAlt + 1][SONIC_SPEED] - mTable[iAtm][iAlt][SONIC_SPEED]) /
                                           (mTable[iAtm][iAlt + 1][ALTITUDE] - mTable[iAtm][iAlt][ALTITUDE]);
         // Calculate interpolants for Relative Humidity
         mTable[iAtm][iAlt][RH_INTERP] = (mTable[iAtm][iAlt + 1][REL_HUMIDITY] - mTable[iAtm][iAlt][REL_HUMIDITY]) /
                                         (mTable[iAtm][iAlt + 1][ALTITUDE] - mTable[iAtm][iAlt][ALTITUDE]);
      }
      mTable[iAtm][NUM_ALTS_MAX - 1][TEMP_INTERP]  = 0.0;
      mTable[iAtm][NUM_ALTS_MAX - 1][PRESS_INTERP] = 0.0;
      mTable[iAtm][NUM_ALTS_MAX - 1][DENS_INTERP]  = 0.0;
      mTable[iAtm][NUM_ALTS_MAX - 1][SSPD_INTERP]  = 0.0;
      mTable[iAtm][NUM_ALTS_MAX - 1][RH_INTERP]    = 0.0;
   }
}

// ==========================================================================
bool UtAtmosphereData::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "default_atmosphere_type")
   {
      // This is a static command, applied to all atmosphere instances
      // instantiated after this command is encountered.
      AtmosphereType atmosphereType;
      std::string    atmosphereTypeString;
      aInput.ReadValue(atmosphereTypeString);
      bool valueIsValid = UtAtmosphere::StringToAtmosphere(atmosphereTypeString, atmosphereType);
      if (valueIsValid && atmosphereType != UtAtmosphere::cDEFAULT)
      {
         SetAtmosphereType(atmosphereType);
         if (atmosphereType == UtAtmosphere::cSIMPLE_DT)
         {
            aInput.ReadValueOfType(mSimpleDeltaTempK, UtInput::cTEMPERATURE);
         }
      }
      else if (valueIsValid && atmosphereType == UtAtmosphere::cDEFAULT)
      {
         std::string notice =
            std::string("Atmosphere type of '") + atmosphereTypeString + std::string("' not valid in a global context.");
         throw UtInput::BadValue(aInput, notice);
      }

      else
      {
         std::string notice =
            std::string("Unknown atmosphere type '") + atmosphereTypeString + std::string("' supplied for '") + command;
         throw UtInput::BadValue(aInput, notice);
      }
   }
   else if (command == "atmosphere_calibration")
   {
      bool   altSet      = false;
      bool   densSet     = false;
      bool   tempSet     = false;
      double altitude    = 0.0;
      double density     = 0.0;
      double temperature = 0.0;

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         UtInput& input = inputBlock.GetInput();
         command        = input.GetCommand();
         if (command == "altitude")
         {
            input.ReadValueOfType(altitude, UtInput::cLENGTH);
            altSet = true;
         }
         else if (command == "density")
         {
            input.ReadValueOfType(density, UtInput::cMASS_DENSITY);
            densSet = true;
         }
         else if (command == "temperature")
         {
            input.ReadValueOfType(temperature, UtInput::cTEMPERATURE);
            input.ValueGreater(temperature, 180.0);
            tempSet = true;
         }
      }

      if (densSet && altSet && !tempSet)
      {
         double deltaTemp;
         if (SimpleDeltaTempKforDensity(altitude, density, deltaTemp))
         {
            SetAtmosphereType(UtAtmosphere::cSIMPLE_DT);
            SetSimpleDeltaTempK(deltaTemp);
            auto ou = ut::log::info()
                      << "Per the atmosphere calibration block provided, selected default atmosphere is 'simple_dt'.";
            ou.AddNote() << "Deviation: " << deltaTemp << " deg K";
         }
         else
         {
            std::string notice = std::string(
               "The 'atmosphere_density_calibration' block could not match the supplied altitude and density.");
            throw UtInput::BadValue(aInput, notice);
         }
      }
      else if (altSet && tempSet && !densSet)
      {
         double deltaTemp;
         if (SimpleDeltaTempKforTemperature(altitude, temperature, deltaTemp))
         {
            SetAtmosphereType(UtAtmosphere::cSIMPLE_DT);
            SetSimpleDeltaTempK(deltaTemp);
            auto out = ut::log::info()
                       << "Per the atmosphere calibration block provided, selected default atmosphere is 'simple_dt'.";
            out.AddNote() << "Deviation: " << deltaTemp << " deg K";
         }
         else
         {
            throw UtInput::BadValue(
               aInput,
               "The 'atmosphere_temperature_calibration' block could not match the supplied altitude and temperature.");
         }
      }
      else
      {
         throw UtInput::BadValue(
            aInput,
            "The 'atmosphere_calibration' block must be supplied an altitude, and either temperature or density.");
      }
   }
   else if (command == "print_mks_atmosphere_tables")
   {
      GetAtmosphere().PrintAll(true, true);
   }
   else if (command == "print_sae_atmosphere_tables")
   {
      GetAtmosphere().PrintAll(false, true);
   }
   else if (command == "atmosphere_table")
   {
      int          atmIndex = AtmosphereToInt(UtAtmosphere::cCUSTOM);
      UtInputBlock inputBlock(aInput);

      int         altIndex = 0;
      std::string subCommand;
      std::string layerDataString;

      while (inputBlock.ReadCommand(subCommand))
      {
         if (altIndex >= NUM_ALTS_MAX)
         {
            throw UtInput::BadValue(aInput, "There can be no more than 512 table entries");
         }

         inputBlock.GetInput().PushBack(subCommand);

         aInput.ReadLine(layerDataString, false);

         UtInput altitudeEntry;
         altitudeEntry.PushInputString(layerDataString);

         // Expected order of data is:
         double& altitudeMSL_m  = mTable[atmIndex][altIndex][ALTITUDE];
         double& temperature_K  = mTable[atmIndex][altIndex][TEMPERATURE];
         double& pressure_Pa    = mTable[atmIndex][altIndex][PRESSURE];
         double& density_kgm3   = mTable[atmIndex][altIndex][DENSITY];     // optional, can be estimated
         double& sonicSpeed_mps = mTable[atmIndex][altIndex][SONIC_SPEED]; // optional, can be estimated

         altitudeEntry.ReadValue(altitudeMSL_m);

         if (altIndex > 0)
         {
            if (altitudeMSL_m <= mTable[atmIndex][altIndex - 1][ALTITUDE])
            {
               throw UtInput::BadValue(aInput, "Altitude entries must be monotonically increasing");
            }
         }

         altitudeEntry.ReadValue(temperature_K);
         altitudeEntry.ValueGreaterOrEqual(temperature_K, 0.0);

         altitudeEntry.ReadValue(pressure_Pa);
         altitudeEntry.ValueGreaterOrEqual(pressure_Pa, 0.0);

         try
         {
            altitudeEntry.ReadValue(density_kgm3);
            altitudeEntry.ValueGreaterOrEqual(density_kgm3, 0.0);
         }
         catch (const UtInput::EndOfData&)
         {
            // No density data provided, infer from temperature and pressure
            density_kgm3 = CalcDensity(altitudeMSL_m, temperature_K, pressure_Pa);
         }

         try
         {
            altitudeEntry.ReadValue(sonicSpeed_mps);
            altitudeEntry.ValueGreaterOrEqual(sonicSpeed_mps, 0.0);
         }
         catch (const UtInput::EndOfData&)
         {
            // No speed-of-sound data provided, infer from temperature
            sonicSpeed_mps = UtAtmosphere::cSSL_SONIC_VELOCITY * sqrt(temperature_K / UtEarth::cSSL_TEMP);
         }

         // If we encounter more than 5 entries in a column, there may be a data input error
         if (altitudeEntry.TryReadCommand(subCommand))
         {
            throw UtInput::BadValue(aInput, "There may be no more than 5 columns to a row");
         }

         // We do not expect relative humidity as an input
         mTable[atmIndex][altIndex][REL_HUMIDITY] = CalcRelHumidity(altitudeMSL_m, temperature_K, density_kgm3);

         ++altIndex;
      }
      if (altIndex == 0)
      {
         throw UtInput::BadValue(aInput, "No table data supplied");
      }
      while (altIndex < NUM_ALTS_MAX)
      {
         mTable[atmIndex][altIndex][ALTITUDE]     = mTable[atmIndex][altIndex - 1][ALTITUDE] + 1.0;
         mTable[atmIndex][altIndex][TEMPERATURE]  = mTable[atmIndex][altIndex - 1][TEMPERATURE];
         mTable[atmIndex][altIndex][PRESSURE]     = mTable[atmIndex][altIndex - 1][PRESSURE];
         mTable[atmIndex][altIndex][DENSITY]      = mTable[atmIndex][altIndex - 1][DENSITY];
         mTable[atmIndex][altIndex][REL_HUMIDITY] = mTable[atmIndex][altIndex - 1][REL_HUMIDITY];
         mTable[atmIndex][altIndex][SONIC_SPEED]  = mTable[atmIndex][altIndex - 1][SONIC_SPEED];
         ++altIndex;
      }
      SetAtmosphereType(UtAtmosphere::cCUSTOM);
      CalcInterpolants();
   }
   else if (command == "contrailing_altitude_floor")
   {
      aInput.ReadValueOfType(mContrailingAltitudes_m.first, UtInput::cLENGTH);
   }
   else if (command == "contrailing_altitude_ceiling")
   {
      aInput.ReadValueOfType(mContrailingAltitudes_m.second, UtInput::cLENGTH);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ==========================================================================
int UtAtmosphereData::AtmosphereToInt(AtmosphereType aAtmosphereType)
{
   int index;
   switch (aAtmosphereType)
   {
   case UtAtmosphere::cSTANDARD_DAY:
      index = 0;
      break;
   case UtAtmosphere::cHOT_DAY:
      index = 1;
      break;
   case UtAtmosphere::cTROPICAL_DAY:
      index = 2;
      break;
   case UtAtmosphere::cCOLD_DAY:
      index = 3;
      break;
   case UtAtmosphere::cPOLAR_DAY:
      index = 4;
      break;
   case UtAtmosphere::cSIMPLE_DT:
      index = 5;
      break;
   case UtAtmosphere::cCUSTOM:
      index = 6;
      break;
   case UtAtmosphere::cDEFAULT:
   default:
      index = AtmosphereToInt(mDefaultAtmosphere);
   }
   return index;
}

// ==========================================================================
// protected
double UtAtmosphereData::ReverseInterpolation(double aValue, int aAtmTypeIndex, int aBase, int aInterp)
{
   double lastValue = 0.0;
   for (int altIndex = 0; altIndex != UtAtmosphereData::NUM_ALTS_MAX; ++altIndex)
   {
      double thisValue = mTable[aAtmTypeIndex][altIndex][aBase];
      if ((altIndex > 0) && Bridges(aValue, thisValue, lastValue))
      {
         return mTable[aAtmTypeIndex][altIndex][ALTITUDE] +
                (aValue - mTable[aAtmTypeIndex][altIndex][aBase]) / mTable[aAtmTypeIndex][altIndex][aInterp];
      }
      lastValue = thisValue;
   }
   return 0.0;
}

// ==========================================================================
//! Returns the Geopotential Altitude of the atmosphere, given a Pressure Altitude.
// double UtAtmosphere::GeopotentialAltitude(double aPressureAltitude) const
// {
//    SelectAtmosphere();
//    if (mAtmosphereTypeIndex == 0)
//    {
//       // For standard atmosphere, geo and press alt are the same.
//       return aPressureAltitude;
//    }
//    else
//    {
//       return 0.0; // TO-DO Pressure(Pressure(aAltitude);)
//    }
// }

// ==========================================================================
//! Relative humidity as a function of Altitude (meters) above sea level.
// static
double UtAtmosphereData::CalcRelHumidity(double aAltitude, double aTemp, double aDensity)
{
   // NOTE:  This has NOT been re-visited after non-standard atmospheres were implemented.
   if (aTemp == -1.0)
   {
      aTemp = CalcStdTemperature(aAltitude);
   }

   if (aDensity == -1.0)
   {
      aDensity = CalcDensity(aAltitude, aTemp, -1.0);
   }

   if (aAltitude >= 30000.0)
   {
      return 0.0;
   }

   double aWaterDensity     = aDensity * CalcWaterVapor(aAltitude);
   double aSaturatedDensity = SaturatedVaporDensity(aTemp);
   return aWaterDensity / aSaturatedDensity;
}


// ==========================================================================
//! aAltitude is Geopotential Altitude, meters
// static
double UtAtmosphereData::CalcStdTemperature(double aAltitude)
{
   double result = 0.0;

   // This altitude is meters.
   if ((0 <= aAltitude) && (aAltitude < 11000))
   {
      result = 288.15 - 6.5 * aAltitude / 1000; // Troposphere
   }
   else if ((11000 <= aAltitude) && (aAltitude < 20000))
   {
      result = 216.65; // Stratosphere A
   }
   else if ((20000 <= aAltitude) && (aAltitude < 32000))
   {
      result = 216.65 + 1.0 * (aAltitude - 20000) / 1000; // Stratosphere B
   }
   else if ((32000 <= aAltitude) && (aAltitude < 47000))
   {
      result = 228.65 + 2.8 * (aAltitude - 32000) / 1000; // Stratosphere C
   }
   else if ((47000 <= aAltitude) && (aAltitude < 51000))
   {
      result = 270.65; // Mesosphere
   }
   else if ((51000 <= aAltitude) && (aAltitude < 71000))
   {
      result = 270.65 - 2.8 * (aAltitude - 51000) / 1000;
   }
   else if ((71000 <= aAltitude) && (aAltitude < 84852))
   {
      result = 214.65 - 2.0 * (aAltitude - 71000) / 1000;
   }
   else if (84852 <= aAltitude)
   {
      result = 186.946;
   }
   return result;
}

// ==========================================================================
//! aAltitude is meters Pressure Altitude
double UtAtmosphereData::CalcNonStdTemperature(double aPressureAltitude, int aAtmIndex)
{
   if (aAtmIndex == UtAtmosphere::cSIMPLE_DT)
   {
      // Note, this function uses meters rather than feet...
      double resultTempK = CalcStdTemperature(aPressureAltitude);
      double tempDelta   = mSimpleDeltaTempK;
      if (aPressureAltitude > 47000)
      {
         tempDelta = 0.0;
      }
      else if (aPressureAltitude < 30480)
      {
         // Do nothing
      }
      else // if (aPressureAltitude > 30480)
      {
         // Above 30480 m (100000 ft), begin to reduce the temperature delta, until it matches standard at 47000 m
         double k  = (aPressureAltitude - 41000) / (30480 - 41000);
         tempDelta = k * mSimpleDeltaTempK;
      }
      resultTempK += tempDelta;
      return resultTempK;
   }

   double pressAltFt = aPressureAltitude * UtMath::cFT_PER_M;

   int atmIndex = 0;
   switch (aAtmIndex)
   {
   case (1):
      atmIndex = MS_210_TF_HOT;
      break;
   case (2):
      atmIndex = MS_210_TF_TRP;
      break;
   case (3):
      atmIndex = MS_210_TF_CLD;
      break;
   case (4):
      atmIndex = MS_210_TF_PLR;
   }

   int altIndex = 0;
   while (sMS_210A[altIndex][MS_210_ALT] < pressAltFt && altIndex + 1 < NUM_210_ALTS)
   {
      ++altIndex;
   }
   return sInterp(pressAltFt,
                  sMS_210A[altIndex][MS_210_ALT],
                  sMS_210A[altIndex + 1][MS_210_ALT],
                  sMS_210A[altIndex][atmIndex],
                  sMS_210A[altIndex + 1][atmIndex]);
};

// ==========================================================================
/**
 *  Water Vapor mixing ratio (parts per million by mass) based on
 *  US Standard 1976 atmosphere.
 */
// static
double UtAtmosphereData::CalcWaterVapor(double aAltitude)
{
   double result = 0.0;
   double aAltKM = aAltitude * 0.001;

   if (aAltKM <= 14.0)
   {
      result = 4753.9 - 1202.9 * aAltKM + 101.5 * pow(aAltKM, 2.0) - 2.8433 * pow(aAltKM, 3.0);
   }
   else if (aAltKM <= 16.0)
   {
      result = 3.3;
   }
   else if (aAltKM < 30.0)
   {
      result = 2.5;
   }

   return result;
}

// ==========================================================================
/**
 *  Pressure based on US Standard 1976 temperature profiles
 *  If temperature at this altitude has already been calculated from
 *  previous call to CalcTemperature, pass it in as aTemp. Otherwise,
 *  CalcTemperature will be called in this function.
 */
// static
double UtAtmosphereData::CalcPressure(double aAltitude, double aTemp)
{
   if (aTemp == -1.0)
   {
      aTemp = CalcStdTemperature(aAltitude);
   }

   double ratio = 0.0;

   if ((0 <= aAltitude) && (aAltitude < 11000))
   {
      ratio = 1.0000000e00 * pow(288.15 / aTemp, 1000 * UtAtmosphere::cGMR / -6.5);
   }
   else if ((11000 <= aAltitude) && (aAltitude < 20000))
   {
      ratio = 2.2336110e-1 * exp(-UtAtmosphere::cGMR * (aAltitude - 11000) / 216.65);
   }
   else if ((20000 <= aAltitude) && (aAltitude < 32000))
   {
      ratio = 5.4032950e-2 * pow(216.65 / aTemp, 1000 * UtAtmosphere::cGMR / 1.0);
   }
   else if ((32000 <= aAltitude) && (aAltitude < 47000))
   {
      ratio = 8.5666784e-3 * pow(228.65 / aTemp, 1000 * UtAtmosphere::cGMR / 2.8);
   }
   else if ((47000 <= aAltitude) && (aAltitude < 51000))
   {
      ratio = 1.0945601e-3 * exp(-UtAtmosphere::cGMR * (aAltitude - 47000) / 270.65);
   }
   else if ((51000 <= aAltitude) && (aAltitude < 71000))
   {
      ratio = 6.6063531e-4 * pow(270.65 / aTemp, 1000 * UtAtmosphere::cGMR / -2.8);
   }
   else if ((71000 <= aAltitude) && (aAltitude < 84852))
   {
      ratio = 3.9046830e-5 * pow(214.65 / aTemp, 1000 * UtAtmosphere::cGMR / -2.0);
   }
   else if (84852 <= aAltitude)
   {
      ratio = 3.6850100e-6 * exp(-UtAtmosphere::cGMR * (aAltitude - 84852) / 186.946);
   }

   return (UtEarth::cSSL_PRESSURE * ratio);
}

// ==========================================================================
//! Calculates the saturated vapor density (kg/m^3)
//! Empirical curve fit retrieved from
//!   http://hyperphysics.phy-astr.gsu.edu/hbase/kinetic/relhum.html
//! on 22 Aug 2007
// static
double UtAtmosphereData::SaturatedVaporDensity(double aTemp)
{
   double tempC = aTemp - 273.15;

   double densityGM_M3 = 5.018 + 0.32321 * tempC + 8.1847e-3 * pow(tempC, 2.0) + 3.1243e-4 * pow(tempC, 3.0);
   return densityGM_M3 * 0.001;
}

// ==========================================================================
/**
 *  If temperature or pressure at this altitude has already been
 *  calculated from previous call to CalcTemperature or CalcPressure,
 *  pass them in as aTemp and aPressure, respectively. Otherwise,
 *  CalcTemperature and CalcPressure will be called in this function.
 */
// static
double UtAtmosphereData::CalcDensity(double aAltitude, double aTemp, double aPressure)
{
   if (aTemp == -1.0)
   {
      aTemp = CalcStdTemperature(aAltitude);
   }

   if (aPressure == -1.0)
   {
      aPressure = CalcPressure(aAltitude, aTemp);
   }

   if (aTemp == 0.0) // prevent divide by zero
   {
      return 0.0;
   }
   else
   {
      return ((aPressure / UtEarth::cSSL_PRESSURE) / (aTemp / UtEarth::cSSL_TEMP)) * UtEarth::cSSL_AIR_DENSITY;
   }
}

double UtAtmosphereData::StandardAltitudeForDensity(double aDensity)
{
   if (aDensity >= mTable[0][0][DENSITY])
   {
      return 0.0;
   }

   for (int iAlt = 0; iAlt != NUM_ALTS_STD; ++iAlt)
   {
      if (mTable[0][iAlt][DENSITY] < aDensity)
      {
         return sInterp(aDensity,
                        mTable[0][iAlt - 1][DENSITY],
                        mTable[0][iAlt][DENSITY],
                        mTable[0][iAlt - 1][ALTITUDE],
                        mTable[0][iAlt][ALTITUDE]);
      }
   }

   // Return top of chart.
   return mTable[0][NUM_ALTS_STD - 1][ALTITUDE];
}

// ==========================================================================
// static
bool UtAtmosphereData::SimpleDeltaTempKforDensity(double aAltitude, double aNonStandardDensity, double& aChosenDeltaTemperature)
{
   double stdTempAtNSalt  = CalcStdTemperature(aAltitude);
   double stdPressAtNSalt = CalcPressure(aAltitude, stdTempAtNSalt);

   // Now begin at low (negative) temperature delta, and increase it, until the desired
   // density value is bracketed.  When bracketed, interpolate to get the actual DT needed.
   const double cMIN_DT   = -60.0;
   const double cMAX_DT   = 60.0;
   const double cDELTA_DT = 5.0;

   double lastDT      = cMIN_DT;
   double lastDensity = 0.0;

   for (double dt = cMIN_DT; dt < cMAX_DT; dt += cDELTA_DT)
   {
      double nonStdTemp = stdTempAtNSalt + dt;
      double density    = CalcDensity(aAltitude, nonStdTemp, stdPressAtNSalt);
      if ((density <= aNonStandardDensity) && (lastDensity > aNonStandardDensity))
      {
         // Bracketed desired density.  Interpolate on density to get needed DT.
         aChosenDeltaTemperature = sInterp(aNonStandardDensity, lastDensity, density, lastDT, dt);
         return true;
      }
      lastDensity = density;
      lastDT      = dt;
   }

   return false;
}

// ==========================================================================
// static
bool UtAtmosphereData::SimpleDeltaTempKforTemperature(double  aAltitude,
                                                      double  aNonStandardTemperature,
                                                      double& aChosenDeltaTemperature)
{
   double stdTemp          = CalcStdTemperature(aAltitude);
   aChosenDeltaTemperature = aNonStandardTemperature - stdTemp;
   return true;
}

// ==============================================================================
//! Change the default UtAtmosphere type instantiated using a default constructor.
void UtAtmosphereData::SetAtmosphereType(AtmosphereType aAtmosphereType)
{
   if ((aAtmosphereType != mDefaultAtmosphere) && (aAtmosphereType != UtAtmosphere::cDEFAULT))
   {
      mDefaultAtmosphere = aAtmosphereType;
   }
}

// ==============================================================================
//! Change the default Delta Temperature used by the cSIMPLE_DT atmosphere type.
void UtAtmosphereData::SetSimpleDeltaTempK(double aSimpleDeltaT_K)
{
   if (mSimpleDeltaTempK != aSimpleDeltaT_K)
   {
      mSimpleDeltaTempK = aSimpleDeltaT_K;
      FillTable(UtAtmosphere::cSIMPLE_DT);
      CalcInterpolants();
   }
}

UtAtmosphereData::UtAtmosphereData()
{
   FillTables();
}

UtAtmosphere UtAtmosphereData::GetAtmosphere()
{
   return UtAtmosphere(this, mDefaultAtmosphere);
}

// ==========================================================================
//! Returns the Pressure Altitude of the atmosphere, given a Geopotential Altitude.
// double UtAtmosphere::PressureAltitude(double aGeopotentialAltitude) const
// {
//    SelectAtmosphere();
//    {
//       if (mAtmosphereTypeIndex == 0)
//       {
//          // For standard atmosphere, geo and press alt are the same.
//          return aGeopotentialAltitude;
//       }
//       else
//       {
//          return 0.0; // TO-DO FindPressure(Pressure(aAltitude);)
//       }
//    }
// }

UtAtmosphere::UtAtmosphere(UtAtmosphereData* aDataPtr, AtmosphereType aAtmosphereType)
   : mSelectedAtmosphereType((int)aAtmosphereType)
   , mLastAltIndex(0)
   , mDataPtr(aDataPtr)
{
   mDataPtr->AddRef();
}

// ==========================================================================
UtAtmosphere::UtAtmosphere(const UtAtmosphere& aSrcAtmosphere, AtmosphereType aAtmosphereType)
   : mSelectedAtmosphereType((int)aAtmosphereType)
   , mLastAltIndex(0)
   , mDataPtr(aSrcAtmosphere.mDataPtr)
{
   mDataPtr->AddRef();
}

// ==========================================================================
UtAtmosphere::~UtAtmosphere()
{
   mDataPtr->Unref();
}

// ==========================================================================
//! Access the type of atmosphere represented by this particular instance.
UtAtmosphere::AtmosphereType UtAtmosphere::GetAtmosphereType() const
{
   return IntToAtmosphere(GetAtmosphereIndex());
}

// ==========================================================================
// static
bool UtAtmosphere::StringToAtmosphere(const std::string& aAtmosphereString, AtmosphereType& aAtmosphereType)
{
   bool success = true;
   if (aAtmosphereString == "default")
   {
      aAtmosphereType = cDEFAULT;
   }
   else if (aAtmosphereString == "standard_day")
   {
      aAtmosphereType = cSTANDARD_DAY;
   }
   else if (aAtmosphereString == "hot_day")
   {
      aAtmosphereType = cHOT_DAY;
   }
   else if (aAtmosphereString == "tropical_day")
   {
      aAtmosphereType = cTROPICAL_DAY;
   }
   else if (aAtmosphereString == "cold_day")
   {
      aAtmosphereType = cCOLD_DAY;
   }
   else if (aAtmosphereString == "polar_day")
   {
      aAtmosphereType = cPOLAR_DAY;
   }
   else if (aAtmosphereString == "simple_dt")
   {
      aAtmosphereType = cSIMPLE_DT;
   }
   else if (aAtmosphereString == "custom")
   {
      aAtmosphereType = cCUSTOM;
   }
   else
   {
      success = false;
   }
   return success;
}

// ==========================================================================
bool UtAtmosphere::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "atmosphere_type")
   {
      AtmosphereType atmosphereType;
      std::string    atmosphereTypeString;
      aInput.ReadValue(atmosphereTypeString);
      if (StringToAtmosphere(atmosphereTypeString, atmosphereType))
      {
         mSelectedAtmosphereType = (int)atmosphereType;
      }
      else
      {
         std::string notice =
            std::string("Unknown atmosphere type '") + atmosphereTypeString + std::string("' supplied for '") + command;
         throw UtInput::BadValue(aInput, notice);
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

bool UtAtmosphere::ProcessGlobalInput(UtInput& aInput)
{
   return mDataPtr->ProcessInput(aInput);
}

// ==========================================================================
// static
UtAtmosphere::AtmosphereType UtAtmosphere::IntToAtmosphere(int aValue)
{
   if (aValue >= cDEFAULT && aValue <= cCUSTOM)
   {
      return (AtmosphereType)aValue;
   }
   return cDEFAULT;
}
//
// // ==========================================================================
// void UtAtmosphere::SelectAtmosphere() const
// {
//    if (mAtmosphereTypeIndex == INDEX_NOT_SET)
//    {
//       mAtmosphereTypeIndex = mDataPtr->AtmosphereToInt(mDataPtr->sGlobalDefaultAtmosphere);
//    }
// }

// ==========================================================================
double UtAtmosphere::Interpolate(double aAltitude, int aBase, int aInterp) const
{
   double result = 0.0;

   // Table lookup and interpolation routines:
   int aIndex = GetAtmosphereIndex();

   // The atmospheres based on the US1976 table should produce zeroes for temperature,
   // pressure, and density, and return the last valid piece of data for speed of sound.
   int indexUpperBound;
   if (aIndex == UtAtmosphere::cCUSTOM)
   {
      indexUpperBound = UtAtmosphereData::NUM_ALTS_MAX - 1;
   }
   else
   {
      indexUpperBound = UtAtmosphereData::NUM_ALTS_STD - 1;
   }

   if (aAltitude <= mDataPtr->mTable[aIndex][0][ALTITUDE])
   {
      // Off the bottom of the chart.
      mLastAltIndex = 0;
      return mDataPtr->mTable[aIndex][0][aBase];
   }
   else if (aAltitude > mDataPtr->mTable[aIndex][indexUpperBound][ALTITUDE])
   {
      // Off the top of the chart.
      // Assume space has zero air properties.
      mLastAltIndex = UtAtmosphereData::NUM_ALTS_MAX - 2;

      if (aBase == SONIC_SPEED)
      {
         // We want a nonzero speed of sound for conversions between Mach
         // and airspeed, so we take the highest available value

         // For compatibility with previous results:
         // Set to highest available value:  318.6 m/s / 340.3 m/s = 0.9362
         return cSSL_SONIC_VELOCITY * 0.9362;

         // TODO replace hardcoded value with the true highest speed of sound
         // return mDataPtr->mTable[aIndex][mLastAltIndex][aBase];
      }
   }
   else
   {
      // Pick off an x value in the table that is
      // just below the altitude value of interest...
      while (mDataPtr->mTable[aIndex][mLastAltIndex + 1][ALTITUDE] < aAltitude)
      {
         ++mLastAltIndex;
      }
      while (mDataPtr->mTable[aIndex][mLastAltIndex][ALTITUDE] >= aAltitude)
      {
         --mLastAltIndex;
      }

      result = mDataPtr->mTable[aIndex][mLastAltIndex][aBase] +
               mDataPtr->mTable[aIndex][mLastAltIndex][aInterp] *
                  (aAltitude - mDataPtr->mTable[aIndex][mLastAltIndex][ALTITUDE]);
   }

   return result;
}

// ==========================================================================
//! Returns the Density Altitude of the atmosphere, given a Geopotential Altitude.
double UtAtmosphere::DensityAltitude(double aGeopotentialAltitude) const
{
   // SelectAtmosphere();
   int aIndex = GetAtmosphereIndex();
   if (aIndex == 0)
   {
      // For standard atmosphere, geo and density alt are the same.
      return aGeopotentialAltitude;
   }
   else
   {
      // The zero here indicates we want the density for standard atmosphere...
      double dens = Density(aGeopotentialAltitude);
      return mDataPtr->ReverseInterpolation(dens, 0, DENSITY, DENS_INTERP);
   }
}

// ==========================================================================
double UtAtmosphere::Temperature(double aAltitude) const
{
   return Interpolate(aAltitude, TEMPERATURE, TEMP_INTERP);
}

// ==========================================================================
double UtAtmosphere::TemperatureRatio(double aAltitude) const
{
   return Interpolate(aAltitude, TEMPERATURE, TEMP_INTERP) / UtEarth::cSSL_TEMP;
}

// ==========================================================================
double UtAtmosphere::Pressure(double aAltitude) const
{
   return Interpolate(aAltitude, PRESSURE, PRESS_INTERP);
}

// ==========================================================================
double UtAtmosphere::PressureRatio(double aAltitude) const
{
   return Interpolate(aAltitude, PRESSURE, PRESS_INTERP) / UtEarth::cSSL_PRESSURE;
}

// ==========================================================================
//! Air density ratio relative to SSL, as a function of Altitude (meters) above sea level.
double UtAtmosphere::DensityRatio(double aAltitude) const
{
   double density = Interpolate(aAltitude, DENSITY, DENS_INTERP);
   return density / UtEarth::cSSL_AIR_DENSITY;
}

// ==========================================================================
/**
Air density as a function of Altitude (meters) above sea level.
Air density can be specified in two ways:
- const double UtEarth::cSSL_AIR_DENSITY = 1.2250;  // kg / m^3
The value returned here is specific weight, which is required for
dynamic pressure calculations:
- Dynamic Pressure = q = 1/2 * SpecWt * v^2,
*/

double UtAtmosphere::Density(double aAltitude) const
{
   return Interpolate(aAltitude, DENSITY, DENS_INTERP);
}

// ==========================================================================
/**
    Sonic velocity as a function of Altitude (meters) above sea level.
    Sonic velocity is needed to calculate Mach number, which greatly
    influences aerodynamic forces.
*/

double UtAtmosphere::SonicVelocity(double aAltitude) const
{
   // Standard Sea Level Sonic Velocity, meters per second.
   return Interpolate(aAltitude, SONIC_SPEED, SSPD_INTERP);
}

// ==========================================================================
//! Relative humidity as a function of Altitude (meters) above sea level.
double UtAtmosphere::RelativeHumidity(double aAltitude) const
{
   return Interpolate(aAltitude, REL_HUMIDITY, RH_INTERP);
}

// ==========================================================================
//! Returns the water vapor density at an altitude (kg/m^3)
double UtAtmosphere::WaterVaporDensity(double aAltitude) const
{
   // The value returned by CalcWaterVapor is in parts-per-million by mass.
   return Density(aAltitude) * UtAtmosphereData::CalcWaterVapor(aAltitude) * 1.0E-6;
}

// ==========================================================================
//! Returns true if contrails can form at an altitude
bool UtAtmosphere::IsWithinContrailAltitudeBand(double aAltitude) const
{
   return (mDataPtr->mContrailingAltitudes_m.first < aAltitude && aAltitude < mDataPtr->mContrailingAltitudes_m.second);
}

// ==========================================================================
//! Returns the current lower bound on the contrail band
double UtAtmosphere::GetContrailFloor() const
{
   return mDataPtr->mContrailingAltitudes_m.first;
}

// ==========================================================================
//! Returns the current upper bound on the contrail band
double UtAtmosphere::GetContrailCeiling() const
{
   return mDataPtr->mContrailingAltitudes_m.second;
}

// ==========================================================================
//! Returns the partial derivative change in speed (m/s) per change in
//! altitude (m) at constant Mach number.
double UtAtmosphere::dSpeed_dAltitudeAtMach(double aAltitude) const
{
   // Return value discarded, as we really want to set internal variable mLastAltIndex
   Interpolate(aAltitude, SONIC_SPEED, SSPD_INTERP);
   return mDataPtr->mTable[GetAtmosphereIndex()][mLastAltIndex][SSPD_INTERP];
}

// ==========================================================================
//! Returns the partial derivative change in density ratio per change
//! in altitude (m).
double UtAtmosphere::dDensityRatio_dAltitude(double aAltitude) const
{
   // Return value discarded, as we really want to set internal variable mLastAltIndex
   Interpolate(aAltitude, DENSITY, DENS_INTERP);
   return mDataPtr->mTable[GetAtmosphereIndex()][mLastAltIndex][DENS_INTERP];
}


// ==========================================================================
//! Returns the dynamic (or absolute) viscosity (kg/(m-s)) at a given altitude.
//! Based on Sutherland's law.
double UtAtmosphere::DynamicViscosity(double aAltitude) const
{
   // From www.cfd-online/Wiki/Sutherland's_Law
   // (These give the best comparison with the Pratt & Whitney Aeronautical Vest Pocket Handbook)

   static const double cS    = 110.4;
   static const double cT_0  = 273.15;
   static const double cMU_0 = 1.716E-5;

   // From 'Dynamic Viscosity' on Wikipedia
   // (These are just included for reference, the uncommented values give 'better' results)

   // static const double cS    = 120.0;
   // static const double cT_0  = 291.15;
   // static const double cMU_0 = 1.827E-5;

   double t  = Temperature(aAltitude);
   double mu = cMU_0 * ((cT_0 + cS) / (t + cS)) * pow(t / cT_0, 1.5);
   return mu;
}

double UtAtmosphere::CalcAirDensity_slug_ft3(double aAltitude_ft) const
{
   double altitude_m      = aAltitude_ft * UtMath::cM_PER_FT;
   double airDensity_kgm3 = Density(altitude_m);
   return UtMath::cSLUG_FT3_PER_KG_M3 * airDensity_kgm3;
}

double UtAtmosphere::CalcAmbientPressure_psf(double aAltitude_ft) const
{
   double altitude_m        = aAltitude_ft * UtMath::cM_PER_FT;
   double staticPressure_Pa = Pressure(altitude_m);
   return UtMath::cPSF_PER_PASCAL * staticPressure_Pa;
}

double UtAtmosphere::CalcKtasFromKcas(double aAltitude_m, double aSpeed_kcas) const
{
   double densityRatio = DensityRatio(aAltitude_m);

   // Since the dynamic pressure is the same between calibrated and true,
   // we get: 0.5 * rho_alt * (trueSpeed_fps^2) = q = 0.5 * rho_SL * (calSpeed_fps^2)
   //         0.5 * rho_alt * (trueSpeed_fps^2) = 0.5 * rho_SL * (calSpeed_fps^2)
   //               rho_alt * (trueSpeed_fps^2) = rho_SL * (calSpeed_fps^2)
   //                         (trueSpeed_fps^2) = rho_SL / rho_alt * (calSpeed_fps^2)
   //                             trueSpeed_fps = sqrt(rho_SL / rho_alt * (calSpeed_fps^2))

   double speed_ktas = aSpeed_kcas * sqrt(1.0 / densityRatio);
   return speed_ktas;
}

double UtAtmosphere::CalcKcasFromKtas(double aAltitude_m, double aSpeed_ktas) const
{
   double densityRatio = DensityRatio(aAltitude_m);

   // Since the dynamic pressure is the same between calibrated and true,
   // we get: 0.5 * rho_alt * (trueSpeed_fps^2) = q = 0.5 * rho_SL * (calSpeed_fps^2)
   //         0.5 * rho_alt * (trueSpeed_fps^2) = 0.5 * rho_SL * (calSpeed_fps^2)
   //               rho_alt * (trueSpeed_fps^2) = rho_SL * (calSpeed_fps^2)
   //                         (trueSpeed_fps^2) = rho_SL / rho_alt * (calSpeed_fps^2)
   //                             trueSpeed_fps = sqrt(rho_SL / rho_alt * (calSpeed_fps^2))

   double speed_kcas = aSpeed_ktas * sqrt(densityRatio);
   return speed_kcas;
}

double UtAtmosphere::CalcKtasFromMach(double aAltitude_m, double aSpeed_mach) const
{
   double speed_fps = CalcFpsFromMach(aAltitude_m, aSpeed_mach);
   return UtMath::cNMPH_PER_FPS * speed_fps;
}

double UtAtmosphere::CalcMachFromKtas(double aAltitude_m, double aSpeed_ktas) const
{
   double speed_fps = UtMath::cFPS_PER_NMPH * aSpeed_ktas;
   return CalcMachFromFps(aAltitude_m, speed_fps);
}

double UtAtmosphere::CalcKcasFromMach(double aAltitude_m, double aSpeed_mach) const
{
   double speed_fps = CalcFpsFromMach(aAltitude_m, aSpeed_mach);
   return CalcKcasFromFps(aAltitude_m, speed_fps);
}

double UtAtmosphere::CalcMachFromKcas(double aAltitude_m, double aSpeed_kcas) const
{
   double speed_fps = CalcFpsFromKcas(aAltitude_m, aSpeed_kcas);
   return CalcMachFromFps(aAltitude_m, speed_fps);
}

double UtAtmosphere::CalcKcasFromFps(double aAltitude_m, double aSpeed_fps) const
{
   double speed_ktas = UtMath::cNMPH_PER_FPS * aSpeed_fps;
   return CalcKcasFromKtas(aAltitude_m, speed_ktas);
}

double UtAtmosphere::CalcFpsFromKcas(double aAltitude_m, double aSpeed_kcas) const
{
   double speed_ktas = CalcKtasFromKcas(aAltitude_m, aSpeed_kcas);
   return UtMath::cFPS_PER_NMPH * speed_ktas;
}

double UtAtmosphere::CalcMachFromFps(double aAltitude_m, double aSpeed_fps) const
{
   double speed_mps        = aSpeed_fps * UtMath::cM_PER_FT;
   double speedOfSound_mps = SonicVelocity(aAltitude_m);

   if (UtMath::NearlyZero(speedOfSound_mps))
   {
      return 0.0;
   }
   return speed_mps / speedOfSound_mps;
}

double UtAtmosphere::CalcFpsFromMach(double aAltitude_m, double aSpeed_mach) const
{
   double speedOfSound_mps = SonicVelocity(aAltitude_m);
   double speedOfSound_fps = speedOfSound_mps * UtMath::cFT_PER_M;
   return aSpeed_mach * speedOfSound_fps;
}

double UtAtmosphere::CalcFpsFromDynamicPressure(double aAltitude_m, double aPressure_psf) const
{
   double rho_kgm3    = Density(aAltitude_m);
   double rho_slugft3 = rho_kgm3 * UtMath::cSLUG_FT3_PER_KG_M3;

   return sqrt(aPressure_psf / (0.5 * rho_slugft3));
}

double UtAtmosphere::CalcDynamicPressure_psf(double aAltitude_m, double aSpeed_fps) const
{
   double altitude_ft = UtMath::cFT_PER_M * aAltitude_m;
   return 0.5 * CalcAirDensity_slug_ft3(altitude_ft) * (aSpeed_fps * aSpeed_fps);
}


// ==========================================================================
//! This function is needed for backward compatibility.  Deprecated.
double UtAtmosphere::CalcPressureExported(double aAltitude)
{
   return UtAtmosphereData::CalcPressure(aAltitude, -1.0);
}

// ==========================================================================
//! This function accepts an air density value, and returns the standard atmosphere altitude at which
//! that density occurs.  (This is a utility to help better replicate existing flight test data.)
double UtAtmosphere::StandardAltitudeForDensity(double aDensity)
{
   return mDataPtr->StandardAltitudeForDensity(aDensity);
}

// ==========================================================================
//! This function is needed for backward compatibility.  Deprecated.  Use UtAirspeed.
double UtAtmosphere::KnotsEquivalentAirspeed(double aAltitude, double aTrueAirspeed)
{
   return SeaLevelEquivalentAirspeed(aAltitude, aTrueAirspeed) * UtMath::cNMPH_PER_MPS;
}

// ==========================================================================
//! This function is needed for backward compatibility.  Deprecated.  Use UtAirspeed.
double UtAtmosphere::SeaLevelEquivalentAirspeed(double aAltitude, double aTrueAirSpeed)
{
   // Iterate, using the function above to get the desired answer:
   double guessMPS  = 380.0 * UtMath::cMPS_PER_NMPH;
   bool   converged = false;
   for (int iGuess = 0; (iGuess < 6) && (!converged); ++iGuess)
   {
      double resultingMPS = TrueAirspeed(aAltitude, guessMPS);
      double ratio        = aTrueAirSpeed / resultingMPS;
      if (fabs(ratio - 1.0) < 0.0001)
      {
         converged = true;
      }
      guessMPS *= ratio;
   }
   return guessMPS;
}

// ==========================================================================
//! This function is needed for backward compatibility.  Deprecated.  Use UtAirspeed.
//! Translate from sea-level equivalent airspeed to true airspeed, given the input altitude (m).
//! (Depending upon the context, Calibrated, Indicated, Equivalent airspeeds may be roughly the same.)
double UtAtmosphere::TrueAirspeed(double aAltitude, double aEquivalentAirspeed) const
{
   // Thanks to Gary Richardson for providing a Perl script
   // implementation for this.  Does include compressibility
   // effects, but is not valid above Mach = 1.0
   double exponent = (cGAMMA - 1) / cGAMMA;
   // double coefficient = 2.0 * cGAMA / (cGAMA - 1);

   double speedRatio     = aEquivalentAirspeed / cSSL_SONIC_VELOCITY;
   double term1          = 1.0 + 0.2 * speedRatio * speedRatio;
   double pressRatio1    = pow(term1, 3.5) - 1.0;
   double sensedPressure = UtEarth::cSSL_PRESSURE * pressRatio1;

   // There are two pressure values that COULD be used here, the first is standard
   // pressure for this altitude.  The second is actual pressure for this atmosphere.
   // If the atmosphere is non-standard, then they will differ.)
   double nominalPressure = UtAtmosphereData::CalcPressure(aAltitude, -1.0);
   double pressRatio2     = sensedPressure / nominalPressure;
   double mach            = sqrt(5.0 * (pow(pressRatio2 + 1.0, exponent) - 1.0));
   return mach * SonicVelocity(aAltitude);
}

// ==========================================================================

void UtAtmosphere::PrintTableTo(std::ostream& aOutStream, bool aMetricValues) const
{
   if (aOutStream)
   {
      aOutStream.setf(std::ios::fixed);

      aOutStream << "# ATMOSPHERE TYPE = " << AtmosphereTypeName();
      if (GetAtmosphereIndex() == 5)
      {
         aOutStream << " (dt = " << mDataPtr->mSimpleDeltaTempK << " deg K from standard day)";
      }
      aOutStream << std::endl;
      aOutStream << "# "
                    "=================================================================================================="
                    "=================================="
                 << std::endl;
      aOutStream << "#    Alt(ft),    Alt(m),    Temp(K),    Temp(F),    Temp(C),    Pres(pa),  Pres(psi),  Sigma(), "
                    "Dens(), DensSAE,  Sonic(fps),  H20Dens "
                 << std::endl;
      aOutStream << "# "
                    "=================================================================================================="
                    "=================================="
                 << std::endl;

      aOutStream << "    ";

      double startAlt  = 0;
      double deltaAlt  = 5000;
      double finishAlt = 200000 + deltaAlt;

      if (aMetricValues)
      {
         deltaAlt  = 500;
         finishAlt = 60000 + deltaAlt;
      }

      for (double alt = startAlt; alt < finishAlt; alt += deltaAlt)
      {
         double altFt = alt;
         double altM  = alt;
         if (aMetricValues)
         {
            altFt = alt * UtMath::cFT_PER_M;
         }
         else
         {
            altM = alt * UtMath::cM_PER_FT;
         }

         double tempK       = Temperature(altM);
         double tempF       = UtMath::TempKtoF(tempK);
         double tempC       = tempK - UtMath::cZERO_C_IN_K;
         double pressMetric = Pressure(altM);
         double h20dens     = WaterVaporDensity(altM);
         double presPSI     = pressMetric * UtMath::cLB_PER_NT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / 144;
         aOutStream << altFt << ", " << altM << ", " << tempK << ", " << tempF << ", " << tempC << ", " << pressMetric
                    << ", " << presPSI << ", " << DensityRatio(altM) << ", " << Density(altM) << ", "
                    << DensityRatio(altM) * 0.0023769 << ", " << SonicVelocity(altM) * UtMath::cFT_PER_M << ", "
                    << h20dens << std::endl;
      }
      aOutStream << std::endl;
   }
}

// ==========================================================================
// static
void UtAtmosphere::PrintAll(bool aMetricValues, bool aPrintRawTables)
{
   // Write atmosphere output tables:
   std::string   fileName = "UtAtmosphereTables.lis";
   std::ofstream outFile(fileName.c_str());
   if (outFile)
   {
      UtAtmosphere a(*this, UtAtmosphere::cSTANDARD_DAY);
      a.PrintTableTo(outFile, aMetricValues);
      UtAtmosphere b(*this, UtAtmosphere::cHOT_DAY);
      b.PrintTableTo(outFile, aMetricValues);
      UtAtmosphere c(*this, UtAtmosphere::cTROPICAL_DAY);
      c.PrintTableTo(outFile, aMetricValues);
      UtAtmosphere d(*this, UtAtmosphere::cCOLD_DAY);
      d.PrintTableTo(outFile, aMetricValues);
      UtAtmosphere e(*this, UtAtmosphere::cPOLAR_DAY);
      e.PrintTableTo(outFile, aMetricValues);
      UtAtmosphere f(*this, UtAtmosphere::cSIMPLE_DT);
      f.PrintTableTo(outFile, aMetricValues);
      UtAtmosphere g(*this, UtAtmosphere::cCUSTOM);
      g.PrintTableTo(outFile, aMetricValues);

      if (aPrintRawTables)
      {
         PrintDataTable(outFile);
      }

      outFile.close();
   }
}

// ==========================================================================
void UtAtmosphere::PrintDataTable(std::ostream& aOutStream)
{
   for (int i = 0; i != UtAtmosphereData::NUM_ATMOSPHERES; ++i)
   {
      aOutStream << std::endl;
      aOutStream << "# RAW TABLE DATA, ATMOSPHERE TYPE = " << AtmosphereTypeName(i);
      if (i == 5)
      {
         aOutStream << " tempDT = " << mDataPtr->mSimpleDeltaTempK << " deg K";
      }
      aOutStream << std::endl;
      aOutStream << "# ALT_VALUE, ALT_FT, TEMP_K, TEMP_F, PRESSURE, DENSITY,   SPD_SOUND, REL_HUMIDITY,   Tinterp,   "
                    "Pinterp,   Dinterp,   Ainterp,   RHinterp"
                 << std::endl;
      for (int j = 0; j != UtAtmosphereData::NUM_ALTS_MAX; ++j)
      {
         for (int k = 0; k != UtAtmosphereData::NUM_VALUES; ++k)
         {
            aOutStream << mDataPtr->mTable[i][j][k] << ", ";

            if (k == 0)
            {
               aOutStream << UtMath::cFT_PER_M * mDataPtr->mTable[i][j][k] << ", ";
            }

            if (k == 1)
            {
               aOutStream << UtMath::TempKtoF(mDataPtr->mTable[i][j][k]) << ", ";
            }
         }
         aOutStream << std::endl;
      }
   }
   aOutStream << std::endl;
   aOutStream << std::endl;
}

// ============================================================================
std::string UtAtmosphere::AtmosphereTypeName() const
{
   return AtmosphereTypeName(GetAtmosphereIndex());
}

// ============================================================================
// protected static
std::string UtAtmosphere::AtmosphereTypeName(int aAtmTypeIndex)
{
   if (aAtmTypeIndex == -1)
   {
      return "default";
   }
   else if (aAtmTypeIndex == 0)
   {
      return "standard_day";
   }
   else if (aAtmTypeIndex == 1)
   {
      return "hot_day";
   }
   else if (aAtmTypeIndex == 2)
   {
      return "tropical_day";
   }
   else if (aAtmTypeIndex == 3)
   {
      return "cold_day";
   }
   else if (aAtmTypeIndex == 4)
   {
      return "polar_day";
   }
   else if (aAtmTypeIndex == 5)
   {
      return "simple_dt";
   }
   else if (aAtmTypeIndex == 6)
   {
      return "custom";
   }
   return "unknown";
}

int UtAtmosphere::GetAtmosphereIndex() const
{
   return mSelectedAtmosphereType != cDEFAULT ? mSelectedAtmosphereType : mDataPtr->mDefaultAtmosphere;
}

//! Create a new atmosphere object that is bound to a new atmosphere table.
//! This should only be called once for most applications.  The constructor should be preferred.
UtAtmosphere UtAtmosphere::CreateNewAtmosphereTable()
{
   UtAtmosphereData* dataPtr = new UtAtmosphereData;
   UtAtmosphere      at(dataPtr, cSTANDARD_DAY);
   dataPtr->Unref();
   return at;
}

void UtAtmosphere::AddRefP(UtAtmosphereData* aDataPtr)
{
   aDataPtr->AddRef();
}
