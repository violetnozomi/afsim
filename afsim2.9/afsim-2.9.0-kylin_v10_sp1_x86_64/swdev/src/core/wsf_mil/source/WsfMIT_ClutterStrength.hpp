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

#ifndef WSFMIT_CLUTTERSTRENGTH_HPP
#define WSFMIT_CLUTTERSTRENGTH_HPP

#include "wsf_mil_export.h"

#include <string>
#include <vector>

#include "TblLookup.hpp"
#include "WsfEM_Types.hpp"
#include "WsfEnvironment.hpp"

//! This class represents two sets of clutter data tables and the methods to
//! perform table lookups, both of which derive from experiments performed
//! by MIT / Lincoln Labs.  The first of these come from the SALRAM clutter
//! model, whose data derive from various open sources.  The SALRAM data set
//! contain the only sea clutter data available.  The second source of data
//! are for land clutter only and originate from The book Low Angle Radar Clutter,
//! by J. Barrie Billingsley.  As SALRAM land clutter data are now assumed
//! to be superseded by the Billingsley data, lookups for these surface clutter
//! values are performed with the method GetLandClutterStrengthLegacy.
class WSF_MIL_EXPORT WsfMIT_ClutterStrength
{
public:
   //! Define terrain types supported in clutter table lookups.
   //! These are not the same as enumerated in WsfEnvironment,
   //! but those landform/landcover designations are mapped to
   //! the corresponding type (which has its own table).
   enum TerrainType
   {
      cLOW_RELIEF_DESERT               = 0,
      cHIGH_RELIEF_DESERT              = 1,
      cLEVEL_DESERT                    = 2,
      cLOW_RELIEF_FOREST               = 3,
      cHIGH_RELIEF_FOREST              = 4,
      cLOW_RELIEF_AGRICULTURAL         = 5,
      cHIGH_RELIEF_AGRICULTURAL        = 6,
      cLEVEL_AGRICULTURAL              = 7,
      cHIGH_RELIEF_SHRUBLAND           = 8,
      cLOW_RELIEF_SHRUBLAND            = 9,
      cHIGH_RELIEF_GRASSLAND           = 10,
      cLOW_RELIEF_GRASSLAND            = 11,
      cWETLAND                         = 12,
      cMOUNTAINOUS                     = 13,
      cLONG_RANGE_MOUNTAINOUS          = 14,
      cHIGH_RELIEF_GENERAL_MIXED_RURAL = 15,
      cLOW_RELIEF_GENERAL_MIXED_RURAL  = 16,
      cGENERAL_URBAN                   = 17,
      cTERRAIN_TYPE_SIZE               = 18
   };

   static TerrainType GetTerrainType(WsfEnvironment::LandFormation aLandForm, WsfEnvironment::LandCover aLandCover);

   //! Define frequency bands supported in clutter table lookups
   //! (Frequencies are not interpolated in lookups.)
   enum FrequencyBand
   {
      cVHF            = 0,
      cUHF            = 1,
      cL_BAND         = 2,
      cS_BAND         = 3,
      cX_BAND         = 4,
      cFREQUENCY_SIZE = 5
   };

   //! Define polarizations supported in clutter table lookups.
   //! (either Horizontal-Horizontal or Vertical-Vertical).
   enum Polarization
   {
      cHH                = 0,
      cVV                = 1,
      cPOLARIZATION_SIZE = 2
   };

   static FrequencyBand GetFrequencyBand(double aFrequency);

   // Polarizations defined in WsfEM_Types.

   static double GetLandClutterStrength(double                        aDepressionAngle,
                                        WsfEnvironment::LandFormation aLandFormation,
                                        WsfEnvironment::LandCover     aLandCover,
                                        double                        aFrequency,
                                        WsfEM_Types::Polarization     aPolarization);

   static double GetLandClutterStrength(double        aDepressionAngle,
                                        TerrainType   aTerrainType,
                                        FrequencyBand aFrequencyBand,
                                        Polarization  aPolarization);

   static double GetSeaClutterStrength(double                    aGrazingAngle,
                                       WsfEnvironment::SeaState  aSeaState,
                                       double                    aFrequency,
                                       WsfEM_Types::Polarization aPolarization);

   static double GetLandClutterStrengthLegacy(double                        aDepressionAngle,
                                              WsfEnvironment::LandFormation aLandFormation,
                                              WsfEnvironment::LandCover     aLandCover,
                                              double                        aFrequency,
                                              WsfEM_Types::Polarization     aPolarization);

protected:
private:
   static void NDimensionalTableLookUp(int          aNumDimensions,
                                       const double aNumTableValues[],
                                       const double aTables[],
                                       const double aFunctionValues[],
                                       double       aIndependantVar[],
                                       double       aResult[],
                                       int&         aErrorCode,
                                       int          aExtrapolated,
                                       int          aNumCoefficients);

   static Polarization GetPolarization(WsfEM_Types::Polarization aPolarization);

   class Table
   {
   public:
      Table() = default;
      ;
      ~Table() = default;
      ;

      double GetClutterStrength(double aDepressionAngle, FrequencyBand aFrequencyBand, Polarization aPolarization);

      void Load(const float* aRawTable, unsigned short aNumRows);

      TblIndVarU<float> mDepressionAngleValues[cFREQUENCY_SIZE][cPOLARIZATION_SIZE]; //<! Depression Angle, independent values
      TblDepVar1<float> mClutterStrength[cFREQUENCY_SIZE][cPOLARIZATION_SIZE];       //<! dependent values

   private:
   };

   static void LoadTables();

   static Table sTables[cTERRAIN_TYPE_SIZE];
   static bool  sTablesLoaded;
};

#endif
