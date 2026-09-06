// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSA_EntityPerception.hpp"

#include <string>

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfIntercept.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSA_Group.hpp"
#include "WsfSA_Processor.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfUtil.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"

// Note:  This will be added in the future
/*
WsfSA_EntityPerception::SA_EntitySystem::SA_EntitySystem()
   : mSystemType()
   , mSystemName()
{
}

WsfSA_EntityPerception::SA_EntitySystem::SA_EntitySystem(WsfWeapon& aWeapon)
   : mSystemType(aWeapon.GetType())
   , mSystemName(aWeapon.GetName())
{
   if ((aWeapon.GetEM_XmtrCount() != 0) || aWeapon.IsA_TypeOf("WSF_RF_JAMMER"))
   {
      mSystemKind        = SA_SYSTEM_KIND_JAMMER;
      mReadyAssignment   = static_cast<int>(aWeapon.GetMaximumBeamCount());
      mQuantityRemaining = aWeapon.GetQuantityRemaining();    //(jammerWeapon.GetMaximumBeamCount() -
jammerWeapon.GetActiveBeamCount());
   }
   else
   {
      mSystemKind        = SA_SYSTEM_KIND_WEAPON;
      mReadyAssignment   = static_cast<int>(aWeapon.GetMaximumRequestCount() - aWeapon.GetActiveRequestCount());
      mQuantityRemaining = aWeapon.GetQuantityRemaining();
   }
}

WsfSA_EntityPerception::SA_EntitySystem::SA_EntitySystem(WsfSensor& aSensor)
   : mSystemKind(SA_SYSTEM_KIND_SENSOR)
   , mSystemType(aSensor.GetType())
   , mSystemName(aSensor.GetName())
   , mReadyAssignment(aSensor.GetMaximumRequestCount())
{
   mQuantityRemaining = (double)(aSensor.GetMaximumRequestCount() - aSensor.GetActiveRequestCount());
}

WsfSA_EntityPerception::SA_EntitySystem::SA_EntitySystem(WsfProcessor& aProcessor)
   : mSystemKind(SA_SYSTEM_KIND_PROCESSOR)
   , mSystemType(aProcessor.GetType())
   , mSystemName(aProcessor.GetName())
   , mReadyAssignment(1)
   , mQuantityRemaining(1.0)
{
}

WsfSA_EntityPerception::SA_EntitySystem::SA_EntitySystem(SA_EntitySystem& aSrc, int aReady, double aQuantity)
   : mSystemKind(aSrc.mSystemKind)
   , mSystemType(aSrc.mSystemType)
   , mSystemName(aSrc.mSystemName)
   , mReadyAssignment(aReady)
   , mQuantityRemaining(aQuantity)
{
}
*/

// -----------------------------------------------------------------------------------------------------

// This constructor is typically used for assets, with aFriendlyAsset being set to true
WsfSA_EntityPerception::WsfSA_EntityPerception(WsfSimulation*     aSimulation,
                                               double             aSimTime,
                                               const WsfPlatform* aOwningPlatform,
                                               WsfPlatform&       aPlatform,
                                               bool               aFriendlyAsset)
   : WsfSA_PerceivedItem()
   , mTime(aSimTime)
   , mSimulationPtr(aSimulation)
{
   OwningPlatform(aOwningPlatform);
   aPlatform.Update(aSimTime);
   mEntityPlatformIndex = aPlatform.GetIndex();

   double lat_deg, lon_deg, altitude_ft;
   aPlatform.GetLocationLLA(lat_deg, lon_deg, altitude_ft);
   altitude_ft *= UtMath::cFT_PER_M;
   mLat_deg     = lat_deg;
   mLon_deg     = lon_deg;
   mAltitude_ft = altitude_ft;

   double locationWCS[3];
   aPlatform.GetLocationWCS(locationWCS);
   double locationNED[3];
   aOwningPlatform->ConvertWCSToNED(locationWCS, locationNED);

   double ownLocationWCS[3];
   aOwningPlatform->GetLocationWCS(ownLocationWCS);
   double ownLocationNED[3];
   aOwningPlatform->ConvertWCSToNED(ownLocationWCS, ownLocationNED);

   mAngleOnly = false;
   SetBearing_deg(aOwningPlatform->TrueBearing(locationWCS) * UtMath::cDEG_PER_RAD);
   SetSpeed_kts(aPlatform.GetSpeed() * UtMath::cNMPH_PER_MPS);

   double heading_rad, pitch_rad, roll_rad;
   aPlatform.GetOrientationNED(heading_rad, pitch_rad, roll_rad);
   SetHeading_deg(heading_rad * UtMath::cDEG_PER_RAD);

   SetThreatLevel(0.0);
   SetTargetValue(0.0);
   SetRisk(0.0);
   SetDefensiveness(0.0);
   SetUrgency(0.0);

   SetIdString(aPlatform.GetName());
   SetPerceivedIndex(aPlatform.GetIndex());
   SetPerceivedName(aPlatform.GetName());
   SetPerceivedType(aPlatform.GetType());

   // Get the SAP for the target platform
   WsfSA_Processor* tgtSAP = WsfSA_Processor::GetSA_Processor(aPlatform);
   if (tgtSAP != nullptr)
   {
      mFlightId = tgtSAP->FlightId();
      mIdFlag   = tgtSAP->IdFlag();
   }

   SetFriendlyAsset(aFriendlyAsset);

   if (mFriendlyAsset)
   {
      mIdentification = FRIENDLY;
   }
   else
   {
      mIdentification = UNKNOWN;
   }

   mIdIffFriend  = false;
   mIdIffFoe     = false;
   mIdIffNeutral = false;
   mIdIffUnknown = true;

   mIdAuxFriend  = false;
   mIdAuxFoe     = false;
   mIdAuxNeutral = false;
   mIdAuxUnknown = true;

   mIdSideFriend  = false;
   mIdSideFoe     = false;
   mIdSideNeutral = false;
   mIdSideUnknown = true;

   mIdTypeFriend  = false;
   mIdTypeFoe     = false;
   mIdTypeNeutral = false;
   mIdTypeUnknown = true;

   SetIsHostile(false);
   SetAltitudeValid(true);
   SetSpeedValid(true);
   SetHeadingValid(true);

   // Get the SA processor for the owning platform
   WsfSA_Processor* owningSAP = WsfSA_Processor::GetSA_Processor(*aOwningPlatform);

   if (owningSAP != nullptr)
   {
      if ((owningSAP->IsMissileType(mPerceivedType)) || (owningSAP->IsMissileType(GetIdString())))
      {
         mIsMissile = true;
      }
      else
      {
         mIsMissile = false;
      }
   }
   else
   {
      mIsMissile = false;
   }
}

// This constructor is typically used for tracks
WsfSA_EntityPerception::WsfSA_EntityPerception(WsfSimulation*     aSimulation,
                                               double             aSimTime,
                                               const WsfPlatform* aOwningPlatform,
                                               WsfLocalTrack*     aTrack)
   : WsfSA_PerceivedItem()
   , mTime(aSimTime)
   , mTrackPtr(aTrack)
   , mSimulationPtr(aSimulation)
{
   OwningPlatform(aOwningPlatform);
   if (aTrack != nullptr)
   {
      // Set the index based on track truth
      SetPerceivedIndex(aTrack->GetTargetIndex());

      // Set the name based on track truth
      mPerceivedName = aTrack->GetTargetName().GetString();

      // Set the type based on track truth
      mPerceivedType = aTrack->GetTargetType().GetString();

      // Get the SA processor for the owning platform
      WsfSA_Processor* owningSAP = WsfSA_Processor::GetSA_Processor(*aOwningPlatform);

      // Get the target platform
      WsfPlatform* tgtPlatform = mSimulationPtr->GetPlatformByIndex(GetPerceivedIndex());

      if (aTrack->LocationValid())
      {
         double lat_deg, lon_deg, altitude_ft;
         aTrack->GetLocationLLA(lat_deg, lon_deg, altitude_ft);
         altitude_ft *= UtMath::cFT_PER_M;
         mLat_deg     = lat_deg;
         mLon_deg     = lon_deg;
         mAltitude_ft = altitude_ft;
         SetAltitudeValid(true);
         mAngleOnly = false;

         if (aTrack->VelocityValid())
         {
            size_t tgtIndex = GetPerceivedIndex();
            if (tgtIndex > 0)
            {
               if (mSimulationPtr != nullptr)
               {
                  if (tgtPlatform != nullptr)
                  {
                     double heading_rad, pitch_rd, roll_rad;
                     tgtPlatform->GetOrientationNED(heading_rad, pitch_rd, roll_rad);
                     SetHeading_deg(heading_rad * UtMath::cDEG_PER_RAD);
                     SetHeadingValid(true);
                     SetSpeed_kts(tgtPlatform->GetSpeed() * UtMath::cNMPH_PER_MPS);
                     SetSpeedValid(true);
                  }
                  else
                  {
                     SetHeadingValid(false);
                     SetSpeedValid(false);
                  }
               }
            }
         }
      }
      else if (aTrack->RangeValid() && aTrack->BearingValid() && aTrack->ElevationValid())
      {
         // This produces a full track
         double tempRange_m       = aTrack->GetRange();
         double tempBearing_rad   = aTrack->GetBearing();
         double tempElevation_rad = aTrack->GetElevation();

         // Use a temporary UtEntity to determine the location
         UtEntity tempEntity;

         // Set the location of the owning platform
         double ownPlatformWCS[3];
         aOwningPlatform->GetLocationWCS(ownPlatformWCS);
         tempEntity.SetLocationWCS(ownPlatformWCS);

         // Set the orientation, but only use the heading
         double heading_rad, pitch_rad, roll_rad;
         aOwningPlatform->GetOrientationNED(heading_rad, pitch_rad, roll_rad);
         tempEntity.SetOrientationNED(heading_rad, 0.0, 0.0);

         // Calculate the relative bearing location using our bearing, elevation, and range
         double realtiveBearing_rad = tempBearing_rad - heading_rad;

         // Calculate the relative location (WCS)
         double relativeLocationWCS[3];
         tempEntity.GetRelativeLocationWCS(realtiveBearing_rad, tempElevation_rad, tempRange_m, relativeLocationWCS);

         // Calculate the location
         double tgtLocationWCS[3] = {ownPlatformWCS[0] + relativeLocationWCS[0],
                                     ownPlatformWCS[1] + relativeLocationWCS[1],
                                     ownPlatformWCS[2] + relativeLocationWCS[2]};

         // Calc the LLA
         double lat_deg, lon_deg, altitude_ft;
         UtEntity::ConvertWCSToLLA(tgtLocationWCS, lat_deg, lon_deg, altitude_ft);
         altitude_ft *= UtMath::cFT_PER_M;
         mLat_deg     = lat_deg;
         mLon_deg     = lon_deg;
         mAltitude_ft = altitude_ft;

         if (aTrack->VelocityValid())
         {
            size_t tgtIndex = GetPerceivedIndex();
            if (tgtIndex > 0)
            {
               if (mSimulationPtr != nullptr)
               {
                  if (tgtPlatform != nullptr)
                  {
                     double heading_rad, pitch_rd, roll_rad;
                     tgtPlatform->GetOrientationNED(heading_rad, pitch_rd, roll_rad);
                     SetHeading_deg(heading_rad * UtMath::cDEG_PER_RAD);
                     SetHeadingValid(true);
                     SetSpeed_kts(tgtPlatform->GetSpeed() * UtMath::cNMPH_PER_MPS);
                     SetSpeedValid(true);
                  }
                  else
                  {
                     SetHeadingValid(false);
                     SetSpeedValid(false);
                  }
               }
            }
         }
      }
      else
      {
         double tempRange_m       = mAssumedRangeForAngleOnlyTargets;
         double tempBearing_rad   = 0.0;
         double tempElevation_rad = 0.0;
         bool   rangeValid        = false;
         bool   elevationValid    = false;

         // If we lack a bearing, this is not a viable entity
         if (aTrack->BearingValid())
         {
            // We have a bearing, so we can proceed
            tempBearing_rad = aTrack->GetBearing();
         }
         else
         {
            // We lack a bearing, so we can not proceed with calculations.
            // Set the malformed condition and return (rather than throwing).
            SetMalformedCondition();
            return;
         }

         // We will use elevation data if it is present
         if (aTrack->ElevationValid())
         {
            tempElevation_rad = aTrack->GetElevation();
            elevationValid    = true;
         }

         // We will use range data if it is present, else we will use the assumed range (above)
         if (aTrack->RangeValid())
         {
            tempRange_m = aTrack->GetRange();
            rangeValid  = true;
         }

         // Now, we will create the best estimate of location

         // Use a temporary UtEntity to determine the location
         UtEntity tempEntity;

         // Set the location of the owning platform
         double ownPlatformWCS[3];
         aOwningPlatform->GetLocationWCS(ownPlatformWCS);
         tempEntity.SetLocationWCS(ownPlatformWCS);

         // Set the orientation, but only use the heading
         double heading_rad, pitch_rad, roll_rad;
         aOwningPlatform->GetOrientationNED(heading_rad, pitch_rad, roll_rad);
         tempEntity.SetOrientationNED(heading_rad, 0.0, 0.0);

         // Calculate the relative bearing location using our bearing, elevation, and range
         double realtiveBearing_rad = tempBearing_rad - heading_rad;

         // Calculate the relative location (WCS)
         double relativeLocationWCS[3];
         tempEntity.GetRelativeLocationWCS(realtiveBearing_rad, tempElevation_rad, tempRange_m, relativeLocationWCS);

         // Calculate the location
         double tgtLocationWCS[3] = {ownPlatformWCS[0] + relativeLocationWCS[0],
                                     ownPlatformWCS[1] + relativeLocationWCS[1],
                                     ownPlatformWCS[2] + relativeLocationWCS[2]};

         // Calc the LLA
         double lat_deg, lon_deg, altitude_ft;
         UtEntity::ConvertWCSToLLA(tgtLocationWCS, lat_deg, lon_deg, altitude_ft);
         altitude_ft *= UtMath::cFT_PER_M;
         mLat_deg     = lat_deg;
         mLon_deg     = lon_deg;
         mAltitude_ft = altitude_ft;

         // Set the flags
         if (elevationValid && rangeValid)
         {
            SetAltitudeValid(true);
            mAngleOnly = false;
         }
         else if (rangeValid)
         {
            SetAltitudeValid(false);
            mAngleOnly = false;
         }
         else if (elevationValid)
         {
            SetAltitudeValid(false);
            mAngleOnly = true;
            SetBearing_deg(tempBearing_rad * UtMath::cDEG_PER_RAD);
         }
      }

      // Check for ID information. First, set the perceived type and identification data

      // Set default values
      SetIdentification(BOGIE);
      mIdTypeFriend  = false;
      mIdTypeFoe     = false;
      mIdTypeNeutral = false;
      mIdTypeUnknown = true;
      bool typeSet   = false; // Denotes if the type (mIdString) has been set
      bool idSet     = false; // Denotes if the mIdentification has been set

      // Try to set mIdString according to type id
      if ((owningSAP != nullptr) && aTrack->TypeIdValid())
      {
         SetIdString(aTrack->GetTargetType().GetString());

         // Check for simple mode
         if (owningSAP->UseSimpleIdByType())
         {
            // This provides a simulated, simple identification by type. Basically, when using
            // the "simple" approach, we confirm that the track supports type ID, and then we
            // check the target platform truth "side", even if the side data is not necessarily
            // provided in the track.

            if (tgtPlatform != nullptr)
            {
               std::string truthSide = tgtPlatform->GetSide();

               if (owningSAP->IsEnemySide(truthSide))
               {
                  typeSet = true;
                  SetIdentification(BANDIT);
                  idSet          = true;
                  mIdTypeFriend  = false;
                  mIdTypeFoe     = true;
                  mIdTypeNeutral = false;
                  mIdTypeUnknown = false;
               }
               else if (owningSAP->IsFriendlySide(truthSide))
               {
                  typeSet = true;
                  SetIdentification(FRIENDLY);
                  idSet          = true;
                  mIdTypeFriend  = true;
                  mIdTypeFoe     = false;
                  mIdTypeNeutral = false;
                  mIdTypeUnknown = false;
               }
               else if (owningSAP->IsNeutralSide(truthSide))
               {
                  typeSet = true;
                  SetIdentification(NEUTRAL);
                  idSet          = true;
                  mIdTypeFriend  = false;
                  mIdTypeFoe     = false;
                  mIdTypeNeutral = true;
                  mIdTypeUnknown = false;
               }
            }
         }
         else
         {
            if (owningSAP->IsEnemyType(GetIdString()))
            {
               typeSet = true;
               SetIdentification(BANDIT);
               idSet          = true;
               mIdTypeFriend  = false;
               mIdTypeFoe     = true;
               mIdTypeNeutral = false;
               mIdTypeUnknown = false;
            }
            else if (owningSAP->IsFriendlyType(GetIdString()))
            {
               typeSet = true;
               SetIdentification(FRIENDLY);
               idSet          = true;
               mIdTypeFriend  = true;
               mIdTypeFoe     = false;
               mIdTypeNeutral = false;
               mIdTypeUnknown = false;
            }
            else if (owningSAP->IsNeutralType(GetIdString()))
            {
               typeSet = true;
               SetIdentification(NEUTRAL);
               idSet          = true;
               mIdTypeFriend  = false;
               mIdTypeFoe     = false;
               mIdTypeNeutral = true;
               mIdTypeUnknown = false;
            }
         }

         if (!typeSet)
         {
            // Clear the id string, since we could not ID the type
            SetIdString("");
         }
      }

      // Try to set mIdString according to side id
      if ((owningSAP != nullptr) && !idSet && aTrack->SideIdValid())
      {
         if (owningSAP->IsFriendlySide(aTrack->GetSideId().GetString()))
         {
            // This is a friend
            SetIdString("SIDE-FRIEND");
            typeSet = true;
            SetIdentification(FRIENDLY);
            idSet          = true;
            mIdSideFriend  = true;
            mIdSideFoe     = false;
            mIdSideNeutral = false;
            mIdSideUnknown = false;
         }
         else if (owningSAP->IsEnemySide(aTrack->GetSideId().GetString()))
         {
            // This is an enemy
            SetIdString("SIDE-ENEMY");
            typeSet = true;
            SetIdentification(BANDIT);
            idSet          = true;
            mIdSideFriend  = false;
            mIdSideFoe     = true;
            mIdSideNeutral = false;
            mIdSideUnknown = false;
         }
         else if (owningSAP->IsNeutralSide(aTrack->GetSideId().GetString()))
         {
            // This is a neutral
            SetIdString("SIDE-NEUTRAL");
            typeSet = true;
            SetIdentification(NEUTRAL);
            idSet          = true;
            mIdSideFriend  = false;
            mIdSideFoe     = false;
            mIdSideNeutral = true;
            mIdSideUnknown = false;
         }
      }

      // Try to set mIdString according to IFF id

      // First, get the SA processor for the owning platform
      if ((owningSAP != nullptr) && !idSet && owningSAP->UseIFF() && aTrack->IFF_StatusValid())
      {
         if (aTrack->GetIFF_Status() == WsfTrack::cIFF_FRIEND)
         {
            SetIdString("IFF-FRIEND");
            typeSet = true;
            SetIdentification(FRIENDLY);
            idSet         = true;
            mIdIffFriend  = true;
            mIdIffFoe     = false;
            mIdIffNeutral = false;
            mIdIffUnknown = false;
         }
         else if (aTrack->GetIFF_Status() == WsfTrack::cIFF_FOE)
         {
            SetIdString("IFF-FOE");
            typeSet = true;
            SetIdentification(BANDIT);
            idSet         = true;
            mIdIffFriend  = false;
            mIdIffFoe     = true;
            mIdIffNeutral = false;
            mIdIffUnknown = false;
         }
         else if (aTrack->GetIFF_Status() == WsfTrack::cIFF_NEUTRAL)
         {
            SetIdString("IFF-NEUTRAL");
            typeSet = true;
            SetIdentification(NEUTRAL);
            idSet         = true;
            mIdIffFriend  = false;
            mIdIffFoe     = false;
            mIdIffNeutral = true;
            mIdIffUnknown = false;
         }
      }

      // Try to set mIdString according to aux id
      if (!idSet && aTrack->HasAuxData())
      {
         const WsfAttributeContainer& auxData = aTrack->GetAuxData();

         if (auxData.AttributeExists("id_aux_type"))
         {
            if (owningSAP != nullptr)
            {
               std::string typeString = auxData.GetString("id_aux_type");

               if (owningSAP->IsEnemyType(typeString))
               {
                  SetIdString(typeString);
                  typeSet = true;
                  SetIdentification(BANDIT);
                  idSet         = true;
                  mIdAuxFriend  = false;
                  mIdAuxFoe     = true;
                  mIdAuxNeutral = false;
                  mIdAuxUnknown = false;
               }
               else if (owningSAP->IsFriendlyType(typeString))
               {
                  SetIdString(typeString);
                  typeSet = true;
                  SetIdentification(FRIENDLY);
                  idSet         = true;
                  mIdAuxFriend  = true;
                  mIdAuxFoe     = false;
                  mIdAuxNeutral = false;
                  mIdAuxUnknown = false;
               }
               else if (owningSAP->IsNeutralType(typeString))
               {
                  SetIdString(typeString);
                  typeSet = true;
                  SetIdentification(NEUTRAL);
                  idSet         = true;
                  mIdAuxFriend  = false;
                  mIdAuxFoe     = false;
                  mIdAuxNeutral = true;
                  mIdAuxUnknown = false;
               }
            }
         }

         if (!idSet && auxData.AttributeExists("id_aux_friend"))
         {
            SetIdString("AUX-FRIEND");
            typeSet = true;
            SetIdentification(FRIENDLY);
            idSet         = true;
            mIdAuxFriend  = true;
            mIdAuxFoe     = false;
            mIdAuxNeutral = false;
            mIdAuxUnknown = false;
         }
         else if (auxData.AttributeExists("id_aux_foe"))
         {
            SetIdString("AUX-FOE");
            typeSet = true;
            SetIdentification(BANDIT);
            idSet         = true;
            mIdAuxFriend  = false;
            mIdAuxFoe     = true;
            mIdAuxNeutral = false;
            mIdAuxUnknown = false;
         }
         else if (auxData.AttributeExists("id_aux_neutral"))
         {
            SetIdString("AUX-NEUTRAL");
            typeSet = true;
            SetIdentification(NEUTRAL);
            idSet         = true;
            mIdAuxFriend  = false;
            mIdAuxFoe     = false;
            mIdAuxNeutral = true;
            mIdAuxUnknown = false;
         }
      }

      if (!typeSet)
      {
         // We have no idea about the perceived type
         SetIdString("UNKNOWN");
      }

      if (!idSet)
      {
         // We have no idea about the identification
         SetIdentification(UNKNOWN);
      }

      // Set missile type
      if (owningSAP != nullptr)
      {
         if ((owningSAP->IsMissileType(mPerceivedType)) || (owningSAP->IsMissileType(GetIdString())))
         {
            mIsMissile = true;
         }
         else
         {
            mIsMissile = false;
         }
      }
      else
      {
         mIsMissile = false;
      }

      // If the track is not perceived as a missile, check the MWS track processor.
      // Any track reported by the MWS is assumed to be a missile.
      if (!mIsMissile && (tgtPlatform != nullptr))
      {
         if (owningSAP->PlatformInTrackListMWS(tgtPlatform))
         {
            mIsMissile = true;
         }
      }

      // Check for multiple, differing identifications. Id priority is as follows:  Type, Side, IFF, and Aux

      // First, check if anything has marked this a friend
      bool somethingMarkedThisAsFriend = false;
      if (GetIdTypeFriend() || GetIdSideFriend() || GetIdIffFriend() || GetIdAuxFriend())
      {
         somethingMarkedThisAsFriend = true;
      }

      // Next, check if anything has marked this a foe
      bool somethingMarkedThisAsFoe = false;
      if (GetIdTypeFoe() || GetIdSideFoe() || GetIdIffFoe() || GetIdAuxFoe())
      {
         somethingMarkedThisAsFoe = true;
      }

      // Check for only friend
      if (somethingMarkedThisAsFriend && !somethingMarkedThisAsFoe)
      {
         // This should be identified as a friend
         SetIdentification(FRIENDLY);
      }
      else
      {
         // Check for only foe
         if (!somethingMarkedThisAsFriend && somethingMarkedThisAsFoe)
         {
            // This should be identified as a foe
            SetIdentification(BANDIT);
         }
         else if (somethingMarkedThisAsFriend && somethingMarkedThisAsFoe) // Both
         {
            // Check priority
            if (GetIdTypeFriend())
            {
               SetIdentification(FRIENDLY);
            }
            else if (GetIdTypeFoe())
            {
               SetIdentification(BANDIT);
            }

            if (GetIdSideFriend())
            {
               SetIdentification(FRIENDLY);
            }
            else if (GetIdSideFoe())
            {
               SetIdentification(BANDIT);
            }

            if (GetIdIffFriend())
            {
               SetIdentification(FRIENDLY);
            }
            else if (GetIdIffFoe())
            {
               SetIdentification(BANDIT);
            }

            if (GetIdAuxFriend())
            {
               SetIdentification(FRIENDLY);
            }
            else if (GetIdAuxFoe())
            {
               SetIdentification(BANDIT);
            }
         }
         else if (!somethingMarkedThisAsFriend && !somethingMarkedThisAsFoe) // Neither
         {
            // Check for neutral
            if (GetIdTypeNeutral() || GetIdSideNeutral() || GetIdIffNeutral() || GetIdAuxNeutral())
            {
               SetIdentification(NEUTRAL);
            }
         }
      }
   } // aTrack != nullptr
}

WsfSA_EntityPerception::WsfSA_EntityPerception(const WsfSA_EntityPerception& aSrc)
   : WsfSA_PerceivedItem(aSrc)
   , mTime(aSrc.mTime)
   , mEntityPlatformIndex(aSrc.mEntityPlatformIndex)
   , mTrackPtr(aSrc.mTrackPtr)
   , mLat_deg(aSrc.mLat_deg)
   , mLon_deg(aSrc.mLon_deg)
   , mAltitude_ft(aSrc.mAltitude_ft)
   , mSpeed_kts(aSrc.mSpeed_kts)
   , mHeading_deg(aSrc.mHeading_deg)
   , mRisk(aSrc.mRisk)
   , mDefensiveness(aSrc.mDefensiveness)
   , mUrgency(aSrc.mUrgency)
   , mIdString(aSrc.mIdString)
   , mPerceivedIndex(aSrc.mPerceivedIndex)
   , mPerceivedName(aSrc.mPerceivedName)
   , mPerceivedType(aSrc.mPerceivedType)
   , mFlightId(aSrc.mFlightId)
   , mIdFlag(aSrc.mIdFlag)
   , mAngleOnly(aSrc.mAngleOnly)
   , mIdIffFriend(aSrc.mIdIffFriend)
   , mIdIffFoe(aSrc.mIdIffFoe)
   , mIdIffNeutral(aSrc.mIdIffNeutral)
   , mIdIffUnknown(aSrc.mIdIffUnknown)
   , mIdAuxFriend(aSrc.mIdAuxFriend)
   , mIdAuxFoe(aSrc.mIdAuxFoe)
   , mIdAuxNeutral(aSrc.mIdAuxNeutral)
   , mIdAuxUnknown(aSrc.mIdAuxUnknown)
   , mIdSideFriend(aSrc.mIdSideFriend)
   , mIdSideFoe(aSrc.mIdSideFoe)
   , mIdSideNeutral(aSrc.mIdSideNeutral)
   , mIdSideUnknown(aSrc.mIdSideUnknown)
   , mIdTypeFriend(aSrc.mIdTypeFriend)
   , mIdTypeFoe(aSrc.mIdTypeFoe)
   , mIdTypeNeutral(aSrc.mIdTypeNeutral)
   , mIdTypeUnknown(aSrc.mIdTypeUnknown)
   , mIsMissile(aSrc.mIsMissile)
   , mIsCoasting(aSrc.mIsCoasting)
   , mIsNonTrackAsset(aSrc.mIsNonTrackAsset)
   , mFriendlyAsset(aSrc.mFriendlyAsset)
   , mIsHostile(aSrc.mIsHostile)
   , mIdentification(aSrc.mIdentification)
   , mImportance(aSrc.mImportance)
   , mMalformedPerception(aSrc.mMalformedPerception)
   , mSimulationPtr(aSrc.mSimulationPtr)
{
}

WsfSA_EntityPerception::WsfSA_EntityPerception(double                        aSimTime,
                                               const WsfPlatform*            aOwningPlatform,
                                               const WsfSA_EntityPerception& aSrc)
   : WsfSA_PerceivedItem(aSrc)
   , mTime(aSrc.mTime)
   , mEntityPlatformIndex(aSrc.mEntityPlatformIndex)
   , mTrackPtr(aSrc.mTrackPtr)
   , mLat_deg(aSrc.mLat_deg)
   , mLon_deg(aSrc.mLon_deg)
   , mAltitude_ft(aSrc.mAltitude_ft)
   , mSpeed_kts(aSrc.mSpeed_kts)
   , mHeading_deg(aSrc.mHeading_deg)
   , mRisk(aSrc.mRisk)
   , mDefensiveness(aSrc.mDefensiveness)
   , mUrgency(aSrc.mUrgency)
   , mIdString(aSrc.mIdString)
   , mPerceivedIndex(aSrc.mPerceivedIndex)
   , mPerceivedName(aSrc.mPerceivedName)
   , mPerceivedType(aSrc.mPerceivedType)
   , mFlightId(aSrc.mFlightId)
   , mIdFlag(aSrc.mIdFlag)
   , mAngleOnly(aSrc.mAngleOnly)
   , mIdIffFriend(aSrc.mIdIffFriend)
   , mIdIffFoe(aSrc.mIdIffFoe)
   , mIdIffNeutral(aSrc.mIdIffNeutral)
   , mIdIffUnknown(aSrc.mIdIffUnknown)
   , mIdAuxFriend(aSrc.mIdAuxFriend)
   , mIdAuxFoe(aSrc.mIdAuxFoe)
   , mIdAuxNeutral(aSrc.mIdAuxNeutral)
   , mIdAuxUnknown(aSrc.mIdAuxUnknown)
   , mIdSideFriend(aSrc.mIdSideFriend)
   , mIdSideFoe(aSrc.mIdSideFoe)
   , mIdSideNeutral(aSrc.mIdSideNeutral)
   , mIdSideUnknown(aSrc.mIdSideUnknown)
   , mIdTypeFriend(aSrc.mIdTypeFriend)
   , mIdTypeFoe(aSrc.mIdTypeFoe)
   , mIdTypeNeutral(aSrc.mIdTypeNeutral)
   , mIdTypeUnknown(aSrc.mIdTypeUnknown)
   , mIsMissile(aSrc.mIsMissile)
   , mIsCoasting(aSrc.mIsCoasting)
   , mIsNonTrackAsset(aSrc.mIsNonTrackAsset)
   , mFriendlyAsset(aSrc.mFriendlyAsset)
   , mIsHostile(aSrc.mIsHostile)
   , mIdentification(aSrc.mIdentification)
   , mImportance(aSrc.mImportance)
   , mMalformedPerception(aSrc.mMalformedPerception)
   , mSimulationPtr(aSrc.mSimulationPtr)
{
   OwningPlatform(aOwningPlatform);
}

WsfSA_EntityPerception::~WsfSA_EntityPerception()
{
   auto parent = GetParentGroup();
   if (parent)
   {
      parent->RemoveElement(this);
   }
   FreeReference();
}

WsfSA_EntityPerception& WsfSA_EntityPerception::operator=(const WsfSA_EntityPerception& aRhs)
{
   if (this != &aRhs)
   {
      WsfSA_PerceivedItem::operator=(aRhs);
      mTime                        = aRhs.mTime;
      mEntityPlatformIndex         = aRhs.mEntityPlatformIndex;
      mTrackPtr                    = aRhs.mTrackPtr;
      mLat_deg                     = aRhs.mLat_deg;
      mLon_deg                     = aRhs.mLon_deg;
      mRisk                        = aRhs.mRisk;
      mDefensiveness               = aRhs.mDefensiveness;
      mUrgency                     = aRhs.mUrgency;
      mPerceivedIndex              = aRhs.mPerceivedIndex;
      mPerceivedName               = aRhs.mPerceivedName;
      mPerceivedType               = aRhs.mPerceivedType;
      mFlightId                    = aRhs.mFlightId;
      mAngleOnly                   = aRhs.mAngleOnly;
      mIdString                    = aRhs.mIdString;
      mIdFlag                      = aRhs.mIdFlag;
      mIdIffFriend                 = aRhs.mIdIffFriend;
      mIdIffFoe                    = aRhs.mIdIffFoe;
      mIdIffNeutral                = aRhs.mIdIffNeutral;
      mIdIffUnknown                = aRhs.mIdIffUnknown;
      mIdAuxFriend                 = aRhs.mIdAuxFriend;
      mIdAuxFoe                    = aRhs.mIdAuxFoe;
      mIdAuxNeutral                = aRhs.mIdAuxNeutral;
      mIdAuxUnknown                = aRhs.mIdAuxUnknown;
      mIdSideFriend                = aRhs.mIdSideFriend;
      mIdSideFoe                   = aRhs.mIdSideFoe;
      mIdSideNeutral               = aRhs.mIdSideNeutral;
      mIdSideUnknown               = aRhs.mIdSideUnknown;
      mIdTypeFriend                = aRhs.mIdTypeFriend;
      mIdTypeFoe                   = aRhs.mIdTypeFoe;
      mIdTypeNeutral               = aRhs.mIdTypeNeutral;
      mIdTypeUnknown               = aRhs.mIdTypeUnknown;
      mIsMissile                   = aRhs.mIsMissile;
      mIsCoasting                  = aRhs.mIsCoasting;
      mLastUpdateTime              = aRhs.mLastUpdateTime;
      mIsNonTrackAsset             = aRhs.mIsNonTrackAsset;
      mFriendlyAsset               = aRhs.mFriendlyAsset;
      mIsHostile                   = aRhs.mIsHostile;
      mLastExtrapolationTime       = aRhs.mLastExtrapolationTime;
      mMalformedPerception         = aRhs.mMalformedPerception;
      mSimulationPtr               = aRhs.mSimulationPtr;
      mIdentification              = aRhs.mIdentification;
      mImportance                  = aRhs.mImportance;
   }
   return *this;
}

bool WsfSA_EntityPerception::IsSame(const WsfSA_PerceivedItem* aOther) const
{
   if (aOther->GetItemType() != WsfSA_PerceivedItem::ENTITY)
   {
      return false;
   }
   const WsfSA_EntityPerception* entity = dynamic_cast<const WsfSA_EntityPerception*>(aOther);
   if (GetPerceivedIndex() == entity->GetPerceivedIndex())
   {
      if (mEntityPlatformIndex > 0)
      {
         if (mEntityPlatformIndex == entity->mEntityPlatformIndex)
         {
            return true;
         }
      }

      if (mTrackPtr != nullptr)
      {
         if (mTrackPtr == entity->mTrackPtr)
         {
            return true;
         }
      }
      else
      {
         // This is so that coasting threat perceptions can be made no longer coasting.
         return true;
      }
   }

   return false;
}

void WsfSA_EntityPerception::SetIdIffFriend()
{
   mIdIffFriend  = true;
   mIdIffFoe     = false;
   mIdIffNeutral = false;
   mIdIffUnknown = false;
}

void WsfSA_EntityPerception::SetIdIffFoe()
{
   mIdIffFriend  = false;
   mIdIffFoe     = true;
   mIdIffNeutral = false;
   mIdIffUnknown = false;

   SetFriendlyAsset(false); // Cannot be a friendly asset
}

void WsfSA_EntityPerception::SetIdIffNeutral()
{
   mIdIffFriend  = false;
   mIdIffFoe     = false;
   mIdIffNeutral = true;
   mIdIffUnknown = false;

   SetFriendlyAsset(false); // Cannot be a friendly asset
}

void WsfSA_EntityPerception::SetIdIffUnknown()
{
   mIdIffFriend  = false;
   mIdIffFoe     = false;
   mIdIffNeutral = false;
   mIdIffUnknown = true;
}

void WsfSA_EntityPerception::ClearIdIff()
{
   mIdIffFriend  = false;
   mIdIffFoe     = false;
   mIdIffNeutral = false;
   mIdIffUnknown = false;
}

void WsfSA_EntityPerception::SetIdAuxFriend()
{
   mIdAuxFriend  = true;
   mIdAuxFoe     = false;
   mIdAuxNeutral = false;
   mIdAuxUnknown = false;
}

void WsfSA_EntityPerception::SetIdAuxFoe()
{
   mIdAuxFriend  = false;
   mIdAuxFoe     = true;
   mIdAuxNeutral = false;
   mIdAuxUnknown = false;

   SetFriendlyAsset(false); // Cannot be a friendly asset
}

void WsfSA_EntityPerception::SetIdAuxNeutral()
{
   mIdAuxFriend  = false;
   mIdAuxFoe     = false;
   mIdAuxNeutral = true;
   mIdAuxUnknown = false;

   SetFriendlyAsset(false); // Cannot be a friendly asset
}

void WsfSA_EntityPerception::SetIdAuxUnknown()
{
   mIdAuxFriend  = false;
   mIdAuxFoe     = false;
   mIdAuxNeutral = false;
   mIdAuxUnknown = true;
}

void WsfSA_EntityPerception::ClearIdAux()
{
   mIdAuxFriend  = false;
   mIdAuxFoe     = false;
   mIdAuxNeutral = false;
   mIdAuxUnknown = false;
}

void WsfSA_EntityPerception::SetIdSideFriend()
{
   mIdSideFriend  = true;
   mIdSideFoe     = false;
   mIdSideNeutral = false;
   mIdSideUnknown = false;
}

void WsfSA_EntityPerception::SetIdSideFoe()
{
   mIdSideFriend  = false;
   mIdSideFoe     = true;
   mIdSideNeutral = false;
   mIdSideUnknown = false;

   SetFriendlyAsset(false); // Cannot be a friendly asset
}

void WsfSA_EntityPerception::SetIdSideNeutral()
{
   mIdSideFriend  = false;
   mIdSideFoe     = false;
   mIdSideNeutral = true;
   mIdSideUnknown = false;

   SetFriendlyAsset(false); // Cannot be a friendly asset
}

void WsfSA_EntityPerception::SetIdSideUnknown()
{
   mIdSideFriend  = false;
   mIdSideFoe     = false;
   mIdSideNeutral = false;
   mIdSideUnknown = true;
}

void WsfSA_EntityPerception::ClearIdSide()
{
   mIdSideFriend  = false;
   mIdSideFoe     = false;
   mIdSideNeutral = false;
   mIdSideUnknown = false;
}

void WsfSA_EntityPerception::SetIdTypeFriend()
{
   mIdTypeFriend  = true;
   mIdTypeFoe     = false;
   mIdTypeNeutral = false;
   mIdTypeUnknown = false;
}

void WsfSA_EntityPerception::SetIdTypeFoe()
{
   mIdTypeFriend  = false;
   mIdTypeFoe     = true;
   mIdTypeNeutral = false;
   mIdTypeUnknown = false;

   SetFriendlyAsset(false); // Cannot be a friendly asset
}

void WsfSA_EntityPerception::SetIdTypeNeutral()
{
   mIdTypeFriend  = false;
   mIdTypeFoe     = false;
   mIdTypeNeutral = true;
   mIdTypeUnknown = false;

   SetFriendlyAsset(false); // Cannot be a friendly asset
}

void WsfSA_EntityPerception::SetIdTypeUnknown()
{
   mIdTypeFriend  = false;
   mIdTypeFoe     = false;
   mIdTypeNeutral = false;
   mIdTypeUnknown = true;
}

void WsfSA_EntityPerception::ClearIdType()
{
   mIdTypeFriend  = false;
   mIdTypeFoe     = false;
   mIdTypeNeutral = false;
   mIdTypeUnknown = false;
}

WsfSA_Group* WsfSA_EntityPerception::GetParentGroup()
{
   if (mParentRC && mParentRC->IsValid())
   {
      return mParentGroup;
   }
   return nullptr;
}

const WsfSA_Group* WsfSA_EntityPerception::GetParentGroup() const
{
   if (mParentRC && mParentRC->IsValid())
   {
      return mParentGroup;
   }
   return nullptr;
}

void WsfSA_EntityPerception::SetParentGroup(WsfSA_Group* aParentGroup)
{
   if (mParentRC && mParentRC->IsValid())
   {
      mParentRC->RemoveWeakRef();
   }
   mParentGroup = aParentGroup;
   if (aParentGroup)
   {
      mParentRC = aParentGroup->GetReferenceCount();
      mParentRC->AddWeakRef();
   }
   else
   {
      mParentRC = nullptr;
   }
}

double WsfSA_EntityPerception::EstimatedTimeToIntercept(WsfPlatform* aTarget) const
{
   if (aTarget != nullptr)
   {
      if (GetAngleOnly())
      {
         // No estimate of estimated time to intercept for angle-only entities
         return -1.0;
      }

      if (!GetSpeedValid())
      {
         // No estimate of estimated time to intercept if speed is not valid
         return -1.0;
      }

      // Get the target location in WCS
      double tgtLocWCS[3];
      aTarget->GetLocationWCS(tgtLocWCS);

      // Get target velocity in WCS
      double tgtVelWCS[3];
      aTarget->GetVelocityWCS(tgtVelWCS);

      // Get the entity altitude
      double altitude = GetAltitude_ft() * UtMath::cM_PER_FT;
      if (!GetAltitudeValid())
      {
         // If we lack altitude data for the entity, we will assume the same altitude as the target
         altitude = aTarget->GetAltitude();
      }

      // Get the entity location in WCS
      double entityLocWCS[3];
      UtEntity::ConvertLLAToWCS(GetLat_deg(), GetLon_deg(), altitude, entityLocWCS);

      // Get the entity speed
      double entitySpeed = GetSpeed_kts() * UtMath::cMPS_PER_NMPH;

      // Compute the intercept time and location using standard/common function
      double intLocWCS[3];
      double timeToIntercept = WsfIntercept::Intercept(entityLocWCS, entitySpeed, tgtLocWCS, tgtVelWCS, intLocWCS);

      return timeToIntercept;
   }
   else
   {
      // No target, so cannot intercept
      return -1.0;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WsfScriptSA_EntityPerceptionClass::WsfScriptSA_EntityPerceptionClass(const std::string& aClassName,
                                                                     UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptSA_PerceivedItemClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSA_EntityPerception");
   mConstructible = true;
   mCloneable     = true;
   AddImplicitCastType("WsfSA_PerceivedItem");

   AddMethod(ut::make_unique<Index>());
   AddMethod(ut::make_unique<Time>());

   AddMethod(ut::make_unique<EntityPlatformIndex>());
   AddMethod(ut::make_unique<Track>());
   AddMethod(ut::make_unique<Lat>());
   AddMethod(ut::make_unique<Lon>());
   AddMethod(ut::make_unique<Altitude>());
   AddMethod(ut::make_unique<Bearing>());
   AddMethod(ut::make_unique<Speed>());
   AddMethod(ut::make_unique<Heading>());
   AddMethod(ut::make_unique<ThreatLevel>());
   AddMethod(ut::make_unique<TargetValue>());
   AddMethod(ut::make_unique<Risk>());
   AddMethod(ut::make_unique<Defensiveness>());
   AddMethod(ut::make_unique<Urgency>());
   AddMethod(ut::make_unique<IdString>());
   AddMethod(ut::make_unique<PerceivedIndex>());
   AddMethod(ut::make_unique<PerceivedName>());
   AddMethod(ut::make_unique<PerceivedType>());
   AddMethod(ut::make_unique<FlightId>());
   AddMethod(ut::make_unique<IdFlag>());
   AddMethod(ut::make_unique<Identification>());
   AddMethod(ut::make_unique<FriendlyAsset>());
   AddMethod(ut::make_unique<AngleOnly>());
   AddMethod(ut::make_unique<IdIffFriend>());
   AddMethod(ut::make_unique<IdIffFoe>());
   AddMethod(ut::make_unique<IdIffNeutral>());
   AddMethod(ut::make_unique<IdIffUnknown>());
   AddMethod(ut::make_unique<IdAuxFriend>());
   AddMethod(ut::make_unique<IdAuxFoe>());
   AddMethod(ut::make_unique<IdAuxNeutral>());
   AddMethod(ut::make_unique<IdAuxUnknown>());
   AddMethod(ut::make_unique<IdSideFriend>());
   AddMethod(ut::make_unique<IdSideFoe>());
   AddMethod(ut::make_unique<IdSideNeutral>());
   AddMethod(ut::make_unique<IdSideUnknown>());
   AddMethod(ut::make_unique<IdTypeFriend>());
   AddMethod(ut::make_unique<IdTypeFoe>());
   AddMethod(ut::make_unique<IdTypeNeutral>());
   AddMethod(ut::make_unique<IdTypeUnknown>());
   AddMethod(ut::make_unique<IsHostile>());
   AddMethod(ut::make_unique<IsMissile>());
   AddMethod(ut::make_unique<AltitudeValid>());
   AddMethod(ut::make_unique<SpeedValid>());
   AddMethod(ut::make_unique<HeadingValid>());
   AddMethod(ut::make_unique<SelectionScore>());
   AddMethod(ut::make_unique<EstimatedTimeToIntercept>());

   // Note:  These will be added in the future
   // AddMethod(ut::make_unique<SystemCount>());
   // AddMethod(ut::make_unique<SystemKind>());
   // AddMethod(ut::make_unique<SystemType>());
   // AddMethod(ut::make_unique<SystemName>());
   // AddMethod(ut::make_unique<SystemReadyAssignment>());
   // AddMethod(ut::make_unique<SystemQuantityRemaining>());
   // AddMethod(ut::make_unique<InterceptLocation>());      // double InterceptLocation(WsfTrack, WsfWaypoint)
   // AddMethod(ut::make_unique<TrueBearingTo>());          // double TrueBearingTo(WsfGeoPoint)
   // AddMethod(ut::make_unique<RelativeBearingTo>());      // double RelativeBearingTo(WsfGeoPoint)
   // AddMethod(ut::make_unique<ClosestApproachOf>());      // WsfGeoPoint ClosestApproachOf(WsfTrack)
   // AddMethod(ut::make_unique<ClosingSpeedOf>());         // double ClosingSpeedOf(WsfTrack)
}

void* WsfScriptSA_EntityPerceptionClass::Create(const UtScriptContext& // aContext*
)
{
   return new WsfSA_EntityPerception();
}

void* WsfScriptSA_EntityPerceptionClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfSA_EntityPerception*>(aObjectPtr)->Clone();
}

void WsfScriptSA_EntityPerceptionClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfSA_EntityPerception*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Index, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetEntityPlatformIndex()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Time, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->Time());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, EntityPlatformIndex, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetTargetPlatformIndex()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Track, 0, "WsfTrack", "")
{
   auto trackPtr = static_cast<WsfTrack*>(aObjectPtr->GetTrackPtr());
   aReturnVal.SetPointer(UtScriptRef::Ref(trackPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Lat, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLat_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Lon, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLon_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Altitude, 0, "double", "")
{
   if (aObjectPtr->GetAltitudeValid())
   {
      aReturnVal.SetDouble(aObjectPtr->GetAltitude_ft() * UtMath::cM_PER_FT);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Bearing, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBearing_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Speed, 0, "double", "")
{
   if (aObjectPtr->GetSpeedValid())
   {
      aReturnVal.SetDouble(aObjectPtr->GetSpeed_kts() * UtMath::cMPS_PER_NMPH);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Heading, 0, "double", "")
{
   if (aObjectPtr->GetHeadingValid())
   {
      aReturnVal.SetDouble(aObjectPtr->GetHeading_deg());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, ThreatLevel, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetThreatLevel());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, TargetValue, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTargetValue());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Risk, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRisk());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Defensiveness, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDefensiveness());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Urgency, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetUrgency());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdString, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetIdString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, PerceivedIndex, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetPerceivedIndex()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, PerceivedName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetPerceivedName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, PerceivedType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetPerceivedType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, FlightId, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetFlightId()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdFlag, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetIdFlag());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, Identification, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetIdentification()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, FriendlyAsset, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetFriendlyAsset());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, AngleOnly, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAngleOnly());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdIffFriend, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdIffFriend());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdIffFoe, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdIffFoe());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdIffNeutral, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdIffNeutral());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdIffUnknown, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdIffUnknown());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdAuxFriend, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdAuxFriend());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdAuxFoe, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdAuxFoe());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdAuxNeutral, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdAuxNeutral());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdAuxUnknown, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdAuxUnknown());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdSideFriend, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdSideFriend());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdSideFoe, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdSideFoe());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdSideNeutral, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdSideNeutral());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdSideUnknown, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdSideUnknown());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdTypeFriend, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdTypeFriend());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdTypeFoe, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdTypeFoe());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdTypeNeutral, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdTypeNeutral());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IdTypeUnknown, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIdTypeUnknown());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IsHostile, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIsHostile());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, IsMissile, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIsMissile());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, AltitudeValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAltitudeValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, SpeedValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSpeedValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, HeadingValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetHeadingValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, SelectionScore, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSelectionScore());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_EntityPerceptionClass, WsfSA_EntityPerception, EstimatedTimeToIntercept, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double time_sec = aObjectPtr->EstimatedTimeToIntercept(platformPtr);
      aReturnVal.SetDouble(time_sec);
   }
   else
   {
      aReturnVal.SetDouble(-1.0);
   }
}
