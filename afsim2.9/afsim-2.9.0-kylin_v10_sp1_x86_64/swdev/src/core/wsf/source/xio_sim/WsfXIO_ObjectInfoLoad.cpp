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

#include "xio_sim/WsfXIO_ObjectInfoLoad.hpp"

#include "PakSerializeImpl.hpp"
#include "WsfComm.hpp"
#include "WsfCommandChain.hpp"
#include "WsfCorrelationStrategy.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfFuel.hpp"
#include "WsfFusionStrategy.hpp"
#include "WsfMover.hpp"
#include "WsfProcessor.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWaypointMover.hpp"
#include "WsfZone.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_ObjectInfo.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_ObjectInfoLoadHelpers.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

// Loaders for objects that don't need to be known externally.
namespace
{
// =================================================================================================
void Load_Antenna(WsfXIO_AntennaInfo& aInfo, const WsfEM_Antenna& aAntenna)
{
   aInfo.mPitch                  = aAntenna.GetPitch();
   aInfo.mEBS_AzCosSteeringLimit = aAntenna.GetEBS_AzCosSteeringLimit();
   aInfo.mEBS_ElCosSteeringLimit = aAntenna.GetEBS_ElCosSteeringLimit();
   aInfo.mEBS_AzLossExponent     = aAntenna.GetEBS_AzLossExponent();
   aInfo.mEBS_ElLossExponent     = aAntenna.GetEBS_ElLossExponent();
   aInfo.mEBS_Mode               = aAntenna.GetEBS_Mode();
   aInfo.mScanMode               = aAntenna.GetScanMode();
   aAntenna.GetAzimuthScanLimits(aInfo.mMinAzScan, aInfo.mMaxAzScan);
   aAntenna.GetElevationScanLimits(aInfo.mMinElScan, aInfo.mMaxElScan);
   aAntenna.GetAzimuthFieldOfView(aInfo.mMinAzFOV, aInfo.mMaxAzFOV);
   aAntenna.GetElevationFieldOfView(aInfo.mMinElFOV, aInfo.mMaxElFOV);
   aAntenna.GetRangeLimits(aInfo.mMinRange, aInfo.mMaxRange);
   aAntenna.GetAltitudeLimits(aInfo.mMinAlt, aInfo.mMaxAlt);
}

// =================================================================================================
void Load_EM_XmtrRcvr(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_EM_XmtrRcvrInfo& aInfo, const WsfEM_XmtrRcvr& aXmtrRcvr)
{
   if (aXmtrRcvr.GetAntenna() != nullptr)
   {
      if (aInfo.mAntennaPtr == nullptr)
      {
         aInfo.mAntennaPtr = new WsfXIO_AntennaInfo;
      }
      ::Load_Antenna(*(aInfo.mAntennaPtr), *(aXmtrRcvr.GetAntenna()));
   }
   else if (aInfo.mAntennaPtr != nullptr)
   {
      delete aInfo.mAntennaPtr;
      aInfo.mAntennaPtr = nullptr;
   }
   aInfo.mBeamTilt = aXmtrRcvr.GetBeamTilt();
}

// =================================================================================================
void Load_TrackManager(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_TrackManagerInfo& aInfo, const WsfTrackManager& aTrackManager)
{
   aInfo.SetCorrelationStrategy(aTrackManager.GetCorrelationStrategy().GetType());
   aInfo.SetFusionStrategy(aTrackManager.GetFusionStrategy().GetType());
}

// =================================================================================================
void Load_SensorMode(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_SensorModeInfo& aInfo, const WsfSensorMode& aMode)
{
   aInfo.mModeName  = aMode.GetNameId();
   aInfo.mFrameTime = aMode.GetFrameTime();
   aInfo.mModeType  = WsfXIO_SensorModeInfo::cSEARCH;
   if (aMode.GetMaximumRequestCount() > 0)
   {
      if (aMode.CanSearchWhileTrack())
      {
         aInfo.mModeType |= WsfXIO_SensorModeInfo::cTRACK;
      }
      else
      {
         aInfo.mModeType = WsfXIO_SensorModeInfo::cTRACK;
      }
   }
}
} // namespace

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_EM_Xmtr(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_EM_XmtrInfo& aInfo, const WsfEM_Xmtr& aXmtr)
{
   ::Load_EM_XmtrRcvr(aXIO_Simulation, aInfo, aXmtr); // Load base class
   aInfo.mFrequency                = aXmtr.GetFrequency();
   aInfo.mBandwidth                = aXmtr.GetBandwidth();
   aInfo.mPower                    = aXmtr.GetPower();
   aInfo.mPulseCompressionRatio    = aXmtr.GetPulseCompressionRatio();
   aInfo.mPulseRepetitionFrequency = aXmtr.GetPulseRepetitionFrequency();
   aInfo.mPulseRepetitionInterval  = aXmtr.GetPulseRepetitionInterval();
   aInfo.mPulseWidth               = aXmtr.GetPulseWidth();
   aInfo.mInternalLoss             = aXmtr.GetInternalLoss();
   aInfo.mPeakAntennaGain          = aXmtr.GetPeakAntennaGain();
   aInfo.mUsePeakPower             = aXmtr.IsUsingPeakPower();

   if (aXmtr.GetLinkedReceiver() != nullptr)
   {
      if (aInfo.mLinkedRcvrPtr == nullptr)
      {
         aInfo.mLinkedRcvrPtr = new WsfXIO_EM_RcvrInfo;
      }
      WsfXIO_ObjectInfoLoad::Load_EM_Rcvr(aXIO_Simulation, *(aInfo.mLinkedRcvrPtr), *(aXmtr.GetLinkedReceiver()));
   }
   else if (aInfo.mLinkedRcvrPtr != nullptr)
   {
      delete aInfo.mLinkedRcvrPtr;
      aInfo.mLinkedRcvrPtr = nullptr;
   }

   // Call components to let them fill extra data.
   const WsfXIO_ComponentList& components(aXIO_Simulation);
   for (WsfXIO_ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
   {
      (*i)->EM_XmtrInfo_Load(aInfo, aXmtr);
   }
}

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_EM_Rcvr(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_EM_RcvrInfo& aInfo, const WsfEM_Rcvr& aRcvr)
{
   ::Load_EM_XmtrRcvr(aXIO_Simulation, aInfo, aRcvr); // Load base class
   aInfo.mFrequency          = aRcvr.GetFrequency();
   aInfo.mBandwidth          = aRcvr.GetBandwidth();
   aInfo.mPeakAntennaGain    = aRcvr.GetPeakAntennaGain();
   aInfo.mDetectionThreshold = aRcvr.GetDetectionThreshold();
   aInfo.mInternalLoss       = aRcvr.GetInternalLoss();
   aInfo.mNoiseFigure        = aRcvr.GetNoiseFigure();
   aInfo.mNoisePower         = aRcvr.GetNoisePower();

   // Call components to let them fill extra data.
   const WsfXIO_ComponentList& components(aXIO_Simulation);
   for (WsfXIO_ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
   {
      (*i)->EM_RcvrInfo_Load(aInfo, aRcvr);
   }
}

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_PlatformPart(WsfXIO_Simulation&       aXIO_Simulation,
                                              WsfXIO_PlatformPartInfo& aInfo,
                                              const WsfPlatformPart&   aPart)
{
   if (aPart.GetPlatform() != nullptr)
   {
      aInfo.mPlatformIndex = aPart.GetPlatform()->GetIndex();
   }
   else
   {
      aInfo.mPlatformIndex = 0;
   }
   aInfo.mName = aPart.GetNameId();
   aInfo.mType = aPart.GetTypeId();
   if (!aPart.GetTypeList().empty())
   {
      aInfo.mBasicType = aPart.GetTypeList().front();
   }
   else
   {
      aInfo.mBasicType = nullptr;
   }
   aInfo.mIsTurnedOn             = aPart.IsTurnedOn();
   aInfo.mIsOperational          = aPart.IsOperational();
   aInfo.mIsExternallyControlled = aPart.IsExternallyControlled();
   aInfo.mDebugEnabled           = aPart.DebugEnabled();
}

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_ArticulatedPart(WsfXIO_Simulation&          aXIO_Simulation,
                                                 WsfXIO_ArticulatedPartInfo& aInfo,
                                                 const WsfArticulatedPart&   aPart)
{
   WsfXIO_ObjectInfoLoad::Load_PlatformPart(aXIO_Simulation, aInfo, aPart); // Load base class
   double yaw, pitch, roll;
   aPart.GetOrientation(yaw, pitch, roll);
   aInfo.mYaw   = (float)yaw;
   aInfo.mPitch = (float)pitch;
   aInfo.mRoll  = (float)roll;
   aInfo.mTilt  = (float)aPart.GetTilt();
}

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_Mover(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_MoverInfo& aInfo, const WsfMover& aMover)
{
   WsfXIO_ObjectInfoLoad::Load_PlatformPart(aXIO_Simulation, aInfo, aMover); // Load base class
   auto routeMoverPtr = dynamic_cast<const WsfRouteMover*>(&aMover);
   if (routeMoverPtr != nullptr)
   {
      aInfo.mSwitchType        = routeMoverPtr->GetSwitch();
      aInfo.mTurnFailOption    = routeMoverPtr->GetTurnFailOption();
      aInfo.mTurnFailThreshold = routeMoverPtr->GetTurnFailThreshold();
      auto wptMoverPtr         = dynamic_cast<const WsfWaypointMover*>(routeMoverPtr);
      if (wptMoverPtr != nullptr)
      {
         aInfo.mConstraints = *wptMoverPtr->GetMoverConstraints();
      }
   }
}

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_Comm(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_CommInfo& aInfo, const wsf::comm::Comm& aComm)
{
   aInfo.mPartType = cCOMPONENT_ROLE<wsf::comm::Comm>();
   WsfXIO_ObjectInfoLoad::Load_ArticulatedPart(aXIO_Simulation, aInfo, aComm); // Load base class
   aInfo.mNetworkName = WsfStringId(aComm.GetNetwork());
}

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_Processor(WsfXIO_Simulation&    aXIO_Simulation,
                                           WsfXIO_ProcessorInfo& aInfo,
                                           const WsfProcessor&   aProcessor)
{
   aInfo.mPartType = cCOMPONENT_ROLE<WsfProcessor>();
   WsfXIO_ObjectInfoLoad::Load_PlatformPart(aXIO_Simulation, aInfo, aProcessor); // Load base class
}

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_Sensor(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_SensorInfo& aInfo, const WsfSensor& aSensor)
{
   aInfo.mPartType = cCOMPONENT_ROLE<WsfSensor>();
   Load_ModedPart(aXIO_Simulation, aInfo, aSensor); // Load base class
   aInfo.mSensorClass = aSensor.GetClass();
   for (size_t i = 0; i < aSensor.GetModeCount(); ++i)
   {
      WsfXIO_SensorModeInfo modeInfo;
      ::Load_SensorMode(aXIO_Simulation, modeInfo, *aSensor.GetModeEntry(i));
      aInfo.mModes.push_back(modeInfo);
   }
   Load_Transmitters(aXIO_Simulation, aSensor, aInfo.mTransmitters);
   Load_Receivers(aXIO_Simulation, aSensor, aInfo.mReceivers);
}

// =================================================================================================
void WsfXIO_ObjectInfoLoad::Load_Platform(WsfXIO_Simulation&   aXIO_Simulation,
                                          WsfXIO_PlatformInfo& aInfo,
                                          const WsfPlatform&   aPlatform)
{
   aInfo.mIndex = static_cast<int32_t>(aPlatform.GetIndex());
   aInfo.mFlags = 0;
   if (aPlatform.IsExternallyControlled())
   {
      aInfo.mFlags |= WsfXIO_PlatformInfo::cEXTERNALLY_CONTROLLED;
   }
   aInfo.mName          = aPlatform.GetNameId();
   aInfo.mType          = aPlatform.GetTypeId();
   aInfo.mIcon          = aPlatform.GetIcon();
   aInfo.mSide          = aPlatform.GetSide();
   aInfo.mSpatialDomain = aPlatform.GetSpatialDomain();

   // set the dis entity id.
   WsfDisInterface* disInterfacePtr = WsfDisInterface::Find(aXIO_Simulation.GetSimulation());
   if (disInterfacePtr != nullptr)
   {
      WsfDisPlatform* disPlatformPtr = disInterfacePtr->GetDisPlatform(aInfo.mIndex);
      if (disPlatformPtr != nullptr)
      {
         aInfo.mEntityId   = disPlatformPtr->GetEntityId();
         aInfo.mEntityType = disPlatformPtr->GetEntityType();

         DisUint8 markingChars[11] = {0};
         disPlatformPtr->GetEntityState()->GetMarkingChars(markingChars);
         std::string markingStr;
         for (unsigned int i = 0; i < 11u; ++i)
         {
            markingStr.push_back((unsigned int)markingChars[i]);
         }
         aInfo.mMarking.SetString(markingStr);
      }
   }

   const auto* trackMgr = aPlatform.GetComponent<WsfTrackManager>();
   if (trackMgr)
   {
      ::Load_TrackManager(aXIO_Simulation, aInfo.mTrackManagerInfo, *trackMgr);
   }

   // Make one pass through the component list and populate the XIO platform with whatever originates
   // from components on the WSF platform.

   WsfMover*        moverPtr = nullptr;
   WsfPlatformPart* partPtr  = nullptr;
   WsfCommandChain* chainPtr = nullptr;
   WsfZone*         zonePtr  = nullptr;
   for (WsfPlatformComponentList::Iterator iter = aPlatform.Begin(); iter != aPlatform.End(); ++iter)
   {
      WsfPlatformComponent* componentPtr = *iter;
      if (componentPtr->QueryInterfaceT(moverPtr))
      {
         aInfo.mMoverPtr = new WsfXIO_MoverInfo;
         Load_Mover(aXIO_Simulation, *(aInfo.mMoverPtr), *moverPtr);
      }
      else if (componentPtr->QueryInterfaceT(partPtr))
      {
         WsfXIO_PlatformPartInfo* infoPtr = CreatePlatformPart(aXIO_Simulation, *partPtr);
         if (infoPtr != nullptr)
         {
            aInfo.mParts.push_back(infoPtr);
         }
      }
      else if (componentPtr->QueryInterfaceT(chainPtr))
      {
         WsfXIO_PlatformInfo::CommandChain commandChain(chainPtr->GetNameId(), chainPtr->GetCommanderNameId());
         aInfo.mCommanders.push_back(commandChain);
      }
      else if (componentPtr->QueryInterfaceT(zonePtr))
      {
         WsfXIO_Zone zoneId;
         zoneId.mGlobalName = zonePtr->GetGlobalName();
         zoneId.mName       = zonePtr->GetNameId();
         aInfo.mZoneNames.push_back(zoneId);
      }
   }

   if (aPlatform.HasAuxData())
   {
      aInfo.mAuxDataPtr = new WsfAttributeContainer(aPlatform.GetAuxDataConst());
   }
}

// =================================================================================================
//! Creates a WsfXIO_PlatformPartInfo from a WsfPlatformPart.
//! Returns null_ptr if the part is not a recognized tpe.
// static
WsfXIO_PlatformPartInfo* WsfXIO_ObjectInfoLoad::CreatePlatformPart(WsfXIO_Simulation& aXIO_Simulation, WsfPlatformPart& aPart)
{
   wsf::comm::Comm* commPtr      = nullptr;
   WsfProcessor*    processorPtr = nullptr;
   WsfSensor*       sensorPtr    = nullptr;
   if (aPart.QueryInterfaceT(commPtr))
   {
      WsfXIO_CommInfo* infoPtr = new WsfXIO_CommInfo;
      WsfXIO_ObjectInfoLoad::Load_Comm(aXIO_Simulation, *infoPtr, (wsf::comm::Comm&)aPart);
      return infoPtr;
   }
   else if (aPart.QueryInterfaceT(sensorPtr))
   {
      WsfXIO_SensorInfo* infoPtr = new WsfXIO_SensorInfo;
      WsfXIO_ObjectInfoLoad::Load_Sensor(aXIO_Simulation, *infoPtr, (WsfSensor&)aPart);
      return infoPtr;
   }
   else if (aPart.QueryInterfaceT(processorPtr))
   {
      WsfXIO_ProcessorInfo* infoPtr = new WsfXIO_ProcessorInfo;
      WsfXIO_ObjectInfoLoad::Load_Processor(aXIO_Simulation, *infoPtr, (WsfProcessor&)aPart);
      return infoPtr;
   }

   // Try the components to see if they handle this part.
   using ComponentList = WsfXIO_Simulation::ComponentList;
   const ComponentList& components(aXIO_Simulation);
   for (ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
   {
      WsfXIO_PlatformPartInfo* infoPtr = (*i)->CreatePlatformPartInfo(aPart);
      if (infoPtr != nullptr)
      {
         return infoPtr;
      }
   }
   return nullptr;
}
