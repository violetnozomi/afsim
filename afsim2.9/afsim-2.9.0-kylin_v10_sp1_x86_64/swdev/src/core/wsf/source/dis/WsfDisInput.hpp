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

#ifndef WSFDISINPUT_HPP
#define WSFDISINPUT_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

class DisBeam;
class DisSystem;
#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
class UtInput;
#include "WsfComponentList.hpp"
#include "WsfDisArticulatedPart.hpp"
#include "WsfDisDevice.hpp"
class WsfDisEmission;
class WsfEM_Xmtr;
#include "ext/WsfExtEntityType.hpp"
class WsfExtInterface;
#include "WsfExtension.hpp"
class WsfScenario;
#include "WsfStringId.hpp"

class WsfDisInput;

class WSF_EXPORT WsfDisComponent : public WsfComponentT<WsfDisInput>
{
public:
   virtual void PrepareComponent(double aSimTime) {}
   virtual void AddedToDisInterface(WsfDisInterface* aDisInterfacePtr) {}
   virtual bool TransferPlatformToInternalControl(double         aSimTime,
                                                  WsfPlatform*   aPlatformPtr,
                                                  DisEntityId&   aEntityId,
                                                  DisEntityType& aEntityType)
   {
      return false;
   }
   virtual bool TransferPlatformToExternalControl(double aSimTime, WsfPlatform* aPlatformPtr) { return false; }
   virtual void PlatformCreated(WsfDisPlatform* aPlatformPtr) {}

   virtual bool UpdateEmissionSystem(WsfPlatform* aPlatformPtr, WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr)
   {
      return false;
   }

   virtual bool AddEmissionSystem(WsfPlatform* aPlatformPtr, WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr)
   {
      return false;
   }

   virtual bool UpdateXmtrFromBeam(WsfDisEmission* aEmissionPtr, DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr, bool& aBeamOn)
   {
      return false;
   }
};

//! Configuration data for the DIS interface.  Seeded by the input file (ProcessInput).
//! Copied to the DIS interface upon creation
class WSF_EXPORT WsfDisInput
{
public:
   using Component     = WsfDisComponent;
   using ComponentList = WsfComponentListT<WsfDisComponent>;

   struct Articulation
   {
      int         mPartNumber;
      WsfStringId mVariableId;
      WsfStringId mScriptId;
   };

   using ArticulationVec = std::vector<Articulation>;
   using ArticulationMap = std::map<WsfStringId, ArticulationVec>;
   struct DeviceConfiguration
   {
      DeviceConfiguration()
         : mDeviceType(WsfDisDevice::cDEVICE_NONE)
         , mRecvPort(0)
         , mSendPort(0)
      {
      }

      bool IsComplete();

      bool IsInputDevice() const
      {
         return ((mDeviceType == WsfDisDevice::cDEVICE_PLAYBACK) || (mDeviceType >= WsfDisDevice::cDEVICE_MULTICAST));
      }

      bool IsOutputDevice() const
      {
         return ((mDeviceType == WsfDisDevice::cDEVICE_RECORD) || (mDeviceType >= WsfDisDevice::cDEVICE_MULTICAST));
      }

      bool IsExternalDevice() const { return (mDeviceType >= WsfDisDevice::cDEVICE_MULTICAST); }

      bool IsDefined() const { return (mDeviceType != WsfDisDevice::cDEVICE_NONE); }

      WsfDisDevice::DeviceType            mDeviceType;
      std::string                         mAddress;
      std::string                         mInterfaceAddress;
      int                                 mRecvPort;
      int                                 mSendPort;
      std::vector<WsfDisFilterAllowance*> mFilterAllowances;
   };
   WsfDisInput();

   bool        ProcessInput(WsfScenario& aScenario, UtInput& aInput);
   bool        ProcessBlockInput(WsfScenario& aScenario, UtInput& aInput);
   static bool ValidSignatureTypeName(std::string& aType);

   //! Return whether device configurations for evaluation
   std::vector<DeviceConfiguration>& GetDeviceConfigurations() { return mDeviceConfigurations; }
   bool ProcessDeviceInput(WsfScenario& aScenario, UtInput& aInput, DeviceConfiguration& aConfiguration);
   bool ProcessFilterAllowance(UtInput& aInput, WsfDisFilterAllowance& aAllowance);

   //! Returns the list of ignored sites and applications.
   std::set<std::pair<unsigned short, unsigned short>>& GetIgnoredSiteAndApps() { return mIgnoredSiteAndApps; }
   bool GetIgnoredSiteAndApplicationPair(unsigned int aIndex, std::pair<unsigned short, unsigned short>& aSiteAndAppPair);
   bool IgnoreSiteAndApplication(unsigned short aSite, unsigned short aApp);

   //! Return whether the user has specified to limit information displayed on stdout
   //! (such as introduction of external platforms).
   bool IsTerseMode() const { return mTerseMode; }

   //! Returns the value of the bool being used to see the acceleration data
   //! required in the entity state PDU.
   bool UseSimpleAccelerations() const { return mUseSimpleAccelerations; }

   //! Returns 'true' if the DIS interface is only sending standard DIS data
   bool SuppressNonStandardData() const { return mSuppressNonStandardData; }

   bool MultiThreaded() const { return mMultiThreaded; }

   bool DebugEnabled() const { return mDebugEnabled; }

   void SuppressNonStandardData(bool aDoSupress);
   //! Is the sensor track-jam list being used to indicate a tracking request.
   bool   UseTrackJamForTrackingRequests() { return mUseTrackJamForTrackingRequests; }
   bool   SuppressCME_PassiveSensor() const { return mSuppressCME_PassiveSensor; }
   bool   SuppressCME_EntityData() const { return mSuppressCME_EntityData; }
   bool   SuppressCME_DetectBeam() const { return mSuppressCME_DetectBeam; }
   double GetSensorUpdateInterval() const { return mSensorUpdateInterval; }
   int    GetTimeToLive() const { return mMulticastTimeToLive; }
   const WsfDisArticulatedPart::ArticulationList& GetArticulationList() const { return mArticulationList; }

   void                 AddComponent(Component* aComponentPtr) { mComponents.AddComponent(aComponentPtr); }
   const ComponentList& GetComponents() const { return mComponents; }

protected:
   bool ProcessArticulatedPart(UtInput& aInput);

   using RangeFilteredPlatforms = std::vector<std::tuple<WsfStringId, size_t, double>>; // Name, Id, range
   using NameTypeToStateIdPair  = std::pair<WsfStringId, int>;
   using StringToStateIdPair    = std::pair<std::string, WsfStringId>;
   using StringToStateIdVec     = std::vector<StringToStateIdPair>;
   using IdIdToStringIDMap      = std::map<NameTypeToStateIdPair, StringToStateIdVec>;
   using IdIdToStringIDMapIter  = std::map<NameTypeToStateIdPair, StringToStateIdVec>::iterator;

public:
   union
   {
      struct
      {
         bool mMultiThreaded : 1;
         bool mDebugEnabled : 1;
         bool mShadowAll : 1;
         bool mLogCreatedEntities : 1;
         bool mSuppressCommData : 1;
         bool mSuppressEmissionsData : 1;
         bool mSendPeriodicsWhilePaused : 1;
         bool mDebugAppearanceUpdates : 1;
         bool mUsePduTime : 1;
         bool mAutostart : 1;
         //! Force acceleration data in acceleration fields and use
         //! simple acceleration equation
         bool mUseSimpleAccelerations : 1;
         bool mTerseMode : 1;
         //! Use the calculated simple orientation rates
         bool mUseSimpleOrientRates : 1;
         //! Join an exercise for non-realtime simulation
         bool mJoinExercise : 1;
         //! If 'true', the track-jam list in tracking sensors will reflect entities for which tracking
         //! requests are active.
         bool mUseTrackJamForTrackingRequests : 1;
         bool mSimpleMissReporting : 1;
         bool mDebugThread : 1;
         //! Force the world (Euler) angular velocities to body axis angular velocities
         bool mUseWorldToBodyAngularVelocities : 1;
         //! Zero out the world (Euler) angular velocities to body axis angular velocities
         bool mZeroBodyAngularVelocities : 1;
         //! Ignore the Stop/Freeze PDU; can be used by simulations running asynchronously
         bool mIgnoreStopFreezePDU : 1;

         //! Deprecated commands

         //! 'true' if the non-standard pseudo-beam used for CME detections should NOT be sent.
         bool mSuppressCME_DetectBeam : 1;
         //! 'true' if the non-standard 'other' PDU used for CME entity data should NOT be sent.
         bool mSuppressCME_EntityData : 1;
         //! 'true' if the non-standard practice of sending WsfDraw PDUs across the network should NOT take place
         bool mSuppressCME_DrawData : 1;
         //! 'true' if the non-standard practice of sending emissions PDUs for passive sensors should NOT take place.
         bool mSuppressCME_PassiveSensor : 1;
         //! 'true' if the WsfDraw DIS 'Other' PDUs containing draw commands will be sent using the deprecated format.
         bool mUseDeprecatedCME_DrawData : 1;
         //! 'true' if WSF will only output standard DIS data.
         bool mSuppressNonStandardData : 1;
      };
      unsigned int mFlags;
   };
   unsigned mMaxBadPDU_Count;
   int      mMulticastTimeToLive;
   double   mThreadSleepTime;

protected:
   std::set<WsfStringId>                               mShadowTypeId;
   std::set<WsfStringId>                               mShadowNameId;
   std::vector<DeviceConfiguration>                    mDeviceConfigurations;
   RangeFilteredPlatforms                              mRangeFilteredPlatforms;
   std::set<WsfExtEntityType>                          mIgnoredTypes;
   std::set<DisEnum8>                                  mIgnoredPduTypes;
   std::set<std::pair<unsigned short, unsigned short>> mIgnoredSiteAndApps;
   std::set<std::pair<unsigned short, unsigned short>> mIgnoredKindAndDomain;
   double                                              mInputHeartbeatMultiplier;
   double                                              mSensorUpdateInterval;
   //! Afterburner map for changing of signatures via DIS Entity Appearance
   //! afterburner field (bit 16).
   IdIdToStringIDMap mAfterBurnerByName;
   IdIdToStringIDMap mAfterBurnerByType;

   //! Map for changing appearance (signatures, etc.) via articulated parts.
   ArticulationMap mArticulationByName;
   ArticulationMap mArticulationByType;

   IdIdToStringIDMap mConfigIdByName;
   IdIdToStringIDMap mConfigIdByType;

   WsfDisArticulatedPart::ArticulationList mArticulationList;

   bool          sDebugEnabled;
   std::set<int> sExcludedPlatformTypes;

   ComponentList mComponents;
};

#endif
