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

#include "WsfMIT_ClutterStrength.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>

#include "TblLookup.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"


// =================================================================================================
// Various constants from ALARM.
// =================================================================================================

namespace
{
// ==============================================================================================
// The following is from ALARM 'physical_constants.f90.
// ==============================================================================================

//--------------------------------------------------------------------
// From CRC Standard Mathematical Tables and Formulae, 30th edition,
// Copyright 1996.
//--------------------------------------------------------------------
const double pi = 3.141592653589793; // pi

//--------------------------------------------------------------------
// NATIONAL IMAGERY AND MAPPING AGENCY TECHNICAL REPORT 8350.2 Third
// Edition
//--------------------------------------------------------------------
const double rezero = 6371008.7714; // m : mean radius of semi-axis of earth

//--------------------------------------------------------------------
// From CRC Standard Mathematical Tables and Formulae, 30th edition,
// Copyright 1996.
//--------------------------------------------------------------------
const double vlight = 299792458.0; // m/s : speed of light

// from 19th CRC Standard Mathematical Tables
const double sqrt2 = 1.41421356237309500; // square root of 2

//--------------------------------------------------------------------
// Derived constants
//--------------------------------------------------------------------
const double twopi  = 2.0 * pi;
const double fourpi = 4.0 * pi;
const double fpicub = 64.0 * pi * pi * pi;
const double halfpi = 0.5 * pi;

//--------------------------------------------------------------------
// Misc unit conversions.
//--------------------------------------------------------------------
const double deg2rad = pi / 180.0; // degrees to radians
const double rad2deg = 180.0 / pi; // radians to degrees

} // namespace

// static
WsfMIT_ClutterStrength::Table WsfMIT_ClutterStrength::sTables[cTERRAIN_TYPE_SIZE];
bool                          WsfMIT_ClutterStrength::sTablesLoaded = false;

//! Computes sea clutter backscatter coefficient.
//!
//! The coefficient is computed as a function of radar frequency (FMhz) and
//! radar beam grazing angle (psi) for several sea environments and radar
//! polarizations -sigc
//!
//! Basic sea clutter routine taken from NISC P001/ADRAD Model.
//! Original data from Nathanson pgs 231-239.
//! Data voids filled by E.M. Schultz, E245 MDAC-STL

// Was seaclut
// static
double WsfMIT_ClutterStrength::GetSeaClutterStrength(double                    aPsi, // grazing angle
                                                     WsfEnvironment::SeaState  aSeaState,
                                                     double                    aFrequency,
                                                     WsfEM_Types::Polarization aPolarization)
{
   using SeaStateTable = const double[9][8];
   int iy;
   int ix;
   int k;
   int i;
   // old s1
   static const SeaStateTable seaState0Horizontal = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                     {0.0017453, -62.0, -80.0, -87.0, -90.0, -97.0, -99.0, -103.0},
                                                     {0.0052359, -56.0, -74.0, -79.0, -83.0, -95.0, -96.0, -97.0},
                                                     {0.0174532, -49.0, -66.0, -70.0, -73.0, -80.0, -86.0, -90.0},
                                                     {0.0523598, -48.0, -58.0, -63.0, -68.0, -72.0, -75.0, -79.0},
                                                     {0.1745329, -41.0, -56.0, -67.0, -67.0, -60.0, -62.0, -66.0},
                                                     {0.5235987, -32.0, -57.0, -62.0, -52.0, -50.0, -49.0, -53.0},
                                                     {1.0471975, -22.0, -34.0, -33.0, -32.0, -32.0, -32.0, -36.0},
                                                     {1.5707962, +10.0, +10.0, +10.0, +10.0, +10.0, +10.0, +10.0}};

   // old s8
   static const SeaStateTable seaState0Vertical = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                   {0.0017453, -61.0, -79.0, -86.0, -90.0, -93.0, -100.0, -110.0},
                                                   {0.0052359, -55.0, -70.0, -75.0, -79.0, -83.0, -90.0, -95.0},
                                                   {0.0174532, -48.0, -60.0, -63.0, -65.0, -68.0, -73.0, -78.0},
                                                   {0.0523598, -47.0, -56.0, -60.0, -57.0, -55.0, -50.0, -50.0},
                                                   {0.1745329, -40.0, -49.0, -49.0, -47.0, -45.0, -45.0, -45.0},
                                                   {0.5235987, -31.0, -42.0, -42.0, -42.0, -42.0, -42.0, -42.0},
                                                   {1.0471975, -21.0, -33.0, -35.0, -34.0, -33.0, -32.0, -32.0},
                                                   {1.5707962, +10.0, +10.0, +10.0, +10.0, +10.0, +10.0, +10.0}};

   // old s2
   static const SeaStateTable seaState1Horizontal = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                     {0.0017453, -58.0, -71.0, -75.0, -80.0, -90.0, -97.0, -101.0},
                                                     {0.0052359, -52.0, -66.0, -71.0, -74.0, -87.0, -91.0, -95.0},
                                                     {0.0174532, -45.0, -51.0, -56.0, -65.0, -73.0, -84.0, -88.0},
                                                     {0.0523598, -45.0, -48.0, -54.0, -59.0, -62.0, -70.0, -74.0},
                                                     {0.1745329, -38.0, -51.0, -53.0, -58.0, -56.0, -57.0, -61.0},
                                                     {0.5235987, -30.0, -39.0, -48.0, -44.0, -46.0, -45.0, -49.0},
                                                     {1.0471975, -20.0, -26.0, -26.0, -25.0, -24.0, -22.0, -26.0},
                                                     {1.5707962, -16.0, -11.0, -2.0, -5.0, -10.0, -11.0, -15.0}};

   // old s9
   static const SeaStateTable seaState1Vertical = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                   {0.0017453, -57.0, -65.0, -67.0, -75.0, -90.0, -95.0, -100.0},
                                                   {0.0052359, -51.0, -58.0, -60.0, -62.0, -78.0, -79.0, -90.0},
                                                   {0.0174532, -44.0, -50.0, -53.0, -56.0, -65.0, -70.0, -72.0},
                                                   {0.0523598, -43.0, -45.0, -49.0, -52.0, -53.0, -60.0, -62.0},
                                                   {0.1745329, -37.0, -42.0, -44.0, -48.0, -47.0, -38.0, -50.0},
                                                   {0.5235987, -29.0, -39.0, -40.0, -40.0, -38.0, -38.0, -40.0},
                                                   {1.0471975, -18.0, -26.0, -26.0, -24.0, -22.0, -23.0, -24.0},
                                                   {1.5707962, -15.0, -10.0, -9.5, -7.0, -5.5, -9.0, -9.0}};

   // old s3
   static const SeaStateTable seaState2Horizontal = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                     {0.0017453, -54.0, -61.0, -67.0, -75.0, -90.0, -95.0, -99.0},
                                                     {0.0052359, -48.0, -56.0, -60.0, -66.0, -78.0, -89.0, -93.0},
                                                     {0.0174532, -41.0, -46.0, -48.0, -55.0, -65.0, -82.0, -86.0},
                                                     {0.0523598, -41.0, -42.0, -48.0, -53.0, -59.0, -66.0, -70.0},
                                                     {0.1745329, -37.0, -43.0, -48.0, -51.0, -53.0, -54.0, -57.0},
                                                     {0.5235987, -34.0, -44.0, -42.0, -40.0, -41.0, -42.0, -46.0},
                                                     {1.0471975, -19.0, -23.0, -22.0, -23.0, -21.0, -22.0, -26.0},
                                                     {1.5707962, -15.0, -11.0, -1.0, -4.0, -9.0, -11.0, -15.0}};

   // old s10
   static const SeaStateTable seaState2Vertical = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                   {0.0017453, -52.0, -56.0, -64.0, -72.0, -87.0, -90.0, -95.0},
                                                   {0.0052359, -47.0, -52.0, -55.0, -59.0, -73.0, -80.0, -85.0},
                                                   {0.0174532, -40.0, -44.0, -47.0, -53.0, -58.0, -63.0, -65.0},
                                                   {0.0523598, -39.0, -41.0, -45.0, -49.0, -53.0, -55.0, -57.0},
                                                   {0.1745329, -34.0, -38.0, -39.0, -38.0, -37.0, -35.0, -33.0},
                                                   {0.5235987, -31.0, -34.0, -34.0, -32.0, -31.0, -30.0, -28.0},
                                                   {1.0471975, -18.0, -20.0, -23.0, -21.0, -21.0, -20.0, -18.0},
                                                   {1.5707962, -1.0, -9.0, -12.0, -5.5, -12.0, -12.0, -12.0}};

   // old s4
   static const SeaStateTable seaState3Horizontal = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                     {0.0017453, -50.0, -53.0, -60.0, -68.0, -82.0, -90.0, -94.0},
                                                     {0.0052359, -44.0, -46.0, -50.0, -58.0, -72.0, -82.0, -86.0},
                                                     {0.0174532, -37.0, -40.0, -43.0, -48.0, -60.0, -76.0, -80.0},
                                                     {0.0523598, -37.0, -39.0, -42.0, -46.0, -55.0, -61.0, -65.0},
                                                     {0.1745329, -33.0, -37.0, -40.0, -46.0, -48.0, -50.0, -54.0},
                                                     {0.5235987, -27.0, -34.0, -37.0, -38.0, -39.0, -40.0, -44.0},
                                                     {1.0471975, -17.0, -21.0, -20.0, -22.0, -20.0, -21.0, -25.0},
                                                     {1.5707962, -13.0, -11.0, -1.0, -4.0, -8.7, -11.0, -15.0}};

   // old s11
   static const SeaStateTable seaState3Vertical = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                   {0.0017453, -37.0, -51.0, -56.0, -60.0, -65.0, -75.0, -80.0},
                                                   {0.0052359, -37.0, -45.0, -48.0, -55.0, -62.0, -69.0, -72.0},
                                                   {0.0174532, -37.0, -39.0, -43.0, -48.0, -54.0, -58.0, -60.0},
                                                   {0.0523598, -36.0, -38.0, -40.0, -43.0, -43.0, -43.0, -44.0},
                                                   {0.1745329, -32.0, -32.0, -34.0, -34.0, -34.0, -34.0, -34.0},
                                                   {0.5235987, -23.0, -28.0, -28.0, -29.0, -30.0, -28.0, -28.0},
                                                   {1.0471975, -14.0, -17.0, -18.0, -19.0, -18.0, -18.0, -18.0},
                                                   {1.5707962, -5.0, -5.5, -9.0, -10.0, -5.5, -5.0, -6.0}};

   // old s5
   static const SeaStateTable seaState4Horizontal = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                     {0.0017453, -47.0, -48.0, -55.0, -58.0, -73.0, -88.0, -92.0},
                                                     {0.0052359, -39.0, -42.0, -45.0, -50.0, -63.0, -78.0, -82.0},
                                                     {0.0174532, -34.0, -36.0, -39.0, -42.0, -52.0, -69.0, -76.0},
                                                     {0.0523598, -34.0, -35.0, -38.0, -41.0, -48.0, -54.0, -61.0},
                                                     {0.1745329, -31.0, -34.0, -36.0, -41.0, -45.0, -48.0, -52.0},
                                                     {0.5235987, -24.0, -33.0, -35.0, -37.0, -37.0, -39.0, -43.0},
                                                     {1.0471975, -15.0, -20.0, -15.0, -21.0, -18.0, -21.0, -25.0},
                                                     {1.5707962, -11.0, -11.0, 0.0, -2.0, -8.0, -11.0, -15.0}};

   // old s12
   static const SeaStateTable seaState4Vertical = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                   {0.0017453, -36.0, -48.0, -53.0, -59.0, -60.0, -69.0, -72.0},
                                                   {0.0052359, -35.0, -43.0, -49.0, -54.0, -57.0, -64.0, -66.0},
                                                   {0.0174532, -34.0, -37.0, -39.0, -42.0, -45.0, -55.0, -58.0},
                                                   {0.0523598, -33.0, -35.0, -36.0, -38.0, -38.0, -38.0, -38.0},
                                                   {0.1745329, -29.0, -31.0, -32.0, -31.0, -31.0, -32.0, -31.0},
                                                   {0.5235987, -24.0, -24.0, -25.0, -27.0, -28.0, -29.0, -30.0},
                                                   {1.0471975, -11.0, -14.0, -15.0, -19.0, -15.0, -14.0, -13.0},
                                                   {1.5707962, -1.0, -2.0, -3.0, -3.0, -3.0, 0.0, -3.0}};
   // old s6
   static const SeaStateTable seaState5Horizontal = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                     {0.0017453, -44.0, -46.0, -48.0, -53.0, -65.0, -84.0, -88.0},
                                                     {0.0052359, -39.0, -39.0, -41.0, -44.0, -58.0, -75.0, -79.0},
                                                     {0.0174532, -31.0, -33.0, -35.0, -42.0, -50.0, -65.0, -69.0},
                                                     {0.0523598, -30.0, -32.0, -34.0, -37.0, -46.0, -53.0, -57.0},
                                                     {0.1745329, -29.0, -33.0, -36.0, -38.0, -43.0, -46.0, -50.0},
                                                     {0.5235987, -22.0, -26.0, -30.0, -32.0, -34.0, -35.0, -42.0},
                                                     {1.0471975, -8.0, -14.0, -17.0, -17.0, -18.0, -21.0, -24.0},
                                                     {1.5707962, -9.0, -11.0, +1.0, -1.0, -8.0, -11.0, -15.0}};
   // old s13
   static const SeaStateTable seaState5Vertical = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                   {0.0017453, -34.0, -44.0, -46.0, -58.0, -62.0, -68.0, -71.0},
                                                   {0.0052359, -32.0, -39.0, -42.0, -50.0, -56.0, -63.0, -65.0},
                                                   {0.0174532, -31.0, -33.0, -35.0, -38.0, -43.0, -53.0, -57.0},
                                                   {0.0523598, -31.0, -31.0, -33.0, -35.0, -38.0, -38.0, -37.0},
                                                   {0.1745329, -26.0, -28.0, -28.0, -28.0, -30.0, -30.0, -30.0},
                                                   {0.5235987, -21.0, -22.0, -23.0, -23.0, -24.0, -28.0, -18.0},
                                                   {1.0471975, -6.0, -13.0, -15.0, -15.0, -15.0, -18.0, -9.0},
                                                   {1.5707962, 5.0, 0.0, -3.0, -3.0, -16., -3.0, -3.0}};

   // old s7
   static const SeaStateTable seaState6Horizontal = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                     {0.0017453, -39.0, -41.0, -47.0, -52.0, -64.0, -83.0, -87.0},
                                                     {0.0052359, -30.0, -38.0, -40.0, -43.0, -57.0, -74.0, -78.0},
                                                     {0.0174532, -29.0, -32.0, -34.0, -41.0, -49.0, -64.0, -68.0},
                                                     {0.0523598, -29.0, -31.0, -33.0, -36.0, -45.0, -52.0, -56.0},
                                                     {0.1745329, -24.0, -29.0, -29.0, -35.0, -37.0, -37.0, -37.0},
                                                     {0.5235987, -20.0, -18.0, -26.0, -29.0, -31.0, -29.0, -26.0},
                                                     {1.0471975, -03.0, -06.0, -09.0, -08.0, -18.0, -19.0, -12.0},
                                                     {1.5707962, -9.0, -00.0, +2.0, -0.0, -6.0, -00.0, -03.0}};

   // old s14
   static const SeaStateTable seaState6Vertical = {{809.0, 17000.0, 9300.0, 5600.0, 3000.0, 1250.0, 500.0, 200.0},
                                                   {0.0017453, -29.0, -41.0, -41.0, -57.0, -59.0, -67.0, -70.0},
                                                   {0.0052359, -30.0, -35.0, -35.0, -47.0, -55.0, -62.0, -64.0},
                                                   {0.0174532, -29.0, -30.0, -31.0, -33.0, -41.0, -52.0, -56.0},
                                                   {0.0523598, -29.0, -29.0, -30.0, -31.0, -37.0, -37.0, -36.0},
                                                   {0.1745329, -24.0, -24.0, -24.0, -25.0, -25.0, -20.0, -22.0},
                                                   {0.5235987, -20.0, -20.0, -20.0, -21.0, -21.0, -19.0, -16.0},
                                                   {1.0471975, -3.0, -08.0, -8.0, -7.0, -16.0, -10.0, -10.0},
                                                   {1.5707962, 9.0, 0.0, -3.0, -2.0, -12.0, 0.0, -3.0}};

   static const SeaStateTable s15                  = {{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};
   static const SeaStateTable s16                  = {{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                                     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};
   SeaStateTable*             seaStateTablePtr[16] = {&seaState0Horizontal,
                                          &seaState1Horizontal,
                                          &seaState2Horizontal,
                                          &seaState3Horizontal,
                                          &seaState4Horizontal,
                                          &seaState5Horizontal,
                                          &seaState6Horizontal,
                                          &seaState0Vertical,
                                          &seaState1Vertical,
                                          &seaState2Vertical,
                                          &seaState3Vertical,
                                          &seaState4Vertical,
                                          &seaState5Vertical,
                                          &seaState6Vertical,
                                          &s15,
                                          &s16};

   // Determine correct data array
   int tableNumber = aSeaState;
   if (aPolarization == WsfEM_Types::cPOL_VERTICAL) // mjm, this is the original form from salram
   {
      tableNumber += 7;
   }

   // PGM=NU6(CGSM) ver. 1, 05-12-71. L.D. Gregory 3-53300. Fortran IV. EBCD
   // Three dimensional storage of 2-way tables
   // inx and iny = -1, 0, 1 according to if <, ==, > tabulated interval
   // double interpolation from 2-dimension bordered table seaStatePtr(L, M)
   // border values must increase.
   // if not in tabulated interval, uses nearest value.
   // kn = table no. in 3-dimensional array.

   double xx = aPsi;
   double yy = aFrequency * 1.0E-6;
   int    im = static_cast<int>((*seaStateTablePtr[tableNumber])[0][0] + 0.1);
   int    in = im % 100;
   im        = im / 100;
   double ym = (*seaStateTablePtr[tableNumber])[0][im - 1];
   double xn = (*seaStateTablePtr[tableNumber])[in - 1][0];
   if ((yy - ym) < 0.0)
   {
      yy = ym;
      iy = im;
   }
   if ((xx - xn) > 0.0)
   {
      xx = xn;
      ix = in;
   }
   double ys = (*seaStateTablePtr[tableNumber])[0][1];
   double xs = (*seaStateTablePtr[tableNumber])[1][0];
   if ((yy - ys) > 0.0)
   {
      yy = ys;
      iy = 3;
   }
   if ((xx - xs) < 0.0)
   {
      xx = xs;
      ix = 3;
   }

   i = 2;
   for (k = 3; i <= im; ++k)
   {
      ++i;
      if (((*seaStateTablePtr[tableNumber])[0][i - 1] - yy) <= 0.0)
      {
         break;
      }
   }
   iy        = i - 1; // Adjust index for 0-based array
   double yu = (*seaStateTablePtr[tableNumber])[0][iy];
   double yl = (*seaStateTablePtr[tableNumber])[0][iy - 1];

   i = 2;
   for (k = 3; k <= in; ++k)
   {
      ++i;
   }
   ix        = i - 1; // Adjust index for 0-based array
   double xu = (*seaStateTablePtr[tableNumber])[ix][0];
   double xl = (*seaStateTablePtr[tableNumber])[ix - 1][0];

   double ax  = (xu - xx) / (xu - xl);
   double ay  = (yu - yy) / (yu - yl);
   double zll = (*seaStateTablePtr[tableNumber])[ix - 1][iy - 1];
   double zlu = (*seaStateTablePtr[tableNumber])[ix][iy - 1];
   double zul = (*seaStateTablePtr[tableNumber])[ix - 1][iy];
   double zuu = (*seaStateTablePtr[tableNumber])[ix][iy];
   double zl  = (ax * zll) + ((1.0 - ax) * zlu);
   double zu  = (ax * zul) + ((1.0 - ax) * zuu);

   // Clutter backscatter coefficient -sigc

   double sigc = (ay * zl) + ((1.0 - ay) * zu);
   return sigc;
}

//! Computes the land clutter signal for various terrain types.
//!
//! Data based on The book "Low-Angle Radar Land Clutter", by
//! J. Barrie Billingsley.  This is the most extensive possible(?)
//! representation of the MIT-LL data.
// static
double WsfMIT_ClutterStrength::GetLandClutterStrength(double        aDepressionAngleRad,
                                                      TerrainType   aTerrainType,
                                                      FrequencyBand aFrequencyBand,
                                                      Polarization  aPolarization)
{
   Table& table = sTables[aTerrainType];
   return table.GetClutterStrength(aDepressionAngleRad, aFrequencyBand, aPolarization);
}

//! Computes the land clutter signal for various terrain types.
double WsfMIT_ClutterStrength::GetLandClutterStrength(double                        aDepressionAngleRad,
                                                      WsfEnvironment::LandFormation aLandFormation,
                                                      WsfEnvironment::LandCover     aLandCover,
                                                      double                        aFrequency,
                                                      WsfEM_Types::Polarization     aPolarization)
{
   if (aLandCover != WsfEnvironment::cWATER)
   {
      // Land Cover and LandForm Classifications

      // aLandCover = 0,  General                   ------ General
      // aLandCover = 1,  Urban                     ------ Urban
      // aLandCover = 2,  Agricultural              ------ Open Farmland
      // aLandCover = 3,  Rangeland-Herbaceous      ------ Grassland
      // aLandCover = 4,  RangeLand-Shurb and Mixed ------ Grassland
      // aLandCover = 5,  Forest-Decidous           ------ Continuous Forest
      // aLandCover = 6,  Forest-Coniferous         ------ Continuous Forest
      // aLandCover = 7,  Forest-Mixed              ------ Continuous Forest
      // aLandCover = 8,  Forest-Clear Cut          ------ Continuous Forest
      // aLandCover = 9,  Forest-Block Cut          ------ Continuous Forest
      // aLandCover = 10, WetLand-Forested          ------ Marsh
      // aLandCover = 11, WetLand-Non-forested      ------ Marsh
      // aLandCover = 12, Barren                    ------ Desert

      // aLandFormation = 1, Level                      ------ Low Relief
      // aLandFormation = 2, Inclined                   ------ Low Relief
      // aLandFormation = 3, Undulating                 ------ Low Relief
      // aLandFormation = 4, Rolling                    ------ Low Relief
      // aLandFormation = 5, Hummocky                   ------ High Relief
      // aLandFormation = 6, Ridged                     ------ High Relief
      // aLandFormation = 7, Moderately Steep           ------ High Relief
      // aLandFormation = 8, Steep                      ------ Mountains
      // aLandFormation = 9, Broken                     ------ Mountains

      // Set size of arrays for lookup of Weibull mean clutter strength
      // as a function of frequency (MHz) and Depression Angle (Deg)

      // Convert land form land cover
      TerrainType terrainType = GetTerrainType(aLandFormation, aLandCover);

      // Convert frequency to frequency Band.
      FrequencyBand frequencyBand = GetFrequencyBand(aFrequency);

      Polarization polarization = GetPolarization(aPolarization);

      // Get the clutter strength.
      return GetLandClutterStrength(aDepressionAngleRad, terrainType, frequencyBand, polarization);
   }
   else // If the land cover is "water", use the sea state calculations instead.
   {
      return GetSeaClutterStrength(-aDepressionAngleRad,          // grazing angle, for low altitude / short range
                                   WsfEnvironment::cCALM_RIPPLED, // up to 0.1 m variation
                                   aFrequency,
                                   aPolarization);
   }
}

//! Computes the land clutter signal for various terrain types.
//!
//! Data based on MIT Lincoln Lab's report on radar propagation at low
//! altitude and tabulated radar ground clutter, and from Nathanson page 272.
//! Data voids filled by J.A. Hueseman, E245 MDAC-STL. Radar ground clutter
//! data  as a function of depression angle and frequency was input by Ray
//! Perry in Nov of 1986.

// Was covfor
// static
double WsfMIT_ClutterStrength::GetLandClutterStrengthLegacy(double                        aDepressionAngleRad,
                                                            WsfEnvironment::LandFormation aLandFormation,
                                                            WsfEnvironment::LandCover     aLandCover,
                                                            double                        aFrequency,
                                                            WsfEM_Types::Polarization     aPolarization)
{
   if (aLandCover != WsfEnvironment::cWATER)
   {
      double sigmaDB = 0.0;
      // Land Cover and LandForm Classifications

      // aLandCover = 0,  General                   ------ General
      // aLandCover = 1,  Urban                     ------ Urban
      // aLandCover = 2,  Agricultural              ------ Open Farmland
      // aLandCover = 3,  Rangeland-Herbaceous      ------ Grassland
      // aLandCover = 4,  RangeLand-Shurb and Mixed ------ Grassland
      // aLandCover = 5,  Forest-Decidous           ------ Continuous Forest
      // aLandCover = 6,  Forest-Coniferous         ------ Continuous Forest
      // aLandCover = 7,  Forest-Mixed              ------ Continuous Forest
      // aLandCover = 8,  Forest-Clear Cut          ------ Continuous Forest
      // aLandCover = 9,  Forest-Block Cut          ------ Continuous Forest
      // aLandCover = 10, WetLand-Forested          ------ Marsh
      // aLandCover = 11, WetLand-Non-forested      ------ Marsh
      // aLandCover = 12, Barren                    ------ Desert

      // aLandFormation = 1, Level                      ------ Low Relief
      // aLandFormation = 2, Inclined                   ------ Low Relief
      // aLandFormation = 3, Undulating                 ------ Low Relief
      // aLandFormation = 4, Rolling                    ------ Low Relief
      // aLandFormation = 5, Hummocky                   ------ High Relief
      // aLandFormation = 6, Ridged                     ------ High Relief
      // aLandFormation = 7, Moderately Steep           ------ High Relief
      // aLandFormation = 8, Steep                      ------ Mountains
      // aLandFormation = 9, Broken                     ------ Mountains

      // Set size of arrays for lookup of Weibull mean clutter strength
      // as a function of frequency (MHz) and Depression Angle (Deg)

      // fill xtreelo array with 5 frequencies(MHz) and 3 depression angles(deg)
      static const double ntreelo[2] = {5.0, 3.0};
      static const double xtreelo[8] = {165.0, 435.0, 1230.0, 3240.0, 9200.0, 0.15, 0.65, 1.0};
      // fill xweedlo array with 5 frequencies(MHz) and 3 depression angles(deg)
      static const double nweedlo[2] = {5.0, 3.0};
      static const double xweedlo[8] = {165.0, 435.0, 1230.0, 3240.0, 9200.0, 0.125, 0.5, 0.75};
      // fill xfarmlo array with 5 frequencies(MHz) and 2 depression angles(deg)
      static const double nfarmlo[2] = {5.0, 2.0};
      static const double xfarmlo[7] = {165.0, 435.0, 1230.0, 3240.0, 9200.0, 0.2, 0.575};
      // fill xgenlo array with 5 frequencies (MHz) and 5 depression angles (Deg)
      static const double ngenlo[2]  = {5.0, 5.0};
      static const double xgenlo[10] = {165.0, 435.0, 1230.0, 3240.0, 9200.0, 0.125, 0.5, 1.125, 2.25, 4.0};
      // fill xgenhi array with 5 frequencies(MHz) and 4 depression angles(deg)
      static const double ngenhi[2] = {5.0, 4.0};
      static const double xgenhi[9] = {165.0, 435.0, 1230.0, 3240.0, 9200.0, 1.0, 3.0, 5.0, 6.0};
      // fill xrockies array with 5 frequencies(MHz) and 2 depression angles(deg)
      static const double nrockies[2] = {5.0, 2.0};
      static const double xrockies[7] = {165.0, 435.0, 1230.0, 3240.0, 9200.0, 1.0, 2.0};
      // fill xtreehi array with 5 frequencies(MHz) and 2 depression angles(deg)
      static const double ntreehi[2] = {5.0, 2.0};
      static const double xtreehi[7] = {165.0, 435.0, 1230.0, 3240.0, 9200.0, 1.0, 2.0};
      // fill xurblo array with 5 frequencies(MHz) and 2 depression angles(deg)
      static const double nurblo[2] = {5.0, 2.0};
      static const double xurblo[7] = {165.0, 435.0, 1230.0, 3240.0, 9200.0, 0.0, 0.25};

      // fill treelo array with sigmadb values at each (FMhz, depression angle) point
      //         FMhz:       165      435      1230     3240     9200
      static const double treelo[15] = {-45.000,
                                        -42.000,
                                        -40.000,
                                        -39.000,
                                        -37.000, // 0.15
                                        -30.000,
                                        -30.000,
                                        -30.000,
                                        -30.000,
                                        -30.000, // 0.65
                                        -15.000,
                                        -19.000,
                                        -22.000,
                                        -24.000,
                                        -26.000}; // 1.0Deg

      // fill weedlo array with sigmadb values at each (FMhz, depression angle) point
      //           FMhz:     165     435     1230    3240    9200
      static const double weedlo[15] = {-68.000,
                                        -74.000,
                                        -68.000,
                                        -51.000,
                                        -42.000, // 0.125
                                        -56.000,
                                        -58.000,
                                        -46.000,
                                        -41.000,
                                        -36.000, // 0.5
                                        -38.000,
                                        -40.000,
                                        -40.000,
                                        -38.000,
                                        -26.000}; // 0.75deg

      // fill farmlo array with sigmadb values at each (FMhz, depression angle) point
      //           FMhz:     165     435     1230    3240    9200
      static const double farmlo[10] = {-51.000,
                                        -39.000,
                                        -30.000,
                                        -30.000,
                                        -30.000, // 0.2
                                        -30.000,
                                        -30.000,
                                        -30.000,
                                        -30.000,
                                        -30.000}; // 0.675deg

      // fill genlo array with sigmadb values at each (FMhz, Depression angle) point
      //          FMhz:     165      435      1230     3240     9200
      static const double genlo[25] = {-33.000, -33.000, -33.000, -33.000, -33.000,  // 0.125
                                       -32.000, -32.000, -32.000, -32.000, -32.000,  // 0.5
                                       -30.000, -30.000, -30.000, -30.000, -30.000,  // 1.125
                                       -27.000, -27.000, -27.000, -27.000, -27.000,  // 2.25
                                       -25.000, -25.000, -25.000, -25.000, -25.000}; // 4.0Deg

      // fill genhi array with sigmadb values at each (FMhz, depression angle) point
      //          FMhz:     165     435     1230    3240    9200
      static const double genhi[20] = {-27.000, -27.000, -27.000, -27.000, -27.000,  // 1.0
                                       -24.000, -24.000, -24.000, -24.000, -24.000,  // 3.0
                                       -21.000, -21.000, -21.000, -21.000, -21.000,  // 5.0
                                       -19.000, -19.000, -19.000, -19.000, -19.000}; // 6.0deg

      // fill rockies array with sigmadb values at each (FMhz, depression angle) point
      //            FMhz:    165      435     1230    3240    9200
      static const double rockies[10] = {-8.0000,
                                         -11.000,
                                         -18.000,
                                         -20.000,
                                         -20.000, // 1.0
                                         -8.0000,
                                         -11.000,
                                         -18.000,
                                         -20.000,
                                         -20.000}; // 2.0deg

      // fill treehi array with sigmadb values at each (FMhz, depression angle) point
      //           FMhz:     165     435     1230    3240    9200
      static const double treehi[10] = {-15.000,
                                        -19.000,
                                        -22.000,
                                        -22.000,
                                        -22.000, // 1.0
                                        -15.000,
                                        -19.000,
                                        -22.000,
                                        -22.000,
                                        -22.000}; // 2.0deg

      // fill urblo array with sigmadb values at each (FMhz, depression angle) point
      //          FMhz:     165     435     1230    3240    9200
      static const double urblo[10] = {-32.000,
                                       -24.000,
                                       -15.000,
                                       -10.000,
                                       -10.000, // 0.0
                                       -32.000,
                                       -24.000,
                                       -15.000,
                                       -10.000,
                                       -10.000}; // 0.25deg

      double xr[2] = {aFrequency * 1.0E-6, rad2deg * aDepressionAngleRad};
      double zr[2];
      int    ie;

      /*
      // Note: This snippet is copied from the original salram clutter.
      // It was decided to remove this logic due to the fact that it is
      // only valid for bald earth calculations (not with terrain).
      double rHill = 4123.0 * sqrt(antennaHeight);
      if ((aRange - rHill) > 0.0)
      {
         // Diffracted clutter disabled by Ray Perry on 91-9-4.
         // Set diffracted clutter backscatter coefficient to almost zero(-180 db).
         sigmaDB = -180.0;
      }
      */

      int f  = 3;
      int nl = 0;
      int l  = 1;
      if (aLandCover == 0)
      {
         if ((aLandFormation == 1) || (aLandFormation == 2) || (aLandFormation == 3) || (aLandFormation == 4))
         {
            // General rural low-relief land
            NDimensionalTableLookUp(f, ngenlo, xgenlo, genlo, xr, zr, ie, nl, l);
            sigmaDB = zr[0];
         }
         else if ((aLandFormation == 5) || (aLandFormation == 6) || (aLandFormation == 7))
         {
            // General high-relief land
            NDimensionalTableLookUp(f, ngenhi, xgenhi, genhi, xr, zr, ie, nl, l);
            sigmaDB = zr[0];
         }
         else if ((aLandFormation == 8) || (aLandFormation == 9))
         {
            // Mountainous hi-relief land
            NDimensionalTableLookUp(f, nrockies, xrockies, rockies, xr, zr, ie, nl, l);
            sigmaDB = zr[0];
         }
         else
         {
            ut::log::fatal() << "Both LANCOV and LANFOR can not be general.";
            exit(1);
         }
      }
      else if (aLandCover == 1)
      {
         // Urban landcover, either general or low-relief landform
         if (aLandFormation > 4)
         {
            sigmaDB = -20.0;
         }
         else
         {
            NDimensionalTableLookUp(f, nurblo, xurblo, urblo, xr, zr, ie, nl, l);
            sigmaDB = zr[0];
         }
      }
      else if (aLandCover == 2)
      {
         // Farmland over rural low-relief land
         NDimensionalTableLookUp(f, nfarmlo, xfarmlo, farmlo, xr, zr, ie, nl, l);
         sigmaDB = zr[0];
      }
      else if ((aLandCover == 3) || (aLandCover == 4) || ((aLandCover > 9) && (aLandCover < 13)))
      {
         // Desert, Marsh or Grassland over rural low-relief land
         NDimensionalTableLookUp(f, nweedlo, xweedlo, weedlo, xr, zr, ie, nl, l);
         sigmaDB = zr[0];
      }
      else if ((aLandCover > 4) && (aLandCover < 10))
      {
         // continuous forest over either rural low-relief or hi-relief land
         if (aLandFormation < 5)
         {
            NDimensionalTableLookUp(f, ntreelo, xtreelo, treelo, xr, zr, ie, nl, l);
            sigmaDB = zr[0];
         }
         else
         {
            NDimensionalTableLookUp(f, ntreehi, xtreehi, treehi, xr, zr, ie, nl, l);
            sigmaDB = zr[0];
         }
      }
      else
      {
         ut::log::fatal() << "Both LANCOV and LANFOR cannot be General.";
         exit(1);
      }

      return sigmaDB;
   }
   else // If the land cover is "water", use the sea state calculations instead.
   {
      return GetSeaClutterStrength(-aDepressionAngleRad,          // grazing angle, for low altitude / short range
                                   WsfEnvironment::cCALM_RIPPLED, // up to 0.1 m variation
                                   aFrequency,
                                   aPolarization);
   }
}

// private static
void WsfMIT_ClutterStrength::NDimensionalTableLookUp(int          aNumDimensions,
                                                     const double aNumTableValues[],
                                                     const double aTables[],
                                                     const double aFunctionValues[],
                                                     double       aIndependantVar[],
                                                     double       aResult[],
                                                     int&         aErrorCode,
                                                     int          aExtrapolated,
                                                     int          aNumCoefficients)
{
   double ratio;
   int    l1 = 2;
   int    lf;
   int    k;
   int    mw;
   int    iz;
   int    npt;
   int    n;
   int    iup = aNumDimensions - 1;
   int    l;
   int    j;
   int    i;
   int    mm;
   int    m;
   int    intm;
   double ns[5];
   double wj[32];
   double nzb[6];
   aErrorCode = 0;
   nzb[0]     = 1;
   for (l = 0; l < iup; ++l)
   {
      nzb[l + 1] = aNumTableValues[l] * nzb[l];
   }
   lf = aNumDimensions - 1;
   for (i = 0; i < lf; ++i)
   {
      int    l2    = l1 + static_cast<int>(aNumTableValues[i]) - 2;
      double found = 0.0;
      for (j = l1 - 1; j < l2; ++j)
      {
         if (aTables[j] <= aTables[j - 1])
         {
            aErrorCode = 2;
            return;
         }
         if (found != 0.0)
         {
            continue;
         }
         if ((aIndependantVar[i] - aTables[j - 1]) < 0)
         {
            if (j > (l1 - 1))
            {
               found = 1.0;
               ns[i] = j - 1;
            }
            else
            {
               if (aExtrapolated == 0)
               {
                  aErrorCode = -1;
               }
               found = 1.0;
               ns[i] = l1 - 1;
            }
         }
      }
      if (found == 0.0)
      {
         if ((aIndependantVar[i] - aTables[l2 - 1]) > 0.0)
         {
            if (aExtrapolated == 0)
            {
               aErrorCode = 1;
            }
         }
         ns[i] = l2 - 1;
      }
      l1 = l2 + 2;
   }
   for (k = 0; k < aNumCoefficients; ++k)
   {
      int icoef = static_cast<int>(nzb[iup]) * k;
      int kf    = 1 << lf; // 2^lf
      mw        = -2;
      for (i = 0; i < kf; i += 2)
      {
         l1 = 0;
         iz = 0;
         mw += 2;
         npt = 1;
         for (j = 0; j < lf; ++j)
         {
            mm   = 1 << j; // 2^j
            n    = static_cast<int>(ns[j]);
            intm = (mm & mw);
            if (intm != 0)
            {
               n = n + 1;
            }
            n   = n - l1;
            l1  = l1 + static_cast<int>(aNumTableValues[j]);
            iz  = (npt * (n - 1)) + iz;
            npt = npt * static_cast<int>(aNumTableValues[j]);
         }
         wj[i]     = aFunctionValues[iz + icoef];
         wj[i + 1] = aFunctionValues[iz + 1 + icoef];
      }
      for (i = 0; i < lf; ++i)
      {
         m  = static_cast<int>(ns[i]);
         kf = kf / 2;
         if (aExtrapolated == 0)
         {
            if (aIndependantVar[i] > aTables[m])
            {
               aIndependantVar[i] = aTables[m];
            }
            if (aIndependantVar[i] < aTables[m - 1])
            {
               aIndependantVar[i] = aTables[m - 1];
            }
         }
         ratio = (aIndependantVar[i] - aTables[m - 1]) / (aTables[m] - aTables[m - 1]);
         for (j = 0; j < kf; ++j)
         {
            wj[j] = wj[(2 * (j + 1)) - 1 - 1] + (wj[(2 * (j + 1)) - 1] - wj[(2 * (j + 1)) - 1 - 1]) * ratio;
         }
         wj[j] = wj[(2 * (j + 1)) - 1 - 1] + (wj[(2 * (j + 1)) - 1] - wj[(2 * (j + 1)) - 1 - 1]) * ratio;
      }
      aResult[k] = wj[kf - 1];
   }
}

//! Core method to retrieve the clutter strength.  It uses the table lookup library to
//! interpolate based on depression angle.
double WsfMIT_ClutterStrength::Table::GetClutterStrength(double        aDepressionAngle,
                                                         FrequencyBand aFrequencyBand,
                                                         Polarization  aPolarization)
{
   if (!sTablesLoaded)
   {
      LoadTables();
   }
   TblLookupLU<float> lookup;
   float              depressionAngle = (float)(aDepressionAngle * UtMath::cDEG_PER_RAD); // table lookup is in degrees
   lookup.Lookup(mDepressionAngleValues[aFrequencyBand][aPolarization], depressionAngle);
   float clutterStrength = TblEvaluate(mClutterStrength[aFrequencyBand][aPolarization], lookup);
   return (double)clutterStrength;
}

//! Return the frequency band associated with the given frequency, for purposes of clutter lookup.
//! Note: Frequencies above X-band still return X-band.
WsfMIT_ClutterStrength::FrequencyBand WsfMIT_ClutterStrength::GetFrequencyBand(double aFrequency)
{
   FrequencyBand frequencyBand;
   if (aFrequency < 300e+6) // 300MHz or less
   {
      frequencyBand = cVHF;
   }
   else if (aFrequency < 1000e+6) // 1000 MHz or less
   {
      frequencyBand = cUHF;
   }
   else if (aFrequency < 2.0e9) // 2 GHz or less
   {
      frequencyBand = cL_BAND;
   }
   else if (aFrequency < 6.0e9) // 6 GHz or less
   {
      frequencyBand = cS_BAND;
   }
   else // < 12 GHz
   {
      frequencyBand = cX_BAND;
   }

   return frequencyBand;
}

//! A trivial method to convert a verical polarization flag to a polarization enum.
WsfMIT_ClutterStrength::Polarization WsfMIT_ClutterStrength::GetPolarization(WsfEM_Types::Polarization aPolarization)
{
   return ((aPolarization == WsfEM_Types::cPOL_VERTICAL) ? cVV : cHH);
}

//! Given a land form / land cover pair, as defined in WsfEnvironment.hpp,
//! return the corresponding terrain type (each terrain type has its own
//! clutter strength table.
WsfMIT_ClutterStrength::TerrainType WsfMIT_ClutterStrength::GetTerrainType(WsfEnvironment::LandFormation aLandForm,
                                                                           WsfEnvironment::LandCover     aLandCover)
{
   TerrainType terrainType = cLOW_RELIEF_GENERAL_MIXED_RURAL; // 16
   if ((aLandForm < WsfEnvironment::cNUM_LAND_FORMS) && (aLandCover < WsfEnvironment::cNUM_LAND_COVERS))
   {
      static unsigned char sTerrainTypeMap[WsfEnvironment::cNUM_LAND_COVERS][WsfEnvironment::cNUM_LAND_FORMS] = //[LAND
                                                                                                                // COVER][LAND FORM]
         {
            {16, 16, 16, 16, 15, 16, 13, 13, 13, 16}, // 0
            {16, 17, 17, 17, 17, 17, 17, 17, 17, 17}, // 1
            {16, 7, 5, 5, 6, 5, 6, 6, 6, 5},          // 2
            {16, 11, 11, 11, 10, 11, 10, 10, 10, 11}, // 3
            {16, 9, 9, 9, 8, 9, 8, 8, 8, 9},          // 4
            {16, 3, 3, 3, 4, 3, 4, 4, 4, 3},          // 5
            {16, 3, 3, 3, 4, 3, 4, 4, 4, 3},          // 6
            {16, 3, 3, 3, 4, 3, 4, 4, 4, 3},          // 7
            {16, 16, 16, 16, 15, 16, 15, 15, 15, 16}, // 8
            {16, 16, 16, 16, 15, 16, 15, 15, 15, 16}, // 9
            {16, 12, 12, 12, 12, 12, 12, 12, 12, 12}, // 10
            {16, 12, 12, 12, 12, 12, 12, 12, 12, 12}, // 11
            {16, 2, 0, 0, 1, 0, 1, 1, 1, 0},          // 12
            {16, 12, 12, 12, 12, 12, 12, 12, 12, 12}  // 13
         };
      // 0     1     2     3     4     5     6     7     8     9

      terrainType = (TerrainType)sTerrainTypeMap[aLandCover][aLandForm];
   }
   return terrainType;
}

namespace
{
// raw data tables
// These are the tabulated data from Billingsley (Low Angle Radar Clutter)
// The first number in each column is a depression angle.
// The Following numbers are clutter strength (sigma zero * F^4) for the
// Various Frequencies and Polarizations, as follows.
// where frequencies and polarizations are enumerated in the header:
// DA,F0P0,F0P1,F1P0,F1P1,F2P0,F2P1,F3P0,F3P1,F4P0,F4P1
float sLOW_RELIEF_DESERT[6 * 11] = {2.75f,  -35.8f, -35.6f, -37.7f, -35.5f,  -34.6f, -38.1f, -38.0f, -37.1f, -19.4f,
                                    -22.1f, 1.125f, -35.3f, -34.5f, -36.7f,  -36.6f, -34.1f, -35.5f, -32.4f, -33.5f,
                                    -22.5f, -22.3f, 0.5f,   -40.8f, -40.6f,  -40.4f, -46.3f, -36.9f, -41.2f, -38.5f,
                                    -36.3f, -33.3f, -33.7f, 0.125f, -42.3f,  -36.5f, -45.0f, -45.1f, -38.5f, -40.0f,
                                    -39.0f, -38.0f, -32.9f, -34.3f, -0.125f, -29.7f, -31.0f, -32.6f, -34.9f, -32.0f,
                                    -34.0f, -30.4f, -27.6f, -28.1f, -27.0f,  -0.5f,  -34.8f, -28.7f, -37.2f, -40.5f,
                                    -32.8f, -24.0f, -37.8f, -38.3f, -30.9f,  -35.1f};

float sHIGH_RELIEF_DESERT[6 * 11] = {5.0f,    -45.8f,  -42.9f,  -54.2f,  -47.4f, -40.7f, -41.7f, -999.0f, -999.0f,
                                     -999.0f, -999.0f, 3.0f,    -36.6f,  -34.6f, -30.0f, -29.6f, -29.1f,  -30.9f,
                                     -42.4f,  -39.8f,  -999.0f, -999.0f, 1.5f,   -37.0f, -32.0f, -32.7f,  -32.2f,
                                     -30.7f,  -32.3f,  -38.7f,  -37.3f,  -23.9f, -22.9f, 0.5f,   -25.5f,  -29.3f,
                                     -28.4f,  -28.0f,  -26.9f,  -28.4f,  -28.3f, -27.9f, -22.9f, -22.8f,  -0.5f,
                                     -24.4f,  -27.2f,  -24.7f,  -26.2f,  -24.6f, -25.7f, -24.5f, -23.0f,  -24.5f,
                                     -24.8f,  -1.0f,   -26.6f,  -28.1f,  -22.3f, -27.5f, -24.8f, -28.9f,  -24.4f,
                                     -21.4f,  -25.0f,  -23.0f};

float sLEVEL_DESERT[6 * 11] = {2.75f,  -37.6f, -39.2f, -38.5f, -36.2f,  -35.6f, -39.2f,  -38.0f, -37.2f, -23.8f,
                               -23.5f, 1.125f, -35.9f, -34.6f, -33.8f,  -31.9f, -34.1f,  -35.1f, -31.8f, -26.5f,
                               -21.2f, -21.3f, 0.5f,   -51.3f, -54.7f,  -54.9f, -60.7f,  -45.1f, -51.9f, -41.4f,
                               -42.3f, -35.3f, -35.5f, 0.125f, -52.7f,  -53.4f, -57.2f,  -54.0f, -53.6f, -54.2f,
                               -42.1f, -40.9f, -33.0f, -33.3f, -0.125f, -43.8f, -999.0f, -33.5f, -35.6f, -35.1f,
                               -36.1f, -33.9f, -33.5f, -27.9f, -25.6f,  -0.5f,  -999.0f, -32.2f, -35.1f, -38.1f,
                               -32.8f, -24.0f, -36.8f, -38.3f, -29.0f,  -31.1f};

float sLOW_RELIEF_FOREST[7 * 11] = {4.0f,    -19.2f, -20.7f, -26.0f, -24.2f, -24.1f,  -25.0f, -999.0f, -999.0f, -999.0f,
                                    -999.0f, 2.75f,  -22.4f, -22.7f, -25.4f, -21.5f,  -25.8f, -26.2f,  -32.3f,  -30.7f,
                                    -25.1f,  -23.8f, 1.125f, -20.6f, -22.9f, -26.0f,  -24.1f, -26.7f,  -25.5f,  -32.6f,
                                    -30.5f,  -27.7f, -26.8f, 0.5f,   -28.0f, -27.1f,  -29.6f, -26.1f,  -28.5f,  -28.2f,
                                    -32.6f,  -30.6f, -30.3f, -28.9f, 0.125f, -34.4f,  -35.4f, -37.4f,  -34.8f,  -32.4f,
                                    -31.8f,  -35.5f, -33.3f, -32.7f, -32.3f, -0.125f, -27.6f, -23.7f,  -35.0f,  -34.9f,
                                    -29.6f,  -29.8f, -35.5f, -33.1f, -31.9f, -30.7f,  -0.5f,  -34.9f,  -32.1f,  -22.6f,
                                    -24.7f,  -29.1f, -31.2f, -32.7f, -28.6f, -30.7f,  -29.1f};

float sHIGH_RELIEF_FOREST[7 * 11] = {6.0f,    -17.4f,  -13.5f,  -18.7f,  -16.9f,  -999.0f, -999.0f, -999.0f, -999.0f,
                                     -999.0f, -999.0f, 5.0f,    -17.7f,  -13.4f,  -21.3f,  -21.2f,  -23.3f,  -23.6f,
                                     -999.0f, -999.0f, -999.0f, -999.0f, 3.0f,    -20.1f,  -17.3f,  -25.2f,  -22.1f,
                                     -26.2f,  -24.3f,  -33.4f,  -31.3f,  -999.0f, -999.0f, 1.5f,    -17.3f,  -15.0f,
                                     -22.6f,  -20.1f,  -24.2f,  -24.5f,  -28.9f,  -27.2f,  -23.2f,  -22.7f,  0.5f,
                                     -20.6f,  -19.3f,  -25.1f,  -22.9f,  -26.7f,  -25.9f,  -29.5f,  -27.8f,  -27.1f,
                                     -26.6f,  -0.5f,   -23.8f,  -16.8f,  -19.8f,  -17.1f,  -22.8f,  -22.2f,  -28.9f,
                                     -27.0f,  -27.3f,  -27.2f,  -1.0f,   -14.5f,  -5.5f,   -8.2f,   -7.3f,   -14.9f,
                                     -14.3f,  -999.0f, -999.0f, -22.6f,  -24.5f};

float sHIGH_RELIEF_AGRICULTURAL[4 * 11] = {3.0f,    -36.0f,  -34.4f, -26.1f, -22.8f, -31.3f, -26.7f, -999.0f, -999.0f,
                                           -999.0f, -999.0f, 1.5f,   -50.2f, -46.1f, -33.4f, -35.9f, -29.0f,  -25.3f,
                                           -46.8f,  -43.8f,  -38.9f, -34.6f, 0.5f,   -35.9f, -31.9f, -32.2f,  -30.4f,
                                           -31.4f,  -31.9f,  -32.5f, -30.3f, -31.3f, -29.2f, -0.5f,  -19.9f,  -14.1f,
                                           -34.5f,  -30.7f,  -27.7f, -28.6f, -34.8f, -32.8f, -33.6f, -32.0f};

float sLOW_RELIEF_AGRICULTURAL[6 * 11] = {2.75f,  -39.8f, -41.0f, -35.4f, -32.4f, -32.2f, -30.5f, -37.0f, -37.4f,
                                          -32.2f, -31.4f, 1.125f, -39.3f, -39.3f, -33.3f, -32.2f, -29.1f, -31.1f,
                                          -34.0f, -32.1f, -32.3f, -30.2f, 0.5f,   -41.9f, -41.2f, -36.3f, -34.1f,
                                          -30.3f, -31.2f, -32.0f, -30.4f, -31.5f, -29.4f, 0.125f, -39.5f, -40.8f,
                                          -42.7f, -41.1f, -32.6f, -32.6f, -32.0f, -30.8f, -31.5f, -29.8f, -0.125f,
                                          -37.8f, -41.2f, -37.9f, -39.5f, -32.8f, -32.8f, -32.7f, -31.6f, -31.9f,
                                          -30.3f, -0.5f,  -33.7f, -46.9f, -43.9f, -43.4f, -31.9f, -32.4f, -34.6f,
                                          -32.5f, -34.7f, -31.9f};

float sLEVEL_AGRICULTURAL[4 * 11] = {4.0f,    -999.0f, -999.0f, -999.0f, -999.0f, -999.0f, -999.0f, -999.0f, -999.0f,
                                     -37.1f,  -34.2f,  2.75f,   -53.2f,  -57.6f,  -49.6f,  -49.6f,  -39.8f,  -39.8f,
                                     -42.1f,  -41.4f,  -35.0f,  -31.5f,  1.125f,  -58.8f,  -59.1f,  -53.5f,  -48.0f,
                                     -37.4f,  -40.3f,  -34.3f,  -33.3f,  -31.4f,  -30.0f,  -0.125f, -999.0f, -999.0f,
                                     -999.0f, -999.0f, -48.4f,  -48.9f,  -35.8f,  -35.1f,  -34.4f,  -33.0f};

float sLOW_RELIEF_GENERAL_MIXED_RURAL[8 * 11] = {
   4.0f,   -17.0f, -18.5f, -22.5f,  -21.7f, -23.2f, -24.8f,  -999.0f, -999.0f, -999.0f, -999.0f, 2.75f,  -20.5f,
   -18.7f, -21.9f, -20.4f, -23.7f,  -23.6f, -31.8f, -28.6f,  -24.5f,  -22.5f,  1.125f,  -26.7f,  -25.4f, -26.0f,
   -23.7f, -26.7f, -26.5f, -31.5f,  -29.5f, -28.4f, -26.9f,  0.5f,    -35.8f,  -35.9f,  -32.7f,  -31.2f, -29.6f,
   -29.6f, -32.9f, -31.1f, -30.5f,  -29.4f, 0.125f, -40.7f,  -40.7f,  -38.4f,  -38.1f,  -32.8f,  -32.1f, -35.3f,
   -33.4f, -32.3f, -30.6f, -0.125f, -37.5f, -39.7f, -36.6f,  -35.4f,  -33.3f,  -34.2f,  -34.1f,  -32.9f, -32.3f,
   -30.6f, -0.5f,  -41.3f, -39.4f,  -32.2f, -31.7f, -30.5f,  -31.6f,  -31.2f,  -28.4f,  -31.8f,  -29.8f, -0.75f,
   -31.2f, -25.3f, -22.3f, -19.9f,  -22.6f, -18.8f, -999.0f, -999.0f, -26.6f,  -28.4f};

float sHIGH_RELIEF_GENERAL_MIXED_RURAL[6 * 11] = {
   5.0f,   -17.3f, -14.3f, -22.9f, -19.4f, -22.1f, -23.0f, -999.0f, -999.0f, -999.0f, -999.0f, 3.0f,   -19.4f, -15.3f,
   -21.0f, -20.4f, -23.0f, -23.5f, -28.7f, -27.1f, -25.9f, -22.5f,  1.5f,    -24.0f,  -20.6f,  -24.7f, -22.8f, -26.0f,
   -26.0f, -30.3f, -28.2f, -25.2f, -24.0f, 0.5f,   -27.6f, -25.1f,  -29.0f,  -27.9f,  -29.0f,  -29.0f, -31.7f, -29.9f,
   -29.5f, -28.0f, -0.5f,  -26.5f, -21.7f, -27.0f, -25.8f, -26.6f,  -27.7f,  -30.3f,  -28.3f,  -29.6f, -28.5f, -1.0f,
   -16.6f, -17.0f, -19.8f, -16.4f, -23.1f, -21.0f, -24.6f, -23.6f,  -24.9f,  -25.9f};

float sGENERAL_URBAN[5 * 11] = {2.75f,  -16.3f, -18.3f, -17.5f, -16.3f,  -18.4f, -18.5f,  -20.8f, -19.0f, -22.7f,
                                -17.0f, 1.125f, -20.6f, -16.7f, -17.1f,  -15.1f, -16.4f,  -17.1f, -20.7f, -15.4f,
                                -20.0f, -15.2f, 0.5f,   -33.9f, -33.9f,  -28.9f, -28.7f,  -25.9f, -26.3f, -22.7f,
                                -20.8f, -19.5f, -16.0f, 0.125f, -35.3f,  -36.5f, -30.3f,  -28.4f, -14.9f, -18.1f,
                                -18.2f, -18.6f, -16.0f, -12.9f, -0.125f, -42.6f, -999.0f, -40.0f, -37.6f, -35.0f,
                                -37.9f, -37.1f, -34.9f, -31.2f, -29.3f};

float sLOW_RELIEF_SHRUBLAND[5 * 11] = {2.75f,  -29.1f, -26.5f, -28.9f, -29.3f,  -28.8f,  -29.8f, -36.5f, -33.5f, -23.8f,
                                       -22.4f, 1.125f, -34.1f, -36.1f, -34.8f,  -43.1f,  -43.4f, -40.3f, -39.8f, -35.0f,
                                       -26.9f, -25.0f, 0.5f,   -49.9f, -41.5f,  -39.4f,  -43.9f, -36.3f, -34.8f, -38.2f,
                                       -34.9f, -31.1f, -29.6f, 0.125f, -40.3f,  -44.5f,  -54.0f, -52.2f, -45.1f, -44.8f,
                                       -42.9f, -39.5f, -34.0f, -32.7f, -0.125f, -999.0f, 36.6f,  -47.2f, -50.2f, -40.3f,
                                       -40.9f, -36.7f, -32.7f, -33.1f, -32.0f};

float sHIGH_RELIEF_SHRUBLAND[3 * 11] = {1.5f,   -32.8f, -31.4f, -33.0f, -35.6f, -30.9f, -36.1f, -36.3f, -35.1f,
                                        -27.7f, -25.4f, 0.5f,   -36.3f, -28.0f, -29.4f, -28.8f, -31.8f, -33.5f,
                                        -34.0f, -31.8f, -28.3f, -26.5f, -0.5f,  -32.9f, -23.1f, -41.1f, -37.8f,
                                        -35.7f, -36.0f, -34.3f, -33.8f, -33.6f, -31.3f};

float sLOW_RELIEF_GRASSLAND[6 * 11] = {2.75f,   -63.3f, -41.7f, -55.2f, -51.5f, -40.7f, -39.5f, -37.3f, -36.2f,
                                       -39.1f,  -36.8f, 1.125f, -52.9f, -54.6f, -47.5f, -47.6f, -40.7f, -42.4f,
                                       -41.4f,  -41.4f, -36.8f, -35.1f, 0.5f,   -57.9f, -54.5f, -41.9f, -40.8f,
                                       -37.3f,  -37.0f, -41.3f, -39.5f, -35.4f, -34.1f, 0.125f, -57.7f, -60.3f,
                                       -45.3f,  -43.2f, -41.8f, -42.6f, -40.8f, -39.9f, -37.2f, -36.2f, -0.125f,
                                       -60.7f,  -48.8f, -45.3f, -42.5f, -43.1f, -39.9f, -40.3f, -35.9f, -32.8f,
                                       -33.7f,  -0.5f,  -15.8f, -33.5f, -26.9f, -22.4f, -35.2f, -39.5f, -999.0f,
                                       -999.0f, -27.5f, -28.7f};

float sHIGH_RELIEF_GRASSLAND[4 * 11] = {3.0f,    -32.8f,  -24.4f,  -999.0f, -999.0f, -29.3f, -29.7f, -38.0f, -33.3f,
                                        -999.0f, -999.0f, 1.5f,    -31.2f,  -26.3f,  -23.3f, -22.6f, -31.3f, -32.2f,
                                        -35.0f,  -32.9f,  -999.0f, -999.0f, 0.5f,    -52.8f, -50.4f, -46.6f, -41.5f,
                                        -33.7f,  -34.0f,  -28.4f,  -27.2f,  -26.1f,  -25.0f, -0.5f,  -53.4f, -53.6f,
                                        -38.8f,  -44.3f,  -33.3f,  -37.2f,  -31.9f,  -27.3f, -32.9f, -30.6f};

float sWETLAND[2 * 11] = {0.5f,   -63.5f, -70.1f, -68.5f, -68.2f, -56.3f, -55.8f, -42.2f, -41.0f, -39.0f, -37.4f,
                          0.125f, -67.3f, -66.9f, -72.4f, -68.9f, -61.9f, -60.4f, -49.6f, -49.5f, -39.5f, -39.0f};

float sMOUNTAINOUS[7 * 11] = {6.0f,  -31.1f, -31.8f, -31.3f, -28.0f, -27.6f, -28.3f, -999.0f, -999.0f, -999.0f, -999.0f,
                              5.0f,  -20.7f, -18.4f, -17.3f, -16.5f, -21.2f, -24.3f, -999.0f, -999.0f, -999.0f, -999.0f,
                              3.0f,  -16.4f, -13.2f, -17.7f, -12.8f, -18.2f, -18.0f, -24.4f,  -24.7f,  -999.0f, -999.0f,
                              1.5f,  -14.7f, -10.8f, -15.1f, -12.4f, -17.6f, -17.5f, -22.9f,  -24.4f,  -999.0f, -999.0f,
                              0.5f,  -14.8f, -14.0f, -18.8f, -16.2f, -20.0f, -19.7f, -22.4f,  -22.2f,  -999.0f, -999.0f,
                              -0.5f, -15.4f, -5.8f,  -23.1f, -20.6f, -25.6f, -27.4f, -25.2f,  -25.5f,  -26.1f,  -25.6f,
                              -1.0f, -11.2f, -7.6f,  -13.9f, -13.3f, -17.8f, -18.7f, -19.2f,  -19.4f,  -21.1f,  -22.0f};

float sLONG_RANGE_MOUNTAINOUS[1 * 11] = {-1.175f, -16.7f, -17.5f, -19.6f, -21.2f, -21.6f, -21.7f, -23.9f, -23.4f, -23.0f, -22.4f};
}; // namespace

//! Load a table from the set of raw tables.
//! Note: The depression angle values are used for each column,
//! as some freq/polarization combinations may not have a complete set of entries,
//! and we want to be able to correctly interpolate in these cases (rather than use 0).
void WsfMIT_ClutterStrength::Table::Load(const float* aRawTable, unsigned short aNumRows)
{
   for (short rowNum = aNumRows - 1; rowNum >= 0; --rowNum) // Data must be monotonically increasing
   {
      short rowStartIndex = rowNum * 11;

      // loop over polarizations
      for (unsigned short polarization = 0; polarization < cPOLARIZATION_SIZE; ++polarization)
      {
         for (unsigned short frequency = 0; frequency < cFREQUENCY_SIZE; ++frequency)
         {
            float              clutterValue     = aRawTable[rowStartIndex + frequency * 2 + polarization + 1];
            TblIndVarU<float>& depressionAngles = mDepressionAngleValues[frequency][polarization];
            TblDepVar1<float>& clutterStrength  = mClutterStrength[frequency][polarization];
            if (clutterValue > -998.0) // using -999 as a blank entry in the table
            {
               size_t size = depressionAngles.GetSize();
               depressionAngles.Resize(size + 1);
               depressionAngles.Set(aRawTable[rowStartIndex], size);
               clutterStrength.Resize(size + 1);
               clutterStrength.Set(clutterValue, size);
            }
         }
      }
   }
}

// static
//! Load all tables in the Billingsley table set.
void WsfMIT_ClutterStrength::LoadTables()
{
   unsigned char sNumRows[cTERRAIN_TYPE_SIZE] = {6, 6, 6, 7, 7, 6, 4, 4, 3, 5, 4, 6, 2, 7, 1, 6, 8, 5};
   float*        sRawData[cTERRAIN_TYPE_SIZE] = {sLOW_RELIEF_DESERT,
                                          sHIGH_RELIEF_DESERT,
                                          sLEVEL_DESERT,
                                          sLOW_RELIEF_FOREST,
                                          sHIGH_RELIEF_FOREST,
                                          sLOW_RELIEF_AGRICULTURAL,
                                          sHIGH_RELIEF_AGRICULTURAL,
                                          sLEVEL_AGRICULTURAL,
                                          sHIGH_RELIEF_SHRUBLAND,
                                          sLOW_RELIEF_SHRUBLAND,
                                          sHIGH_RELIEF_GRASSLAND,
                                          sLOW_RELIEF_GRASSLAND,
                                          sWETLAND,
                                          sMOUNTAINOUS,
                                          sLONG_RANGE_MOUNTAINOUS,
                                          sHIGH_RELIEF_GENERAL_MIXED_RURAL,
                                          sLOW_RELIEF_GENERAL_MIXED_RURAL,
                                          sGENERAL_URBAN};

   for (unsigned short tableNum = 0; tableNum < cTERRAIN_TYPE_SIZE; ++tableNum)
   {
      float*         rawTable = sRawData[tableNum];
      unsigned short numRows  = sNumRows[tableNum];
      sTables[tableNum].Load(rawTable, numRows);
   }

   sTablesLoaded = true;
}
