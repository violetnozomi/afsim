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

#ifndef WSFDISPLAYBACKDEVICE_HPP
#define WSFDISPLAYBACKDEVICE_HPP

#include "wsf_export.h"

#include <set>
#include <string>

#include "DisEntityId.hpp"
class DisEntityState;
class DisPdu;
class DisPduFactory;
class GenFileI;
#include "WsfDisDevice.hpp"
class WsfDisInterface;
class WsfPlatform;

//! Provide the ability to use a recorded DIS stream (playback files) as an input stream.
//!
//! Entities that are created from this device are a sort of hybrid entity. In many regards they are treated
//! as local entities, that is they have Entity State PDU's written to output streams. But their motion comes
//! from Entity State PDU's read from the file (so they are like 'external'
//! entities in this regard).
//!
//! This was initially created so we could read DIS recording files produced by SLAMEM. It has a good traffic
//! model and it was easier to simply read the generated data rather than trying to replicate what they do.
//!
//! @note: At the current time only Entity State PDU's are used from the file. All other PDU's are ignored.
class WSF_EXPORT WsfDisPlaybackDevice : public WsfDisDevice
{
public:
   static DisUint16 cMIN_PLAYBACK_SITE;
   static DisUint16 cMAX_PLAYBACK_SITE;

   WsfDisPlaybackDevice(const std::string& aFileName, WsfDisInterface* aInterfacePtr);

   ~WsfDisPlaybackDevice() override;

   DisPdu* GetPdu(DisPduFactory* aPduFactoryPtr) override;

   // Provide required implementation of abstract base class method.
   // This is not an output device, so it doesn't do anything.
   void PutPduP(double aSimTime, DisPdu& aPdu) override {}

   static void EntityDeleted(const DisEntityId& aEntityId);

   //! the supplied entity ID associated with a playback entity.
   static bool IsPlaybackEntity(const DisEntityId& aEntityId)
   {
      DisUint16 site = aEntityId.GetSite();
      return ((site >= cMIN_PLAYBACK_SITE) && (site <= cMAX_PLAYBACK_SITE));
   }

   static void PreparePlaybackPlatform(double aSimTime, WsfPlatform* aPlatformPtr, DisEntityState* aEntityPtr);
   static void Reset();

private:
   void ReadNextPdu(DisPduFactory* aPduFactoryPtr);

   std::string mFileName;
   GenFileI*   mFilePtr;
   bool        mIsCME_File;
   bool        mEOF;

   //! The site to be used that represents the PDU's coming from this stream.
   DisUint16 mSiteNumber;
   DisPdu*   mNextPduPtr;
   double    mNextPduTime;

   static DisUint16 sNextSiteNumber;

   static std::set<DisEntityId> sDeletedEntities;
};

#endif
