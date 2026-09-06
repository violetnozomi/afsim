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

#include "dis/WsfDisInput.hpp"

#include "Dis.hpp"
#include "DisTime.hpp"
#include "GenNetInfo.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfApplication.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisExchange.hpp"
#include "dis/WsfDisFilterAllowance.hpp"
#include "ext/WsfExtEmission.hpp"
#include "ext/WsfExtInterface.hpp"

// ============================================================================
// begin WsfDisInput

WsfDisInput::WsfDisInput()
   : mMaxBadPDU_Count(5)
   , mMulticastTimeToLive(-1)
   , mInputHeartbeatMultiplier(-2.4)
   , mSensorUpdateInterval(0)
{
   mDeviceConfigurations.emplace_back();
   sDebugEnabled    = false;
   mThreadSleepTime = -1.0;

   mFlags = 0;

   // Initialize the track-jam limit to the maximum. The user must override it to get the true DIS behavior
   // (either by the input maximum_track_jam_entries or the environment variable DIS_EE_HIGH_DENSITY_THRSH).
   // This must done prior to calling Dis::Init to allow the environment variable to override this.
   Dis::SetTargetsInTjFieldDflt(255U);

   mSuppressCME_DetectBeam    = true;
   mSuppressCME_EntityData    = true;
   mSuppressCME_PassiveSensor = true;
   mSuppressCME_DrawData      = true;
}

bool WsfDisInput::ProcessInput(WsfScenario& aScenario, UtInput& aInput)
{
   if (aInput.GetCommand() == "dis_interface")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand())
      {
         if (ProcessBlockInput(aScenario, aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      return true;
   }
   return false;
}

bool WsfDisInput::ProcessBlockInput(WsfScenario& aScenario, UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "shadow")
   {
      // look for qualifier
      std::string qualifier;
      aInput.ReadValue(qualifier);
      if (qualifier == "type")
      {
         std::string type;
         aInput.ReadValue(type);
         mShadowTypeId.insert(WsfStringId(type));
      }
      else if (qualifier == "name")
      {
         std::string name;
         aInput.ReadValue(name);
         mShadowNameId.insert(WsfStringId(name));
      }
      else if (qualifier == "all")
      {
         mShadowAll = true;
      }
      else // unary command (deprecated); Assume we mean "all."
      {
         mShadowAll = true;
         aInput.PushBack(qualifier);
      }
   }
   else if (command == "ignore_type")
   {
      std::string entityTypeString;
      aInput.ReadValue(entityTypeString);
      WsfExtEntityType entityType;
      WsfExtInterface::ConvertInputToEntityType(entityTypeString, entityType);
      mIgnoredTypes.insert(entityType);
   }
   else if (command == "ignore_pdu_type")
   {
      std::string pduTypeString;
      aInput.ReadValue(pduTypeString);
      DisEnum8 pduType = DisEnum::Pdu::Type::ToEnum(pduTypeString);
      if (pduType != 255)
      {
         mIgnoredPduTypes.insert(pduType);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown PDU type string: " + pduTypeString);
      }
   }
   else if ((command == "ignore_kind_and_domain") ||
            (command == "filter_out_by_kind_and_domain")) // "filter_out_by_kind_and_domain" to be removed (never documented).
   {
      int kind;
      int domain;
      aInput.ReadValue(kind);
      aInput.ValueInClosedRange(kind, 0, 255);
      aInput.ReadValue(domain);
      aInput.ValueInClosedRange(domain, 0, 255);
      mIgnoredKindAndDomain.emplace(kind, domain);
   }
   else if (command == "filter_out_by_range")
   {
      std::string entityNameString;
      aInput.ReadValue(entityNameString);
      double range;
      aInput.ReadValueOfType(range, UtInput::cLENGTH);
      // Store the name, range squared; at initialization, the name is changed to platform Id as an optimization.
      // The 0 is a placeholder; later we will retrieve the platform index and use it.
      mRangeFilteredPlatforms.emplace_back(entityNameString, 0, range * range);
   }
   else if (command == "connections" || command == "edit_connections")
   {
      if (command != "edit_connections")
      {
         mDeviceConfigurations.clear();
      }
      std::string  cmd;
      UtInputBlock block(aInput);
      while (block.ReadCommand(cmd))
      {
         if ((cmd == "broadcast") || (cmd == "unicast") || (cmd == "multicast") || (cmd == "record") || (cmd == "playback"))
         {
            if (!mDeviceConfigurations.empty() && !mDeviceConfigurations.back().IsComplete())
            {
               throw UtInput::BadValue(aInput, "Connection information is missing port.");
            }
            mDeviceConfigurations.emplace_back();
         }
         else if (mDeviceConfigurations.empty())
         {
            throw UtInput::UnknownCommand(aInput);
         }
         if (!ProcessDeviceInput(aScenario, aInput, mDeviceConfigurations.back()))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      if (mDeviceConfigurations.empty())
      {
         mDeviceConfigurations.emplace_back();
      }
   }
   else if (command == "filter_out_by_site_and_app")
   {
      int         filterOutSite = 0;
      int         filterOutApp  = 0;
      std::string qualifier;
      aInput.ReadValue(qualifier);
      if (qualifier == "ignore_site")
      {
         aInput.ReadValue(filterOutSite);
      }
      else if (qualifier == "ignore_application")
      {
         aInput.ReadValue(filterOutApp);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }

      aInput.ReadValue(qualifier);
      if (qualifier == "ignore_site")
      {
         aInput.ReadValue(filterOutSite);
      }
      else if (qualifier == "ignore_application")
      {
         aInput.ReadValue(filterOutApp);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }

      aInput.ReadValue(qualifier);
      if (qualifier != "end_filter_out_by_site_and_app")
      {
         throw UtInput::BadValue(aInput);
      }

      if (filterOutSite != 0 && filterOutApp != 0)
      {
         IgnoreSiteAndApplication(static_cast<DisUint16>(filterOutSite), static_cast<DisUint16>(filterOutApp));
      }
   }
   else if (command == "filtered_connection")
   {
      DeviceConfiguration deviceConfig;
      std::string         cmd;
      UtInputBlock        block(aInput);
      while (block.ReadCommand(cmd))
      {
         // Here the default is to filter entity state pdus.
         // only if the user specifies pass_type, do we consider others
         // So if they start with "allow", then it is an EntityState filter
         DisEnum8 pduType = DisEnum::Pdu::Type::EntityState;

         if (cmd == "pdu_type")
         {
            std::string typeToAllow;
            block.GetInput().ReadValue(typeToAllow);
            if (typeToAllow == "entity_state")
            {
               pduType = DisEnum::Pdu::Type::EntityState;
            }
            else if (typeToAllow == "iff_atc_navaids")
            {
               pduType = DisEnum::Pdu::Type::IffAtcNavaids;
            }
         }
         else if (cmd == "allow")
         {
            // it is a filter allowance command
            // aInput.GetCommand(); //peel off the "allow"
            WsfDisFilterAllowance* allowancePtr = new WsfDisFilterAllowance();
            if (pduType != DisEnum::Pdu::Type::EntityState)
            {
               // If the PDU type has been changed, correct the filter:
               allowancePtr->SetPduType(pduType);
            }

            if (!ProcessFilterAllowance(aInput, *allowancePtr))
            {
               throw UtInput::UnknownCommand(aInput);
            }
            // save off the allowance in the device configuration
            deviceConfig.mFilterAllowances.push_back(allowancePtr);
         }
         else if (ProcessDeviceInput(aScenario, aInput, deviceConfig))
         {
            // it was a device command
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // is the defined connection valid
      if (!deviceConfig.IsComplete())
      {
         throw UtInput::BadValue(aInput, "Filtered connection is missing information.");
      }

      // determine if its a new connection, or if its one already
      // specified in the "connections" block or an "edit_connections" block
      auto it = std::find_if(std::begin(mDeviceConfigurations),
                             std::end(mDeviceConfigurations),
                             [&](const DeviceConfiguration& dc)
                             {
                                return (dc.mDeviceType == deviceConfig.mDeviceType) &&
                                       (dc.mAddress == deviceConfig.mAddress) && (dc.mSendPort == deviceConfig.mSendPort);
                             });

      if (it != std::end(mDeviceConfigurations))
      {
         *it = deviceConfig;
      }
      else
      {
         mDeviceConfigurations.push_back(deviceConfig);
      }
   }
   else if (command == "use_deprecated_cme_draw_data")
   {
      ut::log::warning()
         << command << " and the use of non-standard DIS has been deprecated and will be removed in a future release.";
      mUseDeprecatedCME_DrawData = aInput.ReadBool();
   }
   else if (command == "use_simple_accelerations")
   {
      mUseSimpleAccelerations = true;
   }
   else if (command == "use_simple_orientation_rates")
   {
      mUseSimpleOrientRates = true;
   }
   else if (command == "multi_thread")
   {
      mMultiThreaded = true;
   }
   else if (command == "multi_threading")
   {
      mMultiThreaded = aInput.ReadBool();
   }
   else if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else if (command == "terse_mode")
   {
      mTerseMode    = aInput.ReadBool();
      mDebugEnabled = mTerseMode;
      if (mTerseMode)
      {
         Dis::SetSuppressErrorMask(Dis::GetSuppressErrorMask() | Dis::cIO_ERRORS);
      }
      else
      {
         Dis::SetSuppressErrorMask(Dis::GetSuppressErrorMask() & (~Dis::cIO_ERRORS));
      }
   }
   else if (command == "log_created_entities")
   {
      mLogCreatedEntities = true;
   }
   else if (command == "age_out_time")
   {
      ut::log::warning() << "WsfDisInput: 'age_out_time' has been deprecated and will be removed in a future release.";
   }
   else if (command == "send_periodic_pdus_while_paused")
   {
      mSendPeriodicsWhilePaused = true;
   }
   else if (command == "no_periodic_pdus_while_paused")
   {
      mSendPeriodicsWhilePaused = false;
   }
   else if (command == "use_pdu_time")
   {
      mUsePduTime = true;
   }
   else if (command == "ignore_pdu_time")
   {
      mUsePduTime = false;
   }
   else if (command == "suppress_comm_data")
   {
      mSuppressCommData = aInput.ReadBool();
   }
   else if (command == "suppress_emissions_data")
   {
      mSuppressEmissionsData = aInput.ReadBool();
   }
   else if (command == "suppress_non_standard_data")
   {
      ut::log::warning()
         << command << " and the use of non-standard DIS has been deprecated and will be removed in a future release.";
      mSuppressNonStandardData = aInput.ReadBool();
      if (mSuppressNonStandardData)
      {
         SuppressNonStandardData(mSuppressNonStandardData);
      }
   }
   else if (command == "join_exercise")
   {
      mJoinExercise = true;
   }
   else if (command == "no_join_exercise")
   {
      mJoinExercise = false;
   }
   else if (command == "remember_sender_hostname")
   {
      // Unused...
   }
   else if (command == "suppress_cme_detect_beam")
   {
      ut::log::warning()
         << command << " and the use of non-standard DIS has been deprecated and will be removed in a future release.";
      std::string value;
      try
      {
         aInput.ReadValue(value);
         mSuppressCME_DetectBeam = aInput.ConvertStringToBool(value);
      }
      catch (UtInput::ExceptionBase&)
      {
         mSuppressCME_DetectBeam = true;
         aInput.PushBack(value);
      }
   }
   else if (command == "suppress_cme_entity_data")
   {
      ut::log::warning()
         << command << " and the use of non-standard DIS has been deprecated and will be removed in a future release.";
      std::string value;
      try
      {
         aInput.ReadValue(value);
         mSuppressCME_EntityData = aInput.ConvertStringToBool(value);
      }
      catch (UtInput::ExceptionBase&)
      {
         mSuppressCME_EntityData = true;
         aInput.PushBack(value);
      }
   }
   else if (command == "suppress_cme_passive_sensor")
   {
      ut::log::warning()
         << command << " and the use of non-standard DIS has been deprecated and will be removed in a future release.";
      std::string value;
      try
      {
         aInput.ReadValue(value);
         mSuppressCME_PassiveSensor = aInput.ConvertStringToBool(value);
      }
      catch (UtInput::ExceptionBase&)
      {
         mSuppressCME_PassiveSensor = true;
         aInput.PushBack(value);
      }
   }
   else if (command == "suppress_cme_draw_data")
   {
      ut::log::warning()
         << command << " and the use of non-standard DIS has been deprecated and will be removed in a future release.";
      std::string value;
      try
      {
         aInput.ReadValue(value);
         mSuppressCME_DrawData = aInput.ConvertStringToBool(value);
      }
      catch (UtInput::ExceptionBase&)
      {
         mSuppressCME_DrawData = true;
         aInput.PushBack(value);
      }
   }
   else if (command == "use_track_jam_for_tracking_requests")
   {
      mUseTrackJamForTrackingRequests = aInput.ReadBool();
   }
   else if (ProcessDeviceInput(aScenario, aInput, mDeviceConfigurations.front()))
   {
   }
   else if (command == "debug_entity_appearance")
   {
      mDebugAppearanceUpdates = aInput.ReadBool();
   }
   else if (command == "max_allowed_bad_entity_states")
   {
      aInput.ReadValue(mMaxBadPDU_Count);
   }
   else if (command == "sensor_update_interval")
   {
      aInput.ReadValueOfType(mSensorUpdateInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mSensorUpdateInterval, 0.0);
   }
   else if (command == "autostart")
   {
      mAutostart = true;
   }
   else if (command == "no_autostart")
   {
      mAutostart = false;
   }
   else if (command == "maximum_beam_entries")
   {
      int maximumBeamEntries;
      aInput.ReadValue(maximumBeamEntries);
      aInput.ValueGreaterOrEqual(maximumBeamEntries, 0);
      maximumBeamEntries = std::min(maximumBeamEntries, 255);
      Dis::SetEE_MaxBeamsThrsh(static_cast<DisUint8>(maximumBeamEntries));
   }
   else if (command == "maximum_track_jam_entries")
   {
      int maximumTrackJamEntries;
      aInput.ReadValue(maximumTrackJamEntries);
      aInput.ValueGreaterOrEqual(maximumTrackJamEntries, 0);
      maximumTrackJamEntries = std::min(maximumTrackJamEntries, 255);
      Dis::SetTargetsInTjFieldDflt(static_cast<DisUint8>(maximumTrackJamEntries));
   }
   else if (command == "exercise")
   {
      int exercise;
      aInput.ReadValue(exercise);
      aInput.ValueInClosedRange(exercise, 0, 255);
      Dis::SetExercise(static_cast<DisUint8>(exercise));
   }
   else if (command == "time_to_live")
   {
      aInput.ReadValue(mMulticastTimeToLive);
      aInput.ValueInClosedRange(mMulticastTimeToLive, 0, 255);
   }
   else if (command == "protocol_version")
   {
      int protocolVersion;
      aInput.ReadValue(protocolVersion);
      aInput.ValueInClosedRange(protocolVersion, 0, 255);
      Dis::SetProtocolVersion(static_cast<DisEnum8>(protocolVersion));
   }
   else if (command == "absolute_timestamp")
   {
      DisTime::SetUseAbsoluteTimestamp(true);
   }
   else if (command == "articulated_part")
   {
      if (!ProcessArticulatedPart(aInput))
      {
         throw UtInput::BadValue(aInput, "Badly formed articulated part block.");
      }
   }
   else if ((command == "thread_debug") || (command == "debug_thread"))
   {
      mDebugThread = true;
   }
   else if ((command == "thread_sleep_time") || (command == "multi_thread_sleep_time"))
   {
      aInput.ReadValueOfType(mThreadSleepTime, UtInput::cTIME);
      aInput.ValueGreater(mThreadSleepTime, 0.0);
   }
   else if (command == "use_body_angular_velocities")
   {
      mUseWorldToBodyAngularVelocities = true;
   }
   else if (command == "zero_body_angular_velocities")
   {
      mZeroBodyAngularVelocities = true;
   }
   else if (command == "ignore_stop_freeze_pdu")
   {
      mIgnoreStopFreezePDU = true;
   }
   else if (WsfDisExchange::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}


// ============================================================================
bool WsfDisInput::ProcessArticulatedPart(UtInput& aInput)
{
   std::string bodyType;
   std::string systemName;
   std::string command;
   std::string publishValue;
   int         type;
   int         parentType = 0;
   int         publish    = 0;

   aInput.ReadValue(bodyType);
   aInput.ReadValue(systemName);
   aInput.ReadValue(type);

   aInput.ReadValue(command);
   while (command != "end_articulated_part")
   {
      if (command == "parent")
      {
         aInput.ReadValue(parentType);
      }
      else if (command == "publish")
      {
         aInput.ReadValue(publishValue);
         if (publishValue == "x")
         {
            publish += WsfDisArticulatedPart::PublishX;
         }
         else if (publishValue == "y")
         {
            publish += WsfDisArticulatedPart::PublishY;
         }
         else if (publishValue == "z")
         {
            publish += WsfDisArticulatedPart::PublishZ;
         }
         else if (publishValue == "x_rate")
         {
            publish += WsfDisArticulatedPart::PublishX_Rate;
         }
         else if (publishValue == "y_rate")
         {
            publish += WsfDisArticulatedPart::PublishY_Rate;
         }
         else if (publishValue == "z_rate")
         {
            publish += WsfDisArticulatedPart::PublishZ_Rate;
         }
         else if (publishValue == "azimuth")
         {
            publish += WsfDisArticulatedPart::PublishAzimuth;
         }
         else if (publishValue == "elevation")
         {
            publish += WsfDisArticulatedPart::PublishElevation;
         }
         else if (publishValue == "rotation")
         {
            publish += WsfDisArticulatedPart::PublishRotation;
         }
         else if (publishValue == "azimuth_rate")
         {
            publish += WsfDisArticulatedPart::PublishAzimuthRate;
         }
         else if (publishValue == "elevation_rate")
         {
            publish += WsfDisArticulatedPart::PublishElevationRate;
         }
         else if (publishValue == "rotation_rate")
         {
            publish += WsfDisArticulatedPart::PublishRotationRate;
         }
         else
         {
            return false;
         }
      }
      else
      {
         return false;
      }
      aInput.ReadValue(command);
   }

   mArticulationList.emplace_back(bodyType, systemName, type, parentType, publish);

   return true;
}

// ============================================================================
bool WsfDisInput::ProcessFilterAllowance(UtInput& aInput, WsfDisFilterAllowance& aAllowance)
{
   bool myCommand = true;

   std::string typeOrForce;
   aInput.ReadValue(typeOrForce); // strip off the "allow"
   if (typeOrForce == "entity_type")
   {
      aAllowance.mFilterType = WsfDisFilterAllowance::cENTITYTYPE;
      aInput.ReadValue(aAllowance.mEntityTypeName);
   }
   else if (typeOrForce == "force")
   {
      aAllowance.mFilterType = WsfDisFilterAllowance::cENTITYFORCE;
      aInput.ReadValue(aAllowance.mEntitySide);
   }
   else
   {
      std::string errMsg("Filtered connection allowance: unknown allowance type. ");
      errMsg.append(typeOrForce);
      throw UtInput::BadValue(aInput, errMsg);
   }

   std::string trackedFlagStr;
   aInput.ReadValue(trackedFlagStr);
   if (trackedFlagStr == "tracked")
   {
      aAllowance.mTrackedFlag = WsfDisFilterAllowance::cTRACKED;
      aInput.ReadValue(aAllowance.mTrackingSide);
   }
   else if (trackedFlagStr == "all")
   {
      aAllowance.mTrackedFlag = WsfDisFilterAllowance::cALL;
   }
   else
   {
      std::string errMsg("Filtered connection allowance: unknown tracking value. ");
      errMsg.append(trackedFlagStr);
      throw UtInput::BadValue(aInput, errMsg);
   }
   return myCommand;
}

// ============================================================================
bool WsfDisInput::ProcessDeviceInput(WsfScenario& aScenario, UtInput& aInput, DeviceConfiguration& aConfiguration)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "broadcast")
   {
      aConfiguration.mDeviceType = WsfDisDevice::cDEVICE_BROADCAST;
      aInput.ReadValue(aConfiguration.mAddress);
   }
   else if (command == "multicast")
   {
      aConfiguration.mDeviceType = WsfDisDevice::cDEVICE_MULTICAST;
      aInput.ReadValue(aConfiguration.mAddress);
      aInput.ReadValue(aConfiguration.mInterfaceAddress);
   }
   else if (command == "unicast")
   {
      aConfiguration.mDeviceType = WsfDisDevice::cDEVICE_UNICAST;
      aInput.ReadValue(aConfiguration.mAddress);
   }
   else if (command == "record")
   {
      aConfiguration.mDeviceType = WsfDisDevice::cDEVICE_RECORD;
      aInput.ReadValueQuoted(aConfiguration.mAddress);
      aConfiguration.mAddress = aInput.SubstitutePathVariables(aConfiguration.mAddress);
   }
   else if (command == "playback")
   {
      aConfiguration.mDeviceType = WsfDisDevice::cDEVICE_PLAYBACK;
      aInput.ReadValueQuoted(aConfiguration.mAddress);
      aConfiguration.mAddress = aInput.SubstitutePathVariables(aConfiguration.mAddress);
   }
   else if (command == "port")
   {
      aInput.ReadValue(aConfiguration.mRecvPort);
      aInput.ValueInClosedRange(aConfiguration.mRecvPort, 0, 65535);
      aConfiguration.mSendPort = aConfiguration.mRecvPort;
   }
   else if (command == "receive_port")
   {
      aInput.ReadValue(aConfiguration.mRecvPort);
      aInput.ValueInClosedRange(aConfiguration.mRecvPort, 0, 65535);
   }
   else if (command == "send_port")
   {
      aInput.ReadValue(aConfiguration.mSendPort);
      aInput.ValueInClosedRange(aConfiguration.mSendPort, 0, 65535);
   }
   else if (command == "heartbeat_multiplier")
   {
      aInput.ReadValue(mInputHeartbeatMultiplier);
      aInput.ValueGreater(mInputHeartbeatMultiplier, 0.0);
   }
   else if (command == "entity_appearance")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if ((command == "name") || (command == "type"))
         {
            std::string name_typeId;
            std::string appearance_type;
            aInput.ReadValue(name_typeId);
            aInput.ReadValue(appearance_type);
            if (appearance_type == "afterburner")
            {
               std::string stateId;
               aInput.ReadValue(stateId);
               int stateFlag;
               if (stateId == "on")
               {
                  stateFlag = 1;
               }
               else if (stateId == "off")
               {
                  stateFlag = 0;
               }
               else
               {
                  throw UtInput::BadValue(aInput, "Afterburner state must 'on' or 'off'");
               }

               std::string type;
               aInput.ReadValue(type);
               if (!ValidSignatureTypeName(type))
               {
                  throw UtInput::BadValue(aInput, "Invalid signature type: " + type);
               }

               std::string sigState;
               aInput.ReadValue(sigState);
               NameTypeToStateIdPair idToFlagPair = std::make_pair(WsfStringId(name_typeId), stateFlag);
               if (command == "name")
               {
                  mAfterBurnerByName[idToFlagPair].emplace_back(type, sigState);
               }
               else if (command == "type")
               {
                  mAfterBurnerByType[idToFlagPair].emplace_back(type, sigState);
               }
            }
            else if (appearance_type == "articulation")
            {
               Articulation articulation;
               aInput.ReadValue(articulation.mPartNumber);
               aInput.ValueGreater(articulation.mPartNumber, 0);
               std::string variableName;
               aInput.ReadValue(variableName);
               articulation.mVariableId = variableName;
               std::string scriptName;
               aInput.ReadValue(scriptName);
               articulation.mScriptId = scriptName;
               WsfStringId keyId      = name_typeId;
               if (command == "name")
               {
                  mArticulationByName[keyId].push_back(articulation);
               }
               else if (command == "type")
               {
                  mArticulationByType[keyId].push_back(articulation);
               }
            }
            else if (appearance_type == "configuration")
            {
               int configId;
               aInput.ReadValue(configId);
               aInput.ValueInClosedRange(configId, 0, 15);
               std::string type;
               aInput.ReadValue(type);
               if (!ValidSignatureTypeName(type))
               {
                  throw UtInput::BadValue(aInput, "Invalid signature type: " + type);
               }

               std::string sigState;
               aInput.ReadValue(sigState);
               NameTypeToStateIdPair idToFlagPair         = make_pair(name_typeId, configId);
               StringToStateIdPair   sigTypeToStateIdPair = make_pair(type, WsfStringId(sigState));
               if (command == "name")
               {
                  mConfigIdByName[idToFlagPair].push_back(sigTypeToStateIdPair);
               }
               else if (command == "type")
               {
                  mConfigIdByType[idToFlagPair].push_back(sigTypeToStateIdPair);
               }
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "simple_miss_reporting")
   {
      mSimpleMissReporting = aInput.ReadBool();
   }
   else if (mComponents.ProcessComponentInput(aInput))
   {
   }
   else
   {
      // Forward command to ext_interface if it exists:
      WsfExtInput* extExtensionPtr = static_cast<WsfExtInput*>(aScenario.FindExtension("ext_interface"));
      if (extExtensionPtr != nullptr)
      {
         myCommand = extExtensionPtr->ProcessExtInput(aInput);
      }
   }
   return myCommand;
}

// ====================================================================================
//! Returns a site and application begin filtered out by index
bool WsfDisInput::GetIgnoredSiteAndApplicationPair(unsigned int                               aIndex,
                                                   std::pair<unsigned short, unsigned short>& aSiteAndAppPair)
{
   bool ok = true;
   if ((!mIgnoredSiteAndApps.empty()) && (aIndex < mIgnoredSiteAndApps.size()))
   {
      unsigned int i  = 0;
      auto         it = mIgnoredSiteAndApps.begin();
      for (; it != mIgnoredSiteAndApps.end(); ++it)
      {
         if (i == aIndex)
         {
            aSiteAndAppPair = *it;
            break;
         }
         ++i;
      }
   }
   else
   {
      ok = false;
   }
   return ok;
}


// ============================================================================
//! Used to filter out specific sites and applications.
//! Must input site and application as a pair.
bool WsfDisInput::IgnoreSiteAndApplication(unsigned short aSite, unsigned short aApp)
{
   bool ok = true;
   if (mIgnoredSiteAndApps.find(std::pair<unsigned short, unsigned short>(aSite, aApp)) == mIgnoredSiteAndApps.end())
   {
      mIgnoredSiteAndApps.insert(std::pair<unsigned short, unsigned short>(aSite, aApp));
   }
   else
   {
      ok = false;
   }
   return ok;
}

// ============================================================================
//! Called by WsfDisInterface to disable output of non-DIS standard data
void WsfDisInput::SuppressNonStandardData(bool aDoSupress)
{
   mSuppressCME_DetectBeam    = aDoSupress;
   mSuppressCME_EntityData    = aDoSupress;
   mSuppressCME_PassiveSensor = aDoSupress;
}

// ============================================================================
bool WsfDisInput::ValidSignatureTypeName(std::string& aType)
{
   bool valid = false;
   if ((aType == "infrared") || (aType == "infrared_signature"))
   {
      aType = "infrared";
      valid = true;
   }
   else if ((aType == "optical") || (aType == "optical_signature"))
   {
      aType = "optical";
      valid = true;
   }
   else if ((aType == "radar") || (aType == "radar_signature") || (aType == "rcs") || (aType == "rcs_signature"))
   {
      aType = "radar";
      valid = true;
   }
   else if ((aType == "acoustic") || (aType == "acoustic_signature"))
   {
      aType = "acoustic";
      valid = true;
   }
   else if ((aType == "inherent_contrast") || (aType == "contrast"))
   {
      aType = "inherent_contrast";
      valid = true;
   }
   else if (aType == "all")
   {
      aType = "all";
      valid = true;
   }

   return valid;
}
