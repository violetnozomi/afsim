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

#ifndef WSFSIMDISINTERFACE_HPP
#define WSFSIMDISINTERFACE_HPP

#include "wsf_simdis_export.h"

#include <set>

#include "UtCallbackHolder.hpp"

#include "ext/WsfExtEntityDeadReckon.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"

class WsfPlatform;
class WsfSensor;
class WsfTrack;
class WsfTrackId;
class WsfWeaponEngagement;

namespace wsf
{
namespace simdis
{

class SensorUpdateEvent;

//! Scenario Extension.
//! Reads scenario input related to output to SIMDIS asi files
class WSF_SIMDIS_EXPORT ScenarioExtension : public WsfScenarioExtension
{
   public:
      ScenarioExtension();

      static ScenarioExtension* Find(const WsfScenario& aScenario);

      void SimulationCreated(WsfSimulation& aSimulation) override;
      bool ProcessInput(UtInput& aInput) override;

      const std::string& GetOutputFilename() const { return mOutputFilename; }
      const std::string& GetHitIcon() const { return mHitIcon; }
      const std::string& GetKillIcon() const { return mKillIcon; }
      const double GetEventDuration() const { return mEventDuration; }
      const bool EnableBeams() const { return mEnableBeams; }
      const std::string& GetBeamColor(const std::string& aSensorTypeName) const;
      const double GetLatitude() const { return mLatitude; }
      const double GetLongitude() const { return mLongitude; }
      const double GetAltitude() const { return mAltitude; }
      const char GetFHN(const std::string& aSide) const;
   private:
      std::string                        mOutputFilename;
      std::string                        mHitIcon;
      std::string                        mKillIcon;
      double                             mEventDuration;
      bool                               mEnableBeams;
      std::string                        mDefaultBeamColor;
      std::map<std::string, std::string> mBeamColorMap;
      double                             mLatitude;
      double                             mLongitude;
      double                             mAltitude;
      std::map<std::string, char>        mFHN_Map;
};

//! Processes simulation events and writes to a simdis ASI file
class WSF_SIMDIS_EXPORT Interface : public WsfSimulationExtension
{
   public:
      Interface(ScenarioExtension& aExtension);
      ~Interface() override = default;

      static Interface* Find(const WsfSimulation& aSimulation);

      void AddedToSimulation() override;
   private:
      bool OpenOutput();

      void WriteHeader();

      void PlatformInitialized(double       aSimTime,
                               WsfPlatform* aPlatformPtr);
      void PlatformKilled(double            aSimTime,
                          WsfPlatform*      aPlatformPtr);
      void WeaponHit(double                     aSimTime,
                     const WsfWeaponEngagement* aEngagementPtr,
                     WsfPlatform*               aTargetPtr);

      void SensorTrackDropped(double          aSimTime,
                              WsfSensor*      aSensorPtr,
                              const WsfTrack* aTrackPtr);

      void SensorTrackInitiated(double          aSimTime,
                                WsfSensor*      aSensorPtr,
                                const WsfTrack* aTrackPtr);

      void DeadReckonChange(double                              aSimTime,
                            WsfExtEntityDeadReckon::PlatformDR& aDR);
      void UpdatePlatform(double       aSimTime,
                          WsfPlatform* aPlatformPtr);

      unsigned int LookupPlatformId(const std::string& aPlatformName);
      unsigned int LookupTrackId(const WsfTrackId& aTrackId);

      // A reference back to the scenario extension for accessing input data.
      const ScenarioExtension&            mExtension;
      std::string                         mOutputFilename;
      std::ofstream*                      mFileStreamPtr;
      UtCallbackHolder                    mEntityStateCallbacks;
      UtCallbackHolder                    mBaseCallbacks;
      std::map<std::string, unsigned int> mPlatformNameLookup;
      std::set<unsigned int>              mInitSet;
      unsigned int                        mUID_Assigner;
      unsigned int                        mHitID_Assigner;
      unsigned int                        mKillID_Assigner;
      std::map<WsfTrackId, unsigned int>  mTrackIdLookup;
};

}
}


#endif
