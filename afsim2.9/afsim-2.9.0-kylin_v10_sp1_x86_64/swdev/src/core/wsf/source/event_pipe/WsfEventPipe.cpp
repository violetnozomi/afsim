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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfEventPipe.hpp"

#include "UtInputBlock.hpp"
#include "WsfEventPipeInterface.hpp"
#include "WsfEventPipeSchema.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

// ================================================================================================
// Extension registration method.
// ================================================================================================

void WSF_EXPORT Register_event_pipe(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("event_pipe"))
   {
      // This extension REQUIRES the "ext_interface" extension.
      WSF_REGISTER_EXTENSION(aApplication, ext_interface);
      aApplication.RegisterExtension("event_pipe",
                                     ut::make_unique<WsfDefaultApplicationExtension<WsfEventPipeExtension>>());
      aApplication.ExtensionDepends("event_pipe", "ext_interface", true);
      // To register for wsf draw updates, WsfDraw must be added first
      aApplication.ExtensionDepends("event_pipe", "wsf_draw", false);
   }
}

// ================================================================================================
// The scenario extension...

WsfEventPipeExtension* WsfEventPipeExtension::Find(const WsfScenario& aScenario)
{
   return static_cast<WsfEventPipeExtension*>(aScenario.FindExtension("event_pipe"));
}

WsfEventPipeExtension::WsfEventPipeExtension()
   : mData()
   , mDefaultPreset()
   , mDefaultPresetDefined(false)
{
   // Initialize all of the standard events.
   mEventIds.cBASE_DATA = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("_BASE_DATA");
   mEventIds.cDETECTION_CHANGE = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("DETECTION_CHANGE");
   mEventIds.cENTITY_STATE = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("ENTITY_STATE");
   mEventIds.cTRACK = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("TRACK");
   mEventIds.cTRACK_UPDATE = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("TRACK_UPDATE");
   mEventIds.cDETECTION_ATTEMPT = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("DETECTION_ATTEMPT");
   mEventIds.cDRAW = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("DRAW");
   mEventIds.cMESSAGE_RECEIVED = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("MESSAGE_RECEIVED");
   mEventIds.cMESSAGE_HOP = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("MESSAGE_HOP");
   mEventIds.cCOMM_INFO = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("COMM_INFO");
   mEventIds.cMESSAGE_TRANSMITTED = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("MESSAGE_TRANSMITTED");
   mEventIds.cCOMMENT = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("COMMENT");
   mEventIds.cBEHAVIOR_TOOL = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("BEHAVIOR_TOOL");
   mEventIds.cAUX_DATA = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("AUX_DATA");
   mEventIds.cROUTE_CHANGED = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("ROUTE_CHANGED");
   mEventIds.cZONES = static_cast<int>(mEventNames.size());
   mEventNames.emplace_back("ZONES");

   // Build the standard list of settings.
   // Define named sets of events that enabled. These include:
   //   - "default" - The standard events that are emitted.
   //   - "high"    - "default" + a few others.
   //   - "full"    - All events.
   //   - "low"     - This is the same as default.
   WsfEventPipeOptions defaultSettings;
   defaultSettings.SetMaxEventId((int)mEventNames.size());

   defaultSettings.EnableOption(mEventIds.cBASE_DATA);
   defaultSettings.EnableOption(mEventIds.cENTITY_STATE);
   defaultSettings.EnableOption(mEventIds.cDRAW);
   defaultSettings.EnableOption(mEventIds.cDETECTION_CHANGE);
   defaultSettings.EnableOption(mEventIds.cCOMMENT);
   defaultSettings.EnableOption(mEventIds.cTRACK);
   defaultSettings.EnableOption(mEventIds.cAUX_DATA);
   defaultSettings.EnableOption(mEventIds.cROUTE_CHANGED);
   defaultSettings.EnableOption(mEventIds.cZONES);

   mData.mDetailSettings["default"] = defaultSettings;

   WsfEventPipeOptions high = defaultSettings;
   high.SetMaxEventId((int)mEventNames.size());
   high.EnableOption(mEventIds.cTRACK_UPDATE);
   high.EnableOption(mEventIds.cMESSAGE_RECEIVED);
   high.EnableOption(mEventIds.cMESSAGE_HOP);
   high.EnableOption(mEventIds.cMESSAGE_TRANSMITTED);
   high.EnableOption(mEventIds.cCOMM_INFO);
   high.EnableOption(mEventIds.cBEHAVIOR_TOOL);
   mData.mDetailSettings["high"] = high;

   WsfEventPipeOptions full;
   full.SetMaxEventId((int)mEventNames.size());
   full.EnableAll();
   mData.mDetailSettings["full"] = full;

   WsfEventPipeOptions low = defaultSettings;
   low.SetMaxEventId((int)mEventNames.size());
   mData.mDetailSettings["low"] = low;

   // insert null terminator
   AddSchema(cWsfEventPipe_SCHEMA);
}

void WsfEventPipeExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfEventPipeInterface>(*this, mData));
}

bool WsfEventPipeExtension::ProcessInput(UtInput& aInput)
{
   if (aInput.GetCommand() == "event_pipe")
   {
      if (!mDefaultPresetDefined)
      {
         mDefaultPresetDefined = true;
         mDefaultPreset        = mData.mDetailSettings["default"];
      }
      UtInputBlock block(aInput);
      std::string  cmd;
      while (block.ReadCommand(cmd))
      {
         if (cmd == "file")
         {
            aInput.ReadValueQuoted(mData.mFileName);
            mData.mFileName = aInput.SubstitutePathVariables(mData.mFileName);
            if (mData.mFileName == "NULL")
            {
               mData.mFileName.clear();
            }
         }
         else if (cmd == "test_without_file")
         {
            mData.mTestMode = true;
         }
         else if (cmd == "entity_state_angle_threshold")
         {
            aInput.ReadValueOfType(mData.mAngleThreshold.emplace(), UtInput::cANGLE);
            aInput.ValueInClosedRange(mData.mAngleThreshold.value(), 0.0, UtMath::cPI);
         }
         else if (cmd == "entity_state_position_threshold")
         {
            double threshold;
            aInput.ReadValueOfType(threshold, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(threshold, 0.0);
            mData.mPositionThresholdSquared = threshold * threshold;
         }
         else if (cmd == "disable_entity_state_thresholds")
         {
            mData.mPositionThresholdSquared.reset();
            mData.mAngleThreshold.reset();
         }
         else if (cmd == "entity_state_maximum_interval")
         {
            aInput.ReadValueOfType(mData.mEntityStateMaximumInterval, UtInput::cTIME);
            aInput.ValueGreater(mData.mEntityStateMaximumInterval, 0.0);
         }
         else if (cmd == "maximum_mover_update_interval")
         {
            aInput.ReadValueOfType(mData.mMaximumMoverUpdateInterval, UtInput::cTIME);
            aInput.ValueGreaterOrEqual(mData.mMaximumMoverUpdateInterval, 0.0);
         }
         else if (mData.mDetailSettings["default"].ProcessInput(aInput, this->GetEventNames()))
         {
         }
         else if (cmd == "use_preset")
         {
            std::string preset;
            aInput.ReadValue(preset);
            if (mData.mDetailSettings.find(preset) != mData.mDetailSettings.end())
            {
               if (preset == "default")
               {
                  mData.mDetailSettings["default"] = mDefaultPreset;
               }
               else
               {
                  mData.mDetailSettings["default"] = mData.mDetailSettings[preset];
               }
            }
         }
         else if (cmd == "detail")
         {
            std::string level;
            aInput.ReadValue(level);
            if (mData.mDetailSettings.find(level) == mData.mDetailSettings.end())
            {
               throw UtInput::BadValue(aInput, "Not a detail level");
            }
            aInput.ReadCommand(cmd);
            if (cmd == "platform")
            {
               std::string platformName;
               aInput.ReadValue(platformName);
               WsfEventPipeInput::CriteriaToDetail crit;
               crit.mCriteria.mText    = platformName;
               crit.mCriteria.mKind    = WsfEventPipeOptionsCriteria::cPLATFORM_NAME;
               crit.mDetailSettingName = level;
               mData.mCriteriaToDetail.push_back(crit);
            }
            else if (cmd == "category")
            {
               std::string catName;
               aInput.ReadValue(catName);
               WsfEventPipeInput::CriteriaToDetail crit;
               crit.mCriteria.mText    = catName;
               crit.mCriteria.mKind    = WsfEventPipeOptionsCriteria::cCATEGORY_NAME;
               crit.mDetailSettingName = level;
               mData.mCriteriaToDetail.push_back(crit);
            }
            else if (cmd == "type")
            {
               std::string typeName;
               aInput.ReadValue(typeName);
               WsfEventPipeInput::CriteriaToDetail crit;
               crit.mCriteria.mText    = typeName;
               crit.mCriteria.mKind    = WsfEventPipeOptionsCriteria::cPLATFORM_TYPE;
               crit.mDetailSettingName = level;
               mData.mCriteriaToDetail.push_back(crit);
            }
            else if (cmd == "side")
            {
               std::string sideName;
               aInput.ReadValue(sideName);
               WsfEventPipeInput::CriteriaToDetail crit;
               crit.mCriteria.mText    = sideName;
               crit.mCriteria.mKind    = WsfEventPipeOptionsCriteria::cSIDE_NAME;
               crit.mDetailSettingName = level;
               mData.mCriteriaToDetail.push_back(crit);
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
      mData.mEnable = true;
      return true;
   }
   return false;
}

//! Add a new event that can be selected to be included in the output stream.
//! Other extensions can define events that can be included in the output stream.
//!
//! Extensions that want to include additional events should call this option in their scenario
//! extension constructor and should also call AddSchema.
//!
//! @param aOptionName   The name by which the event is to be known.
//! @param aDefaultValue The default value for the "default" and "low" sets.
//! @returns The unique index of the event.
int WsfEventPipeExtension::RegisterEventOption(const std::string& aOptionName, bool aDefaultValue)
{
   int optionIndex = (int)mEventNames.size();
   mEventNames.push_back(aOptionName);
   for (auto& detailSetting : mData.mDetailSettings)
   {
      detailSetting.second.SetMaxEventId(optionIndex);
      detailSetting.second.EnableOption(optionIndex, aDefaultValue);
   }
   mData.mDetailSettings["low"].EnableOption(optionIndex, false);
   mData.mDetailSettings["full"].EnableOption(optionIndex, true);
   return optionIndex;
}

void WsfEventPipeExtension::AddSchema(const std::string& aSchemaText)
{
   mSchema.Insert(mSchema.Size() - 1, std::string(aSchemaText));
}

void WsfEventPipeExtension::AddCallback(std::unique_ptr<UtCallback> aCallbackPtr)
{
   mCallbacks.Add(std::move(aCallbackPtr));
}

//! Return the unique integer identifier for the specified event name.
//! @name aName The event name whose identifier is desired.
//! @returns The identifier.
int WsfEventPipeExtension::GetEventId(const std::string& aName) const
{
   const std::vector<std::string>& names = GetEventNames();
   for (size_t i = 0; i < names.size(); ++i)
   {
      if (names[i] == aName)
      {
         return (int)i;
      }
   }
   return 0;
}
