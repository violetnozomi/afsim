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

#include "dis/WsfDisArticulatedPart.hpp"

#include <iostream>

#include "DisEntityPart.hpp"
#include "DisEntityState.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfPlatform.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"

WsfDisArticulatedPart::WsfDisArticulatedPart(int aType, int aParentType, unsigned int aPublishBitmap, WsfArticulatedPart* aPartPtr)
   : mType(aType)
   , mParentType(aParentType)
   , mPublishBitmap(aPublishBitmap)
   , mPartPtr(aPartPtr)
   , mChangeChecker(0)
   , mLastX(0.0F)
   , mLastY(0.0F)
   , mLastZ(0.0F)
   , mLastX_Rate(0.0F)
   , mLastY_Rate(0.0F)
   , mLastZ_Rate(0.0F)
   , mLastAzimuth(0.0F)
   , mLastElevation(0.0F)
   , mLastRotation(0.0F)
   , mLastAzimuthRate(0.0F)
   , mLastElevationRate(0.0F)
   , mLastRotationRate(0.0F)
{
   for (unsigned int i = 0; i < 32; ++i)
   {
      mIndexList[i] = -1;
   }
}

WsfDisArticulatedPart::~WsfDisArticulatedPart() {}

//! Create the articulation parameter records for this articulated part.
int WsfDisArticulatedPart::CreateArticulationRecords(DisEntityState* aEntityStatePtr, int aStartCount)
{
   int counter = aStartCount;

   if (mPublishBitmap & PublishX)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterX);
      partPtr->SetFloat32Value(mLastX);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterX] = counter;
      counter++;
   }
   if (mPublishBitmap & PublishY)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterY);
      partPtr->SetFloat32Value(mLastY);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterY] = counter;
      counter++;
   }
   if (mPublishBitmap & PublishZ)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterZ);
      partPtr->SetFloat32Value(mLastZ);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterZ] = counter;
      counter++;
   }

   if (mPublishBitmap & PublishX_Rate)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterX_Rate);
      partPtr->SetFloat32Value(mLastX_Rate);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterX_Rate] = counter;
      counter++;
   }
   if (mPublishBitmap & PublishY_Rate)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterY_Rate);
      partPtr->SetFloat32Value(mLastY_Rate);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterY_Rate] = counter;
      counter++;
   }
   if (mPublishBitmap & PublishZ_Rate)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterZ_Rate);
      partPtr->SetFloat32Value(mLastZ_Rate);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterZ_Rate] = counter;
      counter++;
   }

   if (mPublishBitmap & PublishAzimuth)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterAzimuth);
      partPtr->SetFloat32Value(mLastAzimuth);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterAzimuth] = counter;
      counter++;
   }
   if (mPublishBitmap & PublishElevation)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterElevation);
      partPtr->SetFloat32Value(mLastElevation);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterElevation] = counter;
      counter++;
   }
   if (mPublishBitmap & PublishRotation)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterRotation);
      partPtr->SetFloat32Value(mLastRotation);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterRotation] = counter;
      counter++;
   }

   if (mPublishBitmap & PublishAzimuthRate)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterAzimuthRate);
      partPtr->SetFloat32Value(mLastAzimuthRate);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterAzimuthRate] = counter;
      counter++;
   }
   if (mPublishBitmap & PublishElevationRate)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterElevationRate);
      partPtr->SetFloat32Value(mLastElevationRate);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterElevationRate] = counter;
      counter++;
   }
   if (mPublishBitmap & PublishRotationRate)
   {
      DisEntityPart* partPtr = new DisEntityPart();
      partPtr->SetTypeDesignator(0); // articulated part
      partPtr->SetChangeIndicator(mChangeChecker);
      partPtr->SetPartAttachedTo(mParentType);
      partPtr->SetParameterType(DisEntityPart::ParameterRotationRate);
      partPtr->SetFloat32Value(mLastRotationRate);
      aEntityStatePtr->AddEntityPart(partPtr); // note: the aEntityStatePtr owns the part, we save an index
      mIndexList[DisEntityPart::ParameterRotationRate] = counter;
      counter++;
   }

   return counter;
}


//! Update the articulation parameter records for this articulated part.
bool WsfDisArticulatedPart::UpdateArticulationRecords(DisEntityState* aEntityStatePtr,
                                                      double          aPosDeltaThresh,
                                                      double          aOriDeltaThresh)
{
   bool updated = false;
   // first check to see if we are outside of dead-reckoning's allowable error
   // for now I am just going to check for changes... should fix before I am done!

   // a special update needs to occur for early warning radars that spin around 360
   if (mPublishBitmap & (PublishAzimuth | PublishAzimuthRate))
   {
      // check if this is a sensor that needs updated in sync with "sweep sync" value in the emissions pdu
      if (mPartPtr->GetPartType() == cCOMPONENT_ROLE<WsfSensor>())
      {
         // is it a 360 degree sensor
         static const double cALMOST_FULL_CIRCLE = 359.999 * UtMath::cRAD_PER_DEG;
         bool fullCircle = ((mPartPtr->GetMaxAzSlew() - mPartPtr->GetMinAzSlew()) >= cALMOST_FULL_CIRCLE);
         if (fullCircle)
         {
            // try to do this dynamic cast last, its expensive
            WsfSensor* sensorPtr = dynamic_cast<WsfSensor*>(mPartPtr);
            if (sensorPtr != nullptr)
            {
               if (sensorPtr->GetMaximumRequestCount() <= 0)
               {
                  // its not a tracking sensor
               }
            }
         }
      }
   }

   double azimuth;
   double elevation;
   double rotation;
   double xyz[3];
   double dx;
   double dy;
   double dz;

   mPartPtr->GetCurrentOrientationECS(azimuth, elevation, rotation);
   mPartPtr->GetLocation(xyz);
   mPartPtr->GetCurrentPositionRateECS(dx, dy, dz);

   double devaz = fabs(azimuth - mLastAzimuth);
   double devel = fabs(elevation - mLastElevation);
   double devro = fabs(rotation - mLastRotation);
   double devx  = fabs(xyz[0] - mLastX);
   double devy  = fabs(xyz[1] - mLastY);
   double devz  = fabs(xyz[2] - mLastZ);
   double devdx = fabs(dx - mLastX_Rate);
   double devdy = fabs(dy - mLastY_Rate);
   double devdz = fabs(dz - mLastZ_Rate);

   if (((mPublishBitmap & PublishAzimuth) && (devaz > aOriDeltaThresh)) ||
       ((mPublishBitmap & PublishElevation) && (devel > aOriDeltaThresh)) ||
       ((mPublishBitmap & PublishRotation) && (devro > aOriDeltaThresh)) ||
       ((mPublishBitmap & PublishX) && (devx > aPosDeltaThresh)) ||
       ((mPublishBitmap & PublishY) && (devy > aPosDeltaThresh)) ||
       ((mPublishBitmap & PublishZ) && (devz > aPosDeltaThresh)) ||
       ((mPublishBitmap & PublishX_Rate) && (devdx > 0.00001)) ||
       ((mPublishBitmap & PublishY_Rate) && (devdy > 0.00001)) || ((mPublishBitmap & PublishZ_Rate) && (devdz > 0.00001)))
   {
      updated = true;
   }

   if (updated)
   {
      mChangeChecker++;

      if (mPublishBitmap & PublishX)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterX]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastX = float(xyz[0]);
         partPtr->SetFloat32Value(mLastX);
      }
      if (mPublishBitmap & PublishY)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterY]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastY = float(xyz[1]);
         partPtr->SetFloat32Value(mLastY);
      }
      if (mPublishBitmap & PublishZ)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterZ]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastZ = float(xyz[2]);
         partPtr->SetFloat32Value(mLastZ);
      }
      if (mPublishBitmap & PublishX_Rate)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterX_Rate]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastX_Rate = float(dx);
         partPtr->SetFloat32Value(mLastX_Rate);
      }
      if (mPublishBitmap & PublishY_Rate)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterY_Rate]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastY_Rate = float(dy);
         partPtr->SetFloat32Value(mLastY_Rate);
      }
      if (mPublishBitmap & PublishZ_Rate)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterZ_Rate]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastZ_Rate = float(dz);
         partPtr->SetFloat32Value(mLastZ_Rate);
      }
      if (mPublishBitmap & PublishAzimuth)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterAzimuth]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastAzimuth = (float)azimuth;
         partPtr->SetFloat32Value(mLastAzimuth);
      }
      if (mPublishBitmap & PublishElevation)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterElevation]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastElevation = (float)elevation;
         partPtr->SetFloat32Value(mLastElevation);
      }
      if (mPublishBitmap & PublishRotation)
      {
         std::vector<DisEntityPart*>::iterator iter(aEntityStatePtr->GetEntityPartBegin());
         DisEntityPart*                        partPtr = iter[mIndexList[DisEntityPart::ParameterRotation]];
         partPtr->SetChangeIndicator(mChangeChecker);
         mLastRotation = (float)rotation;
         partPtr->SetFloat32Value(mLastRotation);
      }
      /*
            if (mPublishBitmap & PublishAzimuthRate)
            {
               DisEntityPart* partPtr = aEntityStatePtr->GetEntityPart(mIndexList[DisEntityPart::ParameterAzimuthRate]);
               partPtr->SetChangeIndicator(mChangeChecker);
               mLastAzimuthRate = mPartPtr->GetAzimuthRate();
               partPtr->SetFloat32Value(mLastAzimuthRate);
            }
            if (mPublishBitmap & PublishElevationRate)
            {
               DisEntityPart* partPtr = aEntityStatePtr->GetEntityPart(mIndexList[DisEntityPart::ParameterElevationRate]);
               partPtr->SetChangeIndicator(mChangeChecker);
               mLastElevationRate = mPartPtr->GetElevationRate();
               partPtr->SetFloat32Value(mLastElevationRate);
            }
            if (mPublishBitmap & PublishRotationRate)
            {
               DisEntityPart* partPtr = aEntityStatePtr->GetEntityPart(mIndexList[DisEntityPart::ParameterRotationRate]);
               partPtr->SetChangeIndicator(mChangeChecker);
               mLastRotationRate = mPartPtr->GetRotationRate();
               partPtr->SetFloat32Value(mLastRotationRate);
            }*/
   }
   return updated;
}

// ============================================================================
WsfDisArticulatedPart::ArticulationP::ArticulationP(const std::string& aBodyTypeName,
                                                    const std::string& aSystemName,
                                                    int                aType,
                                                    int                aParentType,
                                                    unsigned int       aPublishBitmap)
   : mBodyTypeName(aBodyTypeName)
   , mSystemName(aSystemName)
   , mType(aType)
   , mParentType(aParentType)
   , mPublishBitmap(aPublishBitmap)
{
}

WsfDisArticulatedPartList::WsfDisArticulatedPartList(WsfDisInterface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
{
}

//! This is called by WsfDisPlatform to add any articulated parameter records for the platform.
//! The dis_interface command 'articulated_part' defines which platform types and platform part types
//! should have articulation data produced.
int WsfDisArticulatedPartList::AddArticulatedParts(WsfPlatform* aPlatformPtr, WsfDisPlatform* aDisPlatformPtr)
{
   int counter = 0;
   for (std::vector<ArticulationP>::const_iterator it = GetArticulationList().begin(); it != GetArticulationList().end();
        ++it)
   {
      if ((*it).mBodyTypeName == aPlatformPtr->GetType())
      {
         counter = aDisPlatformPtr->AddArticulation((*it).mType,
                                                    (*it).mParentType,
                                                    (*it).mPublishBitmap,
                                                    aPlatformPtr->GetComponent<WsfArticulatedPart>((*it).mSystemName),
                                                    counter);
      }
   }
   return counter;
}

const WsfDisArticulatedPart::ArticulationList& WsfDisArticulatedPartList::GetArticulationList()
{
   return mInterfacePtr->GetArticulationList();
}
