// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 - 2019 Lockheed Martin Corporation. All Rights Reserved.
// Lockheed Martin Corporation hereby provides the below source code to the
// United States Government with Unlimited Rights as such is defined in
// DFARS 252.227 - 7014.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSIMDIS_Interface.hpp"

#include <iomanip>
#include <iostream>
#include <fstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "UtPlugin.hpp"
#include "UtStringUtil.hpp"

#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "ext/WsfExtInterface.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfVersion.hpp"
#include "WsfWeaponObserver.hpp"

void WSF_SIMDIS_EXPORT Register_wsf_simdis(WsfApplication& aApplication)
{
   aApplication.RegisterFeature("simdis_interface", "wsf_simdis"); // Indicate feature is present
   aApplication.RegisterExtension("wsf_simdis", ut::make_unique<WsfDefaultApplicationExtension<wsf::simdis::ScenarioExtension>>());
}

extern "C"
{
   //! This method is called when the plugin is loaded to ensure that the plugin and the executable loading it were built with
   //! the same version of the plugin API.
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion = UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION,
                                 WSF_PLUGIN_API_MINOR_VERSION,
                                 WSF_PLUGIN_API_COMPILER_STRING);
   }
   //! This method is called when the plugin is loaded. It must have exactly this signature (name and parameter) to succeed.
   //! As we only have an application reference at load time, one usually registers an application extension, within which the
   //! ScenarioCreated method can be overridden to gain access to a scenario.  If one also needs
   //! access to the simulation, one should also instantiate and register a simulation extension by overriding
   //! the SimulationCreated method in the scenario extension.
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication)
   {
      Register_wsf_simdis(aApplication);
   }
}

wsf::simdis::ScenarioExtension::ScenarioExtension()
   : WsfScenarioExtension()
   , mOutputFilename("")
   , mHitIcon("splash")
   , mKillIcon("expl-1")
   , mEventDuration(1.0)
   , mEnableBeams(true)
   , mDefaultBeamColor("yellow")
   , mLatitude(0.0)
   , mLongitude(0.0)
   , mAltitude(0.0)
{

}

wsf::simdis::ScenarioExtension* wsf::simdis::ScenarioExtension::Find(const WsfScenario& aScenario)
{
   return static_cast<ScenarioExtension*>(aScenario.FindExtension("wsf_simdis"));
}

void wsf::simdis::ScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<Interface>(*this));
}

bool wsf::simdis::ScenarioExtension::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "simdis_interface")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "file")
         {
            std::string fileName;
            aInput.ReadValueQuoted(fileName);
            fileName = aInput.SubstitutePathVariables(fileName);
            if (fileName == "NULL")
            {
               mOutputFilename = "";
            }
            else
            {
               mOutputFilename = fileName;
            }
         }
         else if (command == "hit_icon")
         {
            std::string hitIcon;
            aInput.ReadValue(hitIcon);
            if (hitIcon == "NULL")
            {
               mHitIcon = "";
            }
            else
            {
               mHitIcon = hitIcon;
            }
         }
         else if (command == "kill_icon")
         {
            std::string killicon;
            aInput.ReadValue(killicon);
            if (killicon == "NULL")
            {
               mKillIcon = "";
            }
            else
            {
               mKillIcon = killicon;
            }
         }
         else if (command == "event_duration")
         {
            aInput.ReadValueOfType(mEventDuration, UtInput::cTIME);
         }
         else if (command == "enable_beams")
         {
            aInput.ReadValue(mEnableBeams);
         }
         else if (command == "default_beam_color")
         {
            aInput.ReadValue(mDefaultBeamColor);
         }
         else if (command == "beam_color")
         {
            std::string sensorName;
            std::string color;
            aInput.ReadValue(sensorName);
            aInput.ReadValue(color);
            mBeamColorMap[sensorName] = color;
         }
         else if (command == "reference_lla")
         {
            aInput.ReadValueOfType(mLatitude, UtInput::cLATITUDE);
            aInput.ReadValueOfType(mLongitude, UtInput::cLONGITUDE);
            aInput.ReadValueOfType(mAltitude, UtInput::cLENGTH);
         }
         else if (command == "define_fhn")
         {
            std::string side;
            aInput.ReadValue(side);
            std::string fhn;
            aInput.ReadValue(fhn);
            mFHN_Map[side] = fhn[0];
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

const std::string& wsf::simdis::ScenarioExtension::GetBeamColor(const std::string& aSensorTypeName) const
{
   // get a beam color for the given sensor type name
   std::map<std::string, std::string>::const_iterator it = mBeamColorMap.find(aSensorTypeName);
   if (it != mBeamColorMap.end())
   {
      return it->second;
   }
   else
   {
      return mDefaultBeamColor;
   }
}

const char wsf::simdis::ScenarioExtension::GetFHN(const std::string& aSide) const
{
   // get the FHN value for a given side
   std::map<std::string, char>::const_iterator it = mFHN_Map.find(aSide);
   if (it != mFHN_Map.end())
   {
      return it->second;
   }
   else
   {
      std::string lSide(aSide);
      UtStringUtil::ToLower(lSide);

      if (lSide == "blue")
      {
         return 'F';
      }
      else if (lSide == "red")
      {
         return 'H';
      }
      else
      {
         return 'N';
      }
   }
}

wsf::simdis::Interface::Interface(ScenarioExtension& aExtension)
   : mExtension(aExtension)
{

}

wsf::simdis::Interface* wsf::simdis::Interface::Find(const WsfSimulation& aSimulation)
{
   return static_cast<Interface*>(aSimulation.FindExtension("simdis"));
}

void wsf::simdis::Interface::AddedToSimulation()
{
   if (mExtension.GetOutputFilename().empty()) { return; }

   mOutputFilename = GetSimulation().SubstituteOutputFileVariables(mExtension.GetOutputFilename());

   mUID_Assigner = 1;
   mHitID_Assigner = 1;
   mKillID_Assigner = 1;

   // get a stream to our output file
   OpenOutput();
   if (mFileStreamPtr == nullptr)
   {
      std::cerr << "***** ERROR: Failed to open event_pipe file: " << mOutputFilename << '\n';
      return;
   }
   GetScenario().GetSystemLog().WriteOutputLogEntry("ASI", mOutputFilename);

   WriteHeader();

   mBaseCallbacks += WsfObserver::PlatformInitialized(&GetSimulation()).Connect(&Interface::PlatformInitialized, this);
   mBaseCallbacks += WsfObserver::PlatformBroken(&GetSimulation()).Connect(&Interface::PlatformKilled, this);
   mBaseCallbacks += WsfObserver::WeaponHit(&GetSimulation()).Connect(&Interface::WeaponHit, this);
   mBaseCallbacks += WsfObserver::SensorTrackInitiated(&GetSimulation()).Connect(&Interface::SensorTrackInitiated, this);
   mBaseCallbacks += WsfObserver::SensorTrackDropped(&GetSimulation()).Connect(&Interface::SensorTrackDropped, this);

   WsfExtInterface* extPtr = WsfExtInterface::Find(GetSimulation());
   WsfExtEntityDeadReckon* dr = extPtr->GetEntityDeadReckon();
   dr->RequireExternalEntities(true);
   dr->RequestService(true);

   mEntityStateCallbacks += dr->DeadReckonChanged.Connect(&Interface::DeadReckonChange, this);
}

bool wsf::simdis::Interface::OpenOutput()
{
   mFileStreamPtr = new std::ofstream(mOutputFilename.c_str(), std::ofstream::out | std::ofstream::trunc);
   return mFileStreamPtr != nullptr;
}

void wsf::simdis::Interface::WriteHeader()
{
   *mFileStreamPtr << "# -*- Mode: sh -*-" << std::endl;
   *mFileStreamPtr << "# SIMDIS ASCII Scenario Input (ASI) File Format" << std::endl;
   *mFileStreamPtr << std::endl;
   *mFileStreamPtr << "# Scenario Initialization Keywords" << std::endl;
   *mFileStreamPtr << "Version       22" << std::endl;
   *mFileStreamPtr << "RefLLA        " << mExtension.GetLatitude() << " " << mExtension.GetLongitude() << " " << mExtension.GetAltitude() << " //LLA - Latitude, Longitude, Altitude" << std::endl;
   *mFileStreamPtr << "CoordSystem   \"ECEF\"" << std::endl;
   *mFileStreamPtr << "DegreeAngles  1" << std::endl;
   *mFileStreamPtr << "ScenarioInfo  \"Generated by AFSIM - " << GetSimulation().GetApplication().GetApplicationName() << " " << wsf::version::GetString() << "\"" << std::endl;
   *mFileStreamPtr << std::endl;
}

void wsf::simdis::Interface::PlatformInitialized(double       aSimTime,
                                                 WsfPlatform* aPlatformPtr)
{
   unsigned int uid = LookupPlatformId(aPlatformPtr->GetName());
   if (mInitSet.find(uid) == mInitSet.end())
   {
      *mFileStreamPtr << "PlatformID     " << uid << std::endl;
      *mFileStreamPtr << "PlatformName   " << uid << " \"" << aPlatformPtr->GetName() << "\"" << std::endl;
      *mFileStreamPtr << "PlatformIcon   " << uid << " \"" << aPlatformPtr->GetIcon() << "\"" << std::endl;
      std::string side = aPlatformPtr->GetSide();
      char fhn = mExtension.GetFHN(side);
      *mFileStreamPtr << "PlatformFHN    " << uid << " \"" << fhn << "\"" << std::endl;
      double lla[3];
      aPlatformPtr->GetLocationLLA(lla[0], lla[1], lla[2]);
      *mFileStreamPtr << "PlatformRefLLA " << uid << " " << lla[0] << " " << lla[1] << " " << lla[2] << std::endl;
      *mFileStreamPtr << std::endl;
      mInitSet.insert(uid);
   }
}

void wsf::simdis::Interface::PlatformKilled(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (!mExtension.GetKillIcon().empty())
   {
      if (aPlatformPtr->GetDamageFactor() >= 1.0)
      {
         unsigned int uid = mUID_Assigner++;
         unsigned int killid = mKillID_Assigner++;

         UpdatePlatform(aSimTime, aPlatformPtr);

         *mFileStreamPtr << "PlatformID   " << uid << std::endl;
         *mFileStreamPtr << "PlatformName " << uid << " \"" << "Kill " << killid << "\"" << std::endl;
         *mFileStreamPtr << "PlatformIcon " << uid << " \"" << mExtension.GetKillIcon() << "\"" << std::endl;
         *mFileStreamPtr << "PlatformData " << uid << " " << aSimTime;
         double ecef[3];
         aPlatformPtr->GetLocationWCS(ecef);
         double ptp[3];
         aPlatformPtr->GetOrientationWCS(ptp[0], ptp[1], ptp[2]);
         *mFileStreamPtr << std::setprecision(12) << " " << ecef[0] << " " << ecef[1] << " " << ecef[2];
         *mFileStreamPtr << std::setprecision(7) << " " << ptp[0] * UtMath::cDEG_PER_RAD << " " << ptp[1] * UtMath::cDEG_PER_RAD << " " << ptp[2] * UtMath::cDEG_PER_RAD;
         *mFileStreamPtr << " 0 0 0" << std::endl;

         *mFileStreamPtr << "PlatformData " << uid << " " << aSimTime + mExtension.GetEventDuration();
         *mFileStreamPtr << std::setprecision(12) << " " << ecef[0] << " " << ecef[1] << " " << ecef[2];
         *mFileStreamPtr << std::setprecision(7) << " " << ptp[0] * UtMath::cDEG_PER_RAD << " " << ptp[1] * UtMath::cDEG_PER_RAD << " " << ptp[2] * UtMath::cDEG_PER_RAD;
         *mFileStreamPtr << " 0 0 0" << std::endl;
      }
   }
}

void wsf::simdis::Interface::WeaponHit(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr)
{
   if (!mExtension.GetHitIcon().empty())
   {
      unsigned int uid = mUID_Assigner++;
      unsigned int hitid = mHitID_Assigner++;

      *mFileStreamPtr << "PlatformID   " << uid << std::endl;
      *mFileStreamPtr << "PlatformName " << uid << " \"" << "Hit " << hitid << "\"" << std::endl;
      *mFileStreamPtr << "PlatformIcon " << uid << " \"" << mExtension.GetHitIcon() << "\"" << std::endl;
      *mFileStreamPtr << "PlatformData " << uid << " " << aSimTime;
      double ecef[3];
      aTargetPtr->GetLocationWCS(ecef);
      double ptp[3];
      aTargetPtr->GetOrientationWCS(ptp[0], ptp[1], ptp[2]);
      *mFileStreamPtr << std::setprecision(12) << " " << ecef[0] << " " << ecef[1] << " " << ecef[2];
      *mFileStreamPtr << std::setprecision(7) << " " << ptp[0] * UtMath::cDEG_PER_RAD << " " << ptp[1] * UtMath::cDEG_PER_RAD << " " << ptp[2] * UtMath::cDEG_PER_RAD;
      *mFileStreamPtr << " 0 0 0" << std::endl;

      *mFileStreamPtr << "PlatformData " << uid << " " << aSimTime + mExtension.GetEventDuration();
      *mFileStreamPtr << std::setprecision(12) << " " << ecef[0] << " " << ecef[1] << " " << ecef[2];
      *mFileStreamPtr << std::setprecision(7) << " " << ptp[0] * UtMath::cDEG_PER_RAD << " " << ptp[1] * UtMath::cDEG_PER_RAD << " " << ptp[2] * UtMath::cDEG_PER_RAD;
      *mFileStreamPtr << " 0 0 0" << std::endl;
   }
}

void wsf::simdis::Interface::SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   if (mExtension.EnableBeams())
   {
      unsigned int uid = LookupTrackId(aTrackPtr->GetTrackId());
      if (uid != 0)
      {
         *mFileStreamPtr << "BeamOnOffCmd    " << uid << " " << aSimTime << " 0" << std::endl;
      }
   }
}

void wsf::simdis::Interface::SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   if (mExtension.EnableBeams())
   {
      size_t targetId = aTrackPtr->GetTargetIndex();
      WsfPlatform* target = GetSimulation().GetPlatformByIndex(targetId);
      if (target)
      {
         unsigned int pid = LookupPlatformId(aSensorPtr->GetPlatform()->GetName());
         unsigned int tid = LookupPlatformId(target->GetName());
         const WsfTrackId& id = aTrackPtr->GetTrackId();

         std::string color = mExtension.GetBeamColor(aSensorPtr->GetType());
         if (strncmp(color.c_str(), "0x00", 4) == 0)
         {
            // fully transparent, no need to render
            mTrackIdLookup[id] = 0;
            return;
         }

         unsigned int uid = LookupTrackId(aTrackPtr->GetTrackId());
         *mFileStreamPtr << "BeamID          " << pid << " " << uid << std::endl;
         *mFileStreamPtr << "BeamType        " << uid << " \"TARGET\"" << std::endl;
         *mFileStreamPtr << "BeamDesc        " << uid << " \"" << aSensorPtr->GetPlatform()->GetName() << " : " << aSensorPtr->GetName() << " tracking " << target->GetName() << "\"" << std::endl;

         double wcs[3];
         target->GetRelativeLocationWCS(aSensorPtr->GetPlatform(), wcs);

         double bw = 375000.0 / UtVec3d::Magnitude(wcs);

         *mFileStreamPtr << "VertBW          " << uid << " " << bw << std::endl;
         *mFileStreamPtr << "HorzBW          " << uid << " " << bw << std::endl;
         *mFileStreamPtr << "BeamOnOffCmd    " << uid << " " << aSimTime << " 1" << std::endl;
         *mFileStreamPtr << "BeamColorCmd    " << uid << " " << aSimTime << " " << color << std::endl;
         *mFileStreamPtr << "BeamTargetIdCmd " << uid << " " << aSimTime << " " << tid << std::endl;
      }
   }
}

void wsf::simdis::Interface::DeadReckonChange(double                              aSimTime,
                                              WsfExtEntityDeadReckon::PlatformDR& aDR)
{
   UpdatePlatform(aSimTime, aDR.mPlatformPtr);
}

void wsf::simdis::Interface::UpdatePlatform(double       aSimTime,
                                            WsfPlatform* aPlatformPtr)
{
   unsigned int uid = LookupPlatformId(aPlatformPtr->GetName());
   if (mInitSet.find(uid) == mInitSet.end())
   {
      PlatformInitialized(aSimTime, aPlatformPtr);
   }
   double ecef[3];
   aPlatformPtr->GetLocationWCS(ecef);
   double ptp[3];
   aPlatformPtr->GetOrientationWCS(ptp[0], ptp[1], ptp[2]);
   double vwcs[3];
   aPlatformPtr->GetVelocityWCS(vwcs);
   *mFileStreamPtr << "PlatformData   " << uid << " " << aSimTime;
   *mFileStreamPtr << std::setprecision(12) << " " << ecef[0] << " " << ecef[1] << " " << ecef[2];
   *mFileStreamPtr << std::setprecision(7) << " " << ptp[0] * UtMath::cDEG_PER_RAD << " " << ptp[1] * UtMath::cDEG_PER_RAD << " " << ptp[2] * UtMath::cDEG_PER_RAD;
   *mFileStreamPtr << " " << vwcs[0] << " " << vwcs[1] << " " << vwcs[2] << std::endl;
}

unsigned int wsf::simdis::Interface::LookupPlatformId(const std::string& aPlatformName)
{
   std::map<std::string, unsigned int>::iterator it = mPlatformNameLookup.find(aPlatformName);
   if (it != mPlatformNameLookup.end())
   {
      return it->second;
   }
   else
   {
      mPlatformNameLookup[aPlatformName] = mUID_Assigner++;
      return mPlatformNameLookup[aPlatformName];
   }
}

unsigned int wsf::simdis::Interface::LookupTrackId(const WsfTrackId& aTrackId)
{
   std::map<WsfTrackId, unsigned int>::iterator it = mTrackIdLookup.find(aTrackId);
   if (it != mTrackIdLookup.end())
   {
      return it->second;
   }
   else
   {
      mTrackIdLookup[aTrackId] = mUID_Assigner++;
      return mTrackIdLookup[aTrackId];
   }
}
