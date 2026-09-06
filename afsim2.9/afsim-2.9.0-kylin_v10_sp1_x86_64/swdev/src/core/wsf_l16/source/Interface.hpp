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
#ifndef L16_LINK16INTERFACE_HPP
#define L16_LINK16INTERFACE_HPP

#include <list>
#include <map>
#include <set>
#include <utility>

class UtInput;
#include "DisEntityId.hpp"
#include "HeaderTDL.hpp"
#include "Interface_J11.hpp"
#include "MessagesPreDef.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
class WsfDisInterface;
class WsfDisSignal;
class DisTransmitter;
#include "WsfEvent.hpp"
class WsfPlatform;
#include "WsfSimulationExtension.hpp"
#include "WsfTrack.hpp"

namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf

#ifdef _WIN32
#undef GetCurrentTime // avoid conflict with windows macro
#endif

namespace WsfL16
{
class ComputerProcessor;
class SlotParameters;

struct InterfaceSetup
{
   enum TdlHeaderType
   {
      cTDL_6,
      cTDL_100
   };

   enum TdlSwapType
   {
      cWSF,
      cTRAP_TOOL
   };

   InterfaceSetup(WsfScenario& aScenario);
   bool ProcessInput(UtInput& aInput);
   void PrintScriptUsage();
   void PrintScriptUsageRst();
   bool SetNPGTable(int aMsgType, int aNPG);
   bool IsRequested() const { return mIsRequested; }


   TdlHeaderType                   mHeaderType;
   TdlSwapType                     mSwapMethod;
   int                             mTSALevel;
   int                             mTimeSlotType;
   Messages::HeaderTDL100          mHeaderTDL100;
   Messages::WSF_HeaderTDL100      mWSF_HeaderTDL100;
   int                             mSlotOffset;
   Messages::HeaderTDL100          mTdlHeader;
   bool                            mIsRequested;
   bool                            mDebugEnabled;
   bool                            mSendPPLI;
   bool                            mFillWith31_7;
   bool                            mIgnoreInboundMessages;
   double                          mSecondsTillSimStartSinceJan1_1900;
   double                          mSecondsSinceMidnight;
   bool                            mIncludeJ3xTrackDEI; // Set true to include J3.X DIS Entity Id
   std::map<unsigned, WsfStringId> mAirPlatformTypes;   // from air specific types
   std::map<WsfStringId, unsigned> mAirSpecificTypes;   // from platform type
   std::map<unsigned, WsfStringId> mLandPlatformTypes;  // from land specific types
   std::map<WsfStringId, unsigned> mLandSpecificTypes;  // from platform type

   Interface_J11_Setup mInterface_J11_Setup;

   class MessageFilter
   {
   public:
      bool IsFiltered(int aLabel, int aSublabel);

      std::set<int>                 mFilteredMessageTypes;
      std::set<std::pair<int, int>> mFilteredSpecificTypes;
   };

   std::map<size_t, MessageFilter> mPlatformFilters;
   MessageFilter                   mGlobalFilters;

   std::set<std::pair<int, int>> mNPGTable;

   WsfScenario* mWsfSetupPtr;
};

//! WsfL16::Interface is primarily an interface to DIS using
//! the DisSignal PDU.
class Interface : public WsfSimulationExtension, public InterfaceSetup
{
public:
   Interface(InterfaceSetup& aScenario);
   void AddedToSimulation() override;

   ~Interface() override;

   bool Initialize() override;

   // These callbacks allow other interfaces (e.g. JREAP) to send/receive JMessages
   // through the WsfL16 interface
   UtCallbackListN<void(double, const WsfL16::Messages::InitialBase&)> JMessageReceived;
   UtCallbackListN<void(const WsfL16::Messages::InitialBase&)>         JMessageSent;

   static int  GetJ3xIdentityFromIFF(WsfTrack::IFF_Status aIFF_Status);
   static void SetIFF_FromJ3xIdentity(WsfTrack& aTrack, int aJ3xIdentity);

   //! Returns a boolean used to determine whether or not to include track DIS entity id inside J3.X messages.
   bool IncludeJ3xTrackDisEntityId() { return mIncludeJ3xTrackDEI; }

   void SendJMessage(double aSimTime, WsfPlatform& aSender, const Messages::InitialBase* aMessage);

   void SendJMessage(double                       aSimTime,
                     WsfPlatform&                 aSender,
                     const Messages::InitialBase* aMessage,
                     const SlotParameters&        aSlotParameters);

   void SendRawJMessage(double aSimTime,
                        // WsfPlatform& aSender,
                        WsfPlatform* aSender,
                        const char*  aDataPtr,
                        int          aMessageBytes,
                        int          aNPG);

   void SendUserDefinedJMessage(double          aSimTime,
                                WsfPlatform*    aSender,
                                unsigned short* aMessage,
                                int             aNPG,
                                int             aMessageBytes,
                                int             aLabel,
                                int             aSubLabel,
                                int             aNumMsgs);

   void SendUserDefinedJMessage(double                aSimTime,
                                WsfPlatform*          aSender,
                                unsigned short*       aMessage,
                                const SlotParameters& aSlotParameters,
                                int                   aMessageBytes,
                                int                   aLabel,
                                int                   aSubLabel,
                                int                   aNumMsgs);

   void FilterMessageSend(WsfPlatform& aSenderPlatform, int aLabel, int aSubLabel = -1);

   void UnfilterMessageSend(WsfPlatform& aSenderPlatform, int aLabel, int aSubLabel = -1);

   void FilterMessageSend(WsfPlatform* aSenderPlatform, int aLabel, int aSubLabel = -1);

   void UnfilterMessageSend(WsfPlatform* aSenderPlatform, int aLabel, int aSubLabel = -1);

   unsigned    GetAirSpecificType(WsfStringId aPlatformTypeId);
   unsigned    GetLandSpecificType(WsfStringId aPlatformTypeId);
   WsfStringId GetAirPlatformType(unsigned aAirSpecificTypeId);
   WsfStringId GetLandPlatformType(unsigned aLandSpecificTypeId);

   WsfDisInterface* GetDisInterface() const { return mDisInterfacePtr; }

   //! @name Provision to allow an external method to provide the track numbers for a weapon and controller.
   //! This decouples the Link-16 interface from the services that are launching and controlling weapons.
   //@{
   using GetWeaponTrackNumbersFn = void (*)(WsfSimulation& aSimulation,
                                            size_t         aWeaponIndex,
                                            unsigned int&  aWeaponTrackNumber,
                                            unsigned int&  aControllerTrackNumber);

   void SetWeaponTrackNumbersFunction(GetWeaponTrackNumbersFn aFunctionPtr)
   {
      mGetWeaponTrackNumberFunctionPtr = aFunctionPtr;
   }

   void GetWeaponTrackNumbers(size_t aWeaponIndex, unsigned int& aWeaponTrackNumber, unsigned int& aControllerTrackNumber);

   static WSF_L16_EXPORT WsfPlatform* GetControllerFromTrackNumber(WsfSimulation* aSimulationPtr,
                                                                   unsigned int   aTrackNumber);

   static WSF_L16_EXPORT void SetPlatformTrackNumber(WsfPlatform* aPlatformPtr, unsigned int aTrackNumber);


   //@}
private:
   bool IsFiltered(WsfPlatform& aPlatform, const Messages::InitialBase* aMsg);

   bool IsFiltered(WsfPlatform* aPlatform, int aLabel, int aSublabel);


   void HandleSignalPDU(WsfDisInterface* aInterfacePtr, const WsfDisSignal& aPdu);

   void CreateHeader(double aSimTime, const SlotParameters& aSlotParameters);

   void CreateHeaderWSF(double aSimTime, const SlotParameters& aSlotParameters);

   // unsigned int ConvertWsfSlotNumberToDIS(int aSlotNumber);
   int ConvertDIS_SlotNumberToWsf(unsigned int aSlotNumber);

   void GetCurrentTime(double        aSimTime,
                       unsigned int  aWsfSlotIndex,
                       unsigned int& aSecondsSinceJan1_1900,
                       unsigned int& aSecondFraction,
                       unsigned int& aEpoch,
                       unsigned int& aTimeSlotIndex);

   void MessageQueued(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage, size_t aQueueSize);

   void MessageTransmitted(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage);

   void MessageTransmitEnded(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage);

   void MessageTransmittedHeartbeat(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage);

   //! Send Transmitter PDU associated with the Signal PDU.
   //! @param aSimTime           The current simulation time of this event.
   //! @param aSenderPlatformPtr The pointer to the platform class of the platform initiating the track report.
   //! @param aDisEntityId       The DisEntityId of the sending platform
   //! @param aRadioId           The radio id for the FADL communication device sending the track report message.
   //! @param aStatus            Status of the FADL communication device, on/off/transmitting
   //! @param aPduPtr            A pointer to the Transmitter PDU being sent.
   //! @returns a copy of the last Transmitter PDU being sent.  If 0, then sending state OFF, transmission ended signal.
   void SendTransmitterPDU(double       aSimTime,
                           WsfPlatform* aSenderPlatformPtr,
                           DisEntityId  aDisEntityId,
                           int          aRadioId,
                           unsigned int aStatus);

   //! @name J11 Message Processing
   //@{
   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);
   //@}

   Interface_J11*          mInterface_J11;
   UtCallbackHolder        mCallbacks;
   WsfDisInterface*        mDisInterfacePtr;
   GetWeaponTrackNumbersFn mGetWeaponTrackNumberFunctionPtr;
};

} // namespace WsfL16

#endif
