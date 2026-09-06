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
#ifndef L16_COMPUTERPROCESSOR_HPP
#define L16_COMPUTERPROCESSOR_HPP

#include "wsf_l16_export.h"

#include <list>
#include <map>

#include "DisEntityId.hpp"
#include "Messages.hpp"
#include "TrackList.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"

class WsfCommandChain;
class WsfJTIDS_SlotGroup;
class WsfJTIDS_Terminal;
class WsfLocalTrack;
#include "WsfProcessor.hpp"
#include "WsfTrackId.hpp"

namespace wsf
{
namespace comm
{
class Comm;
class Result;
} // namespace comm
} // namespace wsf

namespace WsfL16
{
class ComputerPart;
class JMessage;
class SurveillancePart;
class Interface;

//! Sends and receives link16 messages
class ComputerProcessor : public WsfProcessor
{
public:
   ComputerProcessor(WsfScenario& aScenario);
   ComputerProcessor(const ComputerProcessor& aSrc);
   ~ComputerProcessor() override;

   typedef std::map<WsfStringId, ComputerPart*> PartMap;

   WsfProcessor* Clone() const override;

   const char* GetScriptClassName() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime) override;

   ComputerPart* CreatePart(const std::string& aName, const std::string& aType);

   wsf::comm::Comm*   GetComm() { return mCommPtr; }
   WsfJTIDS_Terminal* GetJTIDS_Comm() { return mUsingJTIDS ? (WsfJTIDS_Terminal*)mCommPtr : nullptr; }

   //! Contains general configuration options, and takes
   //! advantage of the default copy constructor
   struct Configuration
   {
      DisEntityId  mEntityId;
      unsigned int mTrackNumber;
      bool         mIsC2;
      bool         mIsSimulated;
      bool         mIndirectSender;
      std::string  mCallsign;
      int          mSpecificTypeID;
   };

   //! Returns a reference to the Computer's configuration
   /*const*/ Configuration& Config() { return mConfig; }

   unsigned int GetTrackNumber() const { return mConfig.mTrackNumber; }

   // finds the l16 processor and returns the controller track number
   static WSF_L16_EXPORT unsigned int GetPlatformTrackNumber(WsfPlatform& aPlatform);

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   virtual void ProcessJMessage(double aSimTime, const Messages::InitialBase& aMessage);

   void SentJMessage(double aSimTime, ComputerPart* aPartPtr, JMessage& aMessage, WsfCommandChain* aCommandChainPtr);

   bool DisOutputEnabled() const { return mOutputDis; }

   bool WSF_OutputEnabled() const { return mOutputWSF; }

   TrackList& GetPPLI_List() { return mPPLI_TrackList; }

   const PartMap& GetParts() { return mParts; }

   ComputerPart* FindPart(WsfStringId aNameId);

   static void RegisterType(UtScriptTypes& aTypes);

   bool GetTrackNumber(const WsfLocalTrack& aLocalTrack, int& aTrackNumber);

   void ClearTrackNumber(const WsfLocalTrack& aLocalTrack);

   void SetTrackNumber(WsfLocalTrack& aLocalTrack, int aTrackNumber, int aSourceTrackNumber);

   Interface* GetInterface() const { return mInterfacePtr; }
   // static void AddPartType(const std::string& aType,
   //                         ComputerPart*      aPartPtr);
private:
   void ProcessMessageP(double             aSimTime,
                        wsf::comm::Comm*   aXmtrCommPtr,
                        wsf::comm::Comm*   aReceiverCommPtr,
                        const WsfMessage&  aMessage,
                        wsf::comm::Result& aResult);

   // static void InitializePartTypes();
   void HandleMessageSent(double aSimTime, const WsfMessage* aMessagePtr, int aSlotNumber, WsfJTIDS_SlotGroup* aSlotGroup);

   void HandlePPLI_Message(double aSimTime, const Messages::InitialBase& aMessage);

   void FillPPLI_Track(double aSimTime, WsfTrack& aTrack, const Messages::InitialBase& aPPLI);

   void HandleTrackMessage(double aSimTime, const Messages::InitialBase& aMessage);

   void FillSurvTrack(double aSimTime, WsfTrack& aTrack, const class TrackMessageProxy& aTrackMsg);

   bool IsJ11_1_Valid(const Messages::InitialBase* aMessage);

   void LoadPart(UtInput& aInput, ComputerPart* aProcessor);

   bool MessageShouldBeSent(const Messages::InitialBase* aMessage);

   void SetTrackColorFromIFF(WsfTrack* aTrackPtr, int aIFF);

   void SetPlatform(WsfPlatform* aPlatformPtr) override;


   Configuration     mConfig;
   PartMap           mParts;
   wsf::comm::Comm*  mCommPtr;
   WsfStringId       mCommNameId;
   bool              mOutputDis;
   bool              mOutputWSF;
   bool              mUsingJTIDS;
   bool              mIgnoreSurveillanceTracks;
   UtCallbackHolder  mCallbacks;
   TrackList         mPPLI_TrackList;
   TrackList         mSurvTrackList;
   SurveillancePart* mSurvPart;

   std::map<WsfTrackId, int>  mTrackNumberMapping;
   std::map<int, WsfStringId> mIFF_ColorMap;
   Interface*                 mInterfacePtr;
   // static PartMap                               mBasePartTypes;
};

} // namespace WsfL16

#endif
