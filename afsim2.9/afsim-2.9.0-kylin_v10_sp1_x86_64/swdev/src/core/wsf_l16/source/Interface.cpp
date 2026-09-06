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

#include "Interface.hpp"

#include <cassert>
#include <cmath>
#include <cstring>
#include <iomanip>

#include "ComputerProcessor.hpp"
#include "DisTransmitter.hpp"
#include "FieldTypes.hpp"
#include "GenSwapEndian.hpp"
#include "HeaderTDL.hpp"
#include "Message2_2.hpp"
#include "Message2_3.hpp"
#include "Message2_6.hpp"
#include "Message31_7.hpp"
#include "MessageAccessor.hpp"
#include "Messages.hpp"
#include "MessagesFactory.hpp"
#include "SlotParameters.hpp"
#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfCommComponentHW.hpp"
#include "WsfCommObserver.hpp"
#include "WsfDateTime.hpp"
#include "WsfDisObserver.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeaponObserver.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisSignal.hpp"
#include "ext/WsfExtInterface.hpp"

namespace WsfL16
{

InterfaceSetup::InterfaceSetup(WsfScenario& aScenario)
   : mHeaderType(cTDL_100)
   , mSwapMethod(cWSF)
   , mTSALevel(0)
   , mTimeSlotType(0)
   , mIsRequested(false)
   , mDebugEnabled(false)
   , mSendPPLI(false)
   , mFillWith31_7(false)
   , mIncludeJ3xTrackDEI(false)
   , mInterface_J11_Setup()
   , mWsfSetupPtr(&aScenario)
{
}

bool InterfaceSetup::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "link16_interface")
   {
      mIsRequested = true;

      UtInputBlock block(aInput);
      std::string  cmd;
      while (block.ReadCommand(cmd))
      {
         if (cmd == "send_ppli")
         {
            aInput.ReadValue(mSendPPLI);
         }
         else if (cmd == "tdl_header")
         {
            int headerType;
            aInput.ReadValue(headerType);
            if (headerType == 6)
            {
               mHeaderType = cTDL_6;
            }
            else if (headerType == 100)
            {
               mHeaderType = cTDL_100;
            }
         }
         else if (cmd == "print_script_usage")
         {
            PrintScriptUsage();
         }
         else if (cmd == "debug")
         {
            aInput.ReadValue(mDebugEnabled);
            mInterface_J11_Setup.SetDebugEnabled(mDebugEnabled);
         }
         else if (cmd == "fill_with_31.7")
         {
            aInput.ReadValue(mFillWith31_7);
         }
         else if (cmd == "set_label_npg_pair")
         {
            int label;
            int npg;
            aInput.ReadValue(label);
            aInput.ReadValue(npg);
            SetNPGTable(label, npg);
         }
         else if (cmd == "tsa_level")
         {
            aInput.ReadValue(mTSALevel);
         }
         else if (cmd == "time_slot_type")
         {
            aInput.ReadValue(mTimeSlotType);
         }
         else if (cmd == "swap_tdl_header_100_method")
         {
            std::string sMethod;
            aInput.ReadValue(sMethod);
            if (sMethod == "fields_only")
            {
               mSwapMethod = cWSF;
            }
            else if (sMethod == "fields_then_word_boundary")
            {
               mSwapMethod = cTRAP_TOOL;
            }
         }
         else if (cmd == "end_swap_tdl_header_100_method")
         {
         }
         else if (cmd == "include_dis_entity_id_in_j3x_tracks")
         {
            aInput.ReadValue(mIncludeJ3xTrackDEI);
         }
         else if (cmd == "ignore_inbound_messages")
         {
            aInput.ReadValue(mIgnoreInboundMessages);
         }
         else if (aInput.GetCommand() == "air_specific_types")
         {
            UtInputBlock block(aInput, "end_air_specific_types");
            while (block.ReadCommand())
            {
               WsfStringId platformType = block.GetCommand();
               unsigned    airSpecificType;
               aInput.ReadValue(airSpecificType);
               aInput.ValueLess(airSpecificType, 4096U);
               mAirSpecificTypes[platformType]    = airSpecificType;
               mAirPlatformTypes[airSpecificType] = platformType;
            }
         }
         else if (aInput.GetCommand() == "land_specific_types")
         {
            UtInputBlock block(aInput, "end_land_specific_types");
            while (block.ReadCommand())
            {
               WsfStringId platformType = block.GetCommand();
               unsigned    landSpecificType;
               aInput.ReadValue(landSpecificType);
               aInput.ValueLess(landSpecificType, 4096U);
               mLandSpecificTypes[platformType]     = landSpecificType;
               mLandPlatformTypes[landSpecificType] = platformType;
            }
         }
         else if (mInterface_J11_Setup.ProcessInput(aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = mInterface_J11_Setup.ProcessInput(aInput);
   }
   return myCommand;
}

//! Scripting language
void InterfaceSetup::PrintScriptUsage()
{
   using namespace Messages;
   std::map<int, MessageAccessorType>::const_iterator i = MessageAccessorType::sMessageAccessorTypes.begin();
   for (; i != MessageAccessorType::sMessageAccessorTypes.end(); ++i)
   {
      auto                       logInfo     = ut::log::info() << "Script Usage: ";
      const MessageAccessorType& msgAccessor = i->second;
      logInfo.AddNote() << "Message Name: " << msgAccessor.mMessageName;
      for (unsigned j = 0; j < msgAccessor.mFields.size(); ++j)
      {
         const MessageAccessorType::FieldData& fieldData = msgAccessor.mFields[j];
         if (!(fieldData.mAccessorType->mIsDisused || fieldData.mAccessorType->mIsSpare))
         {
            std::string paramType;
            switch (fieldData.mAccessorType->mAccessor)
            {
            case FieldAccessor::cBOOLEAN:
               paramType = "bool";
               break;
            case FieldAccessor::cDOUBLE:
               paramType = "double";
               break;
            case FieldAccessor::cENUMERATION:
            case FieldAccessor::cINTEGER:
               paramType = "int";
               break;
            case FieldAccessor::cSTRING:
               paramType = "string";
               break;
            }

            logInfo.AddNote() << "Param Type: " << paramType;
            logInfo.AddNote() << "Field Data Name: " << fieldData.mName;
            if (fieldData.mAccessorType->mHasUnits)
            {
               logInfo.AddNote() << "Units: " << fieldData.mAccessorType->mUnitsName;
            }
            if (!fieldData.mAccessorType->mEnumMap.empty())
            {
               auto logNote = logInfo.AddNote() << "Enumeration value mapping:";

               std::map<int, std::string>::iterator ej = fieldData.mAccessorType->mEnumMap.begin();
               for (; ej != fieldData.mAccessorType->mEnumMap.end(); ++ej)
               {
                  logNote.AddNote() << ej->first << ej->second;
               }
            }
         }
      }
   }
}

//! Output information about all messages and fields available in the
//! Scripting language.  Print in the wiki format.
void InterfaceSetup::PrintScriptUsageRst()
{
   using namespace Messages;
   std::map<int, MessageAccessorType>::const_iterator i = MessageAccessorType::sMessageAccessorTypes.begin();

   for (i = MessageAccessorType::sMessageAccessorTypes.begin(); i != MessageAccessorType::sMessageAccessorTypes.end(); ++i)
   {
      const MessageAccessorType& msgAccessor = i->second;
      std::cout << msgAccessor.mMessageName << std::endl;
      if (msgAccessor.mWordType == Messages::Base::cINITIAL)
      {
         std::cout << std::string(msgAccessor.mMessageName.size(), '-') << std::endl << std::endl;
      }
      else
      {
         std::cout << std::string(msgAccessor.mMessageName.size(), '^') << std::endl << std::endl;
      }
      std::cout << ".. class:: " << msgAccessor.mMessageName << std::endl << std::endl;
      if (msgAccessor.mWordType == Messages::Base::cINITIAL)
      {
         Messages::InitialBase* initialWord =
            Messages::Factory::CreateInitialWord(msgAccessor.mLabel, msgAccessor.mSubLabel);
         if (initialWord != nullptr)
         {
            // cout << "Message Methods\n";
            // cout << "---------------\n\n";
            for (int c = 0; c < 10; ++c)
            {
               auto cWord = initialWord->CreateContinuationWord(c);
               if (cWord != nullptr)
               {
                  std::string wordName = cWord->GetClassName();
                  std::cout << "   .. method:: " << wordName << " AddContinuation" << c << "()\n\n";
                  std::cout << "      Adds continuation word " << c << " to the message.  Returns the continuation"
                            << " word.\n"
                            << "      Once the continuation word belongs to the message, FindContinuation" << c
                            << "() may be used to retrieve the word.\n\n";
                  std::cout << "   .. method:: " << wordName << " FindContinuation" << c << "()\n\n";
                  std::cout << "      Returns extension word " << c << " if it has been added to the message.\n\n";
               }
               auto eWord = initialWord->CreateExtensionWord(c);
               if (eWord != nullptr)
               {
                  std::string wordName = eWord->GetClassName();
                  std::cout << "   .. method:: " << wordName << " AddExtension" << c << "()\n\n";
                  std::cout << "      Adds extension word " << c << " to the message.  This can only be called "
                            << " once for each extension word";
                  if (c != 0)
                  {
                     std::cout << ", and only after extension word " << c - 1 << " has been added.\n";
                  }
                  else
                  {
                     std::cout << ".\n";
                  }
                  std::cout << "      Returns the extension word.  Once the extension word belongs to the message, "
                               "FindExtension"
                            << c << " may be used to retrieve the word.\n\n";
                  std::cout << "   .. method:: " << wordName << " FindExtension" << c << "()\n\n";
                  std::cout << "      Returns extension word " << c << " if it has been added to the message.\n\n";
               }
            }
         }
      }
      // cout << "Field Accessors\n";
      // cout << "---------------\n\n";
      for (unsigned j = 0; j < msgAccessor.mFields.size(); ++j)
      {
         const MessageAccessorType::FieldData& fieldData = msgAccessor.mFields[j];
         if (!(fieldData.mAccessorType->mIsDisused || fieldData.mAccessorType->mIsSpare))
         {
            std::string paramType;
            switch (fieldData.mAccessorType->mAccessor)
            {
            case FieldAccessor::cBOOLEAN:
               paramType = "bool";
               break;

            case FieldAccessor::cDOUBLE:
               paramType = "double";
               break;

            case FieldAccessor::cENUMERATION:
            case FieldAccessor::cINTEGER:
               paramType = "int";
               break;

            case FieldAccessor::cSTRING:
               paramType = "string";
               break;
            }

            std::cout << "   .. method:: " << paramType << " " << fieldData.mName << "()\n";
            std::cout << "               void " << fieldData.mName << "(" << paramType << ")\n\n";
            std::cout << "      Get or set the value of the " << fieldData.mName << " field.";
            if (fieldData.mAccessorType->mHasUnits)
            {
               std::cout << "     Units are " << fieldData.mAccessorType->mUnitsName << '.';
            }
            std::cout << "\n\n";
         }
      }
   }
}

// Used to map a message family to an NPG value.
// Must input msg type and npg value as a pair.
bool InterfaceSetup::SetNPGTable(int aMsgType, int aNPG)
{
   if (mNPGTable.find(std::pair<int, int>(aMsgType, aNPG)) == mNPGTable.end())
   {
      mNPGTable.insert(std::pair<int, int>(aMsgType, aNPG));
      return true;
   }
   return false;
}

bool InterfaceSetup::MessageFilter::IsFiltered(int aLabel, int aSublabel)
{
   return ((mFilteredMessageTypes.find(aLabel) != mFilteredMessageTypes.end()) ||
           (mFilteredSpecificTypes.find(std::make_pair(aLabel, aSublabel)) != mFilteredSpecificTypes.end()));
}

Interface::Interface(InterfaceSetup& aScenario)
   : InterfaceSetup(aScenario)
   , mInterface_J11(nullptr)
   , mCallbacks()
   , mGetWeaponTrackNumberFunctionPtr(nullptr)
{
   mInterface_J11         = new Interface_J11(this, aScenario.mInterface_J11_Setup);
   mIgnoreInboundMessages = false;
}

void Interface::AddedToSimulation()
{
   mDisInterfacePtr = (WsfDisInterface*)&GetSimulation().GetExtension("dis_interface");
}

Interface::~Interface()
{
   delete mInterface_J11;
}

bool Interface::Initialize()
{
   mInterface_J11->Initialize();
   int          year;
   int          month;
   int          day;
   WsfDateTime& dateTime = GetSimulation().GetDateTime();
   dateTime.GetStartDate(year, month, day);

   double julianDate1900              = dateTime.GetJulianDate(1900, 0, 0, 0);
   mSecondsTillSimStartSinceJan1_1900 = (dateTime.GetStartJulianDate() - julianDate1900) * 24 * 60 * 60;
   mSecondsSinceMidnight = (dateTime.GetStartJulianDate() - dateTime.GetJulianDate(year, month, day, 0)) * 24 * 60 * 60;

   double secondsAfterFrame = fmod(mSecondsSinceMidnight, 12.0);
   mSlotOffset              = (int)ceil(secondsAfterFrame * 1536 / 12.0);
   if (mIsRequested)
   {
      mCallbacks += WsfObserver::WeaponFired(&GetSimulation()).Connect(&Interface::WeaponFired, this);
      mCallbacks += WsfObserver::WeaponTerminated(&GetSimulation()).Connect(&Interface::WeaponTerminated, this);
      mCallbacks += WsfObserver::DisSignalReceived(&GetSimulation()).Connect(&Interface::HandleSignalPDU, this);

      // set up DisTransmission PDUs
      // start transmission pdu
      mCallbacks += WsfObserver::MessageQueued(&GetSimulation()).Connect(&Interface::MessageQueued, this);
      // end transmission pdu
      mCallbacks += WsfObserver::MessageTransmitted(&GetSimulation()).Connect(&Interface::MessageTransmitted, this);
      mCallbacks += WsfObserver::MessageTransmitEnded(&GetSimulation()).Connect(&Interface::MessageTransmitEnded, this);
      mCallbacks +=
         WsfObserver::MessageTransmittedHeartbeat(&GetSimulation()).Connect(&Interface::MessageTransmittedHeartbeat, this);
   }

   // PrintScriptUsageRst();
   return true;
}

int Interface::GetJ3xIdentityFromIFF(WsfTrack::IFF_Status aIFF_Status)
{
   switch (aIFF_Status)
   {
   case WsfTrack::cIFF_UNKNOWN:
      return WsfL16::Messages::IdentityField::cUNKNOWN;
      break;
   case WsfTrack::cIFF_AMBIGUOUS:
      return WsfL16::Messages::IdentityField::cSUSPECT;
      break;
   case WsfTrack::cIFF_FOE:
      return WsfL16::Messages::IdentityField::cHOSTILE;
      break;
   case WsfTrack::cIFF_FRIEND:
      return WsfL16::Messages::IdentityField::cFRIEND;
      break;
   case WsfTrack::cIFF_NEUTRAL:
      return WsfL16::Messages::IdentityField::cNEUTRAL;
      break;
   default: // should never reach here, but need a 'default' to prevent compiler warning
      return WsfL16::Messages::IdentityField::cUNDEFINED;
   }
}

void Interface::SetIFF_FromJ3xIdentity(WsfTrack& aTrack, int aJ3xIdentity)
{
   switch (aJ3xIdentity)
   {
   // MJM Do not set the iff status to unknown, as this may overwrite an existing
   //     iff status that is not unknown.
   // case IdentityField::cUNKNOWN:
   //   aTrack.SetIFF_Status(WsfTrack::cIFF_UNKNOWN); break;
   case WsfL16::Messages::IdentityField::cSUSPECT:
      aTrack.SetIFF_Status(WsfTrack::cIFF_AMBIGUOUS);
      break;
   case WsfL16::Messages::IdentityField::cHOSTILE:
      aTrack.SetIFF_Status(WsfTrack::cIFF_FOE);
      break;
   case WsfL16::Messages::IdentityField::cFRIEND:
      aTrack.SetIFF_Status(WsfTrack::cIFF_FRIEND);
      break;
   case WsfL16::Messages::IdentityField::cNEUTRAL:
      aTrack.SetIFF_Status(WsfTrack::cIFF_NEUTRAL);
      break;
   }
}

void Interface::SendJMessage(double                       aSimTime,
                             WsfPlatform&                 aSender,
                             const Messages::InitialBase* aMessage,
                             const SlotParameters&        aSlotParameters)
{
   aMessage->GetHeader().mTimeSlotType = ut::safe_cast<unsigned int, int>(mTimeSlotType); // Set per input file

   // int lbl, sublbl;
   // aMessage->GetLabel(lbl, sublbl);
   if (IsFiltered(aSender, aMessage))
   {
      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "Filtering out send for J-Message over DIS.";
         out.AddNote() << "Filtering: " << aMessage->GetClassName();
      }

      return;
   }

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "Sending J-Message over DIS.";
      out.AddNote() << "Message class: " << aMessage->GetClassName();
   }

   using namespace Messages;
   WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(aSender.GetIndex());
   if (disPlatformPtr == nullptr)
   {
      return;
   }

   // block of enough data to write a maximum sized message
   // union
   //{
   int packedData[64] = {0};
   // NOT-USED unsigned short packedShort[128];
   // };

   char*       outputPtr        = (char*)packedData;
   int         extraHeaderBytes = 0;
   DisEntityId entityId;
   mDisInterfacePtr->GetEntityId(&aSender, entityId);

   // Fill header information
   {
      if (mHeaderType == cTDL_100)
      {
         if (mNPGTable.size() > 0)
         {
            // First get type of message
            int label;
            int subLabel;
            aMessage->GetLabel(label, subLabel);
            for (std::set<std::pair<int, int>>::const_iterator i = mNPGTable.begin(); i != mNPGTable.end(); ++i)
            {
               std::pair<int, int> npgPair = *i;
               if (npgPair.first == label)
               {
                  mHeaderTDL100.mNPG_Number = static_cast<uint16_t>(npgPair.second);
                  break;
               }
            }
         }

         if (mSwapMethod == cWSF)
         {
            CreateHeaderWSF(aSimTime, aSlotParameters);
            mWSF_HeaderTDL100.SwapBigNativeEndian();
            memcpy(packedData, &mWSF_HeaderTDL100, 20);
         }
         else
         {
            CreateHeader(aSimTime, aSlotParameters);
            mHeaderTDL100.SwapBigNativeEndian();
            memcpy(packedData, &mHeaderTDL100, 20);
         }

         outputPtr        = reinterpret_cast<char*>(packedData) + 20;
         extraHeaderBytes = 20;
      }
   }

   MessageBitOutput output(static_cast<char*>(outputPtr));
   DisSignal        pdu;
   pdu.SetEntityId(entityId);
   aMessage->WriteMessage(output);

   size_t msgLength    = 1 + aMessage->GetMessageLength();
   size_t numFillWords = 0;
   if (mFillWith31_7)
   {
      if (msgLength <= 3)
      {
         numFillWords = 3 - msgLength;
      }
      else if (msgLength <= 6)
      {
         numFillWords = 6 - msgLength;
      }
      else if (msgLength <= 12)
      {
         numFillWords = 12 - msgLength;
      }
   }

   for (size_t i = 0; i < numFillWords; ++i)
   {
      J31_7::Initial fillWord;
      fillWord.GetHeader().mTimeSlotType = ut::safe_cast<unsigned int, int>(mTimeSlotType); // Set per input file
      fillWord.WriteMessage(output);
      output.Skip(10);
   }

   if (mHeaderType == cTDL_100)
   {
      pdu.SetTDLType(100);
   }
   else
   {
      pdu.SetTDLType(6);
   }

   mDisInterfacePtr->GetEntityId(&aSender, entityId);
   pdu.SetEntityId(entityId);

   // pdu.SetEncodingScheme(DisSignal::EcApplicationSpecific);
   pdu.SetEncodingScheme(DisSignal::EcRawBinary | static_cast<DisEnum16>(msgLength + numFillWords));
   pdu.SetSampleRate(0);
   pdu.SetSampleCount(0);

   int bytes = (output.GetBitOffset() + 7) / 8 + extraHeaderBytes;
   int words = (bytes + 3) / 4;
   pdu.SetUserData((unsigned char*)packedData, words * 32);

   auto l16Bytes = reinterpret_cast<char*>(packedData);
   l16Bytes += extraHeaderBytes;
   GenSwapEndian::SwapBigNative().Array(reinterpret_cast<int*>(l16Bytes), (output.GetBitOffset() / 8) / 4 + 1);
   if ((!mDisInterfacePtr->GetSuppressDisOutput()))
   {
      unsigned int radioId = 1;
      for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(aSender); !iter.AtEnd(); ++iter)
      {
         wsf::comm::Comm* commPtr = *iter;
         if ((commPtr != nullptr) &&
             ((commPtr->GetClassId() == "WSF_JTIDS_TERMINAL") || (commPtr->GetClassId() == "WSF_RADIO_XMTR_RCVR")))
         {
            break;
         }
         ++radioId;
      }
      pdu.SetRadioId(radioId);
      mDisInterfacePtr->ImmediatePutPdu(aSimTime, pdu);
   }
}

void Interface::SendJMessage(double aSimTime, WsfPlatform& aSender, const Messages::InitialBase* aMessage)
{
   aMessage->GetHeader().mTimeSlotType = ut::safe_cast<unsigned int, int>(mTimeSlotType); // Set per input file
   SlotParameters p;
   p.mMSEC = p.mTSEC = p.mNetNumber = 0xff;

   int npg = 7;
   if (mNPGTable.size() > 0)
   {
      // First get type of message
      int label;
      int subLabel;
      aMessage->GetLabel(label, subLabel);
      for (std::set<std::pair<int, int>>::const_iterator i = mNPGTable.begin(); i != mNPGTable.end(); ++i)
      {
         std::pair<int, int> npgPair = *i;
         if (npgPair.first == label)
         {
            npg = npgPair.second;
            break;
         }
      }
   }

   p.mNPG        = npg;
   p.mSlotNumber = 0xffffffff;
   SendJMessage(aSimTime, aSender, aMessage, p);
}

void Interface::SendUserDefinedJMessage(double          aSimTime,
                                        WsfPlatform*    aSender,
                                        unsigned short* aMessage,
                                        int             aNPG,
                                        int             aMessageBytes,
                                        int             aLabel,
                                        int             aSubLabel,
                                        int             aNumMsgs)
{
   SlotParameters p;
   p.mMSEC = p.mTSEC = p.mNetNumber = 0xff;
   p.mNPG                           = aNPG;
   p.mSlotNumber                    = 0xffffffff;
   SendUserDefinedJMessage(aSimTime, aSender, aMessage, p, aMessageBytes, aLabel, aSubLabel, aNumMsgs);
}

void Interface::SendUserDefinedJMessage(double                aSimTime,
                                        WsfPlatform*          aSender,
                                        unsigned short*       aMessage,
                                        const SlotParameters& aSlotParameters,
                                        int                   aMessageLength,
                                        int                   aLabel,
                                        int                   aSubLabel,
                                        int                   aNumMsgs)


// num extention words +
// num continuation words
{
   if (mDebugEnabled)
   {
      ut::log::debug() << "Sending User defined J-Message over DIS.";
   }

   // WsfDisInterface&    disInterface = *mDisInterfacePtr;
   WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(aSender->GetIndex());
   if (disPlatformPtr == nullptr)
   {
      return;
   }

   // block of enough data to write a maximum sized message
   int         packedData[64];
   char*       outputPtr        = (char*)packedData;
   int         extraHeaderBytes = 0;
   DisEntityId entityId;
   mDisInterfacePtr->GetEntityId(aSender, entityId);

   // Fill header information
   {
      if (mHeaderType == cTDL_100)
      {
         if (mSwapMethod == cWSF)
         {
            CreateHeaderWSF(aSimTime, aSlotParameters);
            mWSF_HeaderTDL100.SwapBigNativeEndian();
            memcpy(packedData, &mWSF_HeaderTDL100, 20);
         }
         else
         {
            CreateHeader(aSimTime, aSlotParameters);
            mHeaderTDL100.SwapBigNativeEndian();
            memcpy(packedData, &mHeaderTDL100, 20);
         }

         outputPtr        = reinterpret_cast<char*>(packedData) + 20;
         extraHeaderBytes = 20;
      }
   }

   memcpy(outputPtr, aMessage, aMessageLength);
   Messages::InitialBase* lMessage = reinterpret_cast<Messages::InitialBase*>(outputPtr);
   auto                   logInfo  = ut::log::info() << "Messages in the raw link16 message:";
   logInfo.AddNote() << "Number of Messages: " << lMessage->GetMessageLength();

   DisSignal& pdu = *new DisSignal;
   pdu.SetEntityId(entityId);
   if (mHeaderType == cTDL_100)
   {
      pdu.SetTDLType(100);
   }
   else
   {
      pdu.SetTDLType(6);
   }

   mDisInterfacePtr->GetEntityId(aSender, entityId);
   pdu.SetEntityId(entityId);

   // pdu.SetEncodingScheme(DisSignal::EcApplicationSpecific);
   pdu.SetEncodingScheme(DisSignal::EcRawBinary | aNumMsgs);
   pdu.SetSampleRate(0);
   pdu.SetSampleCount(0);

   // pdu.SetUserData((unsigned char*)packedData, output.GetBitOffset() + extraHeaderBytes * 8);
   pdu.SetUserData((unsigned char*)packedData, (aMessageLength * 8 + extraHeaderBytes * 8));

   auto l16Shorts = reinterpret_cast<short*>(packedData);
   for (int i = 0; i < (10 + (aMessageLength / 2)); ++i)
   {
      logInfo.AddNote() << *l16Shorts;
      ++l16Shorts;
   }


   auto l16Bytes = reinterpret_cast<char*>(packedData);
   l16Bytes += extraHeaderBytes;
   GenSwapEndian::SwapBigNative().Array(reinterpret_cast<int*>(l16Bytes), aMessageLength / 4 + 1);
   if ((!mDisInterfacePtr->GetSuppressDisOutput()))
   {
      mDisInterfacePtr->ImmediatePutPdu(aSimTime, pdu);
   }
}

void Interface::FilterMessageSend(WsfPlatform& aSenderPlatform, int aLabel, int aSubLabel)
{
   MessageFilter& filter = mPlatformFilters[aSenderPlatform.GetIndex()];
   if (aSubLabel == -1)
   {
      filter.mFilteredMessageTypes.insert(aLabel);
   }
   else
   {
      filter.mFilteredSpecificTypes.insert(std::make_pair(aLabel, aSubLabel));
   }
}

void Interface::FilterMessageSend(WsfPlatform* aSenderPlatform, int aLabel, int aSubLabel)
{
   MessageFilter& filter = mPlatformFilters[aSenderPlatform->GetIndex()];
   if (aSubLabel == -1)
   {
      filter.mFilteredMessageTypes.insert(aLabel);
   }
   else
   {
      filter.mFilteredSpecificTypes.insert(std::make_pair(aLabel, aSubLabel));
   }
}

void Interface::UnfilterMessageSend(WsfPlatform& aSenderPlatform, int aLabel, int aSubLabel)
{
   MessageFilter& filter = mPlatformFilters[aSenderPlatform.GetIndex()];
   if (aSubLabel == -1)
   {
      filter.mFilteredMessageTypes.erase(aLabel);
   }
   else
   {
      filter.mFilteredSpecificTypes.erase(std::make_pair(aLabel, aSubLabel));
   }
}

void Interface::UnfilterMessageSend(WsfPlatform* aSenderPlatform, int aLabel, int aSubLabel)
{
   MessageFilter& filter = mPlatformFilters[aSenderPlatform->GetIndex()];
   if (aSubLabel == -1)
   {
      filter.mFilteredMessageTypes.erase(aLabel);
   }
   else
   {
      filter.mFilteredSpecificTypes.erase(std::make_pair(aLabel, aSubLabel));
   }
}

void Interface::CreateHeader(double aSimTime, const SlotParameters& aParameters)
{
   unsigned int epoch, timeSlotNumber;
   GetCurrentTime(aSimTime, aParameters.mSlotNumber, mHeaderTDL100.mTransmitInt, mHeaderTDL100.mTransmitFrac, epoch, timeSlotNumber);
   mHeaderTDL100.mSpare       = 0;
   mHeaderTDL100.mTimeSlotId  = timeSlotNumber | (epoch << 24);
   mHeaderTDL100.mNPG_Number  = ut::safe_cast<uint16_t, int>(aParameters.mNPG);
   mHeaderTDL100.mNetNumber   = ut::safe_cast<uint8_t, int>(aParameters.mNetNumber);
   mHeaderTDL100.mTSec        = ut::safe_cast<uint8_t, int>(aParameters.mTSEC);
   mHeaderTDL100.mMSec        = ut::safe_cast<uint8_t, int>(aParameters.mMSEC);
   mHeaderTDL100.mMessageType = 0;

   // Override the parameters if the TSA level = 0; gives lower fidelity
   if (mTSALevel == 0)
   {
      mHeaderTDL100.mTSec         = 0xFF;
      mHeaderTDL100.mMSec         = 0xFF;
      mHeaderTDL100.mNetNumber    = 0;
      mHeaderTDL100.mTimeSlotId   = 0;
      mHeaderTDL100.mTransmitInt  = 0xFFFFFFFF;
      mHeaderTDL100.mTransmitFrac = 0xFFFFFFFF;
   }
}

void Interface::CreateHeaderWSF(double aSimTime, const SlotParameters& aParameters)
{
   unsigned int epoch, timeSlotNumber;
   GetCurrentTime(aSimTime,
                  aParameters.mSlotNumber,
                  mWSF_HeaderTDL100.mTransmitInt,
                  mWSF_HeaderTDL100.mTransmitFrac,
                  epoch,
                  timeSlotNumber);
   mWSF_HeaderTDL100.mSpare       = 0;
   mWSF_HeaderTDL100.mTimeSlotId  = timeSlotNumber | (epoch << 24);
   mWSF_HeaderTDL100.mNPG_Number  = ut::safe_cast<uint16_t, int>(aParameters.mNPG);
   mWSF_HeaderTDL100.mNetNumber   = ut::safe_cast<uint8_t, int>(aParameters.mNetNumber);
   mWSF_HeaderTDL100.mTSec        = ut::safe_cast<uint8_t, int>(aParameters.mTSEC);
   mWSF_HeaderTDL100.mMSec        = ut::safe_cast<uint8_t, int>(aParameters.mMSEC);
   mWSF_HeaderTDL100.mMessageType = 0;

   // Override the parameters if the TSA level = 0; gives lower fidelity
   if (mTSALevel == 0)
   {
      mWSF_HeaderTDL100.mTSec         = 0xFF;
      mWSF_HeaderTDL100.mMSec         = 0xFF;
      mWSF_HeaderTDL100.mNetNumber    = 0;
      mWSF_HeaderTDL100.mTimeSlotId   = 0;
      mWSF_HeaderTDL100.mTransmitInt  = 0xFFFFFFFF;
      mWSF_HeaderTDL100.mTransmitFrac = 0xFFFFFFFF;
   }
}

int Interface::ConvertDIS_SlotNumberToWsf(unsigned int aSlotNumber)
{
   return (aSlotNumber + 1536 - mSlotOffset) % 1536;
}

//! Output information about all messages and fields available in the

void Interface::HandleSignalPDU(WsfDisInterface* aInterfacePtr, const WsfDisSignal& aPdu)
{
   using namespace Messages;
   const unsigned char* dataPtr = nullptr;
   DisUint16            length  = 0;
   aPdu.GetData(dataPtr, length);
   length /= 8;
   unsigned int numJ_words = (aPdu.GetEncodingScheme() & 14);

   if (mDebugEnabled)
   {
      auto logDebug = ut::log::debug() << "L16 Interface - Received DIS Signal PDU.";
      logDebug.AddNote() << "T = " << GetSimulation().GetSimTime();
      logDebug.AddNote() << "length: " << length;
      logDebug.AddNote() << "J-word count: " << numJ_words;
   }

   if ((dataPtr == nullptr) || (length == 0) || (length > 80 * 4))
   {
      return;
   }

   const unsigned char* inputDataPtr = dataPtr;
   int                  tdlType      = aPdu.GetTDLType();
   // Exit if it's not tdl type 100 or 6
   if (tdlType != 6 && tdlType != 100)
   {
      return;
   }
   // Read in header info
   if (tdlType == 100) // Link 16 Standardized Format
   {
      if (mSwapMethod == cWSF)
      {
         memcpy(&mWSF_HeaderTDL100, dataPtr, sizeof(mWSF_HeaderTDL100));
         mWSF_HeaderTDL100.SwapBigNativeEndian();
         inputDataPtr += sizeof(mWSF_HeaderTDL100);
         length -= sizeof(mWSF_HeaderTDL100);
      }
      else
      {
         memcpy(&mHeaderTDL100, dataPtr, sizeof(mHeaderTDL100));
         mHeaderTDL100.SwapBigNativeEndian();
         inputDataPtr += sizeof(mHeaderTDL100);
         length -= sizeof(mHeaderTDL100);
      }
   }

   int tmpBuffer[160];
   assert(length < 160 * 4);
   memcpy(tmpBuffer, inputDataPtr, length);
   GenSwapEndian::SwapBigNative().Array(tmpBuffer, length / 4);

   MessageBitInput           input((const char*)tmpBuffer);
   std::vector<InitialBase*> msgs     = Messages::Factory::ReadMessage(input, numJ_words);
   size_t                    msgCount = msgs.size();
   for (size_t idx = 0u; idx < msgCount; ++idx)
   {
      if (msgs[idx])
      {
         int lbl, sublbl;
         msgs[idx]->GetLabel(lbl, sublbl);
         if (mDebugEnabled)
         {
            auto logDebug = ut::log::debug() << "L16 Interface - Received message.";
            logDebug.AddNote() << "Message: J" << lbl << "." << sublbl;
         }

         if (lbl == 11)
         {
            // Process J11 messages in a different way
            mInterface_J11->ProcessJ11Message(aPdu.GetEntityId(), *msgs[idx], sublbl);
         }
         else
         {
            if (!mIgnoreInboundMessages)
            {
               // TODO: Forward the message to only the right link16 computers
               //       this will work in only simple scenarios
               //   should take into account: TSEC, MSEC, NPG, and net number
               JMessageReceived(GetSimulation().GetSimTime(), *msgs[idx]);
            }
         }
      }
      else
      {
         if (mDebugEnabled)
         {
            ut::log::error() << "L16 Interface - Cannot create message.";
         }
      }
   }
}

//! Calculate the seconds, fraction seconds, and epoch used in the TDL header

//! @param aSimTime The current simulation time
void Interface::GetCurrentTime(double        aSimTime,
                               unsigned int  aWsfSlotIndex,
                               unsigned int& aSecondsSinceJan1_1900,
                               unsigned int& aSecondFraction,
                               unsigned int& aEpoch,
                               unsigned int& aTimeSlotIndex)
{
   double secondsTillNow  = aSimTime + mSecondsTillSimStartSinceJan1_1900;
   aSecondsSinceJan1_1900 = (unsigned int)secondsTillNow;

   // Warning: I don't have documentation on how the fraction should be encoded.
   //          This could be incorrect.
   double fraction = secondsTillNow - aSecondsSinceJan1_1900;
   aSecondFraction = (unsigned int)(0xffffffff * fraction);

   unsigned int secondsSinceMidnight = (unsigned int)(mSecondsSinceMidnight + aSimTime);

   // 1 epoch is 12.8 minutes (768 seconds).  112.5 epoch's / day.
   aEpoch = (unsigned char)(secondsSinceMidnight / 768);

   // WSF slot numbers should be in the range 0 to 1535
   // dis slot numbers will be in the range 0 to 1536 * 64 - 1
   // because each slot in an epoch has a unique ID for DIS
   // WSF time slots start at sim time = 0,
   // but the DIS time slots are relative to the current epoch.
   unsigned int secondsSinceEpochStart = secondsSinceMidnight % 768;
   unsigned int frameNumber            = secondsSinceEpochStart / 12;
   aTimeSlotIndex                      = frameNumber * 1536 + ((aWsfSlotIndex + mSlotOffset) % 1536);
}

void Interface::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
{
   // Call the J11 interface class
   mInterface_J11->WeaponFired(aSimTime, aEngagementPtr, aTargetTrackPtr);
}

void Interface::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   // Call the J11 interface class
   mInterface_J11->WeaponTerminated(aSimTime, aEngagementPtr);
}

void Interface::SendRawJMessage(double aSimTime, WsfPlatform* aSender, const char* aDataPtr, int aMessageBytes, int aNPG)
{
   if (mDebugEnabled)
   {
      ut::log::debug() << "Sending RAW J-Message over DIS.";
   }

   // block of enough data to write a maximum sized message
   int         packedData[64];
   char*       outputPtr        = (char*)packedData;
   int         extraHeaderBytes = 0;
   DisEntityId entityId;
   mDisInterfacePtr->GetEntityId(aSender, entityId);

   WsfDisPlatform* disPlatformPtr = mDisInterfacePtr->GetDisPlatform(aSender->GetIndex());
   if (disPlatformPtr == nullptr)
   {
      return;
   }

   // Fill header information
   {
      if (mHeaderType == cTDL_100)
      {
         SlotParameters params;
         params.mMSEC       = 0;
         params.mTSEC       = 0;
         params.mNPG        = aNPG;
         params.mNetNumber  = 0;
         params.mSlotNumber = 0;
         if (mSwapMethod == cWSF)
         {
            CreateHeaderWSF(aSimTime, params);
            mWSF_HeaderTDL100.SwapBigNativeEndian();
            memcpy(packedData, &mWSF_HeaderTDL100, 20);
         }
         else
         {
            CreateHeader(aSimTime, params);
            mHeaderTDL100.SwapBigNativeEndian();
            memcpy(packedData, &mHeaderTDL100, 20);
         }

         outputPtr        = reinterpret_cast<char*>(packedData) + 20;
         extraHeaderBytes = 20;
      }
   }

   // Swap to get the message length and label, sublabel information
   int tmpBuffer[160];
   // NOT-USED char* tmpOutputPtr = (char*)tmpBuffer;
   assert(aMessageBytes < 160 * 4);
   memcpy(tmpBuffer, aDataPtr, aMessageBytes);
   GenSwapEndian::SwapBigNative().Array(tmpBuffer, aMessageBytes / 4);
   Messages::MessageBitInput                   input((const char*)tmpBuffer);
   std::vector<WsfL16::Messages::InitialBase*> msgs = Messages::Factory::ReadMessage(input);
   if (msgs.empty())
   {
      return;
   }
   msgs[0]->GetHeader().mTimeSlotType = ut::safe_cast<unsigned int, int>(mTimeSlotType);

   int numMsgs = static_cast<int>(msgs[0]->GetMessageLength());

   // Keep data in Big Endian format, as expected
   memcpy(outputPtr, aDataPtr, aMessageBytes);

   DisSignal& pdu = *new DisSignal;
   pdu.SetEntityId(entityId);
   if (mHeaderType == cTDL_100)
   {
      pdu.SetTDLType(100);
   }
   else
   {
      pdu.SetTDLType(6);
   }

   pdu.SetEntityId(entityId);
   pdu.SetEncodingScheme(DisSignal::EcRawBinary | numMsgs);
   pdu.SetSampleRate(0);
   pdu.SetSampleCount(0);

   // pdu.SetUserData(((unsigned char*)packedData) - extraHeaderBytes, aMessageBytes * 8 + extraHeaderBytes * 8);
   pdu.SetUserData((unsigned char*)packedData, aMessageBytes * 8 + extraHeaderBytes * 8);
   if ((!mDisInterfacePtr->GetSuppressDisOutput()))
   {
      mDisInterfacePtr->ImmediatePutPdu(aSimTime, pdu);
   }
}

bool Interface::IsFiltered(WsfPlatform& aPlatform, const Messages::InitialBase* aMsg)
{
   bool isFiltered = false;
   int  lbl, sublbl;
   aMsg->GetLabel(lbl, sublbl);
   if (mGlobalFilters.IsFiltered(lbl, sublbl))
   {
      isFiltered = true;
   }

   if (!isFiltered)
   {
      std::map<size_t, MessageFilter>::iterator i = mPlatformFilters.find(aPlatform.GetIndex());
      if (i != mPlatformFilters.end())
      {
         isFiltered = i->second.IsFiltered(lbl, sublbl);
      }
   }

   return isFiltered;
}

bool Interface::IsFiltered(WsfPlatform* aPlatform, int aLabel, int aSublabel)
{
   bool isFiltered = false;
   if (mGlobalFilters.IsFiltered(aLabel, aSublabel))
   {
      isFiltered = true;
   }

   if (!isFiltered)
   {
      std::map<size_t, MessageFilter>::iterator i = mPlatformFilters.find(aPlatform->GetIndex());
      if (i != mPlatformFilters.end())
      {
         isFiltered = i->second.IsFiltered(aLabel, aSublabel);
      }
   }

   return isFiltered;
}

unsigned Interface::GetAirSpecificType(WsfStringId aPlatformTypeId)
{
   unsigned                                  airSpecificType = 0;
   std::map<WsfStringId, unsigned>::iterator iter            = mAirSpecificTypes.find(aPlatformTypeId);
   if (iter != mAirSpecificTypes.end())
   {
      airSpecificType = iter->second;
   }
   return airSpecificType;
}

unsigned Interface::GetLandSpecificType(WsfStringId aPlatformTypeId)
{
   unsigned                                  landSpecificType = 0;
   std::map<WsfStringId, unsigned>::iterator iter             = mLandSpecificTypes.find(aPlatformTypeId);
   if (iter != mLandSpecificTypes.end())
   {
      landSpecificType = iter->second;
   }
   return landSpecificType;
}

WsfStringId Interface::GetAirPlatformType(unsigned aAirSpecificTypeId)
{
   WsfStringId                               platformTypeId;
   std::map<unsigned, WsfStringId>::iterator iter = mAirPlatformTypes.find(aAirSpecificTypeId);
   if (iter != mAirPlatformTypes.end())
   {
      platformTypeId = iter->second;
   }
   return platformTypeId;
}

WsfStringId Interface::GetLandPlatformType(unsigned aLandSpecificTypeId)
{
   WsfStringId                               platformTypeId;
   std::map<unsigned, WsfStringId>::iterator iter = mLandPlatformTypes.find(aLandSpecificTypeId);
   if (iter != mLandPlatformTypes.end())
   {
      platformTypeId = iter->second;
   }
   return platformTypeId;
}

void Interface::MessageQueued(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage, size_t aQueueSize)
{
}

void Interface::MessageTransmitted(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage)
{
   if ((mDisInterfacePtr == nullptr) || (mDisInterfacePtr->GetExtInterface() == nullptr) ||
       (mDisInterfacePtr->GetDeadReckonPtr() == nullptr) || (mDisInterfacePtr->GetSuppressDisOutput()) ||
       (aSimTime < mDisInterfacePtr->GetInitialDistributionInterval()))
   {
      return;
   }

   int radioId = 1;
   // Determine the index of the transmitter on the platform.
   for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*(aXmtrPtr->GetPlatform())); !iter.AtEnd(); ++iter)
   {
      wsf::comm::Comm* commPtr = *iter;
      if (commPtr != nullptr)
      {
         if (commPtr == aXmtrPtr)
         {
            break;
         }
      }
      ++radioId;
   }
   DisEntityId     dpEntityId;
   WsfDisPlatform* dpPtr = mDisInterfacePtr->GetDisPlatform(aXmtrPtr->GetPlatform()->GetIndex());
   if (dpPtr != nullptr)
   {
      if (!dpPtr->GetSendDis())
      {
         // return if this platform is not to send out any DIS
         return;
      }
      dpEntityId = dpPtr->GetEntityId();
   }
   SendTransmitterPDU(aSimTime, aXmtrPtr->GetPlatform(), dpEntityId, radioId, (unsigned int)DisTransmitter::TsOnAndTransmitting);
}

void Interface::MessageTransmitEnded(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage)
{
   if ((mDisInterfacePtr == nullptr) || (mDisInterfacePtr->GetExtInterface() == nullptr) ||
       (mDisInterfacePtr->GetDeadReckonPtr() == nullptr) || (mDisInterfacePtr->GetSuppressDisOutput()) ||
       (aSimTime < mDisInterfacePtr->GetInitialDistributionInterval()))
   {
      return;
   }

   int radioId = 1;
   // Determine the index of the transmitter on the platform.
   for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*(aXmtrPtr->GetPlatform())); !iter.AtEnd(); ++iter)
   {
      wsf::comm::Comm* commPtr = *iter;
      if (commPtr != nullptr)
      {
         if (commPtr == aXmtrPtr)
         {
            break;
         }
      }
      ++radioId;
   }
   DisEntityId     dpEntityId;
   WsfDisPlatform* dpPtr = mDisInterfacePtr->GetDisPlatform(aXmtrPtr->GetPlatform()->GetIndex());
   if (dpPtr != nullptr)
   {
      if (!dpPtr->GetSendDis())
      {
         // return if this platform is not to send out any DIS
         return;
      }
      dpEntityId = dpPtr->GetEntityId();
   }
   SendTransmitterPDU(aSimTime, aXmtrPtr->GetPlatform(), dpEntityId, radioId, (unsigned int)DisTransmitter::TsOnAndNotTransmitting);
}

void Interface::MessageTransmittedHeartbeat(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage)
{
   if ((mDisInterfacePtr == nullptr) || (mDisInterfacePtr->GetExtInterface() == nullptr) ||
       (mDisInterfacePtr->GetDeadReckonPtr() == nullptr) || (mDisInterfacePtr->GetSuppressDisOutput()) ||
       (aSimTime < mDisInterfacePtr->GetInitialDistributionInterval()))
   {
      return;
   }

   int radioId = 1;
   // Determine the index of the transmitter on the platform.
   for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*(aXmtrPtr->GetPlatform())); !iter.AtEnd(); ++iter)
   {
      wsf::comm::Comm* commPtr = *iter;
      if (commPtr != nullptr)
      {
         if (commPtr == aXmtrPtr)
         {
            break;
         }
      }
      ++radioId;
   }
   DisEntityId     dpEntityId;
   WsfDisPlatform* dpPtr = mDisInterfacePtr->GetDisPlatform(aXmtrPtr->GetPlatform()->GetIndex());
   if (dpPtr != nullptr)
   {
      if (!dpPtr->GetSendDis())
      {
         // return if this platform is not to send out any DIS
         return;
      }
      dpEntityId = dpPtr->GetEntityId();
   }
   SendTransmitterPDU(aSimTime, aXmtrPtr->GetPlatform(), dpEntityId, radioId, (unsigned int)DisTransmitter::TsOnAndTransmitting);
}

//! Send Transmitter PDU associated with the Signal PDU.
//! @param aSimTime           The current simulation time of this event.
//! @param aSenderPlatformPtr The pointer to the platform class of the platform initiating the track report.
//! @param aDisEntityId       The DisEntityId of the sending platform
//! @param aRadioId           The radio id for the FADL communication device sending the track report message.
//! @param aStatus            Status of the FADL communication device, on/off/transmitting
//! @param aPduPtr            A pointer to the Transmitter PDU being sent.
//! @returns a copy of the last Transmitter PDU being sent.  If 0, then sending state OFF, transmission ended signal.
void Interface::SendTransmitterPDU(double       aSimTime,
                                   WsfPlatform* aSenderPlatformPtr,
                                   DisEntityId  aDisEntityId,
                                   int          aRadioId,
                                   unsigned int aStatus)
{
   // Transmitter PDU expected before Signal PDU
   DisTransmitter xmtrPDU;
   auto           logDebug = ut::log::debug();
   xmtrPDU.SetEntityId(aDisEntityId);
   xmtrPDU.SetRadioId(aRadioId);

   if (mDebugEnabled)
   {
      logDebug << "Interface::Platform is sending a transmitter pdu with Radio Id.";
      logDebug.AddNote() << "Sender Platform: " << aSenderPlatformPtr->GetName();
      logDebug.AddNote() << "Radio Id" << (int)aRadioId;
   }

   int commIndex = aRadioId - 1;
   if (commIndex < 0)
   {
      commIndex = 0;
   }
   // Save some data before sending the Transmitter PDU since memory is deleted
   WsfExtInterface*      extInterfacePtr = WsfExtInterface::Find(GetSimulation());
   WsfExtRadioEntityType radioEntityType;
   wsf::comm::Comm*      commPtr = nullptr;
   if (extInterfacePtr != nullptr)
   {
      int commIndexMatch = 0;
      for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*aSenderPlatformPtr); !iter.AtEnd(); ++iter)
      {
         if (commIndexMatch == commIndex)
         {
            commPtr = *iter;
            if (commPtr != nullptr)
            {
               extInterfacePtr->GetRadioEntityType(commPtr, radioEntityType);
               DisRadioEntityType disRadioEntityType;
               disRadioEntityType.SetEntityKind((DisEnum8)radioEntityType.GetEntityKind());
               disRadioEntityType.SetDomain((DisEnum8)radioEntityType.GetDomain());
               disRadioEntityType.SetCountry((DisEnum16)radioEntityType.GetCountry());
               disRadioEntityType.SetCategory((DisEnum8)radioEntityType.GetCategory());
               disRadioEntityType.SetNomenclatureVersion((DisEnum8)radioEntityType.GetNomenclatureVersion());
               disRadioEntityType.SetNomenclature((DisEnum16)radioEntityType.GetNomenclature());
               xmtrPDU.SetRadioEntityType(disRadioEntityType);

               break;
            }
         }
         ++commIndexMatch;
      }
   }
   // Radio is should start with 1 not 0
   double locationWCS[3];
   aSenderPlatformPtr->GetLocationWCS(locationWCS);

   if (commPtr != nullptr)
   {
      auto commHW_Ptr = wsf::comm::ComponentHW::Find(*commPtr);
      if ((commHW_Ptr != nullptr) && (commHW_Ptr->GetEM_XmtrCount() > 0))
      {
         WsfEM_Xmtr& xmtr = commHW_Ptr->GetEM_Xmtr(0);
         xmtrPDU.SetFrequency((DisUint64)xmtr.GetFrequency());
         // unsigned int transmitterIndex = 0;
         // cout << aSenderPlatformPtr->GetName() << " Signal: Transmitter frequency = " << uFrequency << " radio id = "
         // << aRadioId << endl;
         xmtrPDU.SetFrequencyBandwidth(static_cast<float>(xmtr.GetBandwidth()));
         xmtrPDU.SetPower(static_cast<DisFloat32>(UtMath::SafeLinearToDB(xmtr.GetPower()) + 30.0));
         xmtr.GetAntenna()->GetLocationWCS(locationWCS);
         double locationECS[3];
         aSenderPlatformPtr->ConvertWCSToECS(locationWCS, locationECS);
         xmtrPDU.SetRelativeLocation(static_cast<DisFloat32>(locationECS[0]),
                                     static_cast<DisFloat32>(locationECS[1]),
                                     static_cast<DisFloat32>(locationECS[2]));

         if (mDebugEnabled)
         {
            logDebug.AddNote() << "Frequency: " << (unsigned int)xmtr.GetFrequency();
         }
      }
   }

   xmtrPDU.SetLocation(locationWCS[0], locationWCS[1], locationWCS[2]);
   // Send the Transmitter PDU indicating that the transmission is active.
   xmtrPDU.SetTransmitState((DisEnum8)aStatus);

   mDisInterfacePtr->ImmediatePutPdu(aSimTime, xmtrPDU);
}

//! Attempt to get the track numbers for a weapon and its controller from an external provider of track numbers.
//! This decouples the Link-16 interface from those interfaces that are launching and controlling weapons.
void Interface::GetWeaponTrackNumbers(size_t aWeaponIndex, unsigned int& aWeaponTrackNumber, unsigned int& aControllerTrackNumber)
{
   aWeaponTrackNumber     = 0;
   aControllerTrackNumber = 0;
   if (mGetWeaponTrackNumberFunctionPtr != nullptr)
   {
      (*mGetWeaponTrackNumberFunctionPtr)(GetSimulation(), aWeaponIndex, aWeaponTrackNumber, aControllerTrackNumber);
   }
}

WsfPlatform* Interface::GetControllerFromTrackNumber(WsfSimulation* aSimulationPtr, unsigned int aTrackNumber)
{
   WsfPlatform* platformPtr = nullptr;
   for (unsigned int i = 0; i < aSimulationPtr->GetPlatformCount(); ++i)
   {
      WsfPlatform* entryPlatformPtr = aSimulationPtr->GetPlatformEntry(i);
      for (WsfComponentList::RoleIterator<WsfProcessor> iter(*entryPlatformPtr); !iter.AtEnd(); ++iter)
      {
         WsfProcessor* procPtr = *iter;
         if (procPtr != nullptr)
         {
            if (procPtr->IsA_TypeOf("WSF_LINK16_COMPUTER"))
            {
               WsfL16::ComputerProcessor* link16ProcPtr = dynamic_cast<WsfL16::ComputerProcessor*>(procPtr);
               if (link16ProcPtr != nullptr)
               {
                  if (link16ProcPtr->Config().mTrackNumber == aTrackNumber)
                  {
                     platformPtr = entryPlatformPtr;
                     break;
                  }
               }
            }
         }
      }
   }
   // if not found, then check to see if a link-16 computer is on board and if a TN match is found
   return platformPtr;
}

void Interface::SetPlatformTrackNumber(WsfPlatform* aPlatformPtr, unsigned int aTrackNumber)
{
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      WsfProcessor*      procPtr    = *iter;
      ComputerProcessor* l16ProcPtr = dynamic_cast<ComputerProcessor*>(procPtr);
      if (l16ProcPtr != nullptr)
      {
         l16ProcPtr->Config().mTrackNumber = aTrackNumber;
      }
   }
}

} // namespace WsfL16
