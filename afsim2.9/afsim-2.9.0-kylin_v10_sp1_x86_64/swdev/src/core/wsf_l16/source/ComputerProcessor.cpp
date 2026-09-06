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

#include "ComputerProcessor.hpp"

#include <iomanip>
#include <iostream>

#include "ComputerPart.hpp"
#include "ComputerPartTypes.hpp"
#include "Interface.hpp"
#include "JMessage.hpp"
#include "Loader.hpp"
#include "Message11_1.hpp"
#include "Message3_2.hpp"
#include "Messages.hpp"
#include "NetworkEnabledWeaponPart.hpp"
#include "PPLI_MessageProxy.hpp"
#include "SlotParameters.hpp"
#include "SurveillancePart.hpp"
#include "TrackMessageProxy.hpp"
#include "UtCallback.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommResult.hpp"
#include "WsfJTIDS_Terminal.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfNonExportableUtil.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackManager.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "script/WsfScriptManager.hpp"
#include "script/WsfScriptProcessorClass.hpp"

namespace
{
unsigned int DecimalToOctal(unsigned int aDecimalValue)
{
   unsigned int octal      = 0;
   int          multiplier = 1;
   for (int i = 0; i < 5; ++i)
   {
      int digit = std::min(7u, aDecimalValue % 10);
      octal += multiplier * digit;
      aDecimalValue /= 10;
      multiplier *= 8;
   }
   return octal;
}
} // namespace

namespace WsfL16
{

WsfProcessor* ComputerProcessor::Clone() const
{
   return new ComputerProcessor(*this);
}

ComputerProcessor::ComputerProcessor(WsfScenario& aScenario)
   : WsfProcessor(aScenario)
   , mCommNameId(nullptr)
   , mOutputDis(false)
   , mOutputWSF(true)
   , mUsingJTIDS(false)
   , mIgnoreSurveillanceTracks(false)
   , mSurvPart(nullptr)
   , mInterfacePtr(nullptr)
{
   mConfig.mTrackNumber = 0;
}

ComputerProcessor::ComputerProcessor(const ComputerProcessor& aSrc)
   : WsfProcessor(aSrc)
   , mConfig(aSrc.mConfig)
   , mParts(aSrc.mParts)
   , mCommNameId(aSrc.mCommNameId)
   , mOutputDis(aSrc.mOutputDis)
   , mOutputWSF(aSrc.mOutputWSF)
   , mUsingJTIDS(aSrc.mUsingJTIDS)
   , mIgnoreSurveillanceTracks(aSrc.mIgnoreSurveillanceTracks)
   , mSurvPart(nullptr)
   , mIFF_ColorMap(aSrc.mIFF_ColorMap)
   , mInterfacePtr(aSrc.mInterfacePtr)
{
   for (PartMap::iterator i = mParts.begin(); i != mParts.end(); ++i)
   {
      i->second = i->second->Clone();
      if (dynamic_cast<SurveillancePart*>(i->second) != nullptr)
      {
         mSurvPart = dynamic_cast<SurveillancePart*>(i->second);
      }
   }
}

ComputerProcessor::~ComputerProcessor()
{
   mCallbacks.Clear();

   for (PartMap::iterator i = mParts.begin(); i != mParts.end(); ++i)
   {
      delete i->second;
   }
}

ComputerPart* ComputerProcessor::CreatePart(const std::string& aName, const std::string& aType)
{
   ComputerPart* procPtr = WsfL16_Extension::Find(GetScenario())->GetComputerPartTypes().Clone(aType);
   if (procPtr != nullptr)
   {
      procPtr->SetName(aName);
      if (dynamic_cast<SurveillancePart*>(procPtr) != nullptr)
      {
         mSurvPart = dynamic_cast<SurveillancePart*>(procPtr);
      }
   }
   return procPtr;
}

// virtual
bool ComputerProcessor::Initialize(double aSimTime)
{
   bool ok = WsfProcessor::Initialize(aSimTime);

   mInterfacePtr = static_cast<Interface*>(GetSimulation()->FindExtension("wsf_l16"));
   mOutputDis    = mOutputDis && (mInterfacePtr != nullptr);

   if (mInterfacePtr == nullptr)
   {
      ok              = false;
      auto logWarning = ut::log::warning()
                        << "Failed to initialize WSF_LINK16_COMPUTER. Link 16 interface is disabled.";
      logWarning.AddNote() << "WSF_LINK16_COMPUTER: " << GetName();
   }
   mCommPtr = GetPlatform()->GetComponent<wsf::comm::Comm>(mCommNameId);
   if (mCommPtr == nullptr)
   {
      ok              = false;
      auto logWarning = ut::log::warning() << "Failed to initialize WSF_LINK16_COMPUTER. Invalid comm name.";
      logWarning.AddNote() << "Comm name: " << GetName() << "." << mCommNameId;
   }
   else
   {
      mUsingJTIDS = nullptr != dynamic_cast<WsfJTIDS_Terminal*>(mCommPtr);
   }

   if (ok)
   {
      for (PartMap::iterator i = mParts.begin(); i != mParts.end(); ++i)
      {
         ok = ok && i->second->Initialize(aSimTime, *this);
      }
   }

   if (ok)
   {
      if (mInterfacePtr != nullptr)
      {
         mInterfacePtr->GetDisInterface()->GetEntityId(GetPlatform(), mConfig.mEntityId);
      }
      if (mConfig.mTrackNumber == 0)
      {
         mConfig.mTrackNumber = mConfig.mEntityId.GetEntity();
      }

      // If using a JTIDS terminal and wanting to send the messages over the DIS interface
      // then connect to the comm devices callback that notifies when a message has been sent
      if (mOutputDis && mUsingJTIDS)
      {
         WsfJTIDS_Terminal* jtidsComm = GetJTIDS_Comm();
         mCallbacks.Add(jtidsComm->MessageSent.Connect(&ComputerProcessor::HandleMessageSent, this));
      }
   }

   if (ok)
   {
      if (mInterfacePtr != nullptr)
      {
         mCallbacks.Add(mInterfacePtr->JMessageReceived.Connect(&ComputerProcessor::ProcessJMessage, this));
      }
   }
   return ok;
}

bool ComputerProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "edit")
   {
      aInput.ReadCommand(command);
      if (command != "message_processor")
      {
         throw UtInput::BadValue(aInput, "Looking for: edit message_processor");
      }
      aInput.ReadCommand(command);
      PartMap::iterator i = mParts.find(command);
      if (i == mParts.end())
      {
         throw UtInput::BadValue(aInput, command + " is not a valid message_processor.");
      }
      LoadPart(aInput, i->second);
   }
   else if (command == "delete")
   {
      aInput.ReadCommand(command);
      PartMap::iterator i = mParts.find(WsfStringId(command));
      if (i == mParts.end())
      {
         throw UtInput::BadValue(aInput, command + " is not a valid message_processor.");
      }
      delete i->second;
      mParts.erase(i);
   }
   else if (command == "message_processor")
   {
      ComputerPart* msgProc = nullptr;
      std::string   procName;
      aInput.ReadCommand(procName);
      WsfStringId procNameId = procName;
      if (mParts.end() != mParts.find(procNameId))
      {
         std::string errorMsg = "message_processor name ";
         throw UtInput::BadValue(aInput,
                                 errorMsg +
                                    " is already used.  Use 'edit' if you are trying to modify an existing processor.");
      }

      std::string procType;
      aInput.ReadCommand(procType);
      msgProc = CreatePart(procName, procType);
      if (!msgProc)
      {
         throw UtInput::BadValue(aInput, procType + " is not a valid message_processor type name.");
      }
      LoadPart(aInput, msgProc);
      mParts.insert(std::make_pair(procNameId, msgProc));
   }
   else if (command == "comm")
   {
      std::string commName;
      aInput.ReadValue(commName);
      mCommNameId = WsfStringId(commName);
   }
   else if (command == "c2")
   {
      aInput.ReadValue(mConfig.mIsC2);
   }
   else if (command == "callsign")
   {
      aInput.ReadValue(mConfig.mCallsign);
   }
   else if (command == "output_dis")
   {
      aInput.ReadValue(mOutputDis);
   }
   else if (command == "output_wsf")
   {
      aInput.ReadValue(mOutputWSF);
   }
   else if (command == "decimal_track_number")
   {
      int trackNumber;
      aInput.ReadValue(trackNumber);
      aInput.ValueInClosedRange(trackNumber, 0, 32767);
      mConfig.mTrackNumber = (unsigned)trackNumber;
   }
   else if ((command == "track_number") || (command == "octal_track_number"))
   {
      int trackNumber;
      aInput.ReadValue(trackNumber);
      aInput.ValueInClosedRange(trackNumber, 0, 77777);
      mConfig.mTrackNumber = DecimalToOctal((unsigned)trackNumber);
   }
   else if (command == "ignore_surveillance")
   {
      aInput.ReadValue(mIgnoreSurveillanceTracks);
   }
   else if (command == "iff_color_mapping")
   {
      std::map<std::string, int> idMap;
      for (std::map<int, const char*>::const_iterator it = WsfL16::Messages::IdentityField::sEnumMap.begin();
           it != WsfL16::Messages::IdentityField::sEnumMap.end();
           ++it)
      {
         std::string enumString(it->second);
         UtStringUtil::ToLower(enumString);
         idMap[enumString.substr(1)] = it->first;
      }

      UtInputBlock block(aInput, "end_iff_color_mapping");
      std::string  id;
      while (block.ReadCommand(id))
      {
         std::map<std::string, int>::const_iterator it = idMap.find(id);
         if (it != idMap.end())
         {
            std::string color;
            aInput.ReadValue(color);
            mIFF_ColorMap[it->second] = color;
         }
         else
         {
            throw UtInput::BadValue(aInput, id + " is not a valid IFF.");
         }
      }
   }
   else if (WsfProcessor::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// This method gets called when DIS output is enabled
// and the comm device was NOT a JTIDS terminal
// or it was a JTIDS terminal and the internal WSF send failed
// It is only called by the base class ComputerPart::SendJMessage method
void ComputerProcessor::SentJMessage(double aSimTime, ComputerPart* aPartPtr, JMessage& aMessage, WsfCommandChain* aCommandChainPtr)
{
   if (mInterfacePtr != nullptr)
   {
      const Messages::InitialBase* jMessage = aMessage.Get().get();
      // TODO: Set slot parameters if needed
      WsfL16::SlotParameters params = aPartPtr->GetSlotParameters();
      mInterfacePtr->SendJMessage(aSimTime, *GetPlatform(), jMessage, params);
      mInterfacePtr->JMessageSent(*jMessage);
   }
}

// static
unsigned int ComputerProcessor::GetPlatformTrackNumber(WsfPlatform& aPlatform)
{
   unsigned int retVal = 0;
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      if (iter->IsA_TypeOf("WSF_LINK16_COMPUTER"))
      {
         WsfL16::ComputerProcessor* l16Proc = dynamic_cast<WsfL16::ComputerProcessor*>(*iter);
         if (l16Proc)
         {
            retVal = l16Proc->GetTrackNumber();
            break;
         }
      }
   }
   return retVal;
}

void ComputerProcessor::ProcessMessageP(double             aSimTime,
                                        wsf::comm::Comm*   aXmtrCommPtr,
                                        wsf::comm::Comm*   aReceiverCommPtr,
                                        const WsfMessage&  aMessage,
                                        wsf::comm::Result& aResult)
{
   ProcessMessage(aSimTime, aMessage);
}

// virtual
bool ComputerProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool            handled = false;
   const JMessage* msg     = dynamic_cast<const JMessage*>(&aMessage);
   if (msg)
   {
      handled                                 = true;
      const Messages::InitialBase* messagePtr = msg->Get().get();
      ProcessJMessage(aSimTime, *messagePtr);
   }
   return handled;
}

void ComputerProcessor::ProcessJMessage(double aSimTime, const Messages::InitialBase& aMessage)
{
   int lbl, sublbl;
   aMessage.GetLabel(lbl, sublbl);

   if (DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Receiving JMessage";
      logDebug.AddNote() << "T = " << aSimTime;
      logDebug.AddNote() << "Platform: " << GetPlatform()->GetName();
      logDebug.AddNote() << "Computer Processor: " << GetName();
      aMessage.PrintMessage(logDebug);
   }

   if (lbl == 2 && sublbl >= 2 && sublbl <= 5)
   {
      HandlePPLI_Message(aSimTime, aMessage);
   }
   else if (lbl == 3 && sublbl >= 2 && sublbl <= 6)
   {
      if (!mIgnoreSurveillanceTracks &&
          (aMessage.GetHeader().mSourceTrackNumber != mConfig.mTrackNumber)) // filter out our own surveillance tracks
      {
         HandleTrackMessage(aSimTime, aMessage);
      }
   }
   for (PartMap::iterator i = mParts.begin(); i != mParts.end(); ++i)
   {
      i->second->HandleReceive(aSimTime, aMessage);
   }
}

void ComputerProcessor::LoadPart(UtInput& aInput, ComputerPart* aPart)
{
   std::string  cmd;
   UtInputBlock block(aInput, "end_message_processor");
   while (block.ReadCommand(cmd))
   {
      if (!aPart->ProcessInput(aInput))
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

void ComputerProcessor::HandlePPLI_Message(double aSimTime, const Messages::InitialBase& aMessage)
{
   unsigned  trackNumber = aMessage.GetHeader().mSourceTrackNumber;
   TrackId   trackNumberId(trackNumber, trackNumber);
   WsfTrack* ppliTrack = mPPLI_TrackList.Find(trackNumberId);
   if (ppliTrack == nullptr)
   {
      ppliTrack = new WsfTrack(WsfTrack::cPROCESSED, this, GetPlatform());
      ppliTrack->SetSideId(GetPlatform()->GetSideId());
      ppliTrack->SetSideIdValid(true);
      ppliTrack->SetFusible(false);
      // TrackAttributes* attributes = new TrackAttributes();
      // attributes->mIsLink16_PPLI = true;
      // attributes->mDisplayId = '0' + UtStringUtil::ToString((int)DecimalToOctal(trackNumber));
      // attributes->mSecondaryId = (int)trackNumber;
      // ppliTrack->GetAuxData()->AddAttribute(attributes);
      std::ostringstream ostr;
      ostr << std::setw(5) << std::setfill('0') << std::oct << trackNumber;
      ppliTrack->GetAuxData().Assign("source_track_number", ostr.str());
      ppliTrack->Initialize(aSimTime, GetPlatform()->GetNextTrackId(), *GetSimulation());
      FillPPLI_Track(aSimTime, *ppliTrack, aMessage);
      mPPLI_TrackList.Add(trackNumberId, ppliTrack);
   }
   else
   {
      FillPPLI_Track(aSimTime, *ppliTrack, aMessage);
   }
   WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().AddTrackReport(aSimTime, *ppliTrack);
   // After the track gets added to the track manager, it is set to non-purgeable
   if (localTrackPtr != nullptr)
   {
      localTrackPtr->SetPurgeable(true);
   }
}

void ComputerProcessor::FillPPLI_Track(double aSimTime, WsfTrack& aTrack, const Messages::InitialBase& aPPLI)
{
   const PPLI_MessageProxy* ppli = PPLI_MessageProxy::Create(&aPPLI);
   if (ppli)
   {
      if (mInterfacePtr != nullptr)
      {
         mInterfacePtr->GetDisInterface()->GetEntityId(GetPlatform(), mConfig.mEntityId);
      }

      if (mConfig.mTrackNumber == 0)
      {
         // mConfig.mTrackNumber = DecimalToOctal((unsigned int)mConfig.mEntityId.GetEntity());
         mConfig.mTrackNumber = mConfig.mEntityId.GetEntity();
      }
      double lat, lon, alt;
      if (ppli->GetLocationLLA(lat, lon, alt))
      {
         aTrack.SetLocationLLA(lat, lon, alt);
         aTrack.Set3D(true);
         // TODO: this is probably not very efficient
         UtEntity tmp;
         tmp.SetLocationLLA(lat, lon, alt);
         double heading        = ppli->GetCourse();
         double speed          = ppli->GetSpeed();
         double velocityNED[3] = {cos(heading) * speed, sin(heading) * speed, 0};
         double velocityWCS[3];
         tmp.SetVelocityNED(velocityNED);
         tmp.GetVelocityWCS(velocityWCS);
         aTrack.SetVelocityWCS(velocityWCS);
         aTrack.SetVelocityValid(true);
      }
      aTrack.SetUpdateTime(aSimTime);

      // For correlation
      UtMatrixd* covarPtr = WsfNonExportableUtil::ConvertLink16_TrackQualityToSphericalCovariance(1.0);
      aTrack.SetStateCovariance(aSimTime, *covarPtr);

      // TrackAttributes& attributes = (TrackAttributes&)aTrack.GetAuxData()->GetAttribute(TrackAttributes::sAttributeName);
      // attributes.mAuxText = ppli->GetCallsign();
      delete ppli;
      delete covarPtr;
   }
}

void ComputerProcessor::HandleTrackMessage(double aSimTime, const Messages::InitialBase& aMessage)
{
   const TrackMessageProxy* trackMsg = TrackMessageProxy::Create(&aMessage);
   if (trackMsg)
   {
      int       trackNumber = trackMsg->GetTrackNumber();
      TrackId   trackId(trackMsg->GetSourceTrackNumber(), trackNumber);
      WsfTrack* survTrack = mSurvTrackList.Find(trackId);
      if (survTrack == nullptr)
      {
         survTrack = new WsfTrack(WsfTrack::cPROCESSED, this, GetPlatform());
         // TrackAttributes* attributes = new TrackAttributes();
         // attributes->mDisplayId = '0' + UtStringUtil::ToString(
         //                (int)DecimalToOctal(trackMsg->GetTrackNumber()));
         // attributes->mSecondaryId = trackMsg->GetTrackNumber();
         // attributes->mIsLink16_Track = true;
         // survTrack->GetAuxData()->AddAttribute(attributes);

         survTrack->Initialize(aSimTime, GetPlatform()->GetNextTrackId(), *GetSimulation());
         mSurvTrackList.Add(trackId, survTrack);
      }

      FillSurvTrack(aSimTime, *survTrack, *trackMsg);

      // Prevent a surveillance part that is only sending sensor reports from being notified
      // of this raw track report.
      bool           suppressed    = mSurvPart && mSurvPart->SuppressSensor(survTrack->GetSensorNameId(), true);
      WsfLocalTrack* localTrackPtr = GetPlatform()->GetTrackManager().AddTrackReport(aSimTime, *survTrack);
      if (mSurvPart)
      {
         mSurvPart->SuppressSensor(survTrack->GetSensorNameId(), suppressed);
      }

      if (localTrackPtr != nullptr)
      {
         int myTN;
         if ((!GetTrackNumber(*localTrackPtr, myTN)) || (trackNumber < myTN))
         {
            mTrackNumberMapping[localTrackPtr->GetTrackId()] = trackNumber;
            SetTrackNumber(*localTrackPtr, trackNumber, aMessage.GetHeader().mSourceTrackNumber);
         }

         if (localTrackPtr->GetAuxDataConst().AttributeExists("link16_iff_status"))
         {
            WsfTrack::IFF_Status iff =
               (WsfTrack::IFF_Status)localTrackPtr->GetAuxDataConst().GetInt("link16_iff_status");
            localTrackPtr->SetIFF_Status(iff);
            SetTrackColorFromIFF(localTrackPtr, Interface::GetJ3xIdentityFromIFF(iff));
         }
      }

      delete trackMsg;
   }
}

//! For an incoming surveillance track, copy common data from the L16 message to the WsfTrack.
void ComputerProcessor::FillSurvTrack(double aSimTime, WsfTrack& aTrack, const TrackMessageProxy& aTrackMsg)
{
   if (mInterfacePtr != nullptr)
   {
      mInterfacePtr->GetDisInterface()->GetEntityId(GetPlatform(), mConfig.mEntityId);
   }

   // TODO What is this doing here; shouldn't it be in "Initialize"? (mjm)
   if (mConfig.mTrackNumber == 0)
   {
      mConfig.mTrackNumber = mConfig.mEntityId.GetEntity();
   }

   double lat, lon, alt;
   if (aTrackMsg.GetLocationLLA(lat, lon, alt))
   {
      aTrack.SetLocationLLA(lat, lon, alt);
      aTrack.Set3D(true);

      double velocityWCS[3];
      if (!aTrackMsg.GetVelocityWCS(velocityWCS))
      {
         // TODO: this is probably not very efficient
         UtEntity tmp;
         tmp.SetLocationLLA(lat, lon, alt);
         double heading        = aTrackMsg.GetCourse();
         double speed          = aTrackMsg.GetSpeed();
         double velocityNED[3] = {cos(heading) * speed, sin(heading) * speed, 0};
         tmp.SetVelocityNED(velocityNED);
         tmp.GetVelocityWCS(velocityWCS);
      }
      aTrack.SetVelocityWCS(velocityWCS);
      aTrack.SetVelocityValid(true);
   }
   aTrack.SetUpdateTime(aSimTime);

   // Store the L16 track number in aux data (eventually this can be stored in the new C2 GTAN).
   std::string tnoString;
   WsfL16::Messages::TrackNumberField::GetTrackNumberString(aTrackMsg.GetTrackNumber(), tnoString);
   aTrack.GetAuxData().AssignString("link16_track_number", tnoString); // as string

   // Store the course in aux data
   aTrack.GetAuxData().AssignDouble("link16_course", aTrackMsg.GetCourse());

   // Get track quality and set covariance matrix
   // double tq = aTrackMsg.GetTrackQuality()/15.0;
   double errorDistance = aTrackMsg.GetTrackQualityErrorDistance95CEP();
   if (errorDistance <= 1.0)
   {
      errorDistance = 1.0;
   }

   double       varTp = pow(errorDistance / 3.0, 2);
   UtCovariance covar(3, 3); // elements are initialized to zero.
   covar(0, 0) = varTp;
   covar(1, 1) = varTp;
   covar(2, 2) = varTp;

   aTrack.SetStateCovariance(aSimTime, covar);
   aTrack.SetTrackQuality(aTrackMsg.GetTrackQuality() / 15.0);

   // Also, assign the quality in aux data (0-15).
   aTrack.GetAuxData().AssignInt("link16_track_quality", (int)aTrackMsg.GetTrackQuality());

   aTrack.GetAuxData().AssignInt("link16_source_track_number", aTrackMsg.GetSourceTrackNumber());

   // Set IFF Status
   Interface::SetIFF_FromJ3xIdentity(aTrack, aTrackMsg.GetIdentity());

   // Set Side Id if Identity has been mapped to a color
   SetTrackColorFromIFF(&aTrack, aTrackMsg.GetIdentity());

   // Set spatial domain
   switch (aTrackMsg.GetEnvironment())
   {
   case TrackMessageProxy::cAIR:
      aTrack.SetSpatialDomain(WSF_SPATIAL_DOMAIN_AIR);
      break;
   case TrackMessageProxy::cSURFACE:
      aTrack.SetSpatialDomain(WSF_SPATIAL_DOMAIN_SURFACE);
      break;
   case TrackMessageProxy::cSUBSURFACE:
      aTrack.SetSpatialDomain(WSF_SPATIAL_DOMAIN_SUBSURFACE);
      break;
   case TrackMessageProxy::cLAND:
      aTrack.SetSpatialDomain(WSF_SPATIAL_DOMAIN_LAND);
      break;
   case TrackMessageProxy::cSPACE:
      aTrack.SetSpatialDomain(WSF_SPATIAL_DOMAIN_SPACE);
      break;
   }

   // Set type if it is found in message
   int specificType = aTrackMsg.GetSpecificType();
   if (specificType != 0)
   {
      WsfStringId platformType = nullptr;
      switch (aTrackMsg.GetEnvironment())
      {
      case TrackMessageProxy::cAIR:
      {
         if (mInterfacePtr != nullptr)
         {
            platformType = mInterfacePtr->GetAirPlatformType(specificType);
         }
         break;
      }
      case TrackMessageProxy::cLAND:
      {
         if (mInterfacePtr != nullptr)
         {
            platformType = mInterfacePtr->GetLandPlatformType(specificType);
         }
         break;
      }
      default:
         break;
      }

      if (platformType != 0)
      {
         aTrack.SetTypeId(platformType);
         aTrack.SetTypeIdValid(true);
      }
   }
   if (mInterfacePtr != nullptr)
   {
      WsfDisInterface* disInterfacePtr = mInterfacePtr->GetDisInterface();
      if (disInterfacePtr)
      {
         DisEntityId trackTruthDisEntityId;
         aTrackMsg.GetTrackDisEntityId(mInterfacePtr, trackTruthDisEntityId);
         if (trackTruthDisEntityId.IsDefined())
         {
            WsfDisPlatform* platformPtr = disInterfacePtr->FindDisPlatform(trackTruthDisEntityId);
            if (platformPtr)
            {
               aTrack.SetTargetIndex(platformPtr->GetPlatform()->GetIndex());
               aTrack.SetTargetType(platformPtr->GetPlatform()->GetTypeId());
               aTrack.SetTargetName(platformPtr->GetPlatform()->GetNameId());
            }
         }
      }
   }
}

void ComputerProcessor::SetTrackColorFromIFF(WsfTrack* aTrackPtr, int aIFF)
{
   std::map<int, WsfStringId>::const_iterator it = mIFF_ColorMap.find(aIFF);
   if (it != mIFF_ColorMap.end())
   {
      // If the identity case is pending (aIFF=0), unknown (aIFF=1), or undefined (aIFF=7),
      // then do NOT set the side id valid flag.
      // An 'unknown' option may have been added to the iff_color_mapping list,
      // and if an aIFF value of 'unknown' comes across we do NOT want to set
      // the side id validity flag just because it was found in the list.
      if ((aIFF > 1) && (aIFF < 7))
      {
         aTrackPtr->SetSideId(it->second);
         ut::log::info() << "ComputerProcessor::SetSide: " << it->second;
         aTrackPtr->SetSideIdValid(true);
      }
   }
}
// This method is the callback when using a JTIDS terminal and wanting
// to send the messages over the DIS interface.
// The JTIDS terminal must successfully send or this callback will not get triggered.
void ComputerProcessor::HandleMessageSent(double              aSimTime,
                                          const WsfMessage*   aMessagePtr,
                                          int                 aSlotNumber,
                                          WsfJTIDS_SlotGroup* aSlotGroup)
{
   if ((mInterfacePtr != nullptr) && (JMessage::GetTypeId() == aMessagePtr->GetType()))
   {
      const JMessage*              msgPtr   = static_cast<const JMessage*>(aMessagePtr);
      const Messages::InitialBase* jMessage = msgPtr->Get().get();
      WsfL16::SlotParameters       params;
      params.Set(*aSlotGroup, aSlotNumber);
      if (MessageShouldBeSent(jMessage))
      {
         mInterfacePtr->SendJMessage(aSimTime, *GetPlatform(), jMessage, params);
         mInterfacePtr->JMessageSent(*jMessage);
      }
   }
}

bool ComputerProcessor::MessageShouldBeSent(const Messages::InitialBase* aMessage)
{
   bool canSend = true;
   int  label   = aMessage->GetLabel();
   ;
   int sublabel = aMessage->GetSubLabel();

   // Performing additional check on J11.1
   // These are Weapon Directive Messages
   if ((label == 11) && (sublabel == 1))
   {
      canSend = IsJ11_1_Valid(aMessage);
   }
   return canSend;
}

bool ComputerProcessor::IsJ11_1_Valid(const Messages::InitialBase* aMessage)
{
   bool bCanSend(true);
   using namespace Messages::J11_1;
   auto msgPtr = dynamic_cast<const Initial*>(aMessage);
   if (msgPtr != nullptr)
   {
      // Check the IFTU
      if (msgPtr->WeaponDirective == WsfL16::Messages::J11_1::WeaponDiscreteField::cIN_FLIGHT_TARGET_UPDATE)
      {
         // Get the continuation word 5
         WsfL16::Messages::J11_1::Continuation5* cont5Ptr =
            static_cast<WsfL16::Messages::J11_1::Continuation5*>(aMessage->FindContinuation(5));
         if (cont5Ptr != nullptr)
         {
            bCanSend = (cont5Ptr->TrackNumber.GetTrackNumberInteger() > 0);
         }
      }
   }
   return bCanSend;
}

ComputerPart* ComputerProcessor::FindPart(WsfStringId aNameId)
{
   PartMap::iterator i = mParts.find(aNameId);
   if (i != mParts.end())
   {
      return i->second;
   }
   return nullptr;
}

bool ComputerProcessor::GetTrackNumber(const WsfLocalTrack& aLocalTrack, int& aTrackNumber)
{
   std::map<WsfTrackId, int>::const_iterator it = mTrackNumberMapping.find(aLocalTrack.GetTrackId());
   if (it != mTrackNumberMapping.end())
   {
      aTrackNumber = it->second;
      return true;
   }
   return false;
}

void ComputerProcessor::SetTrackNumber(WsfLocalTrack& aLocalTrack, int aTrackNumber, int aSourceTrackNumber)
{
   mTrackNumberMapping[aLocalTrack.GetTrackId()] = aTrackNumber;

   WsfL16::Messages::TrackNumberField tnField;
   tnField.SetRawData(aTrackNumber);
   aLocalTrack.GetAuxData().Assign("link16_track_number", (std::string)tnField);

   // Set source track number of message originator
   std::ostringstream oss;
   oss << std::setw(5) << std::setfill('0') << std::oct << aSourceTrackNumber;
   aLocalTrack.GetAuxData().Assign("source_track_number", oss.str());
}

void ComputerProcessor::ClearTrackNumber(const WsfLocalTrack& aLocalTrack)
{
   std::map<WsfTrackId, int>::iterator it = mTrackNumberMapping.find(aLocalTrack.GetTrackId());
   if (it != mTrackNumberMapping.end())
   {
      mTrackNumberMapping.erase(it);
   }
}

const char* ComputerProcessor::GetScriptClassName() const
{
   return "Link16Computer";
}

class Link16ComputerClass : public WsfScriptProcessorClass
{
public:
   Link16ComputerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   UT_DECLARE_SCRIPT_METHOD(Part);
   UT_DECLARE_SCRIPT_METHOD(DecimalSourceTrackNumber);
   UT_DECLARE_SCRIPT_METHOD(OctalSourceTrackNumber);
};

Link16ComputerClass::Link16ComputerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("Link16Computer");

   AddMethod(ut::make_unique<Part>());
   AddMethod(ut::make_unique<DecimalSourceTrackNumber>());
   AddMethod(ut::make_unique<OctalSourceTrackNumber>());
}


UT_DEFINE_SCRIPT_METHOD(Link16ComputerClass, ComputerProcessor, Part, 1, "WsfTadilJProcessor", "string")
{
   ComputerPart* partPtr = aObjectPtr->FindPart(WsfStringId(aVarArgs[0].GetString()));
   UtScriptRef*  partRef = new UtScriptRef(partPtr, aReturnClassPtr);
   aReturnVal.SetPointer(partRef);
}

UT_DEFINE_SCRIPT_METHOD(Link16ComputerClass, ComputerProcessor, DecimalSourceTrackNumber, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetTrackNumber());
}

UT_DEFINE_SCRIPT_METHOD(Link16ComputerClass, ComputerProcessor, OctalSourceTrackNumber, 0, "string", "")
{
   std::ostringstream ostr;
   ostr << std::setw(5) << std::setfill('0') << std::oct << aObjectPtr->GetTrackNumber();
   aReturnVal.SetString(ostr.str());
}

void ComputerProcessor::RegisterType(UtScriptTypes& aTypes)
{
   aTypes.Register(ut::make_unique<Link16ComputerClass>("Link16Computer", &aTypes));
}

void ComputerProcessor::SetPlatform(WsfPlatform* aPlatformPtr)
{
   WsfProcessor::SetPlatform(aPlatformPtr);
   for (std::map<WsfStringId, ComputerPart*>::iterator i = mParts.begin(); i != mParts.end(); ++i)
   {
      i->second->SetPlatform(aPlatformPtr);
   }
}

} // namespace WsfL16
