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

#ifndef WSFDISARTICULATEDPART_HPP
#define WSFDISARTICULATEDPART_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

class DisEntityPart;
class DisEntityState;
class UtInput;
class WsfArticulatedPart;
class WsfDisPlatform;
class WsfPlatform;
class WsfDisInterface;

//! An internal class for maintaining articulated part information.
class WSF_EXPORT WsfDisArticulatedPart
{
public:
   class ArticulationP
   {
   public:
      ArticulationP(const std::string& aBodyTypeName,
                    const std::string& aSystemName,
                    int                aType,
                    int                aParentType,
                    unsigned int       aPublishBitmap);

      std::string  mBodyTypeName;
      std::string  mSystemName;
      int          mType;
      int          mParentType;
      unsigned int mPublishBitmap;
   };

   typedef std::vector<ArticulationP> ArticulationList;


   WsfDisArticulatedPart(int aType, int aParentType, unsigned int aPublishBitmap, WsfArticulatedPart* aPartPtr);

   ~WsfDisArticulatedPart();


   int CreateArticulationRecords(DisEntityState* aEntityStatePtr, int aStartCount);

   bool UpdateArticulationRecords(DisEntityState* aEntityStatePtr, double aPosDeltaThresh, double aOriDeltaThresh);

   // Publication bitmask.
   enum
   {
      PublishX             = 1,
      PublishX_Rate        = 2,
      PublishY             = 4,
      PublishY_Rate        = 8,
      PublishZ             = 16,
      PublishZ_Rate        = 32,
      PublishAzimuth       = 64,
      PublishAzimuthRate   = 128,
      PublishElevation     = 256,
      PublishElevationRate = 512,
      PublishRotation      = 1024,
      PublishRotationRate  = 2048
   };

private:
   int                 mType;
   int                 mParentType;
   unsigned int        mPublishBitmap;
   WsfArticulatedPart* mPartPtr;
   int                 mChangeChecker;
   int                 mIndexList[32];
   float               mLastX;
   float               mLastY;
   float               mLastZ;
   float               mLastX_Rate;
   float               mLastY_Rate;
   float               mLastZ_Rate;
   float               mLastAzimuth;
   float               mLastElevation;
   float               mLastRotation;
   float               mLastAzimuthRate;
   float               mLastElevationRate;
   float               mLastRotationRate;
};

class WsfDisArticulatedPartList
{
public:
   typedef WsfDisArticulatedPart::ArticulationP ArticulationP;

   WsfDisArticulatedPartList(WsfDisInterface* aInterfacePtr);

   int AddArticulatedParts(WsfPlatform* aPlatformPtr, WsfDisPlatform* aDisPlatformPtr);

   const WsfDisArticulatedPart::ArticulationList& GetArticulationList();

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
