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

#include "dis/WsfDisPlaybackDevice.hpp"

#include <cstring>

#include "DisEntityState.hpp"
#include "DisIO.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisPduFactory.hpp"
#include "DisTime.hpp"
#include "GenFileI.hpp"
#include "UtLog.hpp"
#include "WsfComm.hpp"
#include "WsfMover.hpp"
#include "WsfProcessor.hpp"
#include "WsfSensor.hpp"
#include "dis/WsfDisInterface.hpp"

// Enough for 16 playback streams. The should be numbers that shouldn't be used by users, but must
// be below DisEntity::RQST_ASSIGN_ID (0xFFFE).

DisUint16 WsfDisPlaybackDevice::cMIN_PLAYBACK_SITE = 65000U;
DisUint16 WsfDisPlaybackDevice::cMAX_PLAYBACK_SITE = 65015U;

DisUint16             WsfDisPlaybackDevice::sNextSiteNumber = WsfDisPlaybackDevice::cMIN_PLAYBACK_SITE;
std::set<DisEntityId> WsfDisPlaybackDevice::sDeletedEntities;

// =================================================================================================
WsfDisPlaybackDevice::WsfDisPlaybackDevice(const std::string& aFileName, WsfDisInterface* aInterfacePtr)
   : WsfDisDevice(aInterfacePtr)
   , mFileName(aFileName)
   , mFilePtr(nullptr)
   , mIsCME_File(false)
   , mEOF(false)
   , mSiteNumber(0)
   , mNextPduPtr(nullptr)
   , mNextPduTime(-1.0)
{
   mDeviceType = cDEVICE_PLAYBACK;

   mFilePtr = new GenFileI(aFileName.c_str(), GenBuf::BigEndian);
   if (!mFilePtr->IsValid())
   {
      delete mFilePtr;
      mFilePtr = nullptr;
      auto out = ut::log::error() << "Could not open file.";
      out.AddNote() << "File: " << aFileName;
      throw OpenError();
   }

   // Assign the site number for the entity ID's that originate from this stream.

   if (sNextSiteNumber > cMAX_PLAYBACK_SITE)
   {
      auto out = ut::log::error() << "Too many 'playback' files specified.";
      out.AddNote() << "Provided: " << sNextSiteNumber - cMIN_PLAYBACK_SITE + 1;
      out.AddNote() << "Limit: " << cMAX_PLAYBACK_SITE - cMIN_PLAYBACK_SITE + 1;
      throw OpenError();
   }

   mSiteNumber = sNextSiteNumber;
   ++sNextSiteNumber;

   // Determine if this is a standard CME replay file or the recording of a raw DIS stream (such as from SLAMEM)

   mIsCME_File = false;
   char hdrStr[256];
   mFilePtr->GetArray(hdrStr, 10);
   if (memcmp(hdrStr, "CME Replay", 10) == 0)
   {
      mFilePtr->Get(hdrStr + 10);
      mIsCME_File = true;
   }
   else
   {
      // Not a standard CME replay file. Reset the buffer pointers so reading starts at the beginning.
      mFilePtr->SetGetOffset(0);
   }
}

// =================================================================================================
WsfDisPlaybackDevice::~WsfDisPlaybackDevice()
{
   delete mFilePtr;
}

// =================================================================================================
DisPdu* WsfDisPlaybackDevice::GetPdu(DisPduFactory* aPduFactoryPtr)
{
   DisPdu* pduPtr = nullptr;
   while ((!mEOF) && (pduPtr == nullptr))
   {
      // Load the next PDU if one isn't already loaded.
      if (mNextPduPtr == nullptr)
      {
         ReadNextPdu(aPduFactoryPtr);
      }

      // Make sure this PDU is not in advance of the current time.
      if ((mNextPduPtr != nullptr) && (mNextPduTime <= mInterfacePtr->GetSimTime()))
      {
         pduPtr      = mNextPduPtr;
         mNextPduPtr = nullptr;

         // If the originating entity for this PDU has been deleted then just ignore the PDU.
         if (sDeletedEntities.find(pduPtr->GetOriginatingEntity()) != sDeletedEntities.end())
         {
            delete pduPtr;
            pduPtr = nullptr;
         }
      }
      else
      {
         // No PDU's remain, or the current PDU is in the future.
         // If the later, it is left so it is returned when it becomes time.
         break;
      }
   }

   return pduPtr;
}

// =================================================================================================
//! Update the list of deleted entities.
//! This is called by WsfDisInterface::DeletePlatform() whenever a platform is deleted.
// static
void WsfDisPlaybackDevice::EntityDeleted(const DisEntityId& aEntityId)
{
   if (IsPlaybackEntity(aEntityId))
   {
      sDeletedEntities.insert(aEntityId);
   }
}

// =================================================================================================
//! Reset static data back to its initial state.
// static
void WsfDisPlaybackDevice::Reset()
{
   sNextSiteNumber = cMIN_PLAYBACK_SITE;
   sDeletedEntities.clear();
}

// =================================================================================================
//! Prepare a playback platform for use.
//!
//! This is called by WsfDisInterface::AddExternalPlatform after the platform has been instantiated
//! but before it is added to the simulation.
//!
//! @param aSimTime      The current simulation time.
//! @param aPlatformPtr  The platform about to be added to the simulation.
//! @param aEntityPtr    The entity state that is associated with the platform.
// static
void WsfDisPlaybackDevice::PreparePlaybackPlatform(double aSimTime, WsfPlatform* aPlatformPtr, DisEntityState* aEntityPtr)
{
   for (WsfComponentList::RoleIterator<WsfPlatformPart> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      switch (iter->GetPartType())
      {
      case cCOMPONENT_ROLE<WsfMover>():
         break;
      default:
         (*iter)->SetInitiallyTurnedOn(false);
         break;
      }
   }
}

// =================================================================================================
//! Read the next PDU from the input stream.
// private
void WsfDisPlaybackDevice::ReadNextPdu(DisPduFactory* aPduFactoryPtr)
{
   DisPdu* pduPtr = nullptr;
   while ((pduPtr == nullptr) && (!mEOF))
   {
      // Determine if the buffer needs to be refilled. The buffer will be empty on entry if this is
      // the first call, or if the previous 'GetPdu' processed all the data in the buffer.

      long int bytesLeft = mFilePtr->GetGetOffset(GenBuf::FromDataEnd);
      if (bytesLeft == 0)
      {
         // No data is left in the buffer so attempt to refill it.
         if (mFilePtr->Receive(0) > 0)
         {
            bytesLeft = mFilePtr->GetGetOffset(GenBuf::FromDataEnd);
         }
      }

      // If there is data then attempt to read the PDU.
      //
      // If there is no data in the buffer at this point then the end-of-file has been encountered.
      // (The logic above tried to read data, but there was none left to be read.

      if (bytesLeft > 0)
      {
         if (mIsCME_File)
         {
            double tempTime;
            *mFilePtr >> tempTime;
            pduPtr = DisPdu::Create(*mFilePtr, aPduFactoryPtr);
            if (pduPtr != nullptr)
            {
               // Update the timestamp in the PDU so DisTime::GetAbsoluteTime() works.
               pduPtr->SetTime(tempTime);
            }
         }
         else
         {
            pduPtr = DisPdu::Create(*mFilePtr, aPduFactoryPtr);
         }
         if (pduPtr != nullptr)
         {
            if (pduPtr->GetPduType() == DisEnum::Pdu::Type::EntityState)
            {
               DisEntityState* esPtr = static_cast<DisEntityState*>(pduPtr);
               DisEntityId     entityId(esPtr->GetEntityId());
               entityId.SetSite(mSiteNumber);
               // SLAMEM doesn't assign an application number.
               if (entityId.GetApplication() == 0)
               {
                  entityId.SetApplication(1);
               }
               esPtr->SetEntityId(entityId);
            }
            else
            {
               // All PDU's except Entity State are ignored...
               delete pduPtr;
               pduPtr = nullptr;
            }
         }
      }
      else
      {
         mEOF   = true;
         pduPtr = nullptr;
      }
   }

   if (pduPtr != nullptr)
   {
      mNextPduPtr  = pduPtr;
      mNextPduTime = pduPtr->GetAbsoluteTime();
   }
}
