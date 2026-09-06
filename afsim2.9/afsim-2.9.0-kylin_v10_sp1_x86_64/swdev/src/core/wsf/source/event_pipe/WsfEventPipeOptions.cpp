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

#include "WsfEventPipeOptions.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"

// ================================================================================================
// WsfEventPipeOptions
// ================================================================================================

bool WsfEventPipeOptions::ProcessInput(UtInput& aInput, const std::vector<std::string>& aEventNames)
{
   bool        myCommand = true;
   std::string cmd;
   aInput.GetCommand(cmd);
   if ((cmd == "enable") || (cmd == "disable"))
   {
      std::string what;
      aInput.ReadValue(what);
      int boolOptionIndex = -1;
      for (size_t i = 0; i < aEventNames.size(); ++i)
      {
         if (what == aEventNames[i])
         {
            boolOptionIndex = (int)i;
            break;
         }
      }
      if (boolOptionIndex != -1)
      {
         mBoolOptions[boolOptionIndex] = (cmd == "enable");
      }
      else
      {
         auto warning = ut::log::warning() << "Invalid \"event_pipe\" event type";
         warning.AddNote() << "Event: " << what;
         warning.AddNote() << "Location: " << aInput.GetLocation();
      }
   }
   else if (cmd == "visual_part_update_interval")
   {
      aInput.ReadValueOfType(mVisualPartUpdateInterval, UtInput::cTIME);
      aInput.ValueGreater(mVisualPartUpdateInterval, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! This is called by WsfGrammarInterface::OutputForIDE in order to retrieve the event names and their selection state.
std::map<std::string, bool> WsfEventPipeOptions::GetOptionValues(const std::vector<std::string>& aEventNames) const
{
   std::map<std::string, bool> options;
   for (size_t i = 0; i < aEventNames.size(); ++i)
   {
      options[aEventNames[i]] = mBoolOptions[i];
   }
   return options;
}

void WsfEventPipeOptions::EnableAll()
{
   std::fill(mBoolOptions.begin(), mBoolOptions.end(), true);
}

void WsfEventPipeOptions::DisableAll()
{
   std::fill(mBoolOptions.begin(), mBoolOptions.end(), false);
}
