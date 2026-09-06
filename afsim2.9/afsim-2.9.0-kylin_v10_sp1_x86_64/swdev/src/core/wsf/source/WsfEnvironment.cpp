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

#include "WsfEnvironment.hpp"

#include "UtCentralBody.hpp"
#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"
#include "WsfLandCover.hpp"
#include "WsfNoiseCloud.hpp"

WsfEnvironment::WsfEnvironment(WsfScenario& aScenario)
   : mLandCover(cGENERAL)
   , mLandFormation(cLEVEL)
   , mSeaState(cCALM_GLASSY)
   , mWindSpeed(0.0)
   , mWindDirection(0.0)
   , mCloudLowerAlt(0.0)
   , mCloudUpperAlt(0.0)
   , mCloudWaterDensity(0.0)
   , mRainRate(0.0)
   , mRainUpperAlt(0.0)
   , mDustStormVisibility(0.0)
   , mPolarOffsetAngleX(0.0)
   , mPolarOffsetAngleY(0.0)
   , mWindAltitudeTablePtr(nullptr)
   , mCentralPointPtr(ut::make_unique<ut::EarthWGS84>())
{
   mNoiseCloudTypes = new WsfNoiseCloudTypes(aScenario);
}

// =================================================================================================
//! Destructor
WsfEnvironment::~WsfEnvironment()
{
   delete mNoiseCloudTypes;
}

void WsfEnvironment::CreateWindAltitudeTable()
{
   if (mWindAltitudeTablePtr == nullptr)
   {
      mWindAltitudeTablePtr = new WindAltitudeTable();
   }
}

bool WsfEnvironment::UsingPolarOffsetAngles() const
{
   return ((mPolarOffsetAngleX != 0.0) || (mPolarOffsetAngleY != 0.0));
}

const ut::CentralBodyEllipsoid& WsfEnvironment::GetEllipsoid() const
{
   return GetCentralBody().GetEllipsoid();
}

void WsfEnvironment::GetPolarOffsetAngles(double& aPolarOffsetAngleX, double& aPolarOffsetAngleY) const
{
   aPolarOffsetAngleX = mPolarOffsetAngleX;
   aPolarOffsetAngleY = mPolarOffsetAngleY;
}

// =================================================================================================
//! See if the supplied command is one of mine.
//! @param aInput The input stream.
//! @return 'true' if the command was an environment related command or false' if not.
//! @see WsfObject::ProcessInput.
bool WsfEnvironment::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "global_environment")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "central_body")
         {
            std::string  centralBody;
            UtInputBlock block(aInput);
            block.ReadCommand(centralBody);
            mCentralPointPtr = ut::CentralBody::Create(centralBody);
            if (mCentralPointPtr == nullptr)
            {
               throw UtInput::BadValue(aInput, "Unrecognized central body type");
            }
            if (UsingPolarOffsetAngles())
            {
               // The polar offset angles were input before the central body.
               GetCentralBodyP().SetPolarOffsetAngles(mPolarOffsetAngleX, mPolarOffsetAngleY);
            }
            block.ProcessInput(mCentralPointPtr.get());
            if (GetCentralBody().UsingPolarMotion())
            {
               mPolarOffsetAngleX = GetCentralBody().GetPolarOffsetAngleX();
               mPolarOffsetAngleY = GetCentralBody().GetPolarOffsetAngleY();
            }
         }
         else if (command == "land_cover")
         {
            std::string coverType;
            aInput.ReadValue(coverType);
            if (coverType == "general")
            {
               mLandCover = cGENERAL;
            }
            else if (coverType == "urban")
            {
               mLandCover = cURBAN;
            }
            else if ((coverType == "agricultural") || (coverType == "farmland"))
            {
               mLandCover = cAGRICULTURAL;
            }
            else if ((coverType == "rangeland_herbaceous") || (coverType == "rangeland-herbaceous")) // deprecated
            {
               mLandCover = cRANGELAND_HERBACEOUS;
            }
            else if ((coverType == "rangeland_shrub") || (coverType == "rangeland-shrub")) // deprecated
            {
               mLandCover = cRANGELAND_SHRUB;
            }
            else if ((coverType == "forest_deciduous") || (coverType == "forest-deciduous")) // deprecated
            {
               mLandCover = cFOREST_DECIDUOUS;
            }
            else if ((coverType == "forest_coniferous") || (coverType == "forest-coniferous")) // deprecated
            {
               mLandCover = cFOREST_CONIFEROUS;
            }
            else if ((coverType == "forest_mixed") || (coverType == "forest-mixed")) // deprecated
            {
               mLandCover = cFOREST_MIXED;
            }
            else if ((coverType == "forest_clear_cut") || (coverType == "forest-clear_cut")) // deprecated
            {
               mLandCover = cFOREST_CLEARCUT;
            }
            else if ((coverType == "forest_block_cut") || (coverType == "forest-block_cut")) // deprecated
            {
               mLandCover = cFOREST_BLOCKCUT;
            }
            else if ((coverType == "wetland_forested") || (coverType == "wetland-forested")) // deprecated
            {
               mLandCover = cWETLAND_FORESTED;
            }
            else if ((coverType == "wetland_non_forested") || (coverType == "wetland-non_forested")) // deprecated
            {
               mLandCover = cWETLAND_NONFORESTED;
            }
            else if ((coverType == "desert") || (coverType == "barren"))
            {
               mLandCover = cBARREN;
            }
            else if (coverType == "water")
            {
               mLandCover = cWATER;
            }
            else
            {
               throw UtInput::BadValue(aInput, "Incorrect land cover type");
            }
         }
         else if (command == "land_formation")
         {
            std::string formationType;
            aInput.ReadValue(formationType);
            if (formationType == "level")
            {
               mLandFormation = cLEVEL;
            }
            else if (formationType == "inclined")
            {
               mLandFormation = cINCLINED;
            }
            else if (formationType == "undulating")
            {
               mLandFormation = cUNDULATING;
            }
            else if (formationType == "rolling")
            {
               mLandFormation = cROLLING;
            }
            else if (formationType == "hummocky")
            {
               mLandFormation = cHUMMOCKY;
            }
            else if (formationType == "ridged")
            {
               mLandFormation = cRIDGED;
            }
            else if (formationType == "moderately_steep")
            {
               mLandFormation = cMODERATELY_STEEP;
            }
            else if (formationType == "steep")
            {
               mLandFormation = cSTEEP;
            }
            else if (formationType == "broken")
            {
               mLandFormation = cBROKEN;
            }
            else
            {
               throw UtInput::BadValue(aInput, "Incorrect land formation type");
            }
         }
         else if (command == "sea_state")
         {
            int state;
            aInput.ReadValue(state);
            aInput.ValueInClosedRange(state, 0, 6);
            mSeaState = (SeaState)state;
         }
         else if (command == "wind_speed")
         {
            double aSpeed;
            aInput.ReadValueOfType(aSpeed, UtInput::cSPEED);
            aInput.ValueGreaterOrEqual(aSpeed, 0.0);
            mWindSpeed = aSpeed;
         }
         else if (command == "wind_direction")
         {
            double aAngle;
            aInput.ReadValueOfType(aAngle, UtInput::cANGLE);
            mWindDirection = aAngle;
         }
         else if (command == "wind_table")
         {
            std::string         cmd;
            std::vector<double> tempAltitudes;
            std::vector<double> tempDirections;
            std::vector<double> tempSpeeds;

            if (mWindAltitudeTablePtr == nullptr)
            {
               mWindAltitudeTablePtr = new WindAltitudeTable();
            }

            UtInputBlock windBlock(aInput);
            while (windBlock.ReadCommand(cmd))
            {
               aInput.PushBack(cmd);
               double lAltitude;
               double lDirection;
               double lSpeed;
               aInput.ReadValueOfType(lAltitude, UtInput::cLENGTH);
               aInput.ValueGreaterOrEqual(lAltitude, 0.0);
               tempAltitudes.push_back(lAltitude);

               // Allow +- cTWO_PI for input, but bound within +- cPI
               aInput.ReadValueOfType(lDirection, UtInput::cANGLE);
               aInput.ValueInClosedRange(lDirection, -UtMath::cTWO_PI, UtMath::cTWO_PI);
               if (lDirection > UtMath::cPI)
               {
                  lDirection -= UtMath::cTWO_PI;
               }
               if (lDirection < -UtMath::cPI)
               {
                  lDirection += UtMath::cTWO_PI;
               }
               tempDirections.push_back(lDirection);

               aInput.ReadValueOfType(lSpeed, UtInput::cSPEED);
               aInput.ValueGreaterOrEqual(lSpeed, 0.0);
               tempSpeeds.push_back(lSpeed);
            }
            mWindAltitudeTablePtr->mAltitude.Resize(static_cast<int>(tempAltitudes.size()));
            mWindAltitudeTablePtr->mWindDirection.Resize(static_cast<int>(tempDirections.size()));
            mWindAltitudeTablePtr->mWindSpeed.Resize(static_cast<int>(tempSpeeds.size()));
            // One loop to load; Should be exception by now if problem reading
            for (int i = 0; i < static_cast<int>(tempAltitudes.size()); ++i)
            {
               mWindAltitudeTablePtr->mAltitude.Set(tempAltitudes[i], i);
               mWindAltitudeTablePtr->mWindDirection.Set(tempDirections[i], i);
               mWindAltitudeTablePtr->mWindSpeed.Set(tempSpeeds[i], i);
            }
         }
         else if ((command == "cloud_altitude_limits") || (command == "cloud_limits")) // DEPRECATED
         {
            aInput.ReadValueOfType(mCloudLowerAlt, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mCloudLowerAlt, 0.0);
            aInput.ReadValueOfType(mCloudUpperAlt, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mCloudUpperAlt, mCloudLowerAlt);
         }
         else if (command == "cloud_water_density")
         {
            aInput.ReadValueOfType(mCloudWaterDensity, UtInput::cMASS_DENSITY);
            aInput.ValueGreaterOrEqual(mCloudWaterDensity, 0.0);
         }
         else if (command == "rain_altitude_limit")
         {
            aInput.ReadValueOfType(mRainUpperAlt, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mRainUpperAlt, 0.0);
         }
         else if (command == "rain_rate")
         {
            aInput.ReadValueOfType(mRainRate, UtInput::cSPEED);
            aInput.ValueGreaterOrEqual(mRainRate, 0.0);
         }
         else if (command == "dust_storm_visibility")
         {
            aInput.ReadValueOfType(mDustStormVisibility, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mDustStormVisibility, 0.0);
         }
         else if (command == "polar_offset_angles")
         {
            aInput.ReadValueOfType(mPolarOffsetAngleX, UtInput::cANGLE);
            aInput.ReadValueOfType(mPolarOffsetAngleY, UtInput::cANGLE);
            GetCentralBodyP().SetPolarOffsetAngles(mPolarOffsetAngleX, mPolarOffsetAngleY);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (mNoiseCloudTypes->ProcessInput(aInput))
   {
      myCommand = true;
      //       myCommand = true;
      //
      //       UtInputBlock block(aInput);
      //       // create the cloud
      //       std::string name;
      //       aInput.ReadCommand(name);
      //       WsfNoiseCloud* cloudPtr = new WsfNoiseCloud();
      //       cloudPtr->SetName(name);
      //
      //       block.ProcessInput(cloudPtr);
      //
      //       cloudPtr->Regenerate();
      //       mNoiseCloudList.push_back(cloudPtr);
      //       // add the cloud
   }
   return myCommand;
}

//! GeoSpecific land cover routine using WsfLandUse.
//! This is currently just a stub that returns the static land cover.
WsfEnvironment::LandCover WsfEnvironment::GetLandCover(double aLat, double aLon) const
{
   if (mGetLandCoverFunction)
   {
      return mGetLandCoverFunction(aLat, aLon);
   }
   else
   {
      return mLandCover;
   }
}

//! GeoSpecific land formation routine using WsfLandUse.
//! This is currently just a stub that returns the static land formation.
WsfEnvironment::LandFormation WsfEnvironment::GetLandFormation(double aLat, double aLon) const
{
   return mLandFormation;
}

//! Returns wind heading and speed in local NED relative to the lat/lon/alt provided.
//! aLat and aLon are  placeholders and are not used in the initial implementation.
//! aWindHeading is direction wind air is going 'to'. (Not direction coming 'from'.)
//! aWindHeading is radians, and Pi wind (180 deg) blows from north to south.
void WsfEnvironment::GetWind(double  aLat,
                             double  aLon,
                             double  aAlt,
                             double& aWindHeading, // 180 deg wind blows from north to south
                             double& aWindSpeed)
{
   if (mWindAltitudeTablePtr == nullptr)
   {
      // If no table, use single values. These default to zero if inputs are totally omitted.
      aWindHeading = mWindDirection;
      aWindSpeed   = mWindSpeed;
   }
   else
   {
      mWindAltitudeTablePtr->mAltLookup.Lookup(mWindAltitudeTablePtr->mAltitude, aAlt);
      aWindSpeed = TblEvaluate(mWindAltitudeTablePtr->mWindSpeed, mWindAltitudeTablePtr->mAltLookup);

      // We want the interpolation to go shortest direction.
      // So we can't just do a simple table evaluation, which gets
      // an absolute mid-value that would ignore the wrap around.
      // This section completed with V&V documentation for Government.
      // If modified, redo the V&V work too.
      size_t i1 = mWindAltitudeTablePtr->mAltLookup.GetIndex();
      double r1 = mWindAltitudeTablePtr->mAltLookup.GetRatio();
      double f0 = mWindAltitudeTablePtr->mWindDirection.Get(i1);
      double f1 = mWindAltitudeTablePtr->mWindDirection.Get(i1 + 1);

      if (fabs(f1 - f0) > UtMath::cPI)
      {
         // Because these angles are stored in range -cPI to +cPI, this branch
         // only happens when one value is negative and other is positive.
         // So whichever one is negative will be wrapped to positive.
         // This will make interpolation work in correct direction, but
         // we'll then need to unwrap any values that are over +cPI.
         if (f0 < 0.0)
         {
            f0 += UtMath::cTWO_PI;
         }
         else if (f1 < 0.0)
         {
            f1 += UtMath::cTWO_PI;
         }
      }

      aWindHeading = f0 + r1 * (f1 - f0);

      if (aWindHeading >= UtMath::cPI)
      {
         aWindHeading -= UtMath::cTWO_PI;
      }
   }
}


//! Constructor for the WindAltitudeTable
WsfEnvironment::WindAltitudeTable::WindAltitudeTable() {}

// =================================================================================================
//! Check for clear line of sight (in the context of Environmental cloud layers only).
//! @param aEmissionPointWCS Point of emission
//! @param aTargetPointWCS   Desired point of termination of the emission.
//! @return 'true' if the line-of-sight is clear of clouds.
bool WsfEnvironment::LOS_ClearOfClouds(const double aEmissionPointWCS[3], const double aTargetPointWCS[3]) const
{
   // Check for LOS passing through a cloud layer, either up or down.
   // Quick check for no cloud intervention.
   if (mCloudLowerAlt == 0.0 && mCloudUpperAlt == 0.0)
   {
      return true;
   }

   double LatA, LonA, AltA, LatB, LonB, AltB;
   UtEntity::ConvertWCSToLLA(aEmissionPointWCS, LatA, LonA, AltA);
   UtEntity::ConvertWCSToLLA(aTargetPointWCS, LatB, LonB, AltB);

   double highAlt = (AltA > AltB) ? AltA : AltB;
   double lowAlt  = (AltA < AltB) ? AltA : AltB;

   // If above or below all clouds, the LOS is clear.  TO-DO - This is not
   // strictly true, due to curvature of earth.  Ignored for now.
   if ((lowAlt >= mCloudUpperAlt) || (highAlt <= mCloudLowerAlt))
   {
      return true;
   }

   // LOS Blocked.  Determine cloud surface.
   double unitLOS_WCS[3];
   UtVec3d::Subtract(unitLOS_WCS, aTargetPointWCS, aEmissionPointWCS);

   if ((aTargetPointWCS[0] == aEmissionPointWCS[0]) && (aTargetPointWCS[1] == aEmissionPointWCS[1]) &&
       (aTargetPointWCS[2] == aEmissionPointWCS[2]))
   {
      // Null LOS vector.  Return clear LOS.
      return true;
   }

   return false;
}

// =================================================================================================
//! Check for clear line of sight (in the context of Environmental cloud layers only).
//! @param aEmissionPointWCS Point of emission
//! @param aTargetPointWCS   Desired point of termination of the emission.  Will be adjusted to
//!                          terminate upon the nearest cloud layer surface.
//!                          (If currently in clouds, will be placed at 10m range.)
//! @return 'true' if aTargetPointWCS had to be moved.
bool WsfEnvironment::LOS_AdjustedClearOfClouds(const double aEmissionPointWCS[3], double aTargetPointWCS[3]) const
{
   // Quick check for no cloud intervention.
   if (mCloudLowerAlt == 0.0 && mCloudUpperAlt == 0.0)
   {
      return false;
   }

   double LatA, LonA, AltA, LatB, LonB, AltB;
   UtEntity::ConvertWCSToLLA(aEmissionPointWCS, LatA, LonA, AltA);
   UtEntity::ConvertWCSToLLA(aTargetPointWCS, LatB, LonB, AltB);

   double highAlt = (AltA > AltB) ? AltA : AltB;
   double lowAlt  = (AltA < AltB) ? AltA : AltB;

   // If above or below all clouds, the LOS is clear.  TO-DO - This is not
   // strictly true, due to curvature of earth.  Ignored for now.
   if ((lowAlt >= mCloudUpperAlt) || (highAlt <= mCloudLowerAlt))
   {
      return false;
   }

   // LOS Blocked.  Determine cloud surface.
   double unitLOS_WCS[3];
   UtVec3d::Subtract(unitLOS_WCS, aTargetPointWCS, aEmissionPointWCS);

   if ((aTargetPointWCS[0] == aEmissionPointWCS[0]) && (aTargetPointWCS[1] == aEmissionPointWCS[1]) &&
       (aTargetPointWCS[2] == aEmissionPointWCS[2]))
   {
      // Null LOS vector.  Terminus will not be adjusted.
      return false;
   }

   double fullBeamLength     = UtVec3d::Normalize(unitLOS_WCS);
   double adjustedBeamLength = fullBeamLength;

   if ((AltA <= mCloudUpperAlt) && (AltA >= mCloudLowerAlt))
   {
      // If currently in the cloud layer, place the terminus of the beam at 10m range.
      if (adjustedBeamLength > 10.0)
      {
         adjustedBeamLength = 10.0;
      }
   }
   else if ((AltA > mCloudUpperAlt) && (AltB < mCloudUpperAlt))
   {
      // Looking down on cloud from above.  Rest the beam on the top of the cloud layer
      adjustedBeamLength *= (AltA - mCloudUpperAlt) / (AltA - AltB);
   }
   else if ((AltA < mCloudLowerAlt) && (AltB > mCloudLowerAlt))
   {
      // Looking up on cloud from below.  Rest the beam on the bottom of the cloud layer
      adjustedBeamLength *= (mCloudLowerAlt - AltA) / (AltB - AltA);
   }

   // Find the endpoint of the beam, relative to the beginning.
   double relativeLocWCS[3];
   UtVec3d::Multiply(relativeLocWCS, unitLOS_WCS, adjustedBeamLength);

   // Now place in WCS frame:
   UtVec3d::Add(aTargetPointWCS, aEmissionPointWCS, relativeLocWCS);

   // Terminus adjusted.
   return true;
}

// protected
void WsfEnvironment::SetLandCoverStrategy(const GetLandCoverFunction& aFunction)
{
   mGetLandCoverFunction = aFunction;
}
