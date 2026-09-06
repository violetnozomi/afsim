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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************
#include "SurveillancePart.hpp"

#include <cstdio>
#include <cstdlib>
#include <iomanip>

#include "ComputerProcessor.hpp"
#include "FieldTypes.hpp"
#include "Interface.hpp"
#include "Message3_2.hpp"
#include "Message3_3.hpp"
#include "Message3_4.hpp"
#include "Message3_5.hpp"
#include "Message3_6.hpp"
#include "UtScriptContext.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfNonExportableUtil.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "script/WsfScriptContext.hpp"

namespace WsfL16
{

SurveillancePart::SurveillancePart(WsfScenario& aScenario)
   : ComputerPart(aScenario)
{
   mSendInterval.SetUpdateInterval(.25);
   mSpatialDomainInput << WSF_SPATIAL_DOMAIN_AIR << "air";
   mSpatialDomainInput << WSF_SPATIAL_DOMAIN_SURFACE << "surface";
   mSpatialDomainInput << WSF_SPATIAL_DOMAIN_SUBSURFACE << "subsurface";
   mSpatialDomainInput << WSF_SPATIAL_DOMAIN_LAND << "land";
   mSpatialDomainInput << WSF_SPATIAL_DOMAIN_SPACE << "space";
   mTrackVariablePtr = mContextPtr->GetContext().RegisterVariable("TRACK", "WsfTrack");
}

SurveillancePart::SurveillancePart(const SurveillancePart& aSrc)
   : ComputerPart(aSrc)
   , mMaxSendInterval(aSrc.mMaxSendInterval)
   , mSendNonSensorReports(aSrc.mSendNonSensorReports)
   , mSuppressDeadTargets(aSrc.mSuppressDeadTargets)
   , mIgnoreReportingResponsibility(aSrc.mIgnoreReportingResponsibility)
   , mTrackNumberRange(aSrc.mTrackNumberRange)
   , mDefaultTrackDomain(aSrc.mDefaultTrackDomain)
   , mSuppressDomains(aSrc.mSuppressDomains)
   , mSuppressSensors(aSrc.mSuppressSensors)
   , mSpatialDomainInput(aSrc.mSpatialDomainInput)
   , mInterfacePtr(aSrc.mInterfacePtr)
{
   mTrackVariablePtr = mContextPtr->GetContext().RegisterVariable("TRACK", "WsfTrack");
}

// virtual
ComputerPart* SurveillancePart::Clone() const
{
   return new SurveillancePart(*this);
}

// virtual
bool SurveillancePart::Initialize(double aSimTime, ComputerProcessor& aComputer)
{
   mInterfacePtr = aComputer.GetInterface();
   if (mTrackNumberRange.second < mTrackNumberRange.first)
   {
      std::swap(mTrackNumberRange.first, mTrackNumberRange.second);
   }

   if (mTrackNumberRange.second - mTrackNumberRange.first <= 0)
   {
      mTrackNumberRange.first  = GetSimulation()->GetRandom().Uniform(0, (524287 - 4096) - 1);
      mTrackNumberRange.second = mTrackNumberRange.first + 4096;
   }
   mNextTrackNumber = mTrackNumberRange.first;

   bool ok       = ComputerPart::Initialize(aSimTime, aComputer);
   mTrackManager = &GetPlatform()->GetTrackManager();
   mTrackVariablePtr->GetPointer()->SetAppObject(nullptr);

   auto iter = std::find_if(std::begin(mSuppressSensors),
                            std::end(mSuppressSensors),
                            [this](WsfStringId sensorName)
                            { return GetPlatform()->GetComponent<WsfSensor>(sensorName) == nullptr; });
   if (iter != std::end(mSuppressSensors))
   {
      ok = false;
      ut::log::info() << "Sensor: " << *iter << " does not exist.";
   }

   if (ok)
   {
      mCallbacks.Add(mTrackManager->LocalTrackUpdated.Connect(
         [this](double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
         {
            if (mSendNonSensorReports)
            {
               TrackUpdated(*aLocalTrackPtr);
            }
         }));

      mCallbacks.Add(mTrackManager->LocalTrackInitiated.Connect(
         [this](double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
         {
            if (mSendNonSensorReports)
            {
               TrackCreated(*aLocalTrackPtr);
            }
         }));

      mCallbacks.Add(mTrackManager->LocalTrackDropped.Connect(
         [this](double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
         {
            if (mSendNonSensorReports)
            {
               TrackDropped(*aLocalTrackPtr);
            }
         }));

      mCallbacks.Add(mTrackManager->RawTrackReceived.Connect(
         [this](double aSimTime, const WsfTrack* aRawTrackPtr)
         {
            if (!mSendNonSensorReports &&
                (aRawTrackPtr->GetTrackType() == WsfTrack::cFILTERED_SENSOR ||
                 aRawTrackPtr->GetTrackType() == WsfTrack::cUNFILTERED_SENSOR) &&
                !IsSensorTrackSuppressed(*aRawTrackPtr))
            {
               RawTrackReceived(*aRawTrackPtr);
            }
         }));

      mCallbacks.Add(mTrackManager->RawTrackDropped.Connect(
         [this](double aSimTime, const WsfTrack* aRawTrackPtr)
         {
            if (!mSendNonSensorReports &&
                (aRawTrackPtr->GetTrackType() == WsfTrack::cFILTERED_SENSOR ||
                 aRawTrackPtr->GetTrackType() == WsfTrack::cUNFILTERED_SENSOR) &&
                !IsSensorTrackSuppressed(*aRawTrackPtr))
            {
               RawTrackDropped(*aRawTrackPtr);
            }
         }));
   }
   return ok;
}

// virtual
bool SurveillancePart::ProcessInput(UtInput& aInput)
{
   bool ok = true;
   int  domain;
   if (aInput.GetCommand() == "track_number_range")
   {
      const char errorString[] = "Track number formatted incorrectly.\n"
                                 "Should be 5 digits: XXOOO , where O is a value between 0-7, \n"
                                 "And X is a value between 0-7 or A-Z, excluding 'I' and 'O'.\n";

      Messages::TrackNumberField tn1, tn2;
      std::string                low, high;
      aInput.ReadValue(low);
      try
      {
         tn1.SetTrackNumber(low);
      }
      catch (std::exception&)
      {
         throw UtInput::BadValue(aInput, errorString);
      }
      aInput.ReadValue(high);
      try
      {
         tn2.SetTrackNumber(high);
      }
      catch (std::exception&)
      {
         throw UtInput::BadValue(aInput, errorString);
      }

      mTrackNumberRange = std::make_pair(tn1.GetRawData(), tn2.GetRawData());
   }
   else if (mSpatialDomainInput.ProcessInput("environment", aInput, mDefaultTrackDomain))
   {
   }
   else if (mSpatialDomainInput.ProcessInput("suppress_domain", aInput, domain))
   {
      mSuppressDomains.insert(domain);
   }
   else if (aInput.GetCommand() == "suppress_sensor")
   {
      std::string sensorName;
      aInput.ReadCommand(sensorName);
      mSuppressSensors.insert(sensorName);
   }
   else if (aInput.GetCommand() == "ignore_reporting_responsibility")
   {
      aInput.ReadValue(mIgnoreReportingResponsibility);
   }
   else if (aInput.GetCommand() == "send_non_sensor_reports")
   {
      aInput.ReadValue(mSendNonSensorReports);
   }
   else if (aInput.GetCommand() == "suppress_dead_targets")
   {
      aInput.ReadValue(mSuppressDeadTargets);
   }
   else if (aInput.GetCommand() == "maximum_send_interval")
   {
      aInput.ReadValueOfType(mMaxSendInterval, UtInput::cTIME);
   }
   else
   {
      ok = ComputerPart::ProcessInput(aInput);
   }
   return ok;
}

//! Handle an incoming message
// virtual
void SurveillancePart::HandleReceive(double aSimTime, const Messages::InitialBase& aMessage) {}

//! This method is called when a slot is open
// virtual
void SurveillancePart::HandleSend(double aSimTime)
{
   if (TrackAvailableToSend(aSimTime))
   {
      TrackMap::iterator trackMapIter = FindTrackToSend();
      if (trackMapIter != mTracks.end())
      {
         WsfTrack*      trackPtr      = nullptr;
         WsfLocalTrack* localTrackPtr = nullptr;

         if (mSendNonSensorReports)
         {
            trackPtr      = GetPlatform()->GetTrackManager().FindTrack(trackMapIter->first);
            localTrackPtr = static_cast<WsfLocalTrack*>(trackPtr);
            assert(trackPtr != nullptr);
         }
         else
         {
            trackPtr = GetPlatform()->GetTrackManager().FindRawTrack(trackMapIter->first);
            if (trackPtr != nullptr)
            {
               if (IsSensorTrackSuppressed(*trackPtr))
               {
                  trackPtr = nullptr; // Don't send
               }
               else
               {
                  assert(trackPtr != nullptr);
                  localTrackPtr = GetPlatform()->GetTrackManager().FindCorrelatedTrack(trackPtr->GetTrackId());
               }
            }
         }

         if (trackPtr != nullptr)
         {
            if (mSuppressDeadTargets)
            {
               WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(trackPtr->GetTargetIndex());
               if (!targetPtr || (targetPtr->GetDamageFactor() == 1))
               {
                  return;
               }
            }

            WsfSpatialDomain domain = trackPtr->GetSpatialDomain();

            if (domain == WSF_SPATIAL_DOMAIN_UNKNOWN)
            {
               domain = (WsfSpatialDomain)mDefaultTrackDomain;
            }

            int link16_TrackQuality = (int)(WsfNonExportableUtil::GetLink16_TrackQuality(aSimTime, *trackPtr) * 15.0);
            std::string trackNumberString;
            bool        isR2_Switch                 = false;
            bool        haveReportingResponsibility = true;
            if (!mIgnoreReportingResponsibility)
            {
               haveReportingResponsibility = HaveReportingResponsibility(aSimTime,
                                                                         trackPtr,
                                                                         localTrackPtr,
                                                                         link16_TrackQuality,
                                                                         trackNumberString,
                                                                         isR2_Switch);
            }

            // mark r2 for this track
            trackPtr->GetAuxData().AssignBool("link16_r2", haveReportingResponsibility);

            if (haveReportingResponsibility)
            {
               int l16_TrackNumber = 0;
               if (isR2_Switch)
               {
                  if (Debug())
                  {
                     std::ostringstream oss;
                     oss << "Taking R2 for target " << trackPtr->GetTargetName();
                     GetPlatform()->Comment(aSimTime, oss.str());
                  }

                  // Reporting responsibility is changing for this track.
                  // Continue using the track number for the track that we are assuming, and register this with the computer.
                  trackPtr->GetAuxData().AssignString("link16_track_number", trackNumberString); // as a string, not int
                  int mySourceTrackNumber = GetComputer()->Config().mTrackNumber;
                  GetComputer()->SetTrackNumber(*localTrackPtr, l16_TrackNumber, mySourceTrackNumber);

                  l16_TrackNumber = WsfL16::Messages::TrackNumberField::GetTrackNumberInteger(trackNumberString);
               }
               else // We are initiating or continuing to report a track.
               {
                  l16_TrackNumber = GetTrackNumber(*localTrackPtr);
                  WsfL16::Messages::TrackNumberField::GetTrackNumberString(l16_TrackNumber, trackNumberString);
                  trackPtr->GetAuxData().AssignString("link16_track_number", trackNumberString); // as string
               }

               trackPtr->GetAuxData().AssignInt("link16_source_track_number", GetComputer()->Config().mTrackNumber);
               trackPtr->GetAuxData().AssignInt("link16_track_quality", link16_TrackQuality);

               // If this is a raw track and we copy to the local track, make sure the source track # and track number are copied.
               if (localTrackPtr != trackPtr)
               {
                  assert(localTrackPtr != nullptr);
                  if (localTrackPtr)
                  {
                     localTrackPtr->GetAuxData().AssignInt("link16_source_track_number",
                                                           GetComputer()->Config().mTrackNumber);
                     localTrackPtr->GetAuxData().AssignString("link16_track_number", trackNumberString); // as string
                  }
               }

               mTrackVariablePtr->GetPointer()->SetExternalManaged(const_cast<WsfTrack*>(trackPtr));
               if (localTrackPtr)
               {
                  switch (domain)
                  {
                  case WSF_SPATIAL_DOMAIN_SPACE:
                     SendSpaceTrack(aSimTime, *localTrackPtr, l16_TrackNumber);
                     break;
                  case WSF_SPATIAL_DOMAIN_AIR:
                     SendAirTrack(aSimTime, *localTrackPtr, l16_TrackNumber);
                     break;
                  case WSF_SPATIAL_DOMAIN_SURFACE:
                     SendSurfaceTrack(aSimTime, *localTrackPtr, l16_TrackNumber);
                     break;
                  case WSF_SPATIAL_DOMAIN_SUBSURFACE:
                     SendSubsurfaceTrack(aSimTime, *localTrackPtr, l16_TrackNumber);
                     break;
                  case WSF_SPATIAL_DOMAIN_LAND:
                     SendLandTrack(aSimTime, *localTrackPtr, l16_TrackNumber);
                     break;
                  default:
                     break; // No send
                  }
               }

               mTrackVariablePtr->GetPointer()->SetExternalManaged(nullptr);
               trackMapIter->second.mLastSendTime = aSimTime;

               if (Debug())
               {
                  std::ostringstream oss;
                  oss << "Sending track report " << trackPtr->GetTargetName() << "; id: " << trackPtr->GetTrackId()
                      << "; tq " << link16_TrackQuality;
                  GetPlatform()->Comment(aSimTime, oss.str());
               }
            }
            else
            {
               if (isR2_Switch)
               {
                  if (Debug())
                  {
                     std::ostringstream oss;
                     oss << "Relinquishing R2 for target " << trackPtr->GetTargetName();
                     GetPlatform()->Comment(aSimTime, oss.str());
                  }
                  GetComputer()->ClearTrackNumber(*localTrackPtr);
               }
            }
         }
      }
   }
}

//! Handle reporting responsibility (R2) rules for this track.
//! @param aSimTime             [input] The current simulation time.
//! @param aTrackPtr            [input] The pointer of the track for which we will test R2.
//!                             This may or may not be the same as aLocalTrackPtr.
//! @param aLocalTrackPtr       [input] The correlated track pointer for the tested R2 track.
//!                             This may or may not be the same track as aTrackPtr.
//! @param aLink16_TrackQuality [input] The calculated track quality of aTrackPtr.
//! @param aTrackNumberString   [output] The link-16 track number of aTrackPtr.
//! @param aIsR2_Switch         [output] true if the reporting responsibility has been changed (new acquired or relinquished.)
//!                             false if no change in reporting responsibility
//! @return True if this platform has reporting responsibility; false otherwise.
bool SurveillancePart::HaveReportingResponsibility(double               aSimTime,
                                                   const WsfTrack*      aTrackPtr,
                                                   const WsfLocalTrack* aLocalTrackPtr,
                                                   int                  aLink16_TrackQuality, // for aTrackPtr
                                                   std::string&         aTrackNumberString,
                                                   bool&                aIsR2_Switch)
{
   bool haveReportingResponsibility = true; // report by default

   if (aTrackPtr->GetAuxDataConst().AttributeExists("link16_r2"))
   {
      haveReportingResponsibility = aTrackPtr->GetAuxDataConst().GetBool("link16_r2");
   }

   int mySourceTrackNumber       = GetComputer()->Config().mTrackNumber;
   int existingSourceTrackNumber = mySourceTrackNumber;
   aIsR2_Switch                  = false;
   WsfSpatialDomain domain       = aTrackPtr->GetSpatialDomain();

   if (domain != WSF_SPATIAL_DOMAIN_SPACE)
   {
      int  mySourceTrackNumber        = GetComputer()->Config().mTrackNumber;
      bool hadReportingResponsibility = haveReportingResponsibility;

      // Search through the raw track list to find whether some other JU should be reporting this track.
      int numTracks = aLocalTrackPtr->GetRawTrackIds().GetCount();
      for (int trackNum = 0; trackNum < numTracks; ++trackNum)
      {
         WsfTrack* rawTrackPtr = GetPlatform()->GetTrackManager().GetRawTrack(aLocalTrackPtr, trackNum);
         if (rawTrackPtr->GetAuxDataConst().AttributeExists("link16_source_track_number"))
         {
            existingSourceTrackNumber = rawTrackPtr->GetAuxDataConst().GetInt("link16_source_track_number");

            // Only need to check if some other JU is reporting on this target.
            if ((mySourceTrackNumber != existingSourceTrackNumber) ||
                (aLocalTrackPtr->GetTrackType() == WsfTrack::cPROCESSED))
            {
               // The track was reported from another source track number
               // Determine whether the other track takes over R2 or whether we keep current R2.

               // c. A JU assumes R2 on a common track if its local TQ at the time
               // of transmission exceeds the received TQ by 2 or more.
               int existingTrackQuality = rawTrackPtr->GetAuxDataConst().GetInt("link16_track_quality");

               if (hadReportingResponsibility)
               {
                  // Do we keep R2?
                  // Only relinquish if another reported track has quality greater than my quality or
                  // the quality difference is zero and his STN > my STN.
                  int deltaTrackQuality = existingTrackQuality - aLink16_TrackQuality;
                  haveReportingResponsibility =
                     (!((deltaTrackQuality > 0) ||
                        ((deltaTrackQuality == 0) && (existingSourceTrackNumber > mySourceTrackNumber))));
                  if (!haveReportingResponsibility)
                  {
                     static const std::string null;
                     rawTrackPtr->GetAuxData().Assign("link16_track_number", null);
                     aIsR2_Switch = true; // relinquishing R2
                  }
               }
               else
               {
                  // Do we take R2?
                  // Only take R2 if my TQ >= his TQ + 2, or
                  // e. A JU assumes R2 if it has not received a remote report on a
                  // local Air or Surface track for 40 seconds or on a local Land track for
                  // 120 seconds.
                  static const double cAIR_SURFACE_TIMEOUT = 40.0;
                  static const double cLAND_TIMEOUT        = 120.0;
                  double              timeSinceLastUpdate  = aSimTime - rawTrackPtr->GetUpdateTime();
                  bool timeOut = ((domain == WSF_SPATIAL_DOMAIN_LAND && (timeSinceLastUpdate >= cLAND_TIMEOUT)) ||
                                  (timeSinceLastUpdate >= cAIR_SURFACE_TIMEOUT));

                  int deltaTrackQuality       = aLink16_TrackQuality - existingTrackQuality;
                  haveReportingResponsibility = (deltaTrackQuality >= 2) || (timeOut);

                  if (haveReportingResponsibility && (!timeOut))
                  {
                     // Note that there ought to be only one competing track out there for raw tracks,
                     // but we need to search through the whole list to check a local track.
                     aTrackNumberString = rawTrackPtr->GetAuxDataConst().GetString("link16_track_number");
                     aIsR2_Switch       = true;
                     // keep going...
                  }
               }

               if (!haveReportingResponsibility)
               {
                  // no need to keep looking through the raw track list.
                  break;
               }
            }
         }
      } // Loop through raw track list to determine R2
   }    // Not in spatial domain space.

   return haveReportingResponsibility;
}

//! Determine whether there is a track available to send.
//! Either there is an updated track or an unchanged track that hasn't been sent since the last send interval.
bool SurveillancePart::TrackAvailableToSend(double aSimTime)
{
   return (!mUpdatedTracks.empty() ||
           (!mUnchangedTracks.empty() && mUnchangedTracks.front()->second.mLastSendTime < (aSimTime - mMaxSendInterval)));
}

SurveillancePart::TrackMap::iterator SurveillancePart::FindTrackToSend()
{
   TrackMap::iterator iter = mTracks.end();
   if (!mUpdatedTracks.empty())
   {
      iter = mUpdatedTracks.front();
      mUpdatedTracks.pop_front();
      mUnchangedTracks.push_back(iter);
      iter->second.mIterator  = --mUnchangedTracks.end();
      iter->second.mIsUpdated = false;
   }
   else if (!mUnchangedTracks.empty())
   {
      iter = mUnchangedTracks.front();
      mUnchangedTracks.pop_front();
      mUnchangedTracks.push_back(iter);
      iter->second.mIterator = --mUnchangedTracks.end();
   }
   return iter;
}

void SurveillancePart::TrackUpdated(const WsfTrack& aTrack)
{
   TrackMap::iterator trackMapIter = mTracks.find(aTrack.GetTrackId());
   if ((trackMapIter != mTracks.end()) && (!trackMapIter->second.mIsUpdated))
   {
      trackMapIter->second.mIsUpdated = true;
      mUnchangedTracks.erase(trackMapIter->second.mIterator);
      mUpdatedTracks.push_back(trackMapIter);
      trackMapIter->second.mIterator = --mUpdatedTracks.end();
   }
}

void SurveillancePart::TrackCreated(const WsfTrack& aTrack)
{
   if (IsTrackReportable(aTrack))
   {
      mTracks.emplace(aTrack.GetTrackId(), TrackInfo());
      TrackMap::iterator iter = mTracks.find(aTrack.GetTrackId());
      TrackInfo&         info = iter->second;
      mUpdatedTracks.push_front(iter);

      info.mLastSendTime = -1.0;
      info.mIsUpdated    = true;
      info.mIterator     = mUpdatedTracks.begin();
   }
}

void SurveillancePart::TrackDropped(const WsfTrack& aTrack)
{
   TrackMap::iterator trackMapIter = mTracks.find(aTrack.GetTrackId());
   if (trackMapIter != mTracks.end())
   {
      if (trackMapIter->second.mIsUpdated)
      {
         mUpdatedTracks.erase(trackMapIter->second.mIterator);
      }
      else
      {
         mUnchangedTracks.erase(trackMapIter->second.mIterator);
      }
      mTracks.erase(trackMapIter);
   }
}

void SurveillancePart::RawTrackReceived(const WsfTrack& aTrack)
{
   TrackMap::iterator trackMapIter = mTracks.find(aTrack.GetTrackId());
   if (trackMapIter != mTracks.end())
   {
      TrackUpdated(aTrack);
   }
   else
   {
      TrackCreated(aTrack);
   }
}

void SurveillancePart::RawTrackDropped(const WsfTrack& aTrack)
{
   TrackDropped(aTrack);
}

namespace
{
void GetLocationWCS(const WsfTrack* aTrackPtr, double aLocationWCS[3])
{
   UtVec3d::Set(aLocationWCS, 0.0); // Assume location is unknown.
   if (aTrackPtr != nullptr)
   {
      if (aTrackPtr->LocationValid())
      {
         // Location is directly known
         aTrackPtr->GetLocationWCS(aLocationWCS);
      }
      else
      {
         // Location is not directly known. Attempt to derive it from other sources.
         aTrackPtr->GetExtrapolatedLocationWCS(aTrackPtr->GetUpdateTime(), aLocationWCS);
      }
   }
}
//! Return the unextrapolated LLA location from a track.
void GetLocationLLA(const WsfTrack* aTrackPtr, double& aLat, double& aLon, double& aAlt)
{
   // Assume location is unknown.

   aLat = 0.0;
   aLon = 0.0;
   aAlt = 0.0;
   if (aTrackPtr != nullptr)
   {
      if (aTrackPtr->LocationValid())
      {
         // Location is directly known.
         aTrackPtr->GetLocationLLA(aLat, aLon, aAlt);
      }
      else
      {
         // Location is not directly known. Attempt to derive it from other sources.
         double locWCS[3];
         if (aTrackPtr->GetExtrapolatedLocationWCS(aTrackPtr->GetUpdateTime(), locWCS))
         {
            WsfPlatform::ConvertWCSToLLA(locWCS, aLat, aLon, aAlt);
         }
      }
   }
}
// Each track message is slightly different.
// This template function fills out the common data.
template<typename INITIAL, typename EXTENSION, typename EXTENSION_TDEI>
void FillTrackMsg(Interface*      aInterfacePtr,
                  const WsfTrack& aTrack,
                  INITIAL*        msg,
                  EXTENSION*      ext,
                  EXTENSION_TDEI* extTrackDisEntityId,
                  int             aTrackNumber)
{
   msg->TrackNumber.SetRawData(aTrackNumber);
   std::string myTN = msg->TrackNumber.GetTrackNumberString();

   // Add the track number to the platform's aux data just in case it's needed on that level also for scripting
   // purposes. mjm this is a kludge; research removing (TODO).
   WsfPlatform* pltfrm = nullptr;
   if (aInterfacePtr != nullptr)
   {
      pltfrm = aInterfacePtr->GetSimulation().GetPlatformByIndex(aTrack.GetTargetIndex());
   }

   if (pltfrm != nullptr)
   {
      if (!pltfrm->GetAuxDataConst().AttributeExists("link16_track_number"))
      {
         pltfrm->GetAuxData().Assign("link16_track_number", myTN);
      }
   }

   if (aTrack.Is3D())
   {
      double lat, lon, alt;
      GetLocationLLA(&aTrack, lat, lon, alt);

      ext->Latitude  = lat;
      ext->Longitude = lon;

      if (aTrack.VelocityValid())
      {
         double trackVel[3], velNED[3];
         aTrack.GetVelocityWCS(trackVel);
         UtEntity tmp;
         tmp.SetLocationLLA(lat, lon, alt);
         tmp.SetVelocityWCS(trackVel);
         tmp.GetVelocityNED(velNED);
         double course = atan2(velNED[1], velNED[0]);
         ext->Course   = course;
         ext->Speed    = aTrack.GetSpeed();
      }
   }
   else
   {
      // set up no statement values
      ext->Latitude.SetRawData(1048576);
      ext->Longitude.SetRawData(2097152);
   }

   msg->InsertExtensionWord(std::unique_ptr<EXTENSION>(ext));

   // Fill in IFF Status
   msg->Identity.SetRawData(Interface::GetJ3xIdentityFromIFF(aTrack.GetIFF_Status()));

   std::unique_ptr<EXTENSION_TDEI> extTDEI(extTrackDisEntityId);
   if (aInterfacePtr != nullptr)
   {
      WsfDisInterface* disInterfacePtr = aInterfacePtr->GetDisInterface();
      if ((aInterfacePtr->IncludeJ3xTrackDisEntityId()) && (disInterfacePtr))
      {
         DisEntityId trackTruthDisEntityId;
         disInterfacePtr->GetEntityIdFromIndex(aTrack.GetTargetIndex(), trackTruthDisEntityId);
         extTDEI->DisSite        = trackTruthDisEntityId.GetSite();
         extTDEI->DisApplication = trackTruthDisEntityId.GetApplication();
         extTDEI->DisEntityId    = trackTruthDisEntityId.GetEntity();
         msg->InsertExtensionWord(std::move(extTDEI));
      }
   }
}
} // namespace

void SurveillancePart::SendAirTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber)
{
   using namespace Messages::J3_2;
   Initial*    msg                 = new Initial;
   Extension0* ext                 = new Extension0;
   Extension1* extTrackDisEntityId = new Extension1;

   FillTrackMsg(mInterfacePtr, aTrack, msg, ext, extTrackDisEntityId, aTrackNumber);

   double lat, lon, alt;
   GetLocationLLA(&aTrack, lat, lon, alt);
   msg->Altitude = alt;

   int link16TrackQuality = (int)(aTrack.GetTrackQuality() * 15);
   if (aTrack.GetAuxDataConst().AttributeExists("link16_track_quality"))
   {
      aTrack.GetAuxDataConst().Get("link16_track_quality", link16TrackQuality);
   }
   msg->TrackQuality.SetRawData(static_cast<unsigned>(link16TrackQuality));

   // If we have mapped this target platform type, send that type in a continuation word.
   if (aTrack.TypeIdValid() && (mInterfacePtr != nullptr))
   {
      unsigned airSpecificType = mInterfacePtr->GetAirSpecificType(aTrack.GetTypeId());
      if (airSpecificType != 0)
      {
         auto cont               = ut::make_unique<Continuation1>();
         cont->IsAirSpecificType = true; // Do send the type data.
         cont->AirSpecificType   = airSpecificType;
         msg->InsertContinuationWord(std::move(cont));
      }
   }
   SendJMessage(aSimTime, msg);
}

void SurveillancePart::SendSurfaceTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber)
{
   using namespace Messages::J3_3;
   Initial*    msg                 = new Initial;
   Extension0* ext                 = new Extension0;
   Extension1* extTrackDisEntityId = new Extension1;

   FillTrackMsg(mInterfacePtr, aTrack, msg, ext, extTrackDisEntityId, aTrackNumber);

   int link16TrackQuality = (int)(aTrack.GetTrackQuality() * 15);
   if (aTrack.GetAuxDataConst().AttributeExists("link16_track_quality"))
   {
      aTrack.GetAuxDataConst().Get("link16_track_quality", link16TrackQuality);
   }
   msg->TrackQuality.SetRawData(static_cast<unsigned>(link16TrackQuality));

   SendJMessage(aSimTime, msg);
}

void SurveillancePart::SendSubsurfaceTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber)
{
   using namespace Messages::J3_4;
   Initial*    msg                 = new Initial;
   Extension0* ext                 = new Extension0;
   Extension1* extTrackDisEntityId = new Extension1;

   FillTrackMsg(mInterfacePtr, aTrack, msg, ext, extTrackDisEntityId, aTrackNumber);

   SendJMessage(aSimTime, msg);
}

void SurveillancePart::SendLandTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber)
{
   using namespace Messages::J3_5;
   Initial*    msg                 = new Initial;
   Extension0* ext                 = new Extension0;
   Extension1* extTrackDisEntityId = new Extension1;

   FillTrackMsg(mInterfacePtr, aTrack, msg, ext, extTrackDisEntityId, aTrackNumber);

   double lat, lon, alt;
   GetLocationLLA(&aTrack, lat, lon, alt);
   msg->PointOrTrack.SetTrack();
   msg->Altitude = alt;

   int link16TrackQuality = (int)(aTrack.GetTrackQuality() * 15);
   if (aTrack.GetAuxDataConst().AttributeExists("link16_track_quality"))
   {
      aTrack.GetAuxDataConst().Get("link16_track_quality", link16TrackQuality);
   }
   msg->TrackQuality.SetRawData(static_cast<unsigned>(link16TrackQuality));

   // If we have mapped this target platform type, send that type in a continuation word.
   if (aTrack.TypeIdValid() && (mInterfacePtr != nullptr))
   {
      unsigned landSpecificType = mInterfacePtr->GetLandSpecificType(aTrack.GetTypeId());
      if (landSpecificType != 0)
      {
         auto cont                = ut::make_unique<Continuation1>();
         cont->IsLandSpecificType = true;
         cont->LandSpecificType   = landSpecificType;
         msg->InsertContinuationWord(std::move(cont));
      }
   }

   SendJMessage(aSimTime, msg);
}

void SurveillancePart::SendSpaceTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber)
{
   // Will fill this message not using the FillTrackMsg, since
   // most fields in the j3.6 message to not match those defined
   // for other messages.
   using namespace Messages::J3_6;
   Initial* msg  = new Initial;
   auto     ext  = ut::make_unique<Extension0>();
   auto     ext1 = ut::make_unique<Extension1>();
   if (msg && ext && ext1)
   {
      switch (aTrack.GetIFF_Status())
      {
      case WsfTrack::cIFF_FOE:
         msg->Identity = Messages::IdentityField::cHOSTILE;
         break;
      case WsfTrack::cIFF_FRIEND:
         msg->Identity = Messages::IdentityField::cFRIEND;
         break;
      case WsfTrack::cIFF_UNKNOWN:
         msg->Identity = Messages::IdentityField::cUNKNOWN;
         break;
      default:
         msg->Identity = Messages::IdentityField::cPENDING;
      }

      msg->SpacePlatform = 62;

      if (aTrack.Is3D())
      {
         double WCS[3];
         GetLocationWCS(&aTrack, WCS);

         msg->TrackNumber.SetRawData(aTrackNumber);

         std::string myTN = msg->TrackNumber.GetTrackNumberString();

         ext->WcsX              = WCS[0];
         ext->WcsY              = WCS[1];
         ext1->WcsZ             = WCS[2];
         ext->SpaceAmplify      = 7;
         ext->AmplifyConfidence = 3;
         ext1->IsTrackLost      = false;
         // Add the track number to the platform's aux data just in case it's
         // needed on that level also for scripting purposes.
         WsfPlatform* pltfrmPtr = GetSimulation()->GetPlatformByIndex(aTrack.GetTargetIndex());
         if (pltfrmPtr != nullptr)
         {
            if (!pltfrmPtr->GetAuxDataConst().AttributeExists("link16_track_number"))
            {
               pltfrmPtr->GetAuxData().Assign("link16_track_number", myTN);
            }
            ext1->IsTrackLost = true;
         }
         if (aTrack.VelocityValid())
         {
            double trackVel[3];
            aTrack.GetVelocityWCS(trackVel);
            ext->WcsVelX  = trackVel[0];
            ext1->WcsVelY = trackVel[1];
            ext1->WcsVelZ = trackVel[2];
         }
      }

      if ((mInterfacePtr != nullptr) && (mInterfacePtr->IncludeJ3xTrackDisEntityId())) // link 16 j3x truth set to true
      {
         auto        trackTruthIdExtension = ut::make_unique<Extension2>();
         DisEntityId trackTruthDisEntityId =
            mInterfacePtr->GetDisInterface()->GetDisPlatform(aTrack.GetTargetIndex())->GetEntityId();
         trackTruthIdExtension->DisSite        = trackTruthDisEntityId.GetSite();
         trackTruthIdExtension->DisApplication = trackTruthDisEntityId.GetApplication();
         trackTruthIdExtension->DisEntityId    = trackTruthDisEntityId.GetEntity();
         msg->InsertExtensionWord(std::move(trackTruthIdExtension));
      }

      msg->InsertExtensionWord(std::move(ext));
      msg->InsertExtensionWord(std::move(ext1));
      // msg->Identity = Messages::IdentityField::cPENDING;
      SendJMessage(aSimTime, msg);
   }
}

//! WSF track IDs are mapped to WsfL16 track IDs in the range
//! that has been assigned.
int SurveillancePart::GetTrackNumber(WsfLocalTrack& aLocalTrack)
{
   int trackNumber;
   // Check if track number has already been assigned for this track
   // If not, assign from our own track block
   if (!GetComputer()->GetTrackNumber(aLocalTrack, trackNumber))
   {
      trackNumber = mNextTrackNumber++;
      // loop back around to re-use some track numbers.
      // this could cause track numbers to be used in more than one
      // active track.  This can be fixed later, but will take more overhead.
      if (mNextTrackNumber > mTrackNumberRange.second)
      {
         mNextTrackNumber = mTrackNumberRange.first;
      }

      GetComputer()->SetTrackNumber(aLocalTrack, trackNumber, GetComputer()->GetTrackNumber());
   }

   return trackNumber;
}

bool SurveillancePart::IsTrackReportable(const WsfTrack& aTrack)
{
   bool reportable = true;
   if (aTrack.GetSideId() == GetPlatform()->GetSideId() ||
       (mSuppressDomains.find(aTrack.GetSpatialDomain()) != mSuppressDomains.end()))
   {
      reportable = false;
   }
   return reportable;
}

bool SurveillancePart::OwningPlatformWantsTrackRefNumber(const WsfTrack& aTrack)
{
   bool         doesExist = false;
   WsfPlatform* pltfrmPtr = GetSimulation()->GetPlatformByName(aTrack.GetTrackId().GetOwningPlatformId());
   if (pltfrmPtr != nullptr)
   {
      if (pltfrmPtr->GetAuxDataConst().AttributeExists("use_track_ref_number"))
      {
         doesExist = true;
      }
   }
   return doesExist;
}

bool SurveillancePart::IsSensorTrackSuppressed(const WsfTrack& aTrack)
{
   return (mSuppressSensors.find(aTrack.GetSensorNameId()) != mSuppressSensors.end());
}

WsfSimulation* SurveillancePart::GetSimulation() const
{
   return (mInterfacePtr != nullptr) ? &mInterfacePtr->GetSimulation() : nullptr;
}

bool WsfL16::SurveillancePart::SuppressSensor(WsfStringId aSensorName, bool aSuppress)
{
   if (aSuppress)
   {
      return !mSuppressSensors.insert(aSensorName).second;
   }
   else
   {
      return (mSuppressSensors.erase(aSensorName) > 0);
   }
}

} // namespace WsfL16
