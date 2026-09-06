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
#include "WsfEventOutput.hpp"

#include "UtInput.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfEventOutput* WsfEventOutput::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfEventOutput*>(aSimulation.FindExtension("event_output"));
}

void WsfEventOutput::PrintEvent(const wsf::event::Result& aResult) const
{
   aResult.Print(StreamRef());
}

// =================================================================================================
bool WsfEventOutputData::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command;
   aInput.GetCommand(command);
   if (command == "print_cg_miss_distance")
   {
      mPrintCGMissDist = true;
   }
   else if (command == "print_eci_locations")
   {
      bool printECI_Locations;
      aInput.ReadValue(printECI_Locations);
      mSettings.SetPrintLLA_Locations(!printECI_Locations);
   }
   else if (command == "print_failed_message_delivery_attempts")
   {
      aInput.ReadValue(mPrintFailedMessageDeliveryAttempts);
   }
   else if (command == "print_failed_sensor_detection_attempts")
   {
      aInput.ReadValue(mPrintFailedSensorDetectionAttempts);
   }
   else if (command == "print_single_line_per_event")
   {
      bool value;
      aInput.ReadValue(value);
      mSettings.SetPrintSingleLinePerEvent(value);
   }
   else if (command == "print_track_in_message")
   {
      bool value;
      aInput.ReadValue(value);
      mSettings.SetPrintTrackInMessage(value);
   }
   else if (command == "print_track_covariance")
   {
      bool value;
      aInput.ReadValue(value);
      mSettings.SetPrintTrackCovariance(value);
   }
   else if (command == "print_track_residual_covariance")
   {
      bool value;
      aInput.ReadValue(value);
      mSettings.SetPrintTrackResidualCovariance(value);
   }
   else if (command == "time_format")
   {
      std::string format;
      aInput.ReadValue(format);
      UtStringUtil::ToLower(format);
      int                    decimalFormat = 0;
      std::string::size_type decimalPos    = format.find_last_of('.');
      if (decimalPos != std::string::npos)
      {
         int         decimalCount   = 0;
         std::string fractionFormat = format.substr(decimalPos + 1);
         if (!fractionFormat.empty())
         {
            if (fractionFormat.find_first_not_of("0123456789") == std::string::npos)
            {
               // New preferred format - hms.#
               decimalCount = UtStringUtil::ToInt(fractionFormat);
               aInput.ValueInClosedRange(decimalCount, 0, 15);
            }
            else
            {
               // Old format hms.nnn
               decimalCount = static_cast<int>(format.size() - (decimalPos + 1));
            }
         }
         decimalFormat = UtTime::FmtShowPoint | decimalCount;
         format        = format.substr(0, decimalPos);
      }
      if (format == "s")
      {
         mSettings.SetTimeFormat(UtTime::FmtS | decimalFormat | UtTime::FmtNoSuffix);
      }
      else if (format == "m:s")
      {
         mSettings.SetTimeFormat(UtTime::FmtMS | decimalFormat | UtTime::FmtNoSuffix);
      }
      else if (format == "h:m:s")
      {
         mSettings.SetTimeFormat(UtTime::FmtHMS | decimalFormat | UtTime::FmtNoSuffix);
      }
      else
      {
         throw UtInput::BadValue(aInput, "time_format must be in form: [[h:]m:]s[.#] ");
      }
   }
   else if (command == "lat_lon_format")
   {
      std::string format;
      aInput.ReadValue(format);
      UtStringUtil::ToLower(format);
      int                    decimalCount        = -1;
      int                    defaultDecimalCount = 0;
      std::string::size_type decimalPos          = format.find_last_of('.');
      if (decimalPos != std::string::npos)
      {
         std::string fractionFormat = format.substr(decimalPos + 1);
         if (!fractionFormat.empty())
         {
            if (fractionFormat.find_first_not_of("0123456789") == std::string::npos)
            {
               // New preferred format - <dms>.<number>
               decimalCount = UtStringUtil::ToInt(fractionFormat);
               aInput.ValueInClosedRange(decimalCount, 0, 15);
            }
            else
            {
               // Old format <dms>.nnn
               decimalCount = static_cast<int>(format.size() - (decimalPos + 1));
            }
         }
         format = format.substr(0, decimalPos);
      }

      if (format == "d")
      {
         defaultDecimalCount = 6;
         mSettings.SetLatLonFormat(UtAngle::FmtD);
      }
      else if (format == "d:m")
      {
         defaultDecimalCount = 4;
         mSettings.SetLatLonFormat(UtAngle::FmtDM);
      }
      else if (format == "d:m:s")
      {
         defaultDecimalCount = 2;
         mSettings.SetLatLonFormat(UtAngle::FmtDMS);
      }
      else
      {
         throw UtInput::BadValue(aInput, "lat_lon_format must be in form: d[:m[:s]][.#] ");
      }
      if (decimalCount == -1)
      {
         decimalCount = defaultDecimalCount;
      }
      mSettings.SetLatLonFormat(mSettings.GetLatLonFormat() + decimalCount);
   }
   else
   {
      myCommand = wsf::event::output::Data::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
void WSF_EXPORT Register_event_output(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("event_output"))
   {
      using ScenarioExtension = wsf::event::output::ScenarioExtension<WsfEventOutputData, WsfEventOutput>;
      aApplication.RegisterExtension("event_output", ut::make_unique<WsfDefaultApplicationExtension<ScenarioExtension>>());
   }
}
