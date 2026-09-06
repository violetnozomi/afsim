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

#include "Interface_J11.hpp"

#include <iostream>

// L16
#include "ComputerPart.hpp"
#include "ComputerProcessor.hpp"
#include "Interface.hpp"
#include "JMessage.hpp"
#include "Message11_1.hpp"

// Utilities
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"

// WSF
#include "DisIO.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommUtil.hpp"
#include "WsfCommandChain.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfJTIDS_Terminal.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisWeaponEvents.hpp"

namespace WsfL16
{

UtCallbackListN<void(const DisEntityId&, const unsigned int, unsigned int&)> Interface_J11::FindWeaponIndex;
UtCallbackListN<void(WsfWeapon*, WsfDisPlatform*&)>                          Interface_J11::UpdateFiringEntity;

Interface_J11_Setup::Interface_J11_Setup()
   : mDebugEnabled(false)
   , mDebugPrintMessages(false)
   , mAllowAnyComm(false)
   , mUseTimeOfTargetMeasurement(false)
   , mWIFT_Interval(1.0)
{
}

// ============================================================================
//! Process the j11 block.
//! @param aInput    [input]  The input stream.
//! @return 'true' if the command was recognized (and processed) or 'false'
//! if the command was not one recognized by this class.
bool Interface_J11_Setup::ProcessInput(UtInput& aInput)
{
   // Read until end of block
   bool ok(false);
   if (aInput.GetCommand() == "j11")
   {
      ok = true;
      UtInputBlock block(aInput, "end_j11");
      while (block.ReadCommand())
      {
         std::string command(aInput.GetCommand());
         if (command == "allow_any_comm")
         {
            mAllowAnyComm = true;
         }
         else if (command == "wift_interval")
         {
            aInput.ReadValueOfType(mWIFT_Interval, UtInput::cTIME);
            aInput.ValueGreater(mWIFT_Interval, 0.0);
         }
         else if (command == "network_enabled_weapon_type")
         {
            std::string weaponSystemType;
            aInput.ReadValue(weaponSystemType);
            int weaponTypeOfNew;
            aInput.ReadValue(weaponTypeOfNew);
            AddTypeOfNEW(weaponSystemType, weaponTypeOfNew);
         }
         else if (command == "use_time_of_target_measurement")
         {
            mUseTimeOfTargetMeasurement = true;
         }
         else if (command == "print_messages")
         {
            mDebugPrintMessages = true;
         }
         else if (command == "controller_tn_map")
         {
            UtInputBlock controllerBlock(aInput, "end_controller_tn_map");
            while (controllerBlock.ReadCommand())
            {
               std::string cmd(aInput.GetCommand());
               if (cmd == "controller")
               {
                  std::string  controllerName;
                  unsigned int controllerTrackNumber;
                  aInput.ReadValue(controllerName);
                  aInput.ReadValue(controllerTrackNumber);
                  WsfStringId controllerNameId(controllerName);
                  mControllerMap[controllerTrackNumber] = controllerNameId;
               }
            }
         }
         else
         {
            return false;
         }
      }
   }
   return ok;
}

void Interface_J11_Setup::AddTypeOfNEW(WsfStringId aPlatformType, int aNEW_Type)
{
   using namespace Messages::J11_0;

   // Quick return
   if ((aNEW_Type < 0) || (aNEW_Type >= 64))
   {
      return;
   }

   // Add the data
   WeaponNEW_TypeField::WeaponNEW_TypeType NEW_TypeEnum = static_cast<WeaponNEW_TypeField::WeaponNEW_TypeType>(aNEW_Type);
   mTypeOfNEW_Map.insert(TypeOfNEW_MapTypePair(aPlatformType, NEW_TypeEnum));
}


// ****************************************************************************
// Public

// ============================================================================
Interface_J11::Interface_J11(Interface* aInterfacePtr, Interface_J11_Setup& aScenario)
   : Interface_J11_Setup(aScenario)
   , mInterfacePtr(aInterfacePtr)
   , mFiringPlatformToWeaponCountMap()
   , mNEW_TrackNumber()
   , mNEW_ControllerTrackNumber()
   , mPendingDirectorTrackNumber()
   , mDirectorToWeaponMap()
   , mResultMap()
{
   // Fill the termation result map with data
   CreateTerminationResultMap();
}

// ============================================================================
// Virtual
Interface_J11::~Interface_J11() {}

// Find a platform based on its track number found on the link16 computer
WsfPlatform* Interface_J11::FindPlatform(unsigned int aTrackNumber)
{
   WsfPlatform* platformPtr = nullptr;
   for (unsigned int i = 0u; i < GetSimulation()->GetPlatformCount(); ++i)
   {
      platformPtr = GetSimulation()->GetPlatformEntry(i);
      if ((platformPtr != nullptr) && (!platformPtr->IsDeleted()))
      {
         for (WsfComponentList::RoleIterator<WsfProcessor> iter(*platformPtr); !iter.AtEnd(); ++iter)
         {
            if (iter->IsA_TypeOf("WSF_LINK16_COMPUTER"))
            {
               ComputerProcessor* l16Ptr = dynamic_cast<ComputerProcessor*>(*iter);
               if (l16Ptr != nullptr)
               {
                  l16Ptr->Config().mTrackNumber = aTrackNumber;
                  {
                     return platformPtr;
                  }
               }
            }
         }
      }
   }
   return platformPtr;
}

// ============================================================================
//! Process a J11 message
//! @param aSenderEntityId    [input]  DIS Entity ID of the sender.
//! @param aMessage           [input]  JTIDS J11 message
//! @param aSubLabel          [input]  The message sub-label
void Interface_J11::ProcessJ11Message(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage, int aSubLabel)
{
   // Get the sub-message type 0, 1, or 2
   switch (aSubLabel)
   {
   case cWEAPON_PESPONSE_STATUS:
      ProcessJ11_0Message(aSenderEntityId, aMessage);
      break;

   case cWEAPON_DIRECTIVE:
      ProcessJ11_1Message(aSenderEntityId, aMessage);
      break;

   case cWEAPON_COORDINATION:
      ProcessJ11_2Message(aSenderEntityId, aMessage);
      break;

   default:
      break;
   }
}
// ============================================================================
//! Process a J11.0 message (Weapon Response/Status)
//! @param aSenderEntityId    [input]  DIS Entity ID of the sender.
//! @param aMessage           [input]  JTIDS J11 message
void Interface_J11::ProcessJ11_0Message(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage)
{
   auto logDebug = ut::log::debug();
   if (mDebugEnabled || mDebugPrintMessages)
   {
      logDebug << "Received J11.0 Message";
      logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
   }
   if (mDebugPrintMessages)
   {
      aMessage.PrintMessage(logDebug);
   }
   // get message source's track number
   unsigned int senderTrackNumber     = aMessage.GetHeader().mSourceTrackNumber;
   unsigned int controllerTrackNumber = 0;

   // get the discrete and process the message
   // const J11_0::Initial* initBasePtr = static_cast<const J11_0::Initial*>(&aMessage);

   WsfL16::Messages::J11_0::Continuation4* c4 =
      static_cast<WsfL16::Messages::J11_0::Continuation4*>(aMessage.FindContinuation(4));
   if (c4 != nullptr)
   {
      controllerTrackNumber = c4->TrackNumberController;
   }

   if (mDebugEnabled || mDebugPrintMessages)
   {
      logDebug.AddNote() << "Sending Entity: " << aSenderEntityId;
      logDebug.AddNote() << "TN Src: " << senderTrackNumber;
      logDebug.AddNote() << "TN Dst: " << controllerTrackNumber;
   }
   WsfPlatform* controllerPlatformPtr = FindPlatform(controllerTrackNumber);
   if (!mControllerMap.empty())
   {
      // For speed first see if the track number was mapped to a controller name in the
      // link16 interface block in the j11 section.
      ControllerMapType::iterator iter = mControllerMap.find(controllerTrackNumber);
      if (iter != mControllerMap.end())
      {
         controllerPlatformPtr = GetSimulation()->GetPlatformByName((*iter).second);
      }
   }
   if (controllerPlatformPtr != nullptr)
   {
      ProcessJMessage(controllerPlatformPtr, aMessage);
   }
}

// ============================================================================
//! Process a J11.1 message (Weapon Directive)
//! @param aSenderEntityId    [input]  DIS Entity ID of the sender.
//! @param aMessage           [input]  JTIDS J11 message
void Interface_J11::ProcessJ11_1Message(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage)
{
   using namespace Messages;

   // Get message source track number
   unsigned int senderTrackNumber = aMessage.GetHeader().mSourceTrackNumber;

   // Get the discrete and process the message
   WsfPlatform*          weaponPlatformPtr = nullptr;
   const J11_1::Initial* initBasePtr       = static_cast<const J11_1::Initial*>(&aMessage);

   // Get the track number addressee
   unsigned int weaponTrackNumber = initBasePtr->TrackNumberAddressee;

   // Write out some debug info
   if (mDebugEnabled || mDebugPrintMessages)
   {
      auto logDebug = ut::log::debug();
      logDebug << "Received J11.1 Message.";
      logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
      logDebug.AddNote() << "Sending entity: " << aSenderEntityId;
      logDebug.AddNote() << "TN Src: " << senderTrackNumber;
      logDebug.AddNote() << "TN Dst: " << weaponTrackNumber;
      if (mDebugPrintMessages)
      {
         aMessage.PrintMessage(logDebug);
      }
   }

   // Update pending director information, if necessary
   UpdateDirector(aSenderEntityId, senderTrackNumber);

   switch (initBasePtr->WeaponDirective)
   {
   case J11_1::WeaponDiscreteField::cHANDOFF_CONTROL_CHANGE_ORDER:
      weaponPlatformPtr = GetWeapon(aSenderEntityId, weaponTrackNumber);
      HandoffControlChange(aSenderEntityId, senderTrackNumber, weaponPlatformPtr, aMessage);
      break;

   case J11_1::WeaponDiscreteField::cIN_FLIGHT_TARGET_UPDATE:
      weaponPlatformPtr = GetWeapon(aSenderEntityId, weaponTrackNumber);
      InFlightTargetUpdate(aSenderEntityId, weaponPlatformPtr, aMessage);
      break;

   default:
      break;
   }

   // JTIDS part processing
   ProcessJMessage(weaponPlatformPtr, aMessage);
}

// ============================================================================
//! Process a J11.2 message (Weapon Coordination)
//! @param aSenderEntityId    [input]  DIS Entity ID of the sender.
//! @param aMessage           [input]  JTIDS J11 message
void Interface_J11::ProcessJ11_2Message(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage)
{
   if (mDebugEnabled || mDebugPrintMessages)
   {
      auto logDebug = ut::log::debug() << "Recv'd J11.2 MEssage.";
      logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
   }
   if (mDebugPrintMessages)
   {
      auto logDebug = ut::log::debug();
      aMessage.PrintMessage(logDebug);
   }
}

// ============================================================================
//! A simulation observer callback to handle weapon firing events
//! @param aSimTime        The current simulation time.
//! @param aEngagementPtr  A pointer to the weapon engagement data.
//! @param aTargetTrackPtr A pointer to the target track (may be 0).
void Interface_J11::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
{
   // Quick return for bad engagement pointer
   if (aEngagementPtr == nullptr)
   {
      return;
   }
   // Quick return bad firing platform and weapon platform
   WsfPlatform* firingPlatformPtr = aEngagementPtr->GetFiringPlatform();
   WsfPlatform* weaponPlatformPtr = aEngagementPtr->GetWeaponPlatform();
   if ((firingPlatformPtr == nullptr) || (weaponPlatformPtr == nullptr))
   {
      return;
   }

   // If the controlling platform is external and weapon is internal and has a comm device
   if (firingPlatformPtr->IsExternallyControlled() && !weaponPlatformPtr->IsExternallyControlled() &&
       IsNetworkEnabledWeapon(weaponPlatformPtr))
   {
      unsigned int weaponPlatformIndex = ut::safe_cast<unsigned int, size_t>(weaponPlatformPtr->GetIndex());
      // get the weapon server values for weapon T number and controller T number
      unsigned int NewWeaponTnumber     = 0;
      unsigned int NewControllerTnumber = 0;
      // query the weapon server directly for the j11 track (T) numbers
      mInterfacePtr->GetWeaponTrackNumbers(weaponPlatformIndex, NewWeaponTnumber, NewControllerTnumber);
      if (NewWeaponTnumber == 0 || NewControllerTnumber == 0)
      {
         auto logWarning = ut::log::warning() << "Interface_J11: Unknown weapon T number and controller T number by "
                                                 "weapon server, no J11 traffic supported.";
         logWarning.AddNote() << "Weapon: " << weaponPlatformPtr->GetName();
         return;
      }
      // set the mappings (from weapon index to T numbers)
      SetNEW_MapData(mNEW_TrackNumber, weaponPlatformIndex, NewWeaponTnumber);
      SetNEW_MapData(mNEW_ControllerTrackNumber, weaponPlatformIndex, NewControllerTnumber);

      // attempt to set complex director mapping (<shooter dis id, weapon T number> to weapon index)
      WsfDisInterface& disInterface         = *mInterfacePtr->GetDisInterface();
      WsfDisPlatform*  disFiringPlatformPtr = disInterface.GetDisPlatform(firingPlatformPtr->GetIndex());
      if (disFiringPlatformPtr != nullptr) // TODO: throw warning if not true?  should shooter DIS entity always be known???
      {
         const DisEntityId& shooterID = disFiringPlatformPtr->GetEntityId();
         SetDirectorToWeapon(shooterID, NewWeaponTnumber, weaponPlatformIndex);
         SetL16ComputerTrackNumber(weaponPlatformPtr, NewWeaponTnumber);

         if (mDebugEnabled)
         {
            auto logDebug = ut::log::debug() << "Interface_J11: Weapon";
            logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
            logDebug.AddNote() << "Weapon T Number: " << NewWeaponTnumber;
            logDebug.AddNote() << "Shooter: " << shooterID;
            logDebug.AddNote() << "Internal/External: "
                               << (firingPlatformPtr->IsExternallyControlled() ? "External" : "Internal");
         }
      }

      // Schedule the heartbeat processing event.
      GetSimulation()->AddEvent(ut::make_unique<WIFT_Event>(aSimTime, this, weaponPlatformPtr));
   }
}

// ============================================================================
//! A simulation observer callback to handle weapon termination events
//! @param aSimTime        The current simulation time.
//! @param aEngagementPtr  A pointer to the weapon engagement data.
void Interface_J11::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   // Quick return for bad engagement pointer
   if (aEngagementPtr == nullptr)
   {
      return;
   }

   // Quick return bad weapon platform
   WsfPlatform* weaponPlatformPtr = aEngagementPtr->GetWeaponPlatform();
   if (weaponPlatformPtr == nullptr)
   {
      return;
   }

   auto weaponPlatformIndex = ut::safe_cast<unsigned int, size_t>(weaponPlatformPtr->GetIndex());

   // Don't send if the controller track number is still pending
   unsigned int controllerTrackNumber = 0U;
   GetNEW_MapData(mNEW_ControllerTrackNumber, weaponPlatformIndex, controllerTrackNumber);
   if (controllerTrackNumber == 0u)
   {
      return;
   }

   // See if interested in this weapon;
   // may need to send a Weapon Self Abort Report or Bomb Hit Indication Report
   unsigned int weaponTrackNumber = 0U;
   GetNEW_MapData(mNEW_TrackNumber, ut::safe_cast<unsigned int, size_t>(weaponPlatformPtr->GetIndex()), weaponTrackNumber);
   if (weaponTrackNumber != 0u)
   {
      // Weapon is found; we are interested!
      std::string extendedResult(aEngagementPtr->GetExtendedResult());
      if (!extendedResult.empty())
      {
         using namespace Messages::J11_0;

         WeaponDetailedStatusField::WeaponDetailedStatusType detailedStatus = GetDetailedStatusInformation(extendedResult);
         switch (detailedStatus)
         {
         case WsfL16::Messages::J11_0::WeaponDetailedStatusField::cNO_STATEMENT:
            if (extendedResult != "No termination")
            {
               WsfPlatform* targetPlatformPtr =
                  GetSimulation()->GetPlatformByIndex(aEngagementPtr->GetTargetPlatformIndex());
               SendBombHitIndicationReport(weaponPlatformPtr, targetPlatformPtr);
            }
            break;

         default:
            SendWeaponSelfAbortReport(weaponPlatformPtr, extendedResult);
            break;
         }
      }

      // TODO: is this sage to do?  testing
      PurgeWeaponRecords(weaponPlatformIndex);
   }
}

// ============================================================================
//! Process a J11.1-11 message
//! @param aSenderEntityId    [input] DIS Entity ID of the sender.
//! @param aSenderTrackNumber [input] The track number of the sender.
//! @param aWeaponPlatformPtr [input] A pointer to the weapon platform
//! @param aMessage           [input] JTIDS J11 message
void Interface_J11::HandoffControlChange(const DisEntityId&           aSenderEntityId,
                                         const unsigned int           aSenderTrackNumber,
                                         WsfPlatform*                 aWeaponPlatformPtr,
                                         const Messages::InitialBase& aMessage)
{
   using namespace Messages;

   // Quick check for null pointer
   if (aWeaponPlatformPtr == nullptr)
   {
      return;
   }

   auto weaponPlatformIndex = ut::safe_cast<unsigned int, size_t>(aWeaponPlatformPtr->GetIndex());

   // Check to see if this is a valid controller
   unsigned int currentControllerTrackNumber = 0u;
   GetNEW_MapData(mNEW_ControllerTrackNumber, weaponPlatformIndex, currentControllerTrackNumber);
   if ((currentControllerTrackNumber != aSenderTrackNumber) && (currentControllerTrackNumber != 0u))
   {
      // ignore
      return;
   }

   // Get the continuation word
   J11_1::Continuation1* cont1Ptr = static_cast<J11_1::Continuation1*>(aMessage.FindContinuation(1));

   // Add the pending hand off information to the map
   SetNEW_MapData(mPendingDirectorTrackNumber, cont1Ptr->TrackNumberHandOff, weaponPlatformIndex);

   // Remove information from mappings if changing controllers
   if (cont1Ptr->ThirdPartySource == J11_1::ThirdPartySourceField::cALTERNATE_CONTROLLER)
   {
      // Remove the weapon platform index to controller track number mapping
      DeleteNEW_MapData(mNEW_ControllerTrackNumber, weaponPlatformIndex);

      // Remove the director to weapon mapping
      unsigned int weaponTrackNumber = 0u;
      GetNEW_MapData(mNEW_TrackNumber, weaponPlatformIndex, weaponTrackNumber);
      DeleteDirectorToWeapon(aSenderEntityId, weaponTrackNumber);
   }
}

// ============================================================================
//! Process a J11.1-5 message
//! @param aSenderEntityId    [input] The entity ID of the originator
//! @param aWeaponPlatformPtr [input] A pointer to the weapon platform
//! @param aMessage           [input] JTIDS J11 message
void Interface_J11::InFlightTargetUpdate(const DisEntityId&           aSenderEntityId,
                                         WsfPlatform*                 aWeaponPlatformPtr,
                                         const Messages::InitialBase& aMessage)
{
   // Quick check for null pointer
   if (aWeaponPlatformPtr == nullptr)
   {
      return;
   }

   // Pack up a WsfTrack with IFTU data
   WsfTrack* trackPtr = new WsfTrack(); // Constructor for a pre-defined track

   // NOTE: In theory the owner of the track should be the sender and not the weapon, but this causes
   // a couple of problems
   //
   // 1) It would be difficult to generate a proper WSF track ID in the senders context.
   // 2) Many weapons that use WsfWeaponTrackProcessor::ProcessTrackUpdate rely on the fact that these
   //    are ownship reports in order to force it into terminal mode. This should probably be reviewed.
   //    In addition, it really wants to overwrite the handoff track, so the track ID below should match
   //    that ID.

   trackPtr->SetPlatformOriginator(aWeaponPlatformPtr);
   double originLocWCS[3];
   aWeaponPlatformPtr->GetLocationWCS(originLocWCS);
   trackPtr->SetOriginatorLocationWCS(originLocWCS);
   trackPtr->SetTrackId(WsfTrackId(aWeaponPlatformPtr->GetNameId(), 1));
   FillTrack(aSenderEntityId, aMessage, trackPtr);

   // Set truth info
   // Assume the current target does not change
   const WsfTrack* currentTrackPtr = aWeaponPlatformPtr->GetTrackManager().GetCurrentTarget();
   if (currentTrackPtr != nullptr)
   {
      trackPtr->SetTargetIndex(currentTrackPtr->GetTargetIndex());
      trackPtr->SetTargetType(currentTrackPtr->GetTargetType());
      trackPtr->SetTargetName(currentTrackPtr->GetTargetName());
   }

   // Receive the message
   WsfTrackMessage message(aWeaponPlatformPtr, *trackPtr); // g++ didn't allow a temporary in the next statement
   ReceiveMessage(aWeaponPlatformPtr, message);

   // Delete the track
   delete trackPtr;
}

// ============================================================================
//! Send a J11.0-3 message
//! @param aWeaponPlatformPtr [input] A pointer to the weapon platform
//! @param aTargetPlatformPtr [input] A pointer to the target platform
void Interface_J11::SendBombHitIndicationReport(WsfPlatform* aWeaponPlatformPtr, WsfPlatform* aTargetPlatformPtr)
{
   // Base
   using namespace Messages::J11_0;
   Initial* msgPtr      = new Initial;
   msgPtr->WeaponStatus = WeaponDiscreteField::cBOMB_HIT_INIDICATION;

   // E0
   msgPtr->AddExtensionWord(0);

   // C13 - weapon
   double locLLA[3] = {0.0};
   aWeaponPlatformPtr->GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
   auto c13Ptr             = ut::make_unique<Continuation13>();
   c13Ptr->WeaponLatitude  = locLLA[0];
   c13Ptr->WeaponLongitude = locLLA[1];
   msgPtr->InsertContinuationWord(std::move(c13Ptr));

   // C14 - weapon
   double velNED[3] = {0.0};
   aWeaponPlatformPtr->GetVelocityNED(velNED);
   auto c14Ptr                        = ut::make_unique<Continuation14>();
   c14Ptr->WeaponHeightAboveEllipsoid = locLLA[2];
   c14Ptr->WeaponVelocityNorth        = velNED[0];
   c14Ptr->WeaponVelocityEast         = velNED[1];
   c14Ptr->WeaponVelocityDown         = velNED[2];
   msgPtr->InsertContinuationWord(std::move(c14Ptr));

   // Target truth information
   if (aTargetPlatformPtr != nullptr)
   {
      // C15
      aTargetPlatformPtr->GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
      auto c15Ptr             = ut::make_unique<Continuation15>();
      c15Ptr->TargetLatitude  = locLLA[0];
      c15Ptr->TargetLongitude = locLLA[1];
      msgPtr->InsertContinuationWord(std::move(c15Ptr));

      // C16
      aTargetPlatformPtr->GetVelocityNED(velNED);
      auto c16Ptr                        = ut::make_unique<Continuation16>();
      c16Ptr->TargetHeightAboveEllipsoid = locLLA[2];
      c16Ptr->TargetVelocityNorth        = velNED[0];
      c16Ptr->TargetVelocityEast         = velNED[1];
      c16Ptr->TargetVelocityDown         = velNED[2];
      msgPtr->InsertContinuationWord(std::move(c16Ptr));
   }
   // else
   //{
   //    msgPtr->AddContinuationWord(15);
   //    msgPtr->AddContinuationWord(16);
   // }

   // Aimpoint
   const WsfTrack* targetTrackPtr = aWeaponPlatformPtr->GetTrackManager().GetCurrentTarget();
   if (targetTrackPtr != nullptr)
   {
      // C17
      targetTrackPtr->GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
      auto c17Ptr              = ut::make_unique<Continuation17>();
      c17Ptr->AimpointLatitude = locLLA[0];
      msgPtr->InsertContinuationWord(std::move(c17Ptr));

      // C18
      auto c18Ptr                          = ut::make_unique<Continuation18>();
      c18Ptr->AimpointLongitude            = locLLA[1];
      c18Ptr->AimpointHeightAboveEllipsoid = locLLA[2];
      msgPtr->InsertContinuationWord(std::move(c18Ptr));
   }
   // else
   //{
   //    msgPtr->AddContinuationWord(17);
   //    msgPtr->AddContinuationWord(18);
   // }

   if (mDebugEnabled)
   {
      auto logDebug = ut::log::debug() << "Attempting to send J11.0 Bomb Hit Indication message.";
      logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
      logDebug.AddNote() << "Weapon: " << aWeaponPlatformPtr->GetName();
   }

   SendMessage(aWeaponPlatformPtr, msgPtr);
}

// ============================================================================
//! Send a J11.0-7 message
//! @param aWeaponPlatformPtr [input] A pointer to the weapon platform
//! @param aExtendedResult    [input] Extended result from weapon
void Interface_J11::SendWeaponSelfAbortReport(WsfPlatform* aWeaponPlatformPtr, const std::string& aExtendedResult)
{
   // Base
   using namespace Messages::J11_0;
   Initial* msgPtr      = new Initial;
   msgPtr->WeaponStatus = WeaponDiscreteField::cSELF_ABORT;

   // C1
   double locLLA[3] = {0.0};
   aWeaponPlatformPtr->GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
   auto c1Ptr                  = ut::make_unique<Continuation1>();
   c1Ptr->Latitude             = locLLA[0];
   c1Ptr->Longitude            = locLLA[1];
   c1Ptr->HeightAboveEllipsoid = locLLA[2];
   msgPtr->InsertContinuationWord(std::move(c1Ptr));

   // C2
   auto c2Ptr = ut::make_unique<Continuation2>();
   // speed
   c2Ptr->Speed = aWeaponPlatformPtr->GetSpeed();
   // course
   double velNED[3] = {0.0};
   aWeaponPlatformPtr->GetVelocityNED(velNED);
   double course = atan2(velNED[1], velNED[0]);
   c2Ptr->Course = course;
   msgPtr->InsertContinuationWord(std::move(c2Ptr));

   // C3
   msgPtr->AddContinuationWord(3);

   // C9
   auto c9Ptr = ut::make_unique<Continuation9>();
   // detailed status information
   c9Ptr->DetailedStatusInformation = GetDetailedStatusInformation(aExtendedResult);
   msgPtr->InsertContinuationWord(std::move(c9Ptr));

   // Set self abort reason in c2
   // self abort reason
   if (c9Ptr->DetailedStatusInformation == WeaponDetailedStatusField::cTARGET_NOT_ACQUIRED)
   {
      c2Ptr->SelfAbortReason = WeaponSelfAbortField::cPRIMARY_TARGET_NOT_DETECTED;
   }
   else if (c9Ptr->DetailedStatusInformation != WeaponDetailedStatusField::cNO_STATEMENT)
   {
      c2Ptr->SelfAbortReason = WeaponSelfAbortField::cFAILURE;
   }

   if (mDebugEnabled)
   {
      auto logDebug = ut::log::debug() << "Attempting to send J11.0 Self Abort Message.";
      logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
      logDebug.AddNote() << "Weapon: " << aWeaponPlatformPtr->GetName();
      logDebug.AddNote() << "Result: " << aExtendedResult;
   }

   SendMessage(aWeaponPlatformPtr, msgPtr);
}

// ============================================================================
//! Periodic sending of the weapon in-flight track (WIFT) report
//! @param aSimTime        The current simulation time.
//! @param aWeaponPlatformPtr [input] A pointer to the weapon platform
double Interface_J11::ProcessWIFT_Event(double aSimTime, WsfPlatform* aWeaponPlatformPtr)
{
   // Don't send if the interval is <= 0.0
   // or if the controller track number is still pending
   unsigned int controllerTrackNumber = 0u;
   auto         weaponPlatformIndex   = ut::safe_cast<unsigned int, size_t>(aWeaponPlatformPtr->GetIndex());

   GetNEW_MapData(mNEW_ControllerTrackNumber, weaponPlatformIndex, controllerTrackNumber);

   if (mWIFT_Interval > 0.0)
   {
      if (controllerTrackNumber != 0u)
      {
         // Send a weapon in-flight track report

         // Base
         using namespace Messages::J11_0;
         Initial* msgPtr      = new Initial;
         msgPtr->WeaponStatus = WeaponDiscreteField::cBASIC_WIFT;

         // Type of NEW
         msgPtr->WeaponType = GetNEW_Type(aWeaponPlatformPtr);

         // weapon Profile
         // Defaults to midcourse while in flight for simplicity
         msgPtr->WeaponProfile = WeaponProfileField::cMIDCOURSE;
         if (IsActive(aWeaponPlatformPtr))
         {
            // Defaults to terminal for simplicity
            msgPtr->WeaponProfile = WeaponProfileField::cTERMINAL;
         }

         // seeker acquisition confidence
         // If self-contributing to the target track then
         // hard code to 3 to be consistent with DREAMS
         // Indicates a sensor is tracking the target
         if (IsTracking(aWeaponPlatformPtr))
         {
            msgPtr->SeekerAquisitionConfidence = SeekerAcquisitionConfidenceField::c60_THROUGH_69;
         }

         // C1
         double locLLA[3] = {0.0};
         aWeaponPlatformPtr->GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
         auto c1Ptr                  = ut::make_unique<Continuation1>();
         c1Ptr->Latitude             = locLLA[0];
         c1Ptr->Longitude            = locLLA[1];
         c1Ptr->HeightAboveEllipsoid = locLLA[2];
         msgPtr->InsertContinuationWord(std::move(c1Ptr));

         // C2
         auto c2Ptr = ut::make_unique<Continuation2>();
         // speed
         c2Ptr->Speed = aWeaponPlatformPtr->GetSpeed();
         // course
         double velNED[3] = {0.0};
         aWeaponPlatformPtr->GetVelocityNED(velNED);
         double course = atan2(velNED[1], velNED[0]);
         c2Ptr->Course = course;
         // time to go
         double timeToGo  = 0.0;
         double rangeToGo = 0.0;
         GetInterceptData(aWeaponPlatformPtr, timeToGo, rangeToGo);
         c2Ptr->TimeToGoSeconds = timeToGo;
         msgPtr->InsertContinuationWord(std::move(c2Ptr));

         // C3
         msgPtr->AddContinuationWord(3);

         // C4
         auto c4Ptr = ut::make_unique<Continuation4>();
         // track number, controller
         c4Ptr->TrackNumberController = controllerTrackNumber;
         msgPtr->InsertContinuationWord(std::move(c4Ptr));

         // C5
         auto c5Ptr = ut::make_unique<Continuation5>();
         // Time to input lockout (when the weapon becomes self supporting)
         // Just set to '7' (29 - 37 sec.) for now
         // TODO Need something like GetInterceptData that updates TTL
         //   Need to pass it SimTime, weapon pointer and a TTL var to update and return
         //   Based on the previous sim time vs. current sim time update TTL?
         //   Where should this info be stored? On the weapon? As part of J11 Interface?
         // c5Ptr->TimeToLockout = 30.0;
         c5Ptr->TimeToLockout = 0.0; // LBM zero for now
         msgPtr->InsertContinuationWord(std::move(c5Ptr));

         // Set the track number for the weapon in the header
         unsigned int weaponTrackNumber = 0u;
         GetNEW_MapData(mNEW_TrackNumber, weaponPlatformIndex, weaponTrackNumber);
         msgPtr->GetHeader().mSourceTrackNumber = weaponTrackNumber;

         // E0 - Packing this one for the ADS guys
         auto e0Ptr = ut::make_unique<Extension0>();
         // seeker acquisition status/quality
         e0Ptr->SeekerAcquisitionStatus = SeekerAcquisitionStatusField::cNOT_IN_SEARCH_OR_TRACK;
         if (IsTracking(aWeaponPlatformPtr))
         {
            e0Ptr->SeekerAcquisitionStatus = SeekerAcquisitionStatusField::cTRACKING;
         }
         else if (IsActive(aWeaponPlatformPtr))
         {
            e0Ptr->SeekerAcquisitionStatus = SeekerAcquisitionStatusField::cSEARCH_MODE_COURSE;
         }

         // track number, index originator 2
         e0Ptr->TrackNumberIndexOrig2 = controllerTrackNumber;
         msgPtr->InsertExtensionWord(std::move(e0Ptr));

         if (mDebugEnabled)
         {
            auto logDebug = ut::log::debug() << "Attempting to send J11.0 WIFT message.";
            logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
            logDebug.AddNote() << "Weapon: " << aWeaponPlatformPtr->GetName();
            logDebug.AddNote() << "Track Number: " << weaponTrackNumber;
            logDebug.AddNote() << "TN Src: " << weaponTrackNumber;
            logDebug.AddNote() << "TN DST: " << controllerTrackNumber;
         }

         SendMessage(aWeaponPlatformPtr, msgPtr);
      }

      // Next send time
      return (aSimTime + mWIFT_Interval);
   }
   else
   {
      return -1.0;
   }
}

// ****************************************************************************
// Private

// ============================================================================

// ============================================================================
//! Fills the map with extended result termination strings and
//! maps them to the J11.0 DetailedStatusInformation enum
void Interface_J11::CreateTerminationResultMap()
{
   using namespace Messages::J11_0;

   mResultMap.insert(Result_MapTypePair("Target proximity", WeaponDetailedStatusField::cNO_STATEMENT));
   mResultMap.insert(Result_MapTypePair("Commanded termination", WeaponDetailedStatusField::cNO_STATEMENT));
   mResultMap.insert(Result_MapTypePair("AGL limit encountered", WeaponDetailedStatusField::cAGL_LIMIT));
   mResultMap.insert(Result_MapTypePair("MSL limit encountered", WeaponDetailedStatusField::cMSL_LIMIT));
   mResultMap.insert(Result_MapTypePair("Minimum speed encountered", WeaponDetailedStatusField::cMIN_SPEED));
   mResultMap.insert(Result_MapTypePair("Maximum time-of-flight exceeded", WeaponDetailedStatusField::cMAX_TIME_OF_FLIGHT));
   mResultMap.insert(Result_MapTypePair("Coast time exceeded", WeaponDetailedStatusField::cCOAST_TIME_EXCEEDED));

   mResultMap.insert(Result_MapTypePair("No termination", WeaponDetailedStatusField::cNO_STATEMENT));
   mResultMap.insert(Result_MapTypePair("above max gimbal", WeaponDetailedStatusField::cMAX_GIMBAL));
   mResultMap.insert(Result_MapTypePair("above max los rate", WeaponDetailedStatusField::cMAX_LOS_RATE));
   mResultMap.insert(Result_MapTypePair("above max flight time", WeaponDetailedStatusField::cMAX_TIME_OF_FLIGHT));
   mResultMap.insert(Result_MapTypePair("below min mach number", WeaponDetailedStatusField::cMIN_MACH));
   mResultMap.insert(Result_MapTypePair("speed below 0 ft/s", WeaponDetailedStatusField::cMIN_SPEED));
   mResultMap.insert(Result_MapTypePair("below min closing velocity", WeaponDetailedStatusField::cMIN_CLOSING_VELOCITY));
   mResultMap.insert(Result_MapTypePair("below min g's available", WeaponDetailedStatusField::cMIN_ACCELERATION));
   mResultMap.insert(
      Result_MapTypePair("semi-active seeker lost illumination", WeaponDetailedStatusField::cILLUMINATION_LOST));
   mResultMap.insert(Result_MapTypePair("target is no longer available", WeaponDetailedStatusField::cTARGET_NOT_AVAILABLE));
   mResultMap.insert(Result_MapTypePair("no more data link", WeaponDetailedStatusField::cDATA_LINK_LOST));
   mResultMap.insert(Result_MapTypePair("guidance never established", WeaponDetailedStatusField::cNO_GUIDANCE));
   mResultMap.insert(Result_MapTypePair("target never acquired", WeaponDetailedStatusField::cTARGET_NOT_ACQUIRED));
   mResultMap.insert(Result_MapTypePair("seeker requires track at launch event",
                                        WeaponDetailedStatusField::cTRACK_REQUIRED_AT_LAUNCH));
   mResultMap.insert(Result_MapTypePair("warhead not armed", WeaponDetailedStatusField::cNOT_ARMED));
   mResultMap.insert(Result_MapTypePair("outside kill radius", WeaponDetailedStatusField::cOUTSIDE_KILL_RADIUS));
   mResultMap.insert(Result_MapTypePair("above max coast time", WeaponDetailedStatusField::cCOAST_TIME_EXCEEDED));
   mResultMap.insert(
      Result_MapTypePair("above max doppler coast time", WeaponDetailedStatusField::cDOPPLER_COAST_TIME_EXCEEDED));
   mResultMap.insert(Result_MapTypePair("above max gimbal after a doppler coast",
                                        WeaponDetailedStatusField::cDOPPLER_COAST_THEN_MAX_GIMBAL));
   mResultMap.insert(Result_MapTypePair("above max tracking rate after a doppler coast",
                                        WeaponDetailedStatusField::cDOPPLER_COAST_THEN_MAX_LOS_RATE));
   mResultMap.insert(Result_MapTypePair("outside min/max fuze velocities", WeaponDetailedStatusField::cFUZE_VELOCITY));
   mResultMap.insert(Result_MapTypePair("not used", WeaponDetailedStatusField::cUNKNOWN));
   mResultMap.insert(Result_MapTypePair("altitude below 0 ft mean sea level", WeaponDetailedStatusField::cMSL_LIMIT));
   mResultMap.insert(Result_MapTypePair("memory allocation failed", WeaponDetailedStatusField::cINSUFFICIENT_MEMORY));
   mResultMap.insert(Result_MapTypePair("surface-to-air missile fire control not ready for launch",
                                        WeaponDetailedStatusField::cCAN_NOT_LAUNCH));
   mResultMap.insert(
      Result_MapTypePair("target outside max boresight error", WeaponDetailedStatusField::cMAX_BORESIGHT_ERROR));
   mResultMap.insert(Result_MapTypePair("unknown termination reason", WeaponDetailedStatusField::cUNKNOWN));
   mResultMap.insert(
      Result_MapTypePair("launcher is no longer available", WeaponDetailedStatusField::cLAUNCHER_NOT_AVAILABLE));
}

// ============================================================================
//! Removes an entry from the map if found.
//! @param aNEW_Map    [input] The map to use.
//! @param aKey        [input] The key to delete.
//! @returns 'true' if key was found and deleted.
bool Interface_J11::DeleteNEW_MapData(NEW_MapType& aNEW_Map, const unsigned int aKey)
{
   // Default return value
   bool dataFound = false;

   NEW_MapTypeIterator iter = aNEW_Map.find(aKey);
   if (iter != aNEW_Map.end())
   {
      // Entry is found
      aNEW_Map.erase(iter);
      dataFound = true;
   }
   // else
   //{
   //    // Entry is NOT found
   // }

   return dataFound;
}

// ============================================================================
//! Removes an entry from the map if found.
//! @param aDisEntityId [input] First part of the complex key to delete.
//! @param aWeaponTrackNumber [input] Second part of the complex key to delete.
//! @returns 'true' if key was found and deleted.
bool Interface_J11::DeleteDirectorToWeapon(const DisEntityId& aDisEntityId, unsigned int aWeaponTrackNumber)
{
   // Default return value
   bool dataFound = false;

   D_NEW_MapTypeIterator iter = mDirectorToWeaponMap.find(EID_MapTypePair(aDisEntityId, aWeaponTrackNumber));
   if (iter != mDirectorToWeaponMap.end())
   {
      // Entry is found so delete
      mDirectorToWeaponMap.erase(iter);
      dataFound = true;
   }

   return dataFound;
}


void Interface_J11::PurgeWeaponRecords(size_t aWeaponPlatformIndex)
{
   // purge: mNEW_TrackNumber
   DeleteNEW_MapData(mNEW_TrackNumber, ut::safe_cast<unsigned int, size_t>(aWeaponPlatformIndex));

   // purge: mNEW_ControllerTrackNumber
   DeleteNEW_MapData(mNEW_ControllerTrackNumber, ut::safe_cast<unsigned int, size_t>(aWeaponPlatformIndex));

   // purge: mPendingDirectorTrackNumber
   NEW_MapTypeIterator it = mPendingDirectorTrackNumber.begin();
   while (it != mPendingDirectorTrackNumber.end())
   {
      if (it->second == aWeaponPlatformIndex)
      {
         mPendingDirectorTrackNumber.erase(it++);
      }
      else
      {
         ++it;
      }
   }

   // purge: mDirectorToWeaponMap
   D_NEW_MapTypeIterator it2 = mDirectorToWeaponMap.begin();
   while (it2 != mDirectorToWeaponMap.end())
   {
      if (it2->second == aWeaponPlatformIndex)
      {
         mDirectorToWeaponMap.erase(it2++);
      }
      else
      {
         ++it2;
      }
   }
}


// ============================================================================
//! Determine minimum-time intercept point.
//!
//! Given a weapon location and speed and a target location and velocity vector,
//! determine the intercept location (and therefore the desired weapon heading
//! and the time-to-intercept). This method assumes the bodies move at a constant
//! speed and that the weapon can turn instantly to the best heading.
//!
//! @param aWpnLocWCS [input]  The WCS location of the weapon.
//! @param aWpnSpeed  [input]  The speed of the weapon.
//! @param aTgtLocWCS [input]  The WCS location of the target.
//! @param aTgtVelWCS [input]  The WCS velocity of the target.
//! @param aIntLocWCS [output] The WCS location of the intercept. (valid only if the
//!                            function return value is 'true')
//!
//! @returns 'true' if an intercept occurred or 'false' if not.
//!
//! @note: This routine will allow a target to run into a zero velocity weapon.
//! Logic taken from WsfWeaponTrackProcessor
bool Interface_J11::FindIntercept(const double aWpnLocWCS[3],
                                  double       aWpnSpeed,
                                  const double aTgtLocWCS[3],
                                  const double aTgtVelWCS[3],
                                  double       aIntLocWCS[3])
{
   // Compute the WCS location of the target with respect to the weapon.

   double wpnToTgtLocWCS[3];
   UtVec3d::Subtract(wpnToTgtLocWCS, aTgtLocWCS, aWpnLocWCS);

   // Assume a position vector from the weapon to the target.  Compute the radial
   // (along the vector) and tangential (perpendicular to the vector) components of
   // the target velocity.

   double radUnitVec[3];
   double wpnToTgtRange = UtVec3d::Normalize(radUnitVec, wpnToTgtLocWCS);
   if (wpnToTgtRange < 1.0)
   {
      // The weapon and target are so close.  Just set the intercept location to be the
      // target location.
      UtVec3d::Set(aIntLocWCS, aTgtLocWCS);
      return true;
   }

   double tgtRadVelVec[3];
   double tgtTanVelVec[3];
   double tgtRadVel = UtVec3d::DotProduct(aTgtVelWCS, radUnitVec);
   UtVec3d::Multiply(tgtRadVelVec, radUnitVec, tgtRadVel);
   UtVec3d::Subtract(tgtTanVelVec, aTgtVelWCS, tgtRadVelVec);

   // Compute the components of the weapon velocity vector.
   //
   // The weapon tangential velocity must match the target tangential velocity.
   // The weapon radial velocity is whatever part of the velocity that remains
   // after subtracting out the tangential velocity.

   double wpnTanVel = UtVec3d::Magnitude(tgtTanVelVec);
   if (wpnTanVel > aWpnSpeed)
   {
      // The required tangential speed is greater than or equal to the total weapon speed.
      // There is nothing left for the closing radial component.
      return false;
   }

   double wpnRadVel = sqrt((aWpnSpeed * aWpnSpeed) - (wpnTanVel * wpnTanVel));
   double wpnRadVelVec[3];
   UtVec3d::Multiply(wpnRadVelVec, radUnitVec, wpnRadVel);

   // Determine the rate of closure along the radial vector.

   double cloRadVelVec[3];
   UtVec3d::Subtract(cloRadVelVec, wpnRadVelVec, tgtRadVelVec);

   // Make sure the rate of closure is positive toward the target.

   double cloRadVel = UtVec3d::DotProduct(cloRadVelVec, radUnitVec);
   if (cloRadVel <= 0.0)
   {
      return false;
   }

   // Intercept is possible, so compute the intercept location.
   double timeToIntercept = wpnToTgtRange / cloRadVel;
   double tgtIncWCS[3];
   UtVec3d::Multiply(tgtIncWCS, aTgtVelWCS, timeToIntercept);
   UtVec3d::Add(aIntLocWCS, aTgtLocWCS, tgtIncWCS);
   return true;
}

// ============================================================================
//! Fills a WsfTrack from data in a J11 messsage
//! @param aSenderEntityId [input] The DIS entity ID of the sender.
//! @param aMessage        [input] JTIDS J11 message
//! @param aTrackPtr       [output] A WsfTrack to fill
void Interface_J11::FillTrack(const DisEntityId& aSenderEntityId, const Messages::InitialBase& aMessage, WsfTrack* aTrackPtr)
{
   using namespace Messages;

   // Get the continuation words 2&3
   J11_1::Continuation2* cont2Ptr = static_cast<J11_1::Continuation2*>(aMessage.FindContinuation(2));
   J11_1::Continuation3* cont3Ptr = static_cast<J11_1::Continuation3*>(aMessage.FindContinuation(3));

   // Quick return
   if ((cont2Ptr == nullptr) || (cont3Ptr == nullptr))
   {
      ut::log::warning() << "Missing J11.1 continuation words.";
      return;
   }

   // Set the track type to processed since it is assumed to be the product of some 'process'
   aTrackPtr->SetTrackType(WsfTrack::cPROCESSED);
   aTrackPtr->SetFusible(true);
   aTrackPtr->SetUpdateCount(0);

   // Location
   aTrackPtr->SetLocationLLA(cont2Ptr->Latitude, cont2Ptr->Longitude, cont3Ptr->HeightAboveEllipsoid);
   aTrackPtr->SetLLA_LocationValid(true);
   aTrackPtr->Set3D(true);

   // Velocity
   // TODO: this is probably not very efficient (but I am copying it anyway)
   UtEntity tmp;
   tmp.SetLocationLLA(cont2Ptr->Latitude, cont2Ptr->Longitude, cont3Ptr->HeightAboveEllipsoid);

   // Check CW 10 first if it exists
   J11_1::Continuation10* cont10Ptr      = static_cast<J11_1::Continuation10*>(aMessage.FindContinuation(10));
   double                 velocityNED[3] = {0.0};
   if (cont10Ptr != nullptr)
   {
      UtVec3d::Set(velocityNED, cont10Ptr->TargetVelocityNorth, cont10Ptr->TargetVelocityEast, cont10Ptr->TargetVelocityDown);
   }
   else
   {
      // using CW 2&3 data; data fields are limited
      UtVec3d::Set(velocityNED, cont3Ptr->TargetVelocityNorth, cont3Ptr->TargetVelocityEast, cont2Ptr->TargetVelocityDown);
   }
   tmp.SetVelocityNED(velocityNED);
   double velocityWCS[3];
   tmp.GetVelocityWCS(velocityWCS);
   aTrackPtr->SetVelocityWCS(velocityWCS);
   aTrackPtr->SetVelocityValid(true);

   // Update location if necessary
   double curSimTime = GetSimulation()->GetSimTime();
   if (mUseTimeOfTargetMeasurement)
   {
      double timeOfTargetMeasurement = cont3Ptr->TimeOfTgtMeasurement;
      aTrackPtr->SetStartTime(timeOfTargetMeasurement);
      double dt = curSimTime - timeOfTargetMeasurement;
      if (dt >= 0.0)
      {
         double locEstimateWCS[3] = {0.0};
         aTrackPtr->GetExtrapolatedLocationWCS(timeOfTargetMeasurement, locEstimateWCS);
         aTrackPtr->SetLocationWCS(locEstimateWCS);
      }
   }
   else
   {
      aTrackPtr->SetStartTime(curSimTime);
   }
   aTrackPtr->SetUpdateTime(curSimTime);
}

// ============================================================================
//! Finds the Detailed Status enum given the weapon's extended result string
//! @param aExtendedResult    [input] Extended result from weapon
WsfL16::Messages::J11_0::WeaponDetailedStatusField::WeaponDetailedStatusType
Interface_J11::GetDetailedStatusInformation(const std::string& aExtendedResult)
{
   Result_MapTypeIterator iter = mResultMap.find(aExtendedResult);
   if (iter != mResultMap.end())
   {
      // Entry is found
      return (*iter).second;
   }

   // Unknown self-abort reason
   return WsfL16::Messages::J11_0::WeaponDetailedStatusField::cUNKNOWN;
}

// ============================================================================
//! Returns the time and range to go given the weapon pointer
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
//! Logic taken from WsfWeaponTrackProcessor
void Interface_J11::GetInterceptData(WsfPlatform* aWeaponPlatformPtr, double& aTimeToGo, double& aRangeToGo)
{
   // Assume no intercept.
   aTimeToGo  = 1.0E+20;
   aRangeToGo = 1.0E+20;

   const WsfTrack* targetPtr = aWeaponPlatformPtr->GetTrackManager().GetCurrentTarget();
   if (targetPtr != nullptr)
   {
      double wpnLocWCS[3];
      double wpnVelWCS[3];
      double tgtLocWCS[3];
      double tgtVelWCS[3] = {0.0, 0.0, 0.0};

      aWeaponPlatformPtr->GetLocationWCS(wpnLocWCS);
      aWeaponPlatformPtr->GetVelocityWCS(wpnVelWCS);
      double wpnVel = UtVec3d::Magnitude(wpnVelWCS);

      targetPtr->GetExtrapolatedLocationWCS(GetSimulation()->GetSimTime(), tgtLocWCS);
      if (targetPtr->VelocityValid())
      {
         targetPtr->GetVelocityWCS(tgtVelWCS);
      }

      // Find the minimum-time intercept location.
      double intLocWCS[3];
      if (FindIntercept(wpnLocWCS, wpnVel, tgtLocWCS, tgtVelWCS, intLocWCS))
      {
         // Compute the range and time to the intercept location.
         double wpnToIntLocWCS[3];
         UtVec3d::Subtract(wpnToIntLocWCS, intLocWCS, wpnLocWCS);
         aRangeToGo = UtVec3d::Magnitude(wpnToIntLocWCS);
         aTimeToGo  = aRangeToGo / std::max(1.0, wpnVel);
      }
   }
   if (mDebugEnabled)
   {
      auto logDebug = ut::log::debug() << "Intercept Data Report: ";
      logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
      logDebug.AddNote() << "Weapon: " << aWeaponPlatformPtr->GetName();
      logDebug.AddNote() << "TTG: " << aTimeToGo;
      logDebug.AddNote() << "RTG: " << aRangeToGo;
   }
}

// ============================================================================
//! Returns a value from the map using the key provided.
//! @param aNEW_Map    [input] The map to use.
//! @param aKey        [input] The key to find.
//! @param aValue      [output] The return value.
//! @returns 'true' if key was found.
bool Interface_J11::GetNEW_MapData(NEW_MapType& aNEW_Map, const unsigned int aKey, unsigned int& aValue)
{
   // Default return value
   bool dataFound = false;

   NEW_MapTypeIterator iter = aNEW_Map.find(aKey);
   if (iter != aNEW_Map.end())
   {
      // Entry is found
      aValue    = (*iter).second;
      dataFound = true;
   }
   // else
   //{
   //    // Entry is NOT found
   // }

   return dataFound;
}

// ============================================================================
//! Returns the enumeration for the network-enabled type field
WsfL16::Messages::J11_0::WeaponNEW_TypeField::WeaponNEW_TypeType Interface_J11::GetNEW_Type(WsfPlatform* aWeaponPlatformPtr)
{
   using namespace Messages::J11_0;

   WeaponNEW_TypeField::WeaponNEW_TypeType NEW_TypeEnum = WeaponNEW_TypeField::cNO_STATEMENT;
   if (aWeaponPlatformPtr != nullptr)
   {
      TypeOfNEW_MapTypeIterator  NEW_TypeIter;
      const WsfObject::TypeList& typeList = aWeaponPlatformPtr->GetTypeList();
      for (auto type : typeList)
      {
         NEW_TypeIter = mTypeOfNEW_Map.find(type);
         if (NEW_TypeIter != mTypeOfNEW_Map.end())
         {
            NEW_TypeEnum = static_cast<WeaponNEW_TypeField::WeaponNEW_TypeType>(NEW_TypeIter->second);
            break;
         }
      }
   }

   return NEW_TypeEnum;
}

// ============================================================================
//! Returns the number of weapons fired which is used as the weapon track number
//! @param aFiringPlatformIndex [input] Platform index of the firing platform
unsigned int Interface_J11::GetNumberOfWeaponsFired(unsigned int aFiringPlatformIndex)
{
   NEW_MapTypeIterator iter = mFiringPlatformToWeaponCountMap.find(aFiringPlatformIndex);
   if (iter == mFiringPlatformToWeaponCountMap.end())
   {
      // Entry is NOT found
      std::pair<NEW_MapTypeIterator, bool> res =
         mFiringPlatformToWeaponCountMap.insert(NEW_MapTypePair(aFiringPlatformIndex, 1));
      iter = res.first;
   }
   else
   {
      // Entry is found
      ++((*iter).second);
   }
   return (*iter).second;
}

// ============================================================================
//! Returns a pointer to the weapon platform based on the message sender and
//! the weapon track number in the J11 message
//! @param aSenderEntityId       [input]  DIS Entity ID of the sender.
//! @param aWeaponTrackNumber      [input] Track number from the J11 message
WsfPlatform* Interface_J11::GetWeapon(const DisEntityId& aSenderEntityId, unsigned int aWeaponTrackNumber)
{
   using namespace Messages;

   WsfPlatform*          weaponPlatformPtr = nullptr;
   D_NEW_MapTypeIterator iter = mDirectorToWeaponMap.find(EID_MapTypePair(aSenderEntityId, aWeaponTrackNumber));
   if (iter != mDirectorToWeaponMap.end())
   {
      // Entry is found
      weaponPlatformPtr = GetSimulation()->GetPlatformByIndex((*iter).second);
   }

   return weaponPlatformPtr;
}

// ============================================================================
//! Returns 'true' if the given platform has at least one JTIDS comm device
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
bool Interface_J11::IsJTIDS_EnabledWeapon(WsfPlatform* aWeaponPlatformPtr)
{
   // Set the default return value
   bool isNEW = false;

   // See if there is a JTIDS comm device on the platform; only need to find one
   for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*aWeaponPlatformPtr); !iter.AtEnd(); ++iter)
   {
      if (iter->GetClassId() == WsfJTIDS_Terminal::GetJTIDSClassId())
      {
         isNEW = true;
         break;
      }
   }

   return isNEW;
}

// ============================================================================
//! Determines if a platform is comm enabled
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
bool Interface_J11::IsNetworkEnabledWeapon(WsfPlatform* aWeaponPlatformPtr)
{
   // Set the default return value
   bool isNEW = false;

   // If allowing any comm device to receive
   if (mAllowAnyComm)
   {
      isNEW = (aWeaponPlatformPtr->GetComponentCount<wsf::comm::Comm>() > 0); // At least one comm device is present
   }
   else
   {
      // See if there is a JTIDS comm device on the platform; only need to find one
      isNEW = IsJTIDS_EnabledWeapon(aWeaponPlatformPtr);
   }

   return isNEW;
}

// ============================================================================
//! Determines if a platform is active (i.e has a sensor on)
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
bool Interface_J11::IsActive(WsfPlatform* aWeaponPlatformPtr)
{
   // Set the default return value
   bool isActive = false;

   // See if there is an on-board sensor on; only need to find one
   for (WsfComponentList::RoleIterator<WsfSensor> iter(*aWeaponPlatformPtr); !iter.AtEnd(); ++iter)
   {
      if (iter->IsTurnedOn())
      {
         isActive = true;
         break;
      }
   }

   return isActive;
}

// ============================================================================
//! Determines if a platform is tracking the target
//! (i.e an on-board sensor is tracking the target)
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
bool Interface_J11::IsTracking(WsfPlatform* aWeaponPlatformPtr)
{
   // Quick argument check
   if (aWeaponPlatformPtr == nullptr)
   {
      return false;
   }

   // Set the default return value
   bool isTracking = false;

   WsfTrackId targetTrackId = aWeaponPlatformPtr->GetTrackManager().GetCurrentTargetTrackId();
   if (!targetTrackId.IsNull())
   {
      WsfLocalTrack* targetTrackPtr = aWeaponPlatformPtr->GetTrackManager().FindTrack(targetTrackId);
      if (targetTrackPtr != nullptr)
      {
         if (targetTrackPtr->IsCorrelated())
         {
            // Assume track manager is present on this platform

            // Get the list of raw track that contributed in the forming of this local track
            const WsfLocalTrack::RawTrackIdList& idList = targetTrackPtr->GetRawTrackIds();
            unsigned int                         trackListCount(idList.GetCount());
            for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
            {
               const WsfTrackId* idPtr = idList.GetEntry(trackNum);
               // Find the raw track
               WsfTrack* rawTrackPtr = targetTrackPtr->GetTrackManager()->FindRawTrack(*idPtr);
               if (rawTrackPtr != nullptr)
               {
                  // Check the raw track platform ID against the given platform's ID
                  if (rawTrackPtr->GetTrackId().GetOwningPlatformId() == aWeaponPlatformPtr->GetNameId())
                  {
                     // Only need to find one raw track from the given platform
                     isTracking = true;
                     break;
                  }
               }
            }
         }
         else
         {
            // Assume track manager is NOT present on this platform
            // Most likely a weapon track processor is present
            if (targetTrackPtr->GetOriginatorNameId() == aWeaponPlatformPtr->GetNameId())
            {
               // Track originated on weapon platform so an onboard sensor is tracking
               isTracking = true;
            }
         }
      }
   }
   return isTracking;
}

// ============================================================================
//! Process a J11 message by sending to JTIDS computer processors
//! Only valid for JTIDS-equipped weapons
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
//! @param aMessage           [input] JTIDS J11 message
void Interface_J11::ProcessJMessage(WsfPlatform* aWeaponPlatformPtr, const Messages::InitialBase& aMessage)
{
   // Quick check for null pointer
   if (aWeaponPlatformPtr == nullptr)
   {
      return;
   }

   // Get the track number from one of the L16 processors
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*aWeaponPlatformPtr); !iter.AtEnd(); ++iter)
   {
      if (iter->IsA_TypeOf("WSF_LINK16_COMPUTER"))
      {
         ComputerProcessor* l16Ptr = dynamic_cast<ComputerProcessor*>(*iter);
         if (l16Ptr != nullptr)
         {
            l16Ptr->ProcessJMessage(GetSimulation()->GetSimTime(), aMessage);
         }
      }
   }
}

// ============================================================================
//! Receive a message through a comm device on the weapon
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
//! @param aMessage           [input] JTIDS J11 message
bool Interface_J11::ReceiveMessage(WsfPlatform* aWeaponPlatformPtr, WsfMessage& aMessage)
{
   // Set the default return value
   bool recvMessage = false;
   for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*aWeaponPlatformPtr); !iter.AtEnd(); ++iter)
   {
      // Trick the comm device into receiving the message
      if (mAllowAnyComm)
      {
         aMessage.SetDstAddr(iter->GetAddress());
      }

      // Allow any comm or a JTIDS comm
      bool haveCommDevice = mAllowAnyComm || (iter->GetClassId() == WsfJTIDS_Terminal::GetJTIDSClassId());

      // Have a device so attempt to receive the message
      if (haveCommDevice)
      {
         // Use own device for transmitter; just needs to be non-null
         wsf::comm::Message commMessage(ut::clone(aMessage));
         if (iter->Receive(GetSimulation()->GetSimTime(), *iter, commMessage))
         {
            recvMessage = true;
            break;
         }
      }
   }
   return recvMessage;
}

// ============================================================================
//! Send a message through a comm device on the weapon
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
//! @param aMessage           [input] JTIDS J11 message
bool Interface_J11::SendMessage(WsfPlatform* aWeaponPlatformPtr, Messages::InitialBase* aMessage)
{
   // Set the default return value
   bool sendMessage = false;
   for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*aWeaponPlatformPtr); !iter.AtEnd(); ++iter)
   {
      wsf::comm::Comm* commPtr = *iter;

      // Allow any comm or a JTIDS comm
      bool haveCommDevice = mAllowAnyComm || (iter->GetClassId() == WsfJTIDS_Terminal::GetJTIDSClassId());

      // Use the first command chain available (this returns 0 if no command chain present)
      WsfCommandChain* commandChainPtr = aWeaponPlatformPtr->GetComponentEntry<WsfCommandChain>(0);

      // Have a device so send the message
      if (haveCommDevice)
      {
         if (mDebugEnabled)
         {
            auto logDebug = ut::log::debug() << "Sending J11 message.";
            logDebug.AddNote() << "T = " << GetSimulation()->GetSimTime();
            logDebug.AddNote() << "Weapon: " << aWeaponPlatformPtr->GetName();
            logDebug.AddNote() << "Interface: " << iter->GetName();
         }


         // See if there is a NEW L16 processor
         bool NEW_PartFound = false;
         for (WsfComponentList::RoleIterator<WsfProcessor> iter(*aWeaponPlatformPtr); !iter.AtEnd(); ++iter)
         {
            if (iter->IsA_TypeOf("WSF_LINK16_COMPUTER"))
            {
               ComputerProcessor* l16Ptr = dynamic_cast<ComputerProcessor*>(*iter);
               if (l16Ptr != nullptr)
               {
                  // Find the first networked enabled part
                  ComputerProcessor::PartMap::const_iterator iter    = l16Ptr->GetParts().begin();
                  ComputerProcessor::PartMap::const_iterator iterEnd = l16Ptr->GetParts().end();
                  ComputerPart*                              partPtr = nullptr;
                  for (; iter != iterEnd; ++iter)
                  {
                     if (((*iter).second)->IsA_TypeOf("NETWORK_ENABLED_WEAPON"))
                     {
                        partPtr = (*iter).second;
                        break;
                     }
                  }

                  if (partPtr != nullptr)
                  {
                     // Network enabled weapon part is found; send the message
                     partPtr->SendJMessage(GetSimulation()->GetSimTime(), aMessage);
                     NEW_PartFound = true;
                     sendMessage   = true;
                  }
               }
            }
         }

         // If NEW Computer Part was not used to send the message then just use the comm device
         if (!NEW_PartFound)
         {
            JMessage::MessagePtr msgPtr(aMessage);

            std::vector<WsfPlatform*> recipients;
            wsf::comm::util::SelectRecipients(commandChainPtr,
                                              wsf::comm::util::cCOMMANDER | wsf::comm::util::cPEERS |
                                                 wsf::comm::util::cSUBORDINATES,
                                              recipients);
            for (auto platform : recipients)
            {
               for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*platform); !it.AtEnd(); ++it)
               {
                  auto commRcvrPtr = *it;
                  if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
                  {
                     auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
                     if (networkManagerPtr->PathExists(commPtr->GetAddress(), commRcvrPtr->GetAddress()))
                     {
                        auto messagePtr = ut::make_unique<JMessage>(msgPtr, aWeaponPlatformPtr);
                        messagePtr->SetGroup(commandChainPtr->GetNameId());
                        commPtr->Send(GetSimulation()->GetSimTime(), std::move(messagePtr), commRcvrPtr->GetAddress());
                        break;
                     }
                  }
               }
            }
            sendMessage = true;
         }

         break;
      }
   }
   return sendMessage;
}

// ============================================================================
//! Inserts data into a director-weapon pairing map.
//! @param aDisEntityId [input] First part of the complex key to insert.
//! @param aWeaponTrackNumber [input] Second part of the complex key to insert.
//! @param aWeaponPlatformIndex [input] The value to insert.
//! @returns 'true' if data was inserted.
bool Interface_J11::SetDirectorToWeapon(const DisEntityId& aDisEntityId,
                                        unsigned int       aWeaponTrackNumber,
                                        size_t             aWeaponPlatformIndex)
{
   // Default return value
   bool mapUpdated = false;

   D_NEW_MapTypeIterator iter = mDirectorToWeaponMap.find(EID_MapTypePair(aDisEntityId, aWeaponTrackNumber));
   if (iter == mDirectorToWeaponMap.end())
   {
      // Entry is NOT found
      mDirectorToWeaponMap.insert(
         D_NEW_MapTypePair(EID_MapTypePair(aDisEntityId, aWeaponTrackNumber), aWeaponPlatformIndex));
   }
   else
   {
      ut::log::warning() << "Director and Weapon Track Number combination exists.";
   }

   return mapUpdated;
}


// ============================================================================
//! Sets the track number in the JTIDS configuration; overrides the input value
//! @param aWeaponPlatformPtr [input] Weapon platform pointer
//! @param aWeaponTrackNumber [input] Track number from the J11 message
void Interface_J11::SetL16ComputerTrackNumber(WsfPlatform* aWeaponPlatformPtr, unsigned int aWeaponTrackNumber)
{
   // Loop through the processors and set the track number in the L16 computers
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*aWeaponPlatformPtr); !iter.AtEnd(); ++iter)
   {
      if (iter->IsA_TypeOf("WSF_LINK16_COMPUTER"))
      {
         ComputerProcessor* l16Ptr = dynamic_cast<ComputerProcessor*>(*iter);
         if (l16Ptr != nullptr)
         {
            l16Ptr->Config().mTrackNumber = aWeaponTrackNumber;
         }
      }
   }
}

// ============================================================================
//! Inserts data into a map.
//! @param aKey   [input] The key to insert.
//! @param aValue [input] The value to insert.
//! @returns 'true' if data was inserted.
bool Interface_J11::SetNEW_MapData(NEW_MapType& aNEW_Map, const unsigned int aKey, const unsigned int aValue)
{
   // Default return value
   bool mapUpdated = false;

   NEW_MapTypeIterator iter = aNEW_Map.find(aKey);
   if (iter == aNEW_Map.end())
   {
      // Entry is NOT found
      aNEW_Map.insert(NEW_MapTypePair(aKey, aValue));
      mapUpdated = true;
   }
   else
   {
      // Entry is found
      (*iter).second = aValue;
      mapUpdated     = true;
   }

   return mapUpdated;
}

// ============================================================================
//! Process pending director (CC or 3PS) changes.
//! @param aSenderEntityId    [input] DIS Entity ID of the message sender.
//! @param aSenderTrackNumber [input] Track number of the message sender.
void Interface_J11::UpdateDirector(const DisEntityId& aSenderEntityId, const unsigned int aSenderTrackNumber)
{
   // See if there is a pending handoff
   unsigned int weaponPlatformIndex = 0u;
   if (GetNEW_MapData(mPendingDirectorTrackNumber, aSenderTrackNumber, weaponPlatformIndex))
   {
      // Delete from the pending queue
      DeleteNEW_MapData(mPendingDirectorTrackNumber, aSenderTrackNumber);

      // Update the mappings
      SetNEW_MapData(mNEW_ControllerTrackNumber, weaponPlatformIndex, aSenderTrackNumber);

      unsigned int weaponTrackNumber = 0u;
      GetNEW_MapData(mNEW_TrackNumber, weaponPlatformIndex, weaponTrackNumber);
      SetDirectorToWeapon(aSenderEntityId, weaponTrackNumber, weaponPlatformIndex);
   }
}

WsfSimulation* Interface_J11::GetSimulation() const
{
   return &mInterfacePtr->GetSimulation();
}

void Interface_J11::Initialize() {}

// ****************************************************************************
// ****************************************************************************

Interface_J11::WIFT_Event::WIFT_Event(double aSimTime, Interface_J11* aInterfacePtr, WsfPlatform* aPlatformPtr)
   : WsfEvent(aSimTime)
   , mInterfacePtr(aInterfacePtr)
   , mPlatformIndex(aPlatformPtr->GetIndex())
{
}

// virtual
WsfEvent::EventDisposition Interface_J11::WIFT_Event::Execute()
{
   EventDisposition disposition = cDELETE;
   WsfPlatform*     platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr)
   {
      double nextTime = mInterfacePtr->ProcessWIFT_Event(GetTime(), platformPtr);
      if (nextTime > 0.0)
      {
         SetTime(nextTime);
         disposition = cRESCHEDULE;
      }
   }
   return disposition;
}

} // namespace WsfL16
