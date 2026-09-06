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

#include "WsfEventPipeInterface.hpp"

#include "Utml.hpp"
#include "WsfAdvancedBehaviorTreeNode.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfCircularFieldOfView.hpp"
#include "WsfComm.hpp"
#include "WsfCommProtocolLegacy.hpp"
#include "WsfCommProtocolMulticast.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommandChain.hpp"
#include "WsfCovariance.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEquatorialFieldOfView.hpp"
#include "WsfEventPipe.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfEventPipeClassesRegister.hpp"
#include "WsfEventPipeFileWriteWorker.hpp"
#include "WsfEventPipeLogger.hpp"
#include "WsfEventPipePartUpdateEvent.hpp"
#include "WsfFieldOfView.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfPolygonalFieldOfView.hpp"
#include "WsfRoute.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "WsfVersion.hpp"
#include "WsfVisualPart.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneReference.hpp"
#include "WsfZoneSet.hpp"
#include "WsfZoneTypes.hpp"
#include "comm/WsfCommNetworkManager.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtInterface.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptStateMachine.hpp"

namespace
{
// for debugging message serialization.  write message to buffer and read it back in
// void CheckMessage(UtPackSerializer* aSerializer,
//                  UtPackMessage&    aMsg)
//{
//   UtPackStream tmp;
//   UtPackSerializer::MessageInfo* infoPtr = aSerializer->GetMessageInfo(aMsg.GetMessageId());
//   infoPtr->mSerializeFunction->Write(tmp, &aMsg);
//   UtBuffer bufferCopy(tmp.mBuffer);
//   UtPackMessage* copiedMessagePtr = (*infoPtr->mMessageFactory)();
//   infoPtr->mSerializeFunction->Read(tmp, copiedMessagePtr);

//   // stop here to manually check if the messages look the same
//   delete copiedMessagePtr;
//   return;
//}

WsfEventPipe::Vec3d toVec3d(const UtVec3d& aSrc)
{
   WsfEventPipe::Vec3d v;
   v.x(aSrc[0]);
   v.y(aSrc[1]);
   v.z(aSrc[2]);
   return v;
}

WsfEventPipe::Vec3d toVec3d(const double aArray[3])
{
   WsfEventPipe::Vec3d v;
   v.x(aArray[0]);
   v.y(aArray[1]);
   v.z(aArray[2]);
   return v;
}

WsfEventPipe::Vec3f toVec3f(const double aArray[3])
{
   WsfEventPipe::Vec3f v;
   v.x(static_cast<float>(aArray[0]));
   v.y(static_cast<float>(aArray[1]));
   v.z(static_cast<float>(aArray[2]));
   return v;
}
} // namespace

// Helper functions for creating zone-related messages
namespace
{
// Used for variables inside the zone definition, if the value matches the provided unspecified value, use the default instead
double GetDefinedZoneValue(double aValue, double aUnspecified, double aDefault)
{
   return (aValue != aUnspecified) ? aValue : aDefault;
}

WsfEventPipe::ColorF UtColorToColorF(const ut::optional<UtColor>& aColor)
{
   WsfEventPipe::ColorF color;
   float rgba[4] = {-1.0f, -1.0f, -1.0f, -1.0f}; // if not defined, set colors to -1. This will signify the use of
                                                 // default colors defined in settings
   if (aColor)
   {
      aColor->Get(rgba[0], rgba[1], rgba[2], rgba[3]);
   }
   color.red(rgba[0]);
   color.green(rgba[1]);
   color.blue(rgba[2]);
   color.alpha(rgba[3]);
   return color;
};

WsfEventPipe::CommonZoneData GetCommonZoneData(WsfZoneDefinition* aZoneDefPtr, const std::string& aParent = "")
{
   WsfEventPipe::CommonZoneData commonData;
   commonData.zoneName(aZoneDefPtr->GetName());
   if (aZoneDefPtr->GetPlatform())
   {
      commonData.isGlobal(false);
      commonData.parentPlatform(aZoneDefPtr->GetPlatform()->GetName());
   }
   else
   {
      commonData.isGlobal(true);
      commonData.parentPlatform("Global Zones");
   }
   commonData.referencePlatform(aZoneDefPtr->GetReferencePlatformName());
   commonData.lineColor(UtColorToColorF(aZoneDefPtr->GetLineColor()));
   commonData.fillColor(UtColorToColorF(aZoneDefPtr->GetFillColor()));
   double altBounds[2];
   aZoneDefPtr->GetAltBounds(altBounds[0], altBounds[1]);
   commonData.minAltitudeSet(altBounds[0] != WsfZone::cUNSPECIFIED);
   commonData.maxAltitudeSet(altBounds[1] != WsfZone::cUNSPECIFIED);
   commonData.minAltitude(GetDefinedZoneValue(altBounds[0], WsfZone::cUNSPECIFIED, 0.0));
   commonData.maxAltitude(GetDefinedZoneValue(altBounds[1], WsfZone::cUNSPECIFIED, commonData.minAltitude()));

   // Note: This zone could be part of a set, or referenced by some other zone, so the parentPlatform field isn't
   // guaranteed to have the highest level parent.
   //       The highest level parent is provided so that if position is unspecified, the zone will instead reference the
   //       highest level parent platform instead of 0,0 This is only an issue when the zone is global, since a
   //       non-global zone will already have a reference platform
   if (!aParent.empty() && (commonData.isGlobal()) && (!aZoneDefPtr->HasAbsoluteLatLon()) &&
       (aZoneDefPtr->GetReferencePlatformName().Empty()))
   {
      commonData.referencePlatform(aParent);
      commonData.locationDefined(false);
   }
   else
   {
      commonData.referenceLat(GetDefinedZoneValue(aZoneDefPtr->GetReferenceLat(), WsfZone::cUNSPECIFIED, 0.0));
      commonData.referenceLon(GetDefinedZoneValue(aZoneDefPtr->GetReferenceLon(), WsfZone::cUNSPECIFIED, 0.0));
      // A global zone location is defined when lat/lon is defined or when everything (reference, position) is unspecified
      // (it will be placed at 0,0 in that case). A platform zone location is only defined when either lat/lon is defined.
      if (commonData.isGlobal())
      {
         commonData.locationDefined(aZoneDefPtr->HasAbsoluteLatLon() || aZoneDefPtr->GetReferencePlatformName().Empty());
      }
      else
      {
         commonData.locationDefined(aZoneDefPtr->HasAbsoluteLatLon());
      }
   }
   commonData.heading(aZoneDefPtr->GetReferenceHeading());
   return commonData;
};

WsfEventPipe::CircularZoneInfo GetCircularZone(WsfZoneDefinition*                  aZoneDefPtr,
                                               const WsfEventPipe::CommonZoneData& aCommonData)
{
   WsfEventPipe::CircularZoneInfo zoneData;
   // The sqrt comparison is necessary because for an unspecified radius, GetRadius() is effectively returning sqrt(cUNSPECIFIED)
   zoneData.radius(GetDefinedZoneValue(aZoneDefPtr->GetRadius(), std::sqrt(WsfZone::cUNSPECIFIED), 0.0));
   zoneData.minRadius(GetDefinedZoneValue(aZoneDefPtr->GetMinRadius(), std::sqrt(WsfZone::cUNSPECIFIED), 0.0));
   double angleBounds[2];
   aZoneDefPtr->GetAngleBounds(angleBounds[0], angleBounds[1]);
   zoneData.startAngle(angleBounds[0]);
   zoneData.stopAngle(angleBounds[1]);
   zoneData.commonInfo(aCommonData);
   return zoneData;
}

WsfEventPipe::EllipticalZoneInfo GetEllipticalZone(WsfZoneDefinition*                  aZoneDefPtr,
                                                   const WsfEventPipe::CommonZoneData& aCommonData)
{
   WsfEventPipe::EllipticalZoneInfo zoneData;
   zoneData.latAxis(GetDefinedZoneValue(aZoneDefPtr->GetLatAxisLength(), WsfZone::cUNSPECIFIED, 0.0));
   zoneData.lonAxis(GetDefinedZoneValue(aZoneDefPtr->GetLonAxisLength(), WsfZone::cUNSPECIFIED, 0.0));
   zoneData.minRadius(GetDefinedZoneValue(aZoneDefPtr->GetMinRadius(), std::sqrt(WsfZone::cUNSPECIFIED), 0.0));
   double angleBounds[2];
   aZoneDefPtr->GetAngleBounds(angleBounds[0], angleBounds[1]);
   zoneData.startAngle(angleBounds[0]);
   zoneData.stopAngle(angleBounds[1]);
   zoneData.commonInfo(aCommonData);
   return zoneData;
}

WsfEventPipe::SphericalZoneInfo GetSphericalZone(WsfZoneDefinition*                  aZoneDefPtr,
                                                 const WsfEventPipe::CommonZoneData& aCommonData)
{
   WsfEventPipe::SphericalZoneInfo zoneData;
   // The sqrt comparison is necessary because for an unspecified radius, GetRadius() is effectively returning sqrt(cUNSPECIFIED)
   zoneData.radius(GetDefinedZoneValue(aZoneDefPtr->GetRadius(), std::sqrt(WsfZone::cUNSPECIFIED), 0.0));
   zoneData.minRadius(GetDefinedZoneValue(aZoneDefPtr->GetMinRadius(), std::sqrt(WsfZone::cUNSPECIFIED), 0.0));
   double angleBounds[2];
   aZoneDefPtr->GetAngleBounds(angleBounds[0], angleBounds[1]);
   zoneData.startAngle(angleBounds[0]);
   zoneData.stopAngle(angleBounds[1]);
   zoneData.commonInfo(aCommonData);
   return zoneData;
}

WsfEventPipe::PolygonalZoneInfo GetPolygonalZone(WsfZoneDefinition*                  aZoneDefPtr,
                                                 const WsfEventPipe::CommonZoneData& aCommonData)
{
   WsfEventPipe::PolygonalZoneInfo zoneData;
   auto&                           zonePoints = aZoneDefPtr->GetPoints();
   WsfEventPipe::Vec2dList         pointList;
   for (const auto& it : zonePoints)
   {
      WsfEventPipe::Vec2d vec;
      vec.x(it.mX);
      vec.y(it.mY);
      pointList.push_back(vec);
   }
   zoneData.Points(pointList);
   zoneData.useLatLon(aZoneDefPtr->PointsAreLatLon());
   zoneData.commonInfo(aCommonData);
   return zoneData;
};

struct ZoneList
{
   WsfEventPipe::CircularZoneList   mCircularList;
   WsfEventPipe::EllipticalZoneList mEllipticalList;
   WsfEventPipe::SphericalZoneList  mSphericalList;
   WsfEventPipe::PolygonalZoneList  mPolygonalList;
};

void AddZoneDefinitionToList(WsfZoneDefinition* aZoneDefPtr, ZoneList& aList, const std::string aParent)
{
   if (aZoneDefPtr)
   {
      WsfEventPipe::CommonZoneData commonData = GetCommonZoneData(aZoneDefPtr, aParent);
      WsfZoneDefinition::ShapeType shapeType  = aZoneDefPtr->GetShapeType();
      if (shapeType == WsfZoneDefinition::ShapeType::cCIRCULAR)
      {
         aList.mCircularList.push_back(GetCircularZone(aZoneDefPtr, commonData));
      }
      else if (shapeType == WsfZoneDefinition::ShapeType::cELLIPTICAL)
      {
         aList.mEllipticalList.push_back(GetEllipticalZone(aZoneDefPtr, commonData));
      }
      else if (shapeType == WsfZoneDefinition::ShapeType::cSPHERICAL)
      {
         aList.mSphericalList.push_back(GetSphericalZone(aZoneDefPtr, commonData));
      }
      else if (shapeType == WsfZoneDefinition::ShapeType::cPOLYGONAL)
      {
         aList.mPolygonalList.push_back(GetPolygonalZone(aZoneDefPtr, commonData));
      }
   }
}

void GetReferenceZones(WsfZoneReference* aZoneRefPtr, ZoneList& aList, const std::string& aParent)
{
   WsfZone*           refZone    = aZoneRefPtr->GetReferencedZone();
   WsfZoneDefinition* zoneDefPtr = dynamic_cast<WsfZoneDefinition*>(refZone);
   WsfZoneReference*  zoneRefPtr = dynamic_cast<WsfZoneReference*>(refZone);
   WsfZoneSet*        zoneSetPtr = dynamic_cast<WsfZoneSet*>(refZone);
   // A zone definition will not have any further references, so add to the compiled list of zones
   if (zoneDefPtr)
   {
      AddZoneDefinitionToList(zoneDefPtr, aList, aParent);
   }
   // Continue going down the tree until reaching the base zone definition
   else if (zoneRefPtr)
   {
      GetReferenceZones(zoneRefPtr, aList, aParent);
   }
   // A zone set can have further references to both singular zones and zone sets, as well as zone definitions
   else if (zoneSetPtr)
   {
      for (int i = 0; i < zoneSetPtr->GetNumZones(); ++i)
      {
         zoneDefPtr = dynamic_cast<WsfZoneDefinition*>(zoneSetPtr->GetZoneAtIndex(i));
         zoneRefPtr = dynamic_cast<WsfZoneReference*>(zoneSetPtr->GetZoneAtIndex(i));
         if (zoneDefPtr)
         {
            AddZoneDefinitionToList(zoneDefPtr, aList, aParent);
         }
         else if (zoneRefPtr)
         {
            GetReferenceZones(zoneRefPtr, aList, aParent);
         }
      }
   }
}
} // namespace

WsfEventPipeInterface::WsfEventPipeInterface(WsfEventPipeExtension& aExtension, const WsfEventPipeInput& aInput)
   : mExtension(aExtension)
   , mStreamPtr(nullptr)
   , mFileStreamPtr(nullptr)
   , mSerializerPtr(nullptr)
   , mSchemaPtr(nullptr)
   , mWriteThreadPtr(nullptr)
   , mInput(aInput)
   , mEventIds(aExtension.GetEventIds())
   , mOutputFileName(aInput.mFileName)
{
   // make an array out of the map, making sure default is first
   mDetailSettingsArray.emplace_back(mInput.mDetailSettings.find("default")->second);
   for (const auto& detailSetting : mInput.mDetailSettings)
   {
      if (detailSetting.first != "default")
      {
         mDetailSettingsArray.emplace_back(detailSetting.second);
      }
      else
      {
         mCombinedOptions.Add(detailSetting.second);
      }
   }

   // make the filter array indexing into the settings array
   for (const WsfEventPipeInput::CriteriaToDetail& ctd : mInput.mCriteriaToDetail)
   {
      Filter f;
      f.mCritiera      = ctd.mCriteria;
      f.mDetailSetting = ut::safe_cast<unsigned int, ptrdiff_t>(
         std::distance(mInput.mDetailSettings.begin(), mInput.mDetailSettings.find(ctd.mDetailSettingName)));
      mFilters.push_back(f);
   }
}

WsfEventPipeInterface::~WsfEventPipeInterface()
{
   if (mWriteThreadPtr)
   {
      mWriteThreadPtr->StopRunning();
      mWriteThreadPtr->Join();
   }
   delete mWriteThreadPtr;

   delete mStreamPtr;
   delete mSerializerPtr;
   delete mSchemaPtr;
}

WsfEventPipeInterface* WsfEventPipeInterface::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfEventPipeInterface*>(aSimulation.FindExtension("event_pipe"));
}

void WsfEventPipeInterface::AddedToSimulation()
{
   if ((!mInput.mEnable) || (mOutputFileName.empty()))
   {
      if (mInput.mEnable && mInput.mTestMode)
      {
         UpdateFilters();
      }
      return;
   }

   mOutputFileName = GetSimulation().SubstituteOutputFileVariables(mOutputFileName);

   UtmlObject schemaDoc = UtmlObject::makeContainer("schema");

   UtTextDocument schemaText = WsfEventPipeExtension::Find(GetScenario())->GetSchema();
   UtmlParser     parser(schemaText);
   parser.Parse(schemaDoc);
   // we modify the message id's on some schema's to create unique "namespaces"
   // these are stored in settings under the value message_id_or when they are found,
   // any following message ids are modified to be (orig_value | (message_id_or << 24)
   int           orValue = 0;
   std::set<int> orList;
   std::set<int> messageIdList;
   for (int i = 0; i < schemaDoc.GetObjectCount(); ++i)
   {
      UtmlObject& obj = schemaDoc.GetObject(i);
      if (obj.GetType() == "Settings")
      {
         std::stringstream ss(obj.PropertyValue("message_id_or", "0"));
         ss >> orValue;
         if (!orList.insert(orValue).second)
         {
            ut::log::fatal() << "WsfEventPipe error: message_id_or value " << orValue << ", is reused.";
            throw UtException("WsfEventPipe Error, message id duplication");
         }
      }
      else if ((obj.GetType() == "Struct") && (orValue != 0))
      {
         int               value;
         std::stringstream ss(obj.PropertyValue("message", "-1"));
         ss >> value;
         if (-1 != value)
         {
            ss.clear();
            int modifiedValue = value | (orValue << 24);
            ss << modifiedValue;
            if (!messageIdList.insert(modifiedValue).second)
            {
               auto out = ut::log::fatal() << "WsfEventPipe error: message_id value " << modifiedValue << " is reused.";
               out.AddNote() << "message_id_or: " << orValue;
               out.AddNote() << "message id: " << value;
               throw UtException("WsfEventPipe Error, message id duplication");
            }
            UtmlObject* messageObj = obj.FindProperty("message");
            if (nullptr != messageObj)
            {
               messageObj->SetValue(ss.str());
            }
         }
      }
   }
   mSchemaPtr = new UtPackSchema;
   mSchemaPtr->Read(schemaDoc);
   mSchemaPtr->Resolve();

   mSerializerPtr = new UtPackSerializer();
   mSerializerPtr->RegisterBuiltinTypes();
   WsfEventPipe::UtPack_register_all_wsf_types(*mSerializerPtr);
   // Let extensions register messages
   WsfEventPipeExtension::Find(GetScenario())->RegisterExtensionMessages(mSerializerPtr);

   mSerializerPtr->Initialize(*mSchemaPtr);

   OpenOutput();
   if (mFileStreamPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to open event_pipe file.";
      out.AddNote() << "File: " << mOutputFileName;
      return;
   }
   mStreamPtr = new UtPackMessageStdStreamO(mFileStreamPtr.get(), mSerializerPtr);
   GetScenario().GetSystemLog().WriteOutputLogEntry("AER", mOutputFileName);

   // Write file header:
   // File type identifier, schema, and null terminator
   mFileStreamPtr->write("\0\0WSF_PIPE\n", 11);
   UtmlWriter writer;
   writer.mIndent = 1;
   writer.WriteRoot(schemaDoc, *mFileStreamPtr);
   mFileStreamPtr->write("\0", 1);

   SendExecData();
   SendScenarioData();

   mWriteThreadPtr             = new wsf::eventpipe::FileWriterWorker;
   mWriteThreadPtr->mStreamPtr = mStreamPtr;
   mWriteThreadPtr->StartRunning();
   mWriteThreadPtr->Start();

   UpdateFilters();

   GetSimulation().GetScriptContext().GetContext().Var("__EVENTPIPE").GetPointer()->SetAppObject(this);
}

// Get the event ID given an event name
int WsfEventPipeInterface::GetEventId(const std::string& aEventName) const
{
   return mExtension.GetEventId(aEventName);
}

const std::string& WsfEventPipeInterface::GetOutputFileName() const
{
   return mOutputFileName;
}

void WsfEventPipeInterface::UpdateSubscriptions(const WsfEventPipeOptions& aPreviousOptions)
{
   // Called when the combined options changes
   // Used to update observer subscriptions to include only those events that are required
   if (mCombinedOptions.IsOptionEnabled(mEventIds.cBASE_DATA))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cBASE_DATA))
      {
         mBaseCallbacks +=
            WsfObserver::SimulationStarting(&GetSimulation()).Connect(&WsfEventPipeInterface::SimulationStarting, this);

         mBaseCallbacks +=
            WsfObserver::PlatformAdded(&GetSimulation()).Connect(&WsfEventPipeInterface::PlatformAdded, this);
         mBaseCallbacks += WsfObserver::PlatformAppearanceChanged(&GetSimulation())
                              .Connect(&WsfEventPipeInterface::PlatformAppearanceChanged, this);
         mBaseCallbacks +=
            WsfObserver::PlatformInitialized(&GetSimulation()).Connect(&WsfEventPipeInterface::PlatformInitialized, this);
         mBaseCallbacks +=
            WsfObserver::SensorModeActivated(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorModeActivated, this);
         mBaseCallbacks +=
            WsfObserver::SensorModeDeactivated(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorModeDeactivated, this);

         mBaseCallbacks +=
            WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&WsfEventPipeInterface::PlatformDeleted, this);
         mBaseCallbacks +=
            WsfObserver::PlatformBroken(&GetSimulation()).Connect(&WsfEventPipeInterface::PlatformBroken, this);

         ConnectPartStatusCallback(WsfObserver::CommBroken, WsfEventPipe::PartType::comm);
         ConnectPartStatusCallback(WsfObserver::CommNonOperational, WsfEventPipe::PartType::comm);
         ConnectPartStatusCallback(WsfObserver::CommOperational, WsfEventPipe::PartType::comm);
         ConnectPartStatusCallback(WsfObserver::CommTurnedOff, WsfEventPipe::PartType::comm);
         ConnectPartStatusCallback(WsfObserver::CommTurnedOn, WsfEventPipe::PartType::comm);

         mBaseCallbacks +=
            WsfObserver::MoverChanged(&GetSimulation()).Connect(&WsfEventPipeInterface::MoverChangedOnPlatform, this);
         ConnectPartStatusCallback(WsfObserver::MoverBroken, WsfEventPipe::PartType::mover);
         ConnectPartStatusCallback(WsfObserver::MoverNonOperational, WsfEventPipe::PartType::mover);
         ConnectPartStatusCallback(WsfObserver::MoverOperational, WsfEventPipe::PartType::mover);
         ConnectPartStatusCallback(WsfObserver::MoverTurnedOff, WsfEventPipe::PartType::mover);
         ConnectPartStatusCallback(WsfObserver::MoverTurnedOn, WsfEventPipe::PartType::mover);

         ConnectPartStatusCallback(WsfObserver::ProcessorBroken, WsfEventPipe::PartType::processor);
         ConnectPartStatusCallback(WsfObserver::ProcessorNonOperational, WsfEventPipe::PartType::processor);
         ConnectPartStatusCallback(WsfObserver::ProcessorOperational, WsfEventPipe::PartType::processor);
         ConnectPartStatusCallback(WsfObserver::ProcessorTurnedOff, WsfEventPipe::PartType::processor);
         ConnectPartStatusCallback(WsfObserver::ProcessorTurnedOn, WsfEventPipe::PartType::processor);

         ConnectPartStatusCallback(WsfObserver::SensorBroken, WsfEventPipe::PartType::sensor);
         ConnectPartStatusCallback(WsfObserver::SensorNonOperational, WsfEventPipe::PartType::sensor);
         ConnectPartStatusCallback(WsfObserver::SensorOperational, WsfEventPipe::PartType::sensor);
         mBaseCallbacks +=
            WsfObserver::SensorTurnedOff(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorTurnedOff, this);
         mBaseCallbacks +=
            WsfObserver::SensorTurnedOn(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorTurnedOn, this);

         ConnectPartStatusCallback(WsfObserver::VisualPartBroken, WsfEventPipe::PartType::visual);
         ConnectPartStatusCallback(WsfObserver::VisualPartNonOperational, WsfEventPipe::PartType::visual);
         ConnectPartStatusCallback(WsfObserver::VisualPartOperational, WsfEventPipe::PartType::visual);
         mBaseCallbacks +=
            WsfObserver::VisualPartTurnedOff(&GetSimulation()).Connect(&WsfEventPipeInterface::VisualPartTurnedOff, this);
         mBaseCallbacks +=
            WsfObserver::VisualPartTurnedOn(&GetSimulation()).Connect(&WsfEventPipeInterface::VisualPartTurnedOn, this);

         mBaseCallbacks +=
            WsfObserver::DisEmissionReceived(&GetSimulation()).Connect(&WsfEventPipeInterface::DisEmissionReceived, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cBASE_DATA))
   {
      mBaseCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cTRACK))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cTRACK))
      {
         mLocalTrackEventCallbacks +=
            WsfObserver::LocalTrackInitiated(&GetSimulation()).Connect(&WsfEventPipeInterface::LocalTrackInitiated, this);
         mLocalTrackEventCallbacks +=
            WsfObserver::LocalTrackDropped(&GetSimulation()).Connect(&WsfEventPipeInterface::LocalTrackDropped, this);
         mLocalTrackEventCallbacks +=
            WsfObserver::SensorTrackInitiated(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorTrackInitiated, this);
         mLocalTrackEventCallbacks +=
            WsfObserver::SensorTrackDropped(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorTrackDropped, this);
         mLocalTrackEventCallbacks +=
            WsfObserver::LocalTrackCorrelation(&GetSimulation()).Connect(&WsfEventPipeInterface::LocalTrackCorrelation, this);
         mLocalTrackEventCallbacks += WsfObserver::LocalTrackDecorrelation(&GetSimulation())
                                         .Connect(&WsfEventPipeInterface::LocalTrackDecorrelation, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cTRACK))
   {
      mLocalTrackEventCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cTRACK_UPDATE))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cTRACK_UPDATE))
      {
         mLocalTrackUpdatedCallbacks +=
            WsfObserver::LocalTrackUpdated(&GetSimulation()).Connect(&WsfEventPipeInterface::LocalTrackUpdated, this);
         mLocalTrackUpdatedCallbacks +=
            WsfObserver::SensorTrackUpdated(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorTrackUpdated, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cTRACK_UPDATE))
   {
      mLocalTrackUpdatedCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cENTITY_STATE))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cENTITY_STATE))
      {
         mEntityStateCallbacks +=
            WsfObserver::MoverUpdated(&GetSimulation()).Connect(&WsfEventPipeInterface::MoverUpdated, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cENTITY_STATE))
   {
      mEntityStateCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cDETECTION_ATTEMPT))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cDETECTION_ATTEMPT))
      {
         mDetectionAttemptCallbacks +=
            WsfObserver::SensorDetectionAttempt(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorDetectionAttempt, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cDETECTION_ATTEMPT))
   {
      mDetectionAttemptCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cDETECTION_CHANGE))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cDETECTION_CHANGE))
      {
         mDetectionChangeCallbacks +=
            WsfObserver::SensorDetectionChanged(&GetSimulation()).Connect(&WsfEventPipeInterface::SensorDetectionChanged, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cDETECTION_CHANGE))
   {
      mDetectionChangeCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cDRAW))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cDRAW))
      {
         WsfDrawManager* drawMgrPtr = WsfDrawManager::Find(GetSimulation());
         if (drawMgrPtr != nullptr)
         {
            mDrawCallbacks += drawMgrPtr->DrawUpdate.Connect(&WsfEventPipeInterface::DrawUpdate, this);
         }
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cDRAW))
   {
      mDrawCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cMESSAGE_RECEIVED))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cMESSAGE_RECEIVED))
      {
         mMessageReceivedCallback +=
            WsfObserver::MessageReceived(&GetSimulation()).Connect(&WsfEventPipeInterface::MessageReceived, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cMESSAGE_RECEIVED))
   {
      mMessageReceivedCallback.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cMESSAGE_HOP))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cMESSAGE_HOP))
      {
         mMessageHopCallback +=
            WsfObserver::MessageHop(&GetSimulation()).Connect(&WsfEventPipeInterface::MessageHop, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cMESSAGE_HOP))
   {
      mMessageHopCallback.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cMESSAGE_TRANSMITTED))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cMESSAGE_TRANSMITTED))
      {
         mMessageTransmittedCallback +=
            WsfObserver::MessageTransmitted(&GetSimulation()).Connect(&WsfEventPipeInterface::MessageTransmitted, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cMESSAGE_TRANSMITTED))
   {
      mMessageTransmittedCallback.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cCOMMENT))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cCOMMENT))
      {
         mCommentCallback += WsfObserver::Comment(&GetSimulation()).Connect(&WsfEventPipeInterface::Comment, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cCOMMENT))
   {
      mCommentCallback.Clear();
   }
   if (mCombinedOptions.IsOptionEnabled(mEventIds.cROUTE_CHANGED))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cROUTE_CHANGED))
      {
         mRouteCallbacks +=
            WsfObserver::RouteChanged(&GetSimulation()).Connect(&WsfEventPipeInterface::RouteChanged, this);
         mRouteCallbacks += WsfObserver::MoverChanged(&GetSimulation())
                               .Connect([this](WsfPlatform* aPlatformPtr) { RouteChanged(aPlatformPtr->GetMover()); });
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cROUTE_CHANGED))
   {
      mRouteCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cBEHAVIOR_TOOL))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cBEHAVIOR_TOOL))
      {
         mBehaviorToolCallbacks +=
            WsfObserver::AdvancedBehaviorTree(&GetSimulation()).Connect(&WsfEventPipeInterface::AdvancedBehaviorTree, this);
         mBehaviorToolCallbacks += WsfObserver::AdvancedBehaviorTreeState(&GetSimulation())
                                      .Connect(&WsfEventPipeInterface::AdvancedBehaviorTreeState, this);
         mBehaviorToolCallbacks +=
            WsfObserver::StateMachine(&GetSimulation()).Connect(&WsfEventPipeInterface::StateMachine, this);
         mBehaviorToolCallbacks +=
            WsfObserver::StateMachineState(&GetSimulation()).Connect(&WsfEventPipeInterface::StateMachineState, this);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cBEHAVIOR_TOOL))
   {
      mBehaviorToolCallbacks.Clear();
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cAUX_DATA))
   {
      if (!aPreviousOptions.IsOptionEnabled(mEventIds.cAUX_DATA))
      {
         auto cb = [this](double aSimTime, WsfPlatform* aPlatform)
         {
            if (aPlatform->HasAuxData())
            {
               SendPlatformAuxData(aSimTime, aPlatform);
            }
         };

         mAuxDataCallbacks += WsfAuxDataEnabled::AuxDataAccessed.Connect(&WsfEventPipeInterface::AuxDataAccessed, this);
         mAuxDataCallbacks += WsfAuxDataEnabled::AuxDataDestroyed.Connect(&WsfEventPipeInterface::AuxDataDestroyed, this);
         mAuxDataCallbacks += WsfObserver::PlatformInitialized(&GetSimulation()).Connect(cb);
         mAuxDataCallbacks += WsfObserver::PlatformDeleted(&GetSimulation()).Connect(cb);
      }
   }
   else if (aPreviousOptions.IsOptionEnabled(mEventIds.cAUX_DATA))
   {
      mAuxDataCallbacks.Clear();
   }

   for (auto& logger : mLoggers)
   {
      logger->UpdateSubscriptions(mCombinedOptions, aPreviousOptions);
   }
}


const WsfEventPipeOptions& WsfEventPipeInterface::GetPlatformDetailLevel(const WsfPlatform& aPlatform)
{
   size_t idx = aPlatform.GetIndex();
   if (idx >= mPlatformIndexToSettingIndex.size())
   {
      return mDetailSettingsArray[0].mOptions;
   }
   return mDetailSettingsArray[mPlatformIndexToSettingIndex[idx]].mOptions;
}

unsigned int WsfEventPipeInterface::Platform(const WsfPlatform& aPlatform)
{
   return Platform(aPlatform.GetIndex());
}

unsigned int WsfEventPipeInterface::Platform(size_t aPlatformIndex)
{
   auto it = mExternalIdLookup.find(aPlatformIndex);
   if (it != mExternalIdLookup.end())
   {
      return static_cast<unsigned int>(it->second);
   }
   return static_cast<unsigned int>(aPlatformIndex);
}

void WsfEventPipeInterface::Record(double aSimTime, WsfPlatform* aPlatform, const std::string& aKey, double aValue)
{
   if (aPlatform)
   {
      RecordDouble(*aPlatform, aKey, aValue);
   }
}

void WsfEventPipeInterface::RecordDouble(const WsfPlatform& aPlatform, const std::string& aKey, double aValue)
{
   double simTime = GetSimulation().GetSimTime();
   auto   msgPtr  = ut::make_unique<WsfEventPipe::MsgScriptData>();
   msgPtr->platformIndex(Platform(aPlatform));
   msgPtr->key(aKey);
   msgPtr->value().floating(aValue);
   SendDependent(simTime, aPlatform, std::move(msgPtr));
}

void WsfEventPipeInterface::RecordInt(const WsfPlatform& aPlatform, const std::string& aKey, int aValue)
{
   double simTime = GetSimulation().GetSimTime();
   auto   msgPtr  = ut::make_unique<WsfEventPipe::MsgScriptData>();
   msgPtr->platformIndex(Platform(aPlatform));
   msgPtr->key(aKey);
   msgPtr->value().integer(aValue);
   SendDependent(simTime, aPlatform, std::move(msgPtr));
}

void WsfEventPipeInterface::RecordBool(const WsfPlatform& aPlatform, const std::string& aKey, bool aValue)
{
   double simTime = GetSimulation().GetSimTime();
   auto   msgPtr  = ut::make_unique<WsfEventPipe::MsgScriptData>();
   msgPtr->platformIndex(Platform(aPlatform));
   msgPtr->key(aKey);
   msgPtr->value().boolean(aValue);
   SendDependent(simTime, aPlatform, std::move(msgPtr));
}

void WsfEventPipeInterface::RecordString(const WsfPlatform& aPlatform, const std::string& aKey, const std::string& aValue)
{
   double simTime = GetSimulation().GetSimTime();
   auto   msgPtr  = ut::make_unique<WsfEventPipe::MsgScriptData>();
   msgPtr->platformIndex(Platform(aPlatform));
   msgPtr->key(aKey);
   msgPtr->value().text(aValue);
   SendDependent(simTime, aPlatform, std::move(msgPtr));
}

double WsfEventPipeInterface::HandleScheduledEntityStateRequest(double aTime, size_t aPlatformId)
{
   WsfPlatform* platform = GetSimulation().GetPlatformByIndex(aPlatformId);
   if (!platform)
   {
      return 0.0;
   }
   auto data = mEntityStateData.find(Platform(*platform));
   if (data == mEntityStateData.end() ||
       (data->second.Time() + mInput.mEntityStateMaximumInterval <= aTime)) // it's time for a regular update
   {
      SendEntityState(aTime, *platform, true);
      return aTime + mInput.mEntityStateMaximumInterval;
   }
   // an update occurred preemptively, reschedule based on that
   return data->second.Time() + mInput.mEntityStateMaximumInterval;
}

void WsfEventPipeInterface::OpenOutput()
{
   UtPath path(mOutputFileName);
   mFileStreamPtr = ut::make_unique<std::ofstream>(path.GetSystemPath(), std::ios::binary);
   if (!mFileStreamPtr->is_open())
   {
      mFileStreamPtr = nullptr;
   }
}

void WsfEventPipeInterface::SendEntityState(double aSimTime, WsfPlatform& aPlatform, bool aForce)
{
   unsigned int platIdx = Platform(aPlatform);

   const auto aMover      = aPlatform.GetMover();
   double     elapsedTime = 0.0;
   if (aMover)
   {
      elapsedTime = aSimTime - aMover->GetLastUpdateTime();
   }
   UtVec3d origLocV = UtVec3d(aPlatform.GetLocationWCS());
   UtVec3d origVelV = UtVec3d(aPlatform.GetVelocityWCS());
   UtVec3d accV     = UtVec3d(aPlatform.GetAccelerationWCS());
   UtVec3d oriV;
   aPlatform.GetOrientationWCS(oriV.GetData()[0], oriV.GetData()[1], oriV.GetData()[2]);

   UtVec3d locV =
      origLocV + origVelV * elapsedTime + 0.5 * elapsedTime * elapsedTime * accV; // extrapolate to current time

   UtQuaternion q(oriV[0], oriV[1], oriV[2]);

   if (aForce ||
       RequiresEntityStateUpdate(aSimTime, platIdx, locV, q)) // if it breaks any set thresholds, or an update is demanded
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgEntityState>();
      msgPtr->state().platformIndex(platIdx);
      msgPtr->state().damageFactor(aPlatform.GetDamageFactor());
      msgPtr->state().fuelCurrent(aPlatform.GetFuelMass()); // note that this is not extrapolated (as the geometric data is)
      msgPtr->state().locationWCS(toVec3d(locV.GetData()));

      if (aPlatform.GetSpatialDomain() == WSF_SPATIAL_DOMAIN_AIR)
      {
         const auto& atm(aPlatform.GetScenario().GetAtmosphere());
         double      sonicVel = atm.SonicVelocity(aPlatform.GetAltitude());
         if (sonicVel > 0)
         {
            msgPtr->state().machNumber(aPlatform.GetSpeed() / sonicVel);
         }
      }

      UtVec3d velV = origVelV + accV * elapsedTime;
      if (velV.MagnitudeSquared() > 0.0) // if there is any velocity
      {
         msgPtr->state().velocityWCS(toVec3f(velV.GetData()));
      }
      if (accV.MagnitudeSquared() > 0.0) // if there is any acceleration
      {
         msgPtr->state().accelerationWCS(toVec3f(accV.GetData()));
      }
      if (oriV.MagnitudeSquared() > 0.0) // if there is any orientation
      {
         msgPtr->state().orientationWCS(toVec3f(oriV.GetData()));
      }

      auto result = mEntityStateData.emplace(std::piecewise_construct,
                                             std::forward_as_tuple(platIdx),
                                             std::forward_as_tuple(aSimTime, origLocV, origVelV, accV, q));
      if (result.second)
      {
         // if we haven't encountered this before, schedule updates for it
         auto eventPtr =
            ut::make_unique<WsfRecurringEvent>(aSimTime + mInput.mEntityStateMaximumInterval,
                                               [=](WsfEvent& e)
                                               {
                                                  double time = HandleScheduledEntityStateRequest(e.GetTime(), platIdx);
                                                  if (time == 0.0) // platform couldn't be found, delete the update
                                                  {
                                                     return WsfEvent::cDELETE;
                                                  }
                                                  e.SetTime(time);
                                                  return WsfEvent::cRESCHEDULE;
                                               });

         GetSimulation().AddEvent(std::move(eventPtr));
      }
      else
      {
         result.first->second =
            EntityStateData(aSimTime, origLocV, origVelV, accV, q); // store this data for extrapolation
      }
      SendDependent(aSimTime, aPlatform, std::move(msgPtr));
   }
}

//! Fills the auxData field of aMsg with the attributes in aAuxData.
//! aPrefix is used for nested data.
static void FillMessageAuxData(WsfEventPipe::MsgAuxData& aMsg, const UtAttributeContainer& aAuxData, std::string aPrefix = {})
{
   for (const auto& entry : aAuxData.GetAttributeMap())
   {
      const std::string name = (aPrefix.empty() ? entry.first : aPrefix + "." + entry.first);
      if (entry.second->IsContainerType())
      {
         ::FillMessageAuxData(aMsg, static_cast<const UtAttributeContainer&>(*entry.second), name);
      }
      else
      {
         WsfEventPipe::AuxDataValue value;
         value.name(name);

         bool        boolean;
         int         integer;
         double      real;
         std::string text;

         if (entry.second->TryGet(boolean))
         {
            value.type(WsfEventPipe::AuxDataType::Boolean);
            value.boolean(boolean);
         }
         else if (entry.second->TryGet(integer))
         {
            value.type(WsfEventPipe::AuxDataType::Integer);
            value.integer(integer);
         }
         else if (entry.second->TryGet(real))
         {
            value.type(WsfEventPipe::AuxDataType::Real);
            value.real(real);
         }
         else if (entry.second->TryGet(text))
         {
            value.type(WsfEventPipe::AuxDataType::Text);
            value.text(text);
         }

         aMsg.auxData().push_back(std::move(value));
      }
   }
}

void WsfEventPipeInterface::SendAllPlatformAuxData(double aSimTime)
{
   std::lock_guard<std::mutex> lock(mAuxDataAccessedMutex);
   for (const auto* accessed : mAuxDataAccessed)
   {
      auto platform = dynamic_cast<const WsfPlatform*>(accessed);
      if (platform)
      {
         SendPlatformAuxData(aSimTime, platform);
      }
   }
   mAuxDataAccessed.clear();
}

void WsfEventPipeInterface::SendPlatformAuxData(double aSimTime, const WsfPlatform* aPlatform)
{
   auto msgPtr = ut::make_unique<WsfEventPipe::MsgAuxData>();
   msgPtr->platformIndex(Platform(*aPlatform));
   if (aPlatform->HasAuxData())
   {
      ::FillMessageAuxData(*msgPtr, aPlatform->GetAuxDataConst());
   }

   SendDependent(aSimTime, *aPlatform, std::move(msgPtr));
}

void WsfEventPipeInterface::AuxDataAccessed(const WsfAuxDataEnabled* aSource)
{
   std::lock_guard<std::mutex> lock(mAuxDataAccessedMutex);
   if (mAuxDataAccessed.empty())
   {
      double simTime = GetSimulation().GetSimTime();
      GetSimulation().AddEventT<WsfOneShotEvent>(simTime, [this, simTime]() { SendAllPlatformAuxData(simTime); });
   }
   mAuxDataAccessed.insert(aSource);
}

// aDestroyed CANNOT be checked using dynamic_cast!
void WsfEventPipeInterface::AuxDataDestroyed(const WsfAuxDataEnabled* aDestroyed)
{
   std::lock_guard<std::mutex> lock(mAuxDataAccessedMutex);
   mAuxDataAccessed.erase(aDestroyed);
}

double WsfEventPipeInterface::MoverUpdateInterval(const WsfMover& aMover) const
{
   const double updateInterval = aMover.GetUpdateInterval();
   if ((mInput.mMaximumMoverUpdateInterval != 0) &&
       ((updateInterval == 0) || (updateInterval > mInput.mMaximumMoverUpdateInterval)))
   {
      return mInput.mMaximumMoverUpdateInterval;
   }
   else
   {
      return updateInterval;
   }
}

void WsfEventPipeInterface::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // determine detail level for this platform

   size_t idx = aPlatformPtr->GetIndex();
   if (mPlatformIndexToSettingIndex.size() < idx)
   {
      mPlatformIndexToSettingIndex.resize(idx + 1);
   }
   for (size_t i = mFilters.size(); i > 0; --i)
   {
      Filter& f = mFilters[i - 1];
      if (f.mCritiera.Matches(*aPlatformPtr))
      {
         mPlatformIndexToSettingIndex[idx] = f.mDetailSetting;
         if (mDetailSettingsArray[f.mDetailSetting].mActivePlatformCount++ == 0)
         {
            UpdateSubscriptions(mCombinedOptions);
         }
         break;
      }
   }

   MoverChangedOnPlatform(aPlatformPtr);
}

void WsfEventPipeInterface::PlatformAppearanceChanged(double                      aSimTime,
                                                      WsfPlatform*                aPlatformPtr,
                                                      WsfObserver::AppearanceType aAppearanceType)
{
   if (aAppearanceType == WsfObserver::cAPPEARANCE_WORD)
   {
      if (aPlatformPtr != nullptr)
      {
         auto msgPtr = ut::make_unique<WsfEventPipe::MsgPlatformAppearanceChanged>();
         msgPtr->ownerIndex(Platform(*aPlatformPtr));
         static_assert(sizeof(msgPtr->appearance()) >= sizeof(aPlatformPtr->GetAppearance()),
                       "appearance bits won't fit into the appearance changed message, change the data type in "
                       "MsgPlatformAppearanceChanged");
         msgPtr->appearance(aPlatformPtr->GetAppearance());

         SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
      }
   }
}

void WsfEventPipeInterface::PlatformInitialized(double aSimTime, WsfPlatform* aPlatform)
{
   bool statusNeeded = false;
   // if a platform is externally controlled we might be dealing with a reconnected sim, or an entity
   // that is stale, if this is the case, we use the name as the unique identifier and use the old
   // id rather than the new one.
   if (aPlatform->IsExternallyControlled())
   {
      if (mExternalNameMap.find(aPlatform->GetName()) != mExternalNameMap.end())
      {
         mExternalIdLookup[aPlatform->GetIndex()] = mExternalNameMap[aPlatform->GetName()];
         statusNeeded                             = true;
      }
      else
      {
         mExternalNameMap[aPlatform->GetName()] = aPlatform->GetIndex();
      }
   }

   auto msgInfoPtr = ut::make_unique<WsfEventPipe::MsgPlatformInfo>();
   msgInfoPtr->platformIndex(Platform(*aPlatform));
   msgInfoPtr->name(aPlatform->GetName());
   std::vector<WsfStringId> typeList = aPlatform->GetTypeList();
   for (auto type : typeList)
   {
      msgInfoPtr->types().push_back(type.GetString());
      msgInfoPtr->typesValid(true);
   }

   for (WsfComponentList::RoleIterator<WsfCommandChain> iter(*aPlatform); !iter.AtEnd(); ++iter)
   {
      WsfEventPipe::CommandChain c;
      c.name(iter->GetName());
      c.commander(iter->GetCommanderName());
      msgInfoPtr->commandChains().push_back(c);
      msgInfoPtr->commandChainsValid(true);
   }

   msgInfoPtr->icon(aPlatform->GetIcon());
   msgInfoPtr->side(aPlatform->GetSide());
   msgInfoPtr->spatialDomain(aPlatform->GetSpatialDomain());

   // If platform has a fuel object, retrieve fuel capacity.
   if (aPlatform->GetFuel() != nullptr)
   {
      msgInfoPtr->fuelCapacity(aPlatform->GetFuel()->GetMaximumQuantity());
   }
   else // Otherwise set fuel capacity to zero.
   {
      msgInfoPtr->fuelCapacity(0);
      msgInfoPtr->fuelCapacityValid(false);
   }

   const WsfCategoryList::CategoryList& categories = aPlatform->GetCategories().GetCategoryList();
   if (!categories.empty())
   {
      for (auto category : categories)
      {
         msgInfoPtr->categories().push_back(category);
      }
      msgInfoPtr->categoriesValid(true);
   }

   WsfDisInterface* dis = WsfDisInterface::Find(*aPlatform->GetSimulation());
   if (dis != nullptr)
   {
      WsfDisPlatform* disPlatformPtr = dis->GetDisPlatform(aPlatform->GetIndex());
      if (disPlatformPtr != nullptr)
      {
         const DisEntityId&        id = disPlatformPtr->GetEntityId();
         WsfEventPipe::DisEntityId out;
         out.site(id.GetSite());
         out.application(id.GetApplication());
         out.entity(id.GetEntity());
         msgInfoPtr->disId(out);
      }
   }

   SendImmediate(aSimTime, aPlatform->IsExternallyControlled(), std::move(msgInfoPtr));

   if (statusNeeded)
   {
      auto msgStatusPtr = ut::make_unique<WsfEventPipe::MsgPlatformStatus>();
      msgStatusPtr->platformIndex(Platform(*aPlatform));
      msgStatusPtr->removed(false);
      msgStatusPtr->broken(aPlatform->IsBroken() || aPlatform->GetDamageFactor() >= 1.0);
      SendImmediate(aSimTime, aPlatform->IsExternallyControlled(), std::move(msgStatusPtr));
   }

   if (mCombinedOptions.IsOptionEnabled(mEventIds.cROUTE_CHANGED))
   {
      auto mover = aPlatform->GetMover();
      if (mover)
      {
         auto route = mover->GetRoute();
         if (route && !route->Empty())
         {
            RouteChanged(mover); // no routeChanged would be sent for initial state otherwise
         }
      }
   }

   SendEntityState(aSimTime, *aPlatform);

   // Process cached messages
   auto it = mCachedMessages.find(Platform(*aPlatform));
   if (it != mCachedMessages.end())
   {
      for (auto&& msg : it->second)
      {
         SendImmediate(aSimTime, aPlatform->IsExternallyControlled(), std::move(msg));
      }
      it->second.clear();
   }

   // Add zones with a separate message
   if (mCombinedOptions.IsOptionEnabled(mEventIds.cZONES))
   {
      for (WsfComponentList::RoleIterator<WsfZone> iter(*aPlatform); !iter.AtEnd(); ++iter)
      {
         AddZoneSet(aSimTime, *iter);
      }
   }

   // Add networks, comms and routers
   if (GetPlatformDetailLevel(*aPlatform).IsOptionEnabled(mEventIds.cCOMM_INFO))
   {
      for (const auto& curNetworkName : GetSimulation().GetCommNetworkManager()->GetManagedNetworks())
      {
         auto curNetwork = GetSimulation().GetCommNetworkManager()->GetNetwork(curNetworkName);
         std::unique_ptr<WsfEventPipe::MsgNetworkInfo> msg = ut::make_unique<WsfEventPipe::MsgNetworkInfo>();
         msg->networkName(curNetworkName);
         msg->networkAddress(curNetwork->GetAddress().GetAddress());
         msg->networkType(curNetwork->GetBaseType());
         SendImmediate(aSimTime, aPlatform->IsExternallyControlled(), std::move(msg));
      }

      for (WsfComponentList::RoleIterator<wsf::comm::Comm> iter(*aPlatform); !iter.AtEnd(); ++iter)
      {
         auto                                       curComm = *iter;
         std::unique_ptr<WsfEventPipe::MsgCommInfo> msg     = ut::make_unique<WsfEventPipe::MsgCommInfo>();
         msg->commName(curComm->GetName());
         msg->commAddress(curComm->GetAddress().GetAddress());
         msg->parentPlatform(aPlatform->GetName());
         msg->networkName(curComm->GetNetwork());
         msg->networkAddress(curComm->GetInputNetworkAddress().GetAddress());
         msg->routerName(curComm->GetRouterName());
         msg->gatewayAddress(curComm->GetGateway().GetAddress());
         msg->gatewayCommName(curComm->GetGatewayRemoteComm());
         msg->gatewayPlatformName(curComm->GetGatewayRemotePlatform());

         wsf::comm::Comm::LinkPair curCommLinks = curComm->GetLinkPairs();
         for (const auto& curLink : curCommLinks)
         {
            WsfEventPipe::MsgCommInfo_link msgLink;
            msgLink.platformName(curLink.first);
            msgLink.commName(curLink.second);
            msg->links().push_back(msgLink);
         }

         for (const auto& curAddr : curComm->GetLinkAddresses())
         {
            // resolve the link addresses down to their platform and comm
            auto                           curComm = GetSimulation().GetCommNetworkManager()->GetComm(curAddr);
            WsfEventPipe::MsgCommInfo_link msgLink;
            msgLink.platformName(curComm->GetPlatform()->GetName());
            msgLink.commName(curComm->GetName());

            msg->links().push_back(msgLink);
            msg->linkAddresses().push_back(curAddr.GetAddress());
         }

         SendImmediate(aSimTime, aPlatform->IsExternallyControlled(), std::move(msg));

         auto curRouter = curComm->GetRouter();
         if (curRouter)
         {
            std::unique_ptr<WsfEventPipe::MsgRouterInfo> routerMsg = ut::make_unique<WsfEventPipe::MsgRouterInfo>();
            routerMsg->routerName(curRouter->GetName());
            routerMsg->commName(curComm->GetName());
            routerMsg->parentPlatform(aPlatform->GetName());
            routerMsg->routerType(curComm->GetRouter()->GetType());

            if (!curComm->GetRouter()->GetGateway().IsNull())
            {
               routerMsg->gatewayAddress(curRouter->GetGateway().GetAddress());
               routerMsg->gatewayCommName(
                  GetSimulation().GetCommNetworkManager()->GetComm(curComm->GetRouter()->GetGateway())->GetName());
            }

            bool routerUseMulticastProtocol = false;
            auto multicastPtr = curRouter->GetComponents().GetComponent<wsf::comm::router::ProtocolMulticast>();
            if (multicastPtr)
            {
               routerUseMulticastProtocol = true;
            }

            // Get use default protocol
            bool routerUseDefaultProtocol = false;
            auto legacyPtr = curRouter->GetComponents().GetComponent<wsf::comm::router::ProtocolLegacy>();
            if (legacyPtr)
            {
               routerUseDefaultProtocol = true;
            }

            unsigned int hopLim = ut::cast_to_int(curComm->GetRouter()->GetHopLimit());
            routerMsg->hopLimit(hopLim);
            routerMsg->useDefaultProtocol(routerUseDefaultProtocol);
            routerMsg->useMulticastProtocol(routerUseMulticastProtocol);
            SendImmediate(aSimTime, aPlatform->IsExternallyControlled(), std::move(routerMsg));
         }
      }
   }
}

void WsfEventPipeInterface::SendImmediate(double aSimTime, bool aExternal, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
{
   aMessagePtr->simTime(aSimTime);
   aMessagePtr->simIndex(aExternal ? 255 : 0);
   if (mWriteThreadPtr != nullptr)
   {
      mWriteThreadPtr->Push(std::move(aMessagePtr));
   }
   else if (mStreamPtr != nullptr)
   {
      mStreamPtr->Write(*aMessagePtr);
   }
}

void WsfEventPipeInterface::SendDependent(double                                 aSimTime,
                                          const WsfPlatform&                     aPlatform,
                                          std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
{
   if (aPlatform.IsInitialized())
   {
      SendImmediate(aSimTime, aPlatform.IsExternallyControlled(), std::move(aMessagePtr));
   }
   else
   {
      mCachedMessages[aPlatform.GetIndex()].emplace_back(std::move(aMessagePtr));
   }
}

void WsfEventPipeInterface::Pack(WsfEventPipe::Track_Id& aData, const WsfTrackId& aId)
{
   aData.localTrackNumber(aId.GetLocalTrackNumber());
   aData.owner(aId.GetOwningPlatformId());
}

void WsfEventPipeInterface::Pack(WsfEventPipe::Covariance& aData, const UtCovariance& aCovariance)
{
   aData.rowCount(aCovariance.GetRows());
   aData.colCount(aCovariance.GetCols());
   std::vector<float>& values = aData.values().GetVector();
   const UtMatrixd&    matrix = aCovariance;
   for (unsigned int y = 0; y < matrix.GetRows(); ++y)
   {
      for (unsigned int x = 0; x < matrix.GetCols(); ++x)
      {
         values.push_back((float)matrix(y, x));
      }
   }
}

void WsfEventPipeInterface::PackTrack(WsfEventPipe::Track& aTrackData, const WsfTrack& aTrack)
{
   aTrackData.targetIndex(Platform(aTrack.GetTargetIndex()));
   WsfEventPipeInterface::Pack(aTrackData.trackId(), aTrack.GetTrackId());
   aTrackData.startTime((float)aTrack.GetStartTime());
   aTrackData.updateTime((float)aTrack.GetUpdateTime());
   aTrackData.originatorIndex(Platform(aTrack.GetOriginatorIndex()));

   if (aTrack.GetSensorNameId() != 0)
   {
      aTrackData.sensorName(aTrack.GetSensorNameId());
   }
   if (aTrack.GetSensorModeId() != 0)
   {
      aTrackData.sensorMode(aTrack.GetSensorModeId());
   }
   aTrackData.updateCount(aTrack.GetUpdateCount());

   const WsfMeasurement& measurement   = aTrack;
   UtVec3d               originatorWCS = measurement.GetOriginatorLocationWCS();
   if (originatorWCS[0] || originatorWCS[1] || originatorWCS[2])
   {
      aTrackData.originatorWCS(toVec3d(originatorWCS));
   }

   if (aTrack.LocationValid())
   {
      if (aTrack.Is3D())
      {
         double locWCS[3];
         aTrack.GetLocationWCS(locWCS);
         aTrackData.locationWCS(toVec3d(locWCS));
      }
   }
   if (aTrack.VelocityValid())
   {
      double velWCS[3];
      aTrack.GetVelocityWCS(velWCS);
      aTrackData.velocityWCS(toVec3f(velWCS));
   }

   if (aTrack.ElevationValid())
   {
      aTrackData.elevation((float)aTrack.GetElevation());
   }
   if (aTrack.GetElevationError() != 0)
   {
      aTrackData.elevationError((float)aTrack.GetElevationError());
   }

   if (aTrack.BearingValid())
   {
      aTrackData.bearing((float)aTrack.GetBearing());
   }
   if (aTrack.GetBearingError() != 0)
   {
      aTrackData.bearingError((float)aTrack.GetBearingError());
   }

   if (aTrack.RangeValid())
   {
      aTrackData.range((float)aTrack.GetRange());
   }
   if (aTrack.GetRangeError() != 0)
   {
      aTrackData.rangeError((float)aTrack.GetRangeError());
   }

   if (aTrack.RangeRateValid())
   {
      aTrackData.rangeRate((float)aTrack.GetRangeRate());
   }
   if (aTrack.GetRangeRateError() != 0)
   {
      aTrackData.rangeRateError((float)aTrack.GetRangeRateError());
   }

   if (aTrack.GetTrackQuality() != 0.5)
   {
      aTrackData.trackQuality((float)aTrack.GetTrackQuality());
   }
   if (aTrack.SignalToNoiseValid())
   {
      aTrackData.signalToNoise((float)aTrack.GetSignalToNoise());
   }

   if (aTrack.GetPixelCount())
   {
      aTrackData.pixelCount((float)aTrack.GetPixelCount());
   }

   if (aTrack.SideIdValid())
   {
      aTrackData.side((std::string)aTrack.GetSideId());
   }
   if (aTrack.TypeIdValid())
   {
      aTrackData.type((std::string)aTrack.GetTypeId());
   }

   WsfSpatialDomain spatialDomain = aTrack.GetSpatialDomain();
   if (spatialDomain != WSF_SPATIAL_DOMAIN_UNKNOWN)
   {
      aTrackData.spatialDomain(spatialDomain);
   }

   if (aTrack.GetSimulation() != nullptr)
   {
      auto plat = aTrack.GetSimulation()->GetPlatformByIndex(Platform(aTrack.GetTargetIndex()));
      if (plat && !plat->GetIcon().empty())
      {
         aTrackData.icon(plat->GetIcon());
         aTrackData.iconValid(true);
      }
   }
   WsfTrack::IFF_Status iff = aTrack.GetIFF_Status();
   if (iff != WsfTrack::IFF_Status::cIFF_UNKNOWN)
   {
      aTrackData.IFF(iff);
   }
   const WsfCovariance* covPtr = aTrack.GetStateCovariance();
   if (covPtr != nullptr)
   {
      WsfEventPipeInterface::Pack(aTrackData.stateCovariance(), *covPtr);
      aTrackData.stateCovarianceValid(true);
   }
}

void WsfEventPipeInterface::LocalTrackUpdated(double               aSimTime,
                                              WsfPlatform*         aPlatformPtr,
                                              const WsfLocalTrack* aTrackPtr,
                                              const WsfTrack*      aSourcePtr)
{
   if (aPlatformPtr != nullptr)
   {
      if (!GetPlatformDetailLevel(*aPlatformPtr).IsOptionEnabled(mEventIds.cTRACK_UPDATE))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgLocalTrackUpdate>();
      msgPtr->ownerIndex(Platform(*aPlatformPtr));
      PackTrack(msgPtr->track(), *aTrackPtr);

      msgPtr->masterProcessor(aTrackPtr->GetTrackManager() == &aPlatformPtr->GetTrackManager());

      SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }
}

void WsfEventPipeInterface::LocalTrackInitiated(double               aSimTime,
                                                WsfPlatform*         aPlatformPtr,
                                                const WsfLocalTrack* aTrackPtr,
                                                const WsfTrack*      aSourcePtr)
{
   if (aPlatformPtr != nullptr)
   {
      if (!GetPlatformDetailLevel(*aPlatformPtr).IsOptionEnabled(mEventIds.cTRACK))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgLocalTrackCreated>();
      msgPtr->ownerIndex(Platform(*aPlatformPtr));
      Pack(msgPtr->trackId(), aTrackPtr->GetTrackId());
      msgPtr->targetIndex(Platform(aTrackPtr->GetTargetIndex()));
      msgPtr->masterProcessor(aTrackPtr->GetTrackManager() == &aPlatformPtr->GetTrackManager());
      SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }

   LocalTrackUpdated(aSimTime, aPlatformPtr, aTrackPtr, aSourcePtr);
}

void WsfEventPipeInterface::LocalTrackDropped(double aSimTime, WsfPlatform* aPlatformPtr, const WsfLocalTrack* aTrackPtr)
{
   if (aPlatformPtr != nullptr)
   {
      if (!GetPlatformDetailLevel(*aPlatformPtr).IsOptionEnabled(mEventIds.cTRACK))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgLocalTrackDrop>();
      msgPtr->ownerIndex(Platform(*aPlatformPtr));
      msgPtr->targetIndex(Platform(aTrackPtr->GetTargetIndex()));
      Pack(msgPtr->trackId(), aTrackPtr->GetTrackId());
      msgPtr->masterProcessor(aTrackPtr->GetTrackManager() == &aPlatformPtr->GetTrackManager());
      SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }
}


void WsfEventPipeInterface::LocalTrackCorrelation(double            aSimTime,
                                                  WsfPlatform*      aPlatformPtr,
                                                  const WsfTrackId& aLocalTrackId,
                                                  const WsfTrackId& aNonLocalTrackId)
{
   if (aPlatformPtr != nullptr)
   {
      if (!GetPlatformDetailLevel(*aPlatformPtr).IsOptionEnabled(mEventIds.cTRACK))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgLocalTrackCorrelation>();
      msgPtr->ownerIndex(Platform(*aPlatformPtr));
      Pack(msgPtr->trackId(), aLocalTrackId);
      Pack(msgPtr->addTrackId(), aNonLocalTrackId);
      SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }
}

void WsfEventPipeInterface::LocalTrackDecorrelation(double            aSimTime,
                                                    WsfPlatform*      aPlatformPtr,
                                                    const WsfTrackId& aLocalTrackId,
                                                    const WsfTrackId& aNonLocalTrackId)
{
   if (aPlatformPtr != nullptr)
   {
      if (!GetPlatformDetailLevel(*aPlatformPtr).IsOptionEnabled(mEventIds.cTRACK))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgLocalTrackDecorrelation>();
      msgPtr->ownerIndex(Platform(*aPlatformPtr));
      Pack(msgPtr->trackId(), aLocalTrackId);
      Pack(msgPtr->remTrackId(), aNonLocalTrackId);
      SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }
}

void WsfEventPipeInterface::SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   if (aSensorPtr->GetPlatform() != nullptr)
   {
      if (!GetPlatformDetailLevel(*aSensorPtr->GetPlatform()).IsOptionEnabled(mEventIds.cTRACK))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorTrackCreated>();
      msgPtr->ownerIndex(Platform(*aSensorPtr->GetPlatform()));
      Pack(msgPtr->trackId(), aTrackPtr->GetTrackId());
      msgPtr->targetIndex(Platform(aTrackPtr->GetTargetIndex()));
      msgPtr->sensorName(aSensorPtr->GetName());
      SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
   }

   SensorTrackUpdated(aSimTime, aSensorPtr, aTrackPtr);
}

void WsfEventPipeInterface::SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   if (aSensorPtr->GetPlatform() != nullptr)
   {
      if (!GetPlatformDetailLevel(*aSensorPtr->GetPlatform()).IsOptionEnabled(mEventIds.cTRACK_UPDATE))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorTrackUpdate>();
      msgPtr->ownerIndex(Platform(*aSensorPtr->GetPlatform()));
      PackTrack(msgPtr->track(), *aTrackPtr);
      SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
   }
}

void WsfEventPipeInterface::SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   if (aSensorPtr->GetPlatform() != nullptr)
   {
      if (!GetPlatformDetailLevel(*aSensorPtr->GetPlatform()).IsOptionEnabled(mEventIds.cTRACK))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorTrackDrop>();
      msgPtr->ownerIndex(Platform(*aSensorPtr->GetPlatform()));
      msgPtr->targetIndex(Platform(aTrackPtr->GetTargetIndex()));
      Pack(msgPtr->trackId(), aTrackPtr->GetTrackId());
      msgPtr->sensorName(aSensorPtr->GetName());
      SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
   }
}

void WsfEventPipeInterface::SendModeActive(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, bool aActive)
{
   if (aActive)
   {
      SensorModeIdentifier smi(aSensorPtr->GetUniqueId(), aModePtr->GetName());
      if (mModeSet.find(smi) == mModeSet.end())
      {
         // if this is the first time we have encountered this mode, send a mode definition
         PublishMode(aSimTime, aSensorPtr, aModePtr);
      }
   }
   // Limit scope of msgPtr.
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorModeChange>();
      msgPtr->platformIndex(Platform(*aSensorPtr->GetPlatform()));
      msgPtr->sensorName(aSensorPtr->GetName());
      msgPtr->modeName(aModePtr->GetName());
      msgPtr->activate(aActive);
      SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
   }

   auto it = mPartUpdateEvents.find(aSensorPtr->GetUniqueId());
   if (it != mPartUpdateEvents.end())
   {
      it->second->DeleteLater();
   }
   double frameTime = aModePtr->GetFrameTime();
   if (frameTime <= 0)
   {
      frameTime = 5; // this is arbitrary, but I don't think it should ever happen
   }
   auto eventPtr = ut::make_unique<wsf::eventpipe::PartUpdateEvent>(this, aSensorPtr, aSimTime, frameTime);
   mPartUpdateEvents[aSensorPtr->GetUniqueId()] = eventPtr.get();
   GetSimulation().AddEvent(std::move(eventPtr));
}

void WsfEventPipeInterface::SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   SendModeActive(aSimTime, aSensorPtr, aModePtr, true);
}

void WsfEventPipeInterface::SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   SendModeActive(aSimTime, aSensorPtr, aModePtr, false);
}

void WsfEventPipeInterface::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr != nullptr)
   {
      auto platIdx = Platform(*aPlatformPtr);

      auto state = mEntityStateData.find(platIdx);
      if ((state == mEntityStateData.end()) ||
          (state->second.Time() != aSimTime)) // if we haven't already recorded the current entity state
      {
         SendEntityState(aSimTime, *aPlatformPtr, true);
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgPlatformStatus>();
      msgPtr->platformIndex(platIdx);
      msgPtr->removed(true);
      msgPtr->broken(aPlatformPtr->IsBroken() || aPlatformPtr->GetDamageFactor() >= 1.0);
      SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }
}

void WsfEventPipeInterface::PlatformBroken(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // In the case of a normal platform removal, we don't want to emit this event (it gets emitted in PlatformDeleted())
   if ((aPlatformPtr != nullptr) && (aPlatformPtr->IsBroken()))
   {
      auto platIdx = Platform(*aPlatformPtr);

      auto state = mEntityStateData.find(platIdx);
      if ((state == mEntityStateData.end()) ||
          (state->second.Time() != aSimTime)) // if we haven't already recorded the current entity state
      {
         SendEntityState(aSimTime, *aPlatformPtr, true);
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgPlatformStatus>();
      msgPtr->platformIndex(Platform(*aPlatformPtr));
      msgPtr->broken(true);
      SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }
}

void WsfEventPipeInterface::SendPartStatus(double aSimTime, WsfPlatformPart* aPartPtr, int aType)
{
   if (aPartPtr->IsTurnedOn())
   {
      WsfSensor* sensorPtr = dynamic_cast<WsfSensor*>(aPartPtr);
      if (nullptr != sensorPtr)
      {
         WsfSensorMode* modePtr = sensorPtr->GetCurrentMode();

         SensorModeIdentifier smi(sensorPtr->GetUniqueId(), modePtr->GetName());
         if (mModeSet.find(smi) == mModeSet.end())
         {
            SendModeActive(aSimTime, sensorPtr, modePtr, true);
            // if this is the first time we have encountered this mode, send a mode definition, and mode change
         }
      }
   }
   if (aPartPtr->GetPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgPartStatus>();
      msgPtr->platformIndex(Platform(*aPartPtr->GetPlatform()));
      msgPtr->partName(aPartPtr->GetName());
      msgPtr->partType(aType);
      msgPtr->on(aPartPtr->IsTurnedOn());
      msgPtr->disabled(!aPartPtr->IsOperational());
      msgPtr->broken(aPartPtr->IsBroken());
      SendDependent(aSimTime, *aPartPtr->GetPlatform(), std::move(msgPtr));
   }
}

void WsfEventPipeInterface::SendVisualPartDefinition(double aSimTime, WsfVisualPart* aPartPtr, int aType)
{
   if (aPartPtr->GetPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgVisualPartDefinition>();
      msgPtr->ownerIndex(Platform(*aPartPtr->GetPlatform()));
      msgPtr->partName(aPartPtr->GetName());
      msgPtr->partType(aType);
      msgPtr->partIcon(aPartPtr->GetIcon());
      SendDependent(aSimTime, *aPartPtr->GetPlatform(), std::move(msgPtr));
   }
}

void WsfEventPipeInterface::SimulationStarting()
{
   int year;
   int month;
   int day;

   GetSimulation().GetDateTime().GetStartDate(year, month, day);
   // Limit scope of msgPtr.
   {
      double time   = GetSimulation().GetDateTime().GetStartTime();
      auto   msgPtr = ut::make_unique<WsfEventPipe::MsgSetDate>();
      msgPtr->time(static_cast<float>(time));
      msgPtr->year(year);
      msgPtr->month(month);
      msgPtr->day(day);
      SendImmediate(GetSimulation().GetSimTime(), false, std::move(msgPtr));
   }
   wsf::TerrainInterface* ti = GetSimulation().GetTerrainInterface();
   if (ti)
   {
      // convert to absolute path if the provided terrain path is relative
      auto ResolvePath = [](const std::string& aDirName)
      {
         UtPath path = UtPath(aDirName);
         return path.GetRealPath().GetNormalizedPath();
      };

      const std::vector<wsf::TerrainInterface::DtedInput>& inputs = ti->GetDTEDInputs();
      if (!inputs.empty())
      {
         // Limit scope of resMsgPtr
         {
            auto resMsgPtr = ut::make_unique<WsfEventPipe::MsgResource>();
            resMsgPtr->resourceId("");
            resMsgPtr->resourceType(WsfEventPipe::MsgResource_ResourceType::dted);
            for (auto&& input : inputs)
            {
               if (!input.mRaw)
               {
                  resMsgPtr->resourceLocation().push_back(std::to_string(input.mLevel) + " \"" +
                                                          ResolvePath(input.mDirName) + "\"");
               }
            }
            SendImmediate(GetSimulation().GetSimTime(), false, std::move(resMsgPtr));
         }

         // Limit scope of resMsgPtr
         {
            auto resMsgPtr = ut::make_unique<WsfEventPipe::MsgResource>();
            resMsgPtr->resourceId("");
            resMsgPtr->resourceType(WsfEventPipe::MsgResource_ResourceType::raw_dted);
            for (auto&& input : inputs)
            {
               if (input.mRaw)
               {
                  std::stringstream ss;
                  // clang-format off
                  ss << input.mLevel << " "
                     << input.mSWLat << " "
                     << input.mSWLon << " "
                     << input.mNELat << " "
                     << input.mNELon << " \""
                     << ResolvePath(input.mDirName) << "\"";
                  // clang-format on
                  resMsgPtr->resourceLocation().push_back(ss.str());
               }
            }
            SendImmediate(GetSimulation().GetSimTime(), false, std::move(resMsgPtr));
         }
      }
      auto& gtinputs = ti->GetGeotiffInputs();
      if (!gtinputs.empty())
      {
         auto resMsgPtr = ut::make_unique<WsfEventPipe::MsgResource>();
         resMsgPtr->resourceId("");
         resMsgPtr->resourceType(WsfEventPipe::MsgResource_ResourceType::geotiff_dem);
         for (auto&& input : gtinputs)
         {
            resMsgPtr->resourceLocation().push_back(ResolvePath(input.mDirName));
         }
         SendImmediate(GetSimulation().GetSimTime(), false, std::move(resMsgPtr));
      }
   }
   // query the dted resources

   // Send Global Zone Information. Platform dependent zones should be sent when the platforms are added
   if (mCombinedOptions.IsOptionEnabled(mEventIds.cZONES))
   {
      std::vector<WsfZone*> globalZoneData;
      WsfZoneTypes&         globalZones = GetSimulation().GetScenario().GetZoneTypes();
      globalZones.GetCurrentTypes(globalZoneData);
      for (const auto& zonePtr : globalZoneData)
      {
         AddZoneSet(GetSimulation().GetSimTime(), zonePtr);
      }
   }
}

void WsfEventPipeInterface::SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr)
{
   SendPartStatus(aSimTime, aSensorPtr, 0);

   WsfSensorMode* mode = aSensorPtr->GetCurrentMode();
   if (mode)
   {
      auto it = mPartUpdateEvents.find(aSensorPtr->GetUniqueId());
      if (it != mPartUpdateEvents.end())
      {
         it->second->DeleteLater();
      }
      double frameTime = aSensorPtr->GetCurrentMode()->GetFrameTime();
      if (frameTime <= 0)
      {
         frameTime = 5; // this is arbitrary, but I don't think it should ever happen
      }
      auto eventPtr = ut::make_unique<wsf::eventpipe::PartUpdateEvent>(this, aSensorPtr, aSimTime, frameTime);
      mPartUpdateEvents[aSensorPtr->GetUniqueId()] = eventPtr.get();
      GetSimulation().AddEvent(std::move(eventPtr));
   }
}

void WsfEventPipeInterface::SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr)
{
#ifdef DETECTION_CHANGE_WORKAROUND_NECESSARY
   auto it = mDetectionList.find(std::make_pair(aSensorPtr->GetPlatform()->GetIndex(), aSensorPtr->GetName()));
   if (it != mDetectionList.end())
   {
      for (const auto& detection : it->second)
      {
         for (int i = 0; i < detection.second; ++i)
         {
            if (aSensorPtr->GetPlatform() != nullptr)
            {
               auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorDetectionChange>();
               msgPtr->ownerIndex(Platform(*aSensorPtr->GetPlatform()));
               msgPtr->sensorName(aSensorPtr->GetName());
               msgPtr->targetIndex(Platform(detection.first));
               msgPtr->detected(false);
               SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
            }
         }
      }
      it->second.clear();
   }
#endif
   SendPartStatus(aSimTime, aSensorPtr, 0);

   auto kt = mPartUpdateEvents.find(aSensorPtr->GetUniqueId());
   if (kt != mPartUpdateEvents.end())
   {
      kt->second->DeleteLater();
      mPartUpdateEvents.erase(kt);
   }
}

void WsfEventPipeInterface::VisualPartTurnedOn(double aSimTime, WsfVisualPart* aPartPtr)
{
   // send a visual part definition
   SendVisualPartDefinition(aSimTime, aPartPtr, 5);
   // send a part status
   SendPartStatus(aSimTime, aPartPtr, 5);
   // send an articulation update
   UpdateArticulation(aSimTime, aPartPtr, 5);

   double updateInterval = mCombinedOptions.GetVisualPartUpdateInterval();
   if (updateInterval > 0.0)
   {
      auto it = mPartUpdateEvents.find(aPartPtr->GetUniqueId());
      if (it != mPartUpdateEvents.end())
      {
         it->second->DeleteLater();
      }
      auto eventPtr = ut::make_unique<wsf::eventpipe::PartUpdateEvent>(this, aPartPtr, aSimTime, updateInterval);
      mPartUpdateEvents[aPartPtr->GetUniqueId()] = eventPtr.get();
      GetSimulation().AddEvent(std::move(eventPtr));
   }
}

void WsfEventPipeInterface::VisualPartTurnedOff(double aSimTime, WsfVisualPart* aPartPtr)
{
   // send a part status
   SendPartStatus(aSimTime, aPartPtr, 5);

   auto kt = mPartUpdateEvents.find(aPartPtr->GetUniqueId());
   if (kt != mPartUpdateEvents.end())
   {
      kt->second->DeleteLater();
      mPartUpdateEvents.erase(kt);
   }
}

void WsfEventPipeInterface::SendExecData()
{
   // send out MsgExecData message
   auto   msgPtr = ut::make_unique<WsfEventPipe::MsgExecData>();
   int    argc;
   char** argv;
   GetSimulation().GetApplication().GetCommandLineArguments(argc, argv);
   std::string cmdLine;
   if (argc > 0)
   {
      cmdLine += argv[0];
      for (int i = 1; i < argc; ++i)
      {
         cmdLine += " ";
         cmdLine += argv[i];
      }
   }
   msgPtr->cmdLine(cmdLine);
   const std::vector<WsfApplication::Feature>& feats = GetSimulation().GetApplication().GetRegisteredFeatures();
   std::string                                 featstr;
   for (auto&& it : feats)
   {
      featstr += it.mName + "|";
   }
   msgPtr->features(featstr);
   const WsfExtensionList& exts = GetSimulation().GetApplication().GetExtensions();
   std::string             extstr;
   for (auto&& it : exts)
   {
      extstr += it.first + "|";
   }
   msgPtr->extensions(extstr);
   // Note: It should be safe to assume the working directory will be correct here, and has not changed
   msgPtr->execPath(UtPath::WorkingDirectory().GetNormalizedPath());
   msgPtr->version(GetSimulation().GetApplication().GetApplicationName() + " " + wsf::version::GetString());
   time_t     t = time(nullptr);
   char       timeStr[32];
   struct tm* localTimePtr = localtime(&t);
   strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTimePtr);
   msgPtr->timeStamp(timeStr);

   // Set Name, DIS Application, and Run Number.
   msgPtr->simName(GetSimulation().GetScenario().GetSimulationName());
   msgPtr->runNumber(GetSimulation().GetRunNumber());

   WsfDisInterface* dis = WsfDisInterface::Find(GetSimulation());
   if (dis != nullptr)
   {
      msgPtr->disSite(dis->GetExtInterface()->GetSiteId());
      msgPtr->disApplication(dis->GetExtInterface()->GetApplicationId());
   }

   SendImmediate(0.0, false, std::move(msgPtr));

   if (mFileStreamPtr)
   {
      mFileStreamPtr->flush();
   }
   else
   {
      mStreamPtr->Flush();
   }
}

void WsfEventPipeInterface::SendScenarioData()
{
   // send out MsgScenarioData message
   auto msgPtr = ut::make_unique<WsfEventPipe::MsgScenarioData>();

   msgPtr->classification(GetScenario().GetClassificationString());
   auto& utColor = GetScenario().GetClassificationColor();

   float redF, blueF, greenF;
   utColor.Get(redF, greenF, blueF);
   WsfEventPipe::Vec3f color;
   color.x(redF);
   color.y(greenF);
   color.z(blueF);

   msgPtr->classificationColor(color);

   SendImmediate(0.0, false, std::move(msgPtr));
}


void WsfEventPipeInterface::UpdatePartArticulation(double aSimTime, size_t aPlatformId, WsfStringId& aPartNameId)
{
   WsfPlatform* platform = GetSimulation().GetPlatformByIndex(aPlatformId);
   if (platform)
   {
      auto sensorPtr = platform->GetComponent<WsfSensor>(aPartNameId);
      if (sensorPtr)
      {
         if (sensorPtr->IsTurnedOn())
         {
            double az = 0.0;
            double el = 0.0;
            if (sensorPtr->GetEM_XmtrCount() != 0)
            {
               if ((sensorPtr->GetSlewMode() != WsfArticulatedPart::cSLEW_FIXED) && (!sensorPtr->IsCued()) &&
                   (!sensorPtr->TransientCueActive())) // why is there data here if it should only be used conditionally?
               {
                  sensorPtr->GetCurrentCuedOrientation(az, el);
               }
            }
            UpdateArticulation(aSimTime, sensorPtr, WsfEventPipe::PartType::sensor, az, el);
         }
      }
      else
      {
         auto partPtr = platform->GetComponent<WsfArticulatedPart>(aPartNameId);
         if (partPtr != nullptr)
         {
            if (partPtr->IsTurnedOn())
            {
               UpdateArticulation(aSimTime, partPtr, WsfEventPipe::PartType::visual);
            }
         }
      }
   }
}

void WsfEventPipeInterface::AdvancedBehaviorTree(double aSimTime, WsfAdvancedBehaviorTree* aTreePtr)
{
   if (aTreePtr->GetOwningPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgBehaviorTree>();
      msgPtr->platformIndex(static_cast<unsigned int>(aTreePtr->GetOwningPlatform()->GetIndex()));
      msgPtr->simTime(static_cast<float>(aSimTime));
      msgPtr->id(aTreePtr->GetTreeId());
      msgPtr->treeName(aTreePtr->GetName());
      WsfEventPipe::BehaviorTreeNodeList nodeList;
      PopulateAdvancedBehaviorTreeNodeList(nodeList, aTreePtr->RootNode().get());

      msgPtr->nodes(nodeList);
      SendDependent(aSimTime, *aTreePtr->GetOwningPlatform(), std::move(msgPtr));
   }
}

void WsfEventPipeInterface::PopulateAdvancedBehaviorTreeNodeList(WsfEventPipe::BehaviorTreeNodeList& aList,
                                                                 WsfAdvancedBehaviorTreeNode*        aNodePtr) const
{
   if (aNodePtr != nullptr)
   {
      WsfEventPipe::BehaviorTreeNode node;
      node.filePath(aNodePtr->GetFilePath());
      node.fileModTime(aNodePtr->GetFileLastModTime());
      node.nodeId(aNodePtr->Id());
      node.treeId(aNodePtr->GetOwningTree()->GetTreeId());
      node.nodeType(aNodePtr->GetType());
      if (aNodePtr->HasDefinedColor())
      {
         node.platformHistoryColor(UtColorToColorF(aNodePtr->GetColor()));
      }
      node.nodeName(aNodePtr->GetName());
      node.nodeDesc(aNodePtr->GetDescription());
      node.isRootNode(aNodePtr->GetIsTreeRootNode());

      if (aNodePtr->ChildStateMachine() != nullptr)
      {
         node.childFsmId(static_cast<unsigned int>(aNodePtr->ChildStateMachine()->Id()));
      }

      WsfEventPipe::IntList intList;
      if (aNodePtr->HasChildrenContainer())
      {
         node.numChildren(static_cast<unsigned int>(aNodePtr->GetChildren().size()));

         for (auto& child : aNodePtr->GetChildren())
         {
            PopulateAdvancedBehaviorTreeNodeList(aList, child.get());
            intList.push_back(child->Id());
         }
      }
      else
      {
         node.numChildren(0);
      }
      node.childrenIds(intList);

      aList.push_back(node);
   }
}

void WsfEventPipeInterface::AdvancedBehaviorTreeState(double aSimTime, WsfAdvancedBehaviorTreeNode* aNodePtr)
{
   if (aNodePtr->GetOwningPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgBehaviorTreeState>();
      msgPtr->simTime(static_cast<float>(aSimTime));
      msgPtr->platformIndex(static_cast<unsigned int>(aNodePtr->GetOwningPlatform()->GetIndex()));
      msgPtr->treeId(static_cast<unsigned int>(aNodePtr->GetOwningTree()->GetTreeId()));

      // The node being passed to AdvancedBehaviorTreeState is always the root node.
      // Loop recursively and add every node's exec state to the list.  Then add this list
      // to the message, and send it.
      WsfEventPipe::BehaviorTreeNodeExecList       execList;
      WsfEventPipe::BehaviorTreeBlackboardDataList blackboardList;
      WsfEventPipe::StringList                     behaviorHistoryList;

      // Only populate this data for the main tree
      if (aNodePtr->GetIsTreeRootNode() && aNodePtr->GetParent() == nullptr)
      {
         std::vector<WsfAdvancedBehaviorTreeNode*> lastExecuted;
         aNodePtr->GetOwningTree()->FindLastExecuted(lastExecuted);
         for (const auto& node : lastExecuted)
         {
            behaviorHistoryList.push_back(node->GetName());
         }
         msgPtr->behaviorHistoryList(behaviorHistoryList);
      }

      // Get the node's exec state and blackboard state data
      PopulateAdvancedBehaviorTreeStateLists(execList, blackboardList, aNodePtr);

      // Get the shared blackboards from the tree
      for (const auto& blackboardName : aNodePtr->GetOwningTree()->GetSharedBlackboardNames())
      {
         auto                                     bb = aNodePtr->GetOwningTree()->GetSharedBlackboard(blackboardName);
         WsfEventPipe::BehaviorTreeBlackboardData sharedBlackboard;
         sharedBlackboard.nodeId(0);
         sharedBlackboard.sharedBlackboard(blackboardName);

         // Dictionary containers to get sent over
         // Primitives
         WsfEventPipe::BoolDict   boolDict;
         WsfEventPipe::IntDict    intDict;
         WsfEventPipe::StringDict stringDict;
         WsfEventPipe::DoubleDict doubleDict;

         // Other types
         WsfEventPipe::TrackDict    trackDict;
         WsfEventPipe::PlatformDict platformDict;
         // Add more types here... (they need matching utpack containers)

         // If sharedBlackboard has entries, fill them in.
         if (bb->HasAttributes())
         {
            // Loop through each entry and place it in the correct type container before sending the message
            for (const auto& entry : aNodePtr->GetOwningTree()->GetSharedBlackboard(blackboardName)->GetAttributeMap())
            {
               if (entry.second->GetType() == "bool")
               {
                  WsfEventPipe::BoolDictEntry newEntry = WsfEventPipe::BoolDictEntry();
                  newEntry.key(entry.first);
                  newEntry.value(entry.second->GetBool());
                  boolDict.push_back(newEntry);
               }
               else if (entry.second->GetType() == "int")
               {
                  WsfEventPipe::IntDictEntry newEntry = WsfEventPipe::IntDictEntry();
                  newEntry.key(entry.first);
                  newEntry.value(entry.second->GetInt());
                  intDict.push_back(newEntry);
               }
               else if (entry.second->GetType() == "string")
               {
                  WsfEventPipe::StringDictEntry newEntry = WsfEventPipe::StringDictEntry();
                  newEntry.key(entry.first);
                  newEntry.value(entry.second->GetString());
                  stringDict.push_back(newEntry);
               }
               else if (entry.second->GetType() == "double")
               {
                  WsfEventPipe::DoubleDictEntry newEntry = WsfEventPipe::DoubleDictEntry();
                  newEntry.key(entry.first);
                  newEntry.value(entry.second->GetDouble());
                  doubleDict.push_back(newEntry);
               }
               else
               {
                  // Types we check for
                  WsfTrack*    t = nullptr;
                  WsfPlatform* p = nullptr;

                  // If TryGet returns true the pointer will not be nullptr
                  if (entry.second->TryGet<WsfTrack*>(t))
                  {
                     // Validity check in PackTrack crashes attempting to access mTypeIds[0] when it doesn't exist?
                     if (!t->GetTypeIds().empty())
                     {
                        WsfEventPipe::TrackDictEntry newEntry = WsfEventPipe::TrackDictEntry();
                        WsfEventPipe::Track          newTrack;
                        WsfEventPipeInterface::PackTrack(newTrack, *t);
                        newEntry.key(entry.first);
                        newEntry.value(newTrack);
                        trackDict.push_back(newEntry);
                     }
                  }
                  else if (entry.second->TryGet<WsfPlatform*>(p))
                  {
                     WsfEventPipe::BehaviorTreePlatformInfo platformInfo;
                     platformInfo.name(p->GetName());
                     platformInfo.side(p->GetSide());

                     std::vector<WsfStringId> typeList = p->GetTypeList();
                     for (auto type : typeList)
                     {
                        platformInfo.types().push_back(type.GetString());
                        platformInfo.typesValid(true);
                     }

                     WsfEventPipe::PlatformDictEntry newEntry;
                     newEntry.key(entry.first);
                     newEntry.value(platformInfo);
                     platformDict.push_back(newEntry);
                     // Fill platform info here...
                  }
                  // Check more pointer types here...
               }
            }
            sharedBlackboard.boolDict(boolDict);
            sharedBlackboard.intDict(intDict);
            sharedBlackboard.stringDict(stringDict);
            sharedBlackboard.doubleDict(doubleDict);
            sharedBlackboard.trackDict(trackDict);
            sharedBlackboard.platformDict(platformDict);

            // Add the shared blackboard to the list.
            blackboardList.push_back(sharedBlackboard);
         }
      }

      // Set our lists.  We are ready to send the message at this point.
      msgPtr->execList(execList);
      msgPtr->blackboardList(blackboardList);
      SendDependent(aSimTime, *aNodePtr->GetOwningPlatform(), std::move(msgPtr));
   }
}

void WsfEventPipeInterface::PopulateAdvancedBehaviorTreeStateLists(WsfEventPipe::BehaviorTreeNodeExecList& aExecList,
                                                                   WsfEventPipe::BehaviorTreeBlackboardDataList& aBlackboardList,
                                                                   WsfAdvancedBehaviorTreeNode* aNodePtr) const
{
   // Add this node's state exec state data
   WsfEventPipe::BehaviorTreeNodeExec nodeExec;
   nodeExec.execState(aNodePtr->GetNodeStatus());
   nodeExec.nodeId(aNodePtr->Id());
   nodeExec.nodeName(aNodePtr->GetNodeName());
   nodeExec.preconditionTooltip(aNodePtr->GetPreconditionTooltip());
   nodeExec.executeTooltip(aNodePtr->GetExecuteTooltip());
   aExecList.push_back(nodeExec);

   // Add this node's blackboard state data
   WsfEventPipe::BehaviorTreeBlackboardData blackboard;
   blackboard.nodeId(aNodePtr->Id());

   // Dictionary containers to get sent over
   // Primitives
   WsfEventPipe::BoolDict   boolDict;
   WsfEventPipe::IntDict    intDict;
   WsfEventPipe::StringDict stringDict;
   WsfEventPipe::DoubleDict doubleDict;

   // Other types
   WsfEventPipe::TrackDict    trackDict;
   WsfEventPipe::PlatformDict platformDict;
   // Add more types here... (they need matching utpack containers)

   // If we have blackboard attributes to send
   if (aNodePtr->GetBlackboard()->HasAttributes())
   {
      for (const auto& entry : aNodePtr->GetBlackboard()->GetAttributeMap())
      {
         if (entry.second->GetType() == "bool")
         {
            WsfEventPipe::BoolDictEntry newEntry = WsfEventPipe::BoolDictEntry();
            newEntry.key(entry.first);
            newEntry.value(entry.second->GetBool());
            boolDict.push_back(newEntry);
         }
         else if (entry.second->GetType() == "int")
         {
            WsfEventPipe::IntDictEntry newEntry = WsfEventPipe::IntDictEntry();
            newEntry.key(entry.first);
            newEntry.value(entry.second->GetInt());
            intDict.push_back(newEntry);
         }
         else if (entry.second->GetType() == "string")
         {
            WsfEventPipe::StringDictEntry newEntry = WsfEventPipe::StringDictEntry();
            newEntry.key(entry.first);
            newEntry.value(entry.second->GetString());
            stringDict.push_back(newEntry);
         }
         else if (entry.second->GetType() == "double")
         {
            WsfEventPipe::DoubleDictEntry newEntry = WsfEventPipe::DoubleDictEntry();
            newEntry.key(entry.first);
            newEntry.value(entry.second->GetDouble());
            doubleDict.push_back(newEntry);
         }
         else
         {
            // Types we check for
            WsfTrack*    t = nullptr;
            WsfPlatform* p = nullptr;

            // If TryGet returns true the pointer will not be nullptr
            if (entry.second->TryGet<WsfTrack*>(t))
            {
               // Validity check in PackTrack crashes attempting to access mTypeIds[0] when it doesn't exist?
               if (!t->GetTypeIds().empty())
               {
                  WsfEventPipe::TrackDictEntry newEntry;
                  WsfEventPipe::Track          newTrack;
                  WsfEventPipeInterface::PackTrack(newTrack, *t);
                  newEntry.key(entry.first);
                  newEntry.value(newTrack);
                  trackDict.push_back(newEntry);
               }
            }
            else if (entry.second->TryGet<WsfPlatform*>(p))
            {
               WsfEventPipe::BehaviorTreePlatformInfo platformInfo;
               platformInfo.name(p->GetName());
               platformInfo.side(p->GetSide());

               std::vector<WsfStringId> typeList = p->GetTypeList();
               for (auto type : typeList)
               {
                  platformInfo.types().push_back(type.GetString());
                  platformInfo.typesValid(true);
               }

               WsfEventPipe::PlatformDictEntry newEntry;
               newEntry.key(entry.first);
               newEntry.value(platformInfo);
               platformDict.push_back(newEntry);
               // Fill platform info here...
            }
            // Check more pointer types here...
         }
      }
      blackboard.boolDict(boolDict);
      blackboard.intDict(intDict);
      blackboard.stringDict(stringDict);
      blackboard.doubleDict(doubleDict);
      blackboard.trackDict(trackDict);
      blackboard.platformDict(platformDict);

      aBlackboardList.push_back(blackboard);
   }

   // Recursively add for children as well
   if (aNodePtr->HasChildrenContainer())
   {
      for (auto& child : aNodePtr->GetChildren())
      {
         PopulateAdvancedBehaviorTreeStateLists(aExecList, aBlackboardList, child.get());
      }
   }
}

void WsfEventPipeInterface::StateMachine(double aSimTime, WsfScriptStateMachine* aStateMachinePtr)
{
   if (aStateMachinePtr->Processor() != nullptr)
   {
      auto platformPtr = aStateMachinePtr->Processor()->GetPlatform();
      if (platformPtr != nullptr)
      {
         auto msgPtr = ut::make_unique<WsfEventPipe::MsgStateMachine>();
         msgPtr->platformIndex(static_cast<unsigned int>(platformPtr->GetIndex()));
         msgPtr->simTime(static_cast<float>(aSimTime));
         msgPtr->fsmId(static_cast<unsigned int>(aStateMachinePtr->Id()));
         WsfEventPipe::StateList stateList;
         PopulateStateList(stateList, aStateMachinePtr);
         msgPtr->states(stateList);
         SendDependent(aSimTime, *platformPtr, std::move(msgPtr));
      }
   }
}

void WsfEventPipeInterface::PopulateStateList(WsfEventPipe::StateList& aList, WsfScriptStateMachine* aStateMachinePtr)
{
   if (aStateMachinePtr != nullptr)
   {
      for (int i = 1; i <= aStateMachinePtr->GetStateCount(); i++)
      {
         auto statePtr = aStateMachinePtr->GetState(i);
         if (statePtr != nullptr)
         {
            WsfEventPipe::State state;
            state.stateIndex(i);
            state.stateName(statePtr->GetStateName());

            if (statePtr->AdvancedBehaviorTree() != nullptr)
            {
               state.childAbtId(statePtr->AdvancedBehaviorTree()->GetTreeId());
            }

            if (statePtr->ChildStateMachine() != nullptr)
            {
               state.childFsmId(static_cast<unsigned int>(statePtr->ChildStateMachine()->Id()));
            }

            WsfEventPipe::IntList transitionList;
            for (auto& transition : statePtr->GetTransitions())
            {
               transitionList.push_back(transition->GetNextStateIndex());
            }
            state.transitions(transitionList);
            aList.push_back(state);
         }
      }
   }
}

void WsfEventPipeInterface::StateMachineState(double                 aSimTime,
                                              WsfScriptStateMachine* aStateMachinePtr,
                                              const int              aOldStateIndex,
                                              const int              aNewStateIndex)
{
   if (aStateMachinePtr->Processor() != nullptr)
   {
      auto platformPtr = aStateMachinePtr->Processor()->GetPlatform();
      if (platformPtr != nullptr)
      {
         auto msgPtr = ut::make_unique<WsfEventPipe::MsgStateMachineState>();
         msgPtr->platformIndex(static_cast<unsigned int>(platformPtr->GetIndex()));
         msgPtr->simTime(static_cast<float>(aSimTime));
         msgPtr->fsmId(static_cast<unsigned int>(aStateMachinePtr->Id()));
         msgPtr->oldStateIndex(aOldStateIndex);
         msgPtr->newStateIndex(aNewStateIndex);
         SendDependent(aSimTime, *platformPtr, std::move(msgPtr));
      }
   }
}

void WsfEventPipeInterface::UpdateArticulation(double              aSimTime,
                                               WsfArticulatedPart* aPartPtr,
                                               int                 aPartType,
                                               double              aAdditionalAz,
                                               double              aAdditionalEl)
{
   if (nullptr != aPartPtr)
   {
      if (aPartPtr->GetPlatform() != nullptr)
      {
         if (aPartPtr->IsTurnedOn())
         {
            auto msgPtr = ut::make_unique<WsfEventPipe::MsgPartArticulationStatus>();

            msgPtr->ownerIndex(Platform(*aPartPtr->GetPlatform()));
            msgPtr->partName(aPartPtr->GetName());
            msgPtr->partType(aPartType);

            UtVec3d xyz;
            xyz = UtVec3d(aPartPtr->GetLocation());
            msgPtr->locationECS(toVec3f(xyz.GetData()));

            double slewypr[3];
            aPartPtr->GetCurrentOrientationECS(slewypr[0], slewypr[1], slewypr[2]);
            msgPtr->slewECS(toVec3f(slewypr));
            double cue[3] = {aAdditionalAz, aAdditionalEl, 0.0};
            msgPtr->cuePCS(toVec3f(cue));

            ArticulationData art(xyz, slewypr, cue);

            ArticulatedPartId id(msgPtr->ownerIndex(), msgPtr->partName(), msgPtr->partType());

            auto lastArt = mArticulationTracker.emplace(std::make_pair(id, art));
            if (lastArt.second) // a new element was added
            {
               SendDependent(aSimTime, *aPartPtr->GetPlatform(), std::move(msgPtr));
            }
            else if (lastArt.first->second != art) // the state has changed
            {
               mArticulationTracker[id] = art;
               SendDependent(aSimTime, *aPartPtr->GetPlatform(), std::move(msgPtr));
            }
         }
      }
   }
}

void WsfEventPipeInterface::DisEmissionReceived(WsfDisInterface* aDisInterface, const WsfDisEmission& aEmissionPduPtr)
{
   double          simTime        = GetSimulation().GetSimTime();
   WsfDisPlatform* disPlatformPtr = WsfDisUtil::FindDisPlatform(aDisInterface, aEmissionPduPtr.GetEmittingEntityId());
   if ((disPlatformPtr) && (disPlatformPtr->GetPlatform() != nullptr))
   {
      if (disPlatformPtr->GetPlatform()->IsExternallyControlled())
      {
         for (auto systemIt = aEmissionPduPtr.GetSystemBegin(); systemIt != aEmissionPduPtr.GetSystemEnd(); ++systemIt)
         {
            // detect beam?
            //  0     - other
            //  1-16  - sensor
            //  64-66 - EW
            //  96-97 - Weapon
            unsigned int systemId = (*systemIt)->GetNumber();

            for (auto beamIt = (*systemIt)->GetBeamBegin(); beamIt != (*systemIt)->GetBeamEnd(); ++beamIt)
            {
               unsigned int beamNumber = (*beamIt)->GetNumber();
               // 255  - non-standard detection beam
               unsigned int beamFunc = (*beamIt)->GetFunction();
               // 0    - other
               // 1-11 - sensor
               // 12   - jammer
               std::set<DisTrackJam> trackJamSet = (*beamIt)->GetTargetSet();
               std::set<DisEntityId> newSet;
               for (auto&& it : trackJamSet)
               {
                  newSet.insert(it.GetEntityId());
               }
               std::set<DisEntityId> addSet;
               std::set<DisEntityId> remSet;
               UpdateDisTrackJamSet(DisBeamAddress(Platform(disPlatformPtr->GetPlatform()->GetIndex()), systemId, beamNumber),
                                    newSet,
                                    addSet,
                                    remSet);
               for (auto&& addTJ : addSet)
               {
                  WsfDisPlatform* tgtPtr = WsfDisUtil::FindDisPlatform(aDisInterface, addTJ);
                  if (tgtPtr)
                  {
                     if (beamNumber == 255)
                     {
                        // start detection event
                        auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorDetectionChange>();
                        msgPtr->ownerIndex(Platform(*disPlatformPtr->GetPlatform()));
                        msgPtr->sensorName(std::to_string(systemId));
                        msgPtr->targetIndex(Platform(*tgtPtr->GetPlatform()));
                        msgPtr->detected(true);
                        SendDependent(simTime, *disPlatformPtr->GetPlatform(), std::move(msgPtr));
                     }
                     else if (beamFunc != 12)
                     {
                        // start sensor track event
                        auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorTrackCreated>();
                        msgPtr->ownerIndex(Platform(*disPlatformPtr->GetPlatform()));
                        Pack(msgPtr->trackId(), WsfTrackId(disPlatformPtr->GetPlatform()->GetName(), 0));
                        msgPtr->targetIndex(Platform(*tgtPtr->GetPlatform()));
                        SendDependent(simTime, *disPlatformPtr->GetPlatform(), std::move(msgPtr));
                     }
                     else
                     {
                        // start jam event
                     }
                  }
               }
               for (auto&& remTJ : remSet)
               {
                  WsfDisPlatform* tgtPtr = WsfDisUtil::FindDisPlatform(aDisInterface, remTJ);
                  if (tgtPtr)
                  {
                     if (beamNumber == 255)
                     {
                        // stop detection event
                        auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorDetectionChange>();
                        msgPtr->ownerIndex(Platform(disPlatformPtr->GetPlatform()->GetIndex()));
                        msgPtr->sensorName(std::to_string(systemId));
                        msgPtr->targetIndex(Platform(tgtPtr->GetPlatform()->GetIndex()));
                        msgPtr->detected(false);
                        SendDependent(simTime, *disPlatformPtr->GetPlatform(), std::move(msgPtr));
                     }
                     else if (beamFunc != 12)
                     {
                        // stop sensor track event
                        auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorTrackDrop>();
                        msgPtr->ownerIndex(Platform(*disPlatformPtr->GetPlatform()));
                        Pack(msgPtr->trackId(), WsfTrackId(disPlatformPtr->GetPlatform()->GetName(), 0));
                        msgPtr->targetIndex(Platform(*tgtPtr->GetPlatform()));
                        SendDependent(simTime, *disPlatformPtr->GetPlatform(), std::move(msgPtr));
                     }
                     else
                     {
                        // stop jam event
                     }
                  }
               }
            }
         }
      }
   }
}

void WsfEventPipeInterface::Comment(double aSimTime, WsfPlatform* aPlatformPtr, const std::string& aComment)
{
   if (aPlatformPtr != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgComment>();
      msgPtr->text(aComment);
      msgPtr->platformIndex(Platform(*aPlatformPtr));
      SendDependent(aSimTime, *aPlatformPtr, std::move(msgPtr));
   }
}

void WsfEventPipeInterface::Bookmark(double aSimTime, const std::string& aMsgType, const std::string& aMsgText)
{
   auto msgPtr = ut::make_unique<WsfEventPipe::MsgBookmark>();
   msgPtr->time((float)aSimTime);
   msgPtr->msgType(aMsgType);
   msgPtr->msgText(aMsgText);
   SendImmediate(GetSimulation().GetSimTime(), false, std::move(msgPtr));
}

void WsfEventPipeInterface::Pack(WsfEventPipe::EM_Interaction_RelativeData& aData,
                                 const WsfEM_Interaction::RelativeData&     aRelative)
{
   aData.trueAzimuth((float)aRelative.mAz);
   aData.trueElevation((float)aRelative.mEl);
   aData.range((float)aRelative.mRange);
   aData.apparentAzimuth((float)aRelative.mAz);
   aData.apparentElevation((float)aRelative.mEl);
}

void WsfEventPipeInterface::Pack(WsfEventPipe::EM_Interaction_BeamData& aData, const WsfEM_Interaction::BeamData& aBeam)
{
   aData.azimuth((float)aBeam.mAz);
   aData.elevation((float)aBeam.mEl);
   aData.gain((float)aBeam.mGain);
}

void WsfEventPipeInterface::Pack(WsfEventPipe::EM_Interaction& aData, const WsfEM_Interaction& aInteraction)
{
   const WsfEM_Interaction&             i      = aInteraction;
   WsfEventPipe::EM_Interaction_Status& status = aData.status();

   if (i.mCheckedStatus & WsfEM_Interaction::cRCVR_RANGE_LIMITS)
   {
      status.rcvrRangeLimits(0 == (i.mFailedStatus & WsfEM_Interaction::cRCVR_RANGE_LIMITS));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS)
   {
      status.rcvrAltitudeLimits(0 == (i.mFailedStatus & WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cRCVR_ANGLE_LIMITS)
   {
      status.rcvrAngleLimits(0 == (i.mFailedStatus & WsfEM_Interaction::cRCVR_ANGLE_LIMITS));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cRCVR_HORIZON_MASKING)
   {
      status.rcvrHorizonMasking(0 == (i.mFailedStatus & WsfEM_Interaction::cRCVR_HORIZON_MASKING));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cRCVR_TERRAIN_MASKING)
   {
      status.rcvrTerrainMasking(0 == (i.mFailedStatus & WsfEM_Interaction::cRCVR_TERRAIN_MASKING));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cXMTR_RANGE_LIMITS)
   {
      status.xmtrRangeLimits(0 == (i.mFailedStatus & WsfEM_Interaction::cXMTR_RANGE_LIMITS));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cXMTR_ALTITUDE_LIMITS)
   {
      status.xmtrAltitudeLimits(0 == (i.mFailedStatus & WsfEM_Interaction::cXMTR_ALTITUDE_LIMITS));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cXMTR_ANGLE_LIMITS)
   {
      status.xmtrAngleLimits(0 == (i.mFailedStatus & WsfEM_Interaction::cXMTR_ANGLE_LIMITS));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cXMTR_HORIZON_MASKING)
   {
      status.xmtrHorizonMasking(0 == (i.mFailedStatus & WsfEM_Interaction::cXMTR_HORIZON_MASKING));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cXMTR_TERRAIN_MASKING)
   {
      status.xmtrTerrainMasking(0 == (i.mFailedStatus & WsfEM_Interaction::cXMTR_TERRAIN_MASKING));
   }
   if (i.mCheckedStatus & WsfEM_Interaction::cSIGNAL_LEVEL)
   {
      status.signalLevel(0 == (i.mFailedStatus & WsfEM_Interaction::cSIGNAL_LEVEL));
   }

   // unsigned int           mCheckedStatus;
   // unsigned int           mFailedStatus;
   if (aInteraction.mXmtrLoc.mIsValid)
   {
      aData.xmtrWCS(toVec3d(aInteraction.mXmtrLoc.mLocWCS));
   }
   if (aInteraction.mRcvrLoc.mIsValid)
   {
      aData.rcvrWCS(toVec3d(aInteraction.mRcvrLoc.mLocWCS));
   }
   if (aInteraction.mTgtLoc.mIsValid)
   {
      aData.targetWCS(toVec3d(aInteraction.mTgtLoc.mLocWCS));
   }
   if (aInteraction.GetTarget())
   {
      Pack(aData.rcvrToTarget(), aInteraction.mRcvrToTgt);
      aData.rcvrToTargetValid(true);
      Pack(aData.targetToRcvr(), aInteraction.mTgtToRcvr);
      aData.targetToRcvrValid(true);
      if (aInteraction.GetTransmitter())
      {
         Pack(aData.xmtrToTarget(), aInteraction.mXmtrToTgt);
         aData.xmtrToTargetValid(true);
         Pack(aData.targetToXmtr(), aInteraction.mTgtToXmtr);
         aData.targetToXmtrValid(true);
      }
   }

   // how do i tell if it's a one-way interaction????
   //    //! Transmitter-to-receiver relative data.
   //    //! @note This is only valid for one-way transmitter-receiver interactions.
   //    RelativeData           mXmtrToRcvr;
   //
   //    //! Receiver-to-transmitter relative data.
   //    //! @note This is only valid for one-way transmitter-receiver interactions.
   //    RelativeData           mRcvrToXmtr;

   Pack(aData.rcvrBeam(), aInteraction.mRcvrBeam);
   aData.rcvrBeamValid(true);

   Pack(aData.xmtrBeam(), aInteraction.mXmtrBeam);
   aData.xmtrBeamValid(true);

   if (aInteraction.mMaskingFactor < 1.0)
   {
      aData.maskingFactor((float)aInteraction.mMaskingFactor);
   }

   if (aInteraction.mInfraredSig > 0.0)
   {
      aData.infraredSignature((float)aInteraction.mInfraredSig);
      aData.infraredSignatureAzimuth((float)aInteraction.mInfraredSigAz);
      aData.infraredSignatureElevation((float)aInteraction.mInfraredSigEl);
   }

   if (aInteraction.mRadarSig > 0.0)
   {
      aData.radarSignature((float)aInteraction.mRadarSig);
      aData.radarSignatureAzimuth((float)aInteraction.mRadarSigAz);
      aData.radarSignatureElevation((float)aInteraction.mRadarSigEl);
   }

   if (aInteraction.mOpticalSig > 0.0)
   {
      aData.opticalSignature((float)aInteraction.mOpticalSig);
      aData.opticalSignatureAzimuth((float)aInteraction.mOpticalSigAz);
      aData.opticalSignatureElevation((float)aInteraction.mOpticalSigEl);
   }

   if (i.mOpticalReflectivity > 0.0)
   {
      aData.opticalReflectivity((float)i.mOpticalReflectivity);
   }
   if (i.mXmtdPower > 0.0)
   {
      aData.transmittedPower((float)i.mXmtdPower);
   }
   if (i.mPowerDensityAtTarget > 0.0)
   {
      aData.powerDensityAtTarget((float)i.mPowerDensityAtTarget);
   }
   if (i.mRcvdPower > 0.0)
   {
      aData.receivedPower((float)i.mRcvdPower);
   }
   if (i.mRcvrNoisePower > 0.0)
   {
      aData.rcvrNoisePower((float)i.mRcvrNoisePower);
   }
   if (i.mClutterPower > 0.0)
   {
      aData.clutterPower((float)i.mClutterPower);
   }
   if (i.mInterferencePower > 0.0)
   {
      aData.interferencePower((float)i.mInterferencePower);
   }
   if (i.mSignalToNoise > 0.0)
   {
      aData.signalToNoise((float)i.mSignalToNoise);
   }
   if (i.mPropagationFactor > 0.0)
   {
      aData.propagationFactor((float)i.mPropagationFactor);
   }
   if (i.mAbsorptionFactor >= 0.0 && i.mAbsorptionFactor < 1.0)
   {
      aData.absorbtionFactor((float)i.mAbsorptionFactor);
   }
   if (i.mDetectionThreshold > 0.0)
   {
      aData.detectionThreshold((float)i.mDetectionThreshold);
   }
   if (i.mPixelCount > 0.0)
   {
      aData.pixelCount((float)i.mPixelCount);
   }
   if (i.mZoneAttenuationValue > 0.0)
   {
      aData.zoneAttenuationFactor((float)i.mZoneAttenuationValue);
   }
}

void WsfEventPipeInterface::SensorDetectionChanged(double           aSimTime,
                                                   WsfSensor*       aSensorPtr,
                                                   size_t           aTargetIndex,
                                                   WsfSensorResult& aResult)
{
   if (aSensorPtr->GetPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgSensorDetectionChange>();
      msgPtr->ownerIndex(Platform(*aSensorPtr->GetPlatform()));
      msgPtr->sensorName(aSensorPtr->GetName());
      msgPtr->targetIndex(Platform(aTargetIndex));
      msgPtr->detected(aResult.Detected());

#ifdef DETECTION_CHANGE_WORKAROUND_NECESSARY
      if (aResult.Detected())
      {
         mDetectionList[std::make_pair(aSensorPtr->GetPlatform()->GetIndex(), aSensorPtr->GetName())][aTargetIndex]++;
         SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
      }
      else
      {
         auto addr = std::make_pair(aSensorPtr->GetPlatform()->GetIndex(), aSensorPtr->GetName());
         if (mDetectionList[addr][aTargetIndex] > 0)
         {
            mDetectionList[addr][aTargetIndex]--;
            SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
         }
      }
#else
      SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
#endif
   }
}

void WsfEventPipeInterface::SensorDetectionAttempt(double           aSimTime,
                                                   WsfSensor*       aSensorPtr,
                                                   WsfPlatform*     aTargetPtr,
                                                   WsfSensorResult& aResult)
{
   if (aSensorPtr->GetPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgDetectAttempt>();
      if (!GetPlatformDetailLevel(*aSensorPtr->GetPlatform()).IsOptionEnabled(mEventIds.cDETECTION_ATTEMPT))
      {
         return;
      }
      msgPtr->sensorPlatformIndex(Platform(*aSensorPtr->GetPlatform()));
      msgPtr->sensorName(aSensorPtr->GetName());
      if (aTargetPtr != nullptr)
      {
         msgPtr->targetPlatformIndex(Platform(*aTargetPtr));
      }

      msgPtr->beam(static_cast<int>(aResult.mBeamIndex));
      const WsfMeasurement& measurement = aResult.mMeasurement;
      {
         double tmp = measurement.GetRangeError();
         if (tmp != 0.0)
         {
            msgPtr->rangeError((float)tmp);
         }
      }
      {
         double tmp = measurement.GetBearingError();
         if (tmp != 0.0)
         {
            msgPtr->azimuthError((float)tmp);
         }
      }
      {
         double tmp = measurement.GetElevationError();
         if (tmp != 0.0)
         {
            msgPtr->elevationError((float)tmp);
         }
      }
      {
         double tmp = measurement.GetRangeRateError();
         if (tmp != 0.0)
         {
            msgPtr->rangeRateError((float)tmp);
         }
      }

      msgPtr->pd((float)aResult.mPd);
      if (aResult.mRequiredPd != 0.0)
      {
         msgPtr->requiredPd((float)aResult.mRequiredPd);
      }

      WsfStringId sideId = measurement.GetSideId();
      if (!sideId.IsNull())
      {
         msgPtr->reportedSide(sideId);
      }

      WsfStringId typeId = measurement.GetTypeId();
      if (!typeId.IsNull())
      {
         msgPtr->reportedType(typeId);
      }

      Pack(msgPtr->interaction(), aResult);
      SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
   }
}

// A platform has been added or removed such that one of the detail levels
// is now used (or unused), and therefore, the subscriptions need updated
void WsfEventPipeInterface::UpdateFilters()
{
   WsfEventPipeOptions oldOptions = mCombinedOptions;
   oldOptions.DisableAll();
   for (DetailSettingData& settings : mDetailSettingsArray)
   {
      mCombinedOptions.SetVisualPartUpdateInterval(
         std::max(mCombinedOptions.GetVisualPartUpdateInterval(), settings.mOptions.GetVisualPartUpdateInterval()));
      if (settings.mActivePlatformCount > 0)
      {
         oldOptions.Add(settings.mOptions);
      }
   }
   UpdateSubscriptions(oldOptions);
   //   mTrackUpdatesFiltered = mTrackEventsFiltered = false;
   //   mTypeOptions.clear();
   //   mPlatformOptions.clear();
   //   mCombinedOptions = mGlobalOptions;
   //   for (size_t i = 0; i < mAppliedFilters.size(); ++i)
   //   {
   //      const WsfEventOutputOptionsFilter& filter = mAppliedFilters[i];
   //      if (!filter.mPlatformName.empty())
   //      {
   //         WsfEventPipeOptions& options = mPlatformOptions[filter.mPlatformName];
   //         if (filter.mIncrease)
   //         {
   //            options.Add(filter.mOption);
   //            mCombinedOptions.Add(filter.mOption);
   //         }
   //         else options.Subtract(filter.mOption);
   //      }
   //      if (!filter.mTypeName.empty())
   //      {
   //         WsfEventPipeOptions& options = mTypeOptions[filter.mPlatformName];
   //         if (filter.mIncrease)
   //         {
   //            options.Add(filter.mOption);
   //            mCombinedOptions.Add(filter.mOption);
   //         }
   //         else options.Subtract(filter.mOption);
   //      }
   //   }
   //   mFilteredFlags = ~mGlobalOptions.mEnableBits & mCombinedOptions.mEnableBits;
}

void WsfEventPipeInterface::PackDrawVertex(WsfEventPipe::MsgDrawCommand_Vertex& aMsg, const WsfDraw::VertexObject& aVertex)
{
   unsigned char vertexType = (unsigned char)aVertex.mVertexType;
   aMsg.type(vertexType);
   if (vertexType != WsfDraw::cUNSET_VERTEX)
   {
      if (vertexType != WsfDraw::cRELATIVE_ZERO)
      {
         aMsg.xyz(toVec3f(aVertex.mPosition.GetData()));
      }
      if (vertexType != WsfDraw::cABSOLUTE_WCS)
      {
         aMsg.relativePlatformIndex(Platform(aVertex.mPlatformIndex));
      }
   }
}

void WsfEventPipeInterface::UpdateDisTrackJamSet(const DisBeamAddress&        aAddress,
                                                 const std::set<DisEntityId>& aNewList,
                                                 std::set<DisEntityId>&       aAddSet,
                                                 std::set<DisEntityId>&       aRemSet)
{
   auto it = mCurrentTrackJamList.find(aAddress);
   if (it == mCurrentTrackJamList.end())
   {
      aAddSet = aNewList;
   }
   else
   {
      std::set_difference(std::begin(aNewList),
                          std::end(aNewList),
                          std::begin(it->second),
                          std::end(it->second),
                          std::inserter(aAddSet, std::end(aAddSet)));
      std::set_difference(std::begin(it->second),
                          std::end(it->second),
                          std::begin(aNewList),
                          std::end(aNewList),
                          std::inserter(aRemSet, std::end(aRemSet)));
   }
   mCurrentTrackJamList[aAddress] = aNewList;
}

void WsfEventPipeInterface::AddZoneSet(double aSimTime, WsfZone* aZonePtr)
{
   WsfZoneDefinition* zoneDefPtr = dynamic_cast<WsfZoneDefinition*>(aZonePtr);
   WsfZoneSet*        zoneSetPtr = dynamic_cast<WsfZoneSet*>(aZonePtr);
   WsfZoneReference*  zoneRefPtr = dynamic_cast<WsfZoneReference*>(aZonePtr);
   // Handles a single zone
   if (zoneDefPtr)
   {
      zoneDefPtr->UpdateReference(GetSimulation());
      SendZoneDefMessage(zoneDefPtr, aSimTime);
   }
   // Handle a zone set: this can have references to other zones and zone_sets
   else if (zoneSetPtr)
   {
      std::unique_ptr<WsfEventPipe::MsgZoneSet> msg = ut::make_unique<WsfEventPipe::MsgZoneSet>();
      WsfZone*                                  subZonePtr;
      msg->zoneSetName(zoneSetPtr->GetName());
      if (zoneSetPtr->GetPlatform())
      {
         msg->parentPlatform(zoneSetPtr->GetPlatform()->GetName());
         msg->isGlobal(false);
      }
      else
      {
         msg->parentPlatform("");
         msg->isGlobal(true);
      }
      msg->lineColor(UtColorToColorF(zoneSetPtr->GetLineColor()));
      msg->fillColor(UtColorToColorF(zoneSetPtr->GetFillColor()));

      WsfEventPipe::CircularZoneList   circularList;
      WsfEventPipe::EllipticalZoneList ellipticalList;
      WsfEventPipe::SphericalZoneList  sphericalList;
      WsfEventPipe::PolygonalZoneList  polygonalList;
      for (int i = 0; i < zoneSetPtr->GetNumZones(); ++i)
      {
         subZonePtr = zoneSetPtr->GetZoneAtIndex(i);
         RegisterZone(subZonePtr, circularList, ellipticalList, sphericalList, polygonalList, msg);
      }
      for (int i = 0; i < zoneSetPtr->GetNumExclusionZones(); ++i)
      {
         subZonePtr = zoneSetPtr->GetExclusionZoneAtIndex(i);
         RegisterZone(subZonePtr, circularList, ellipticalList, sphericalList, polygonalList, msg);
      }
      if (!circularList.empty() || !ellipticalList.empty() || !sphericalList.empty() || !polygonalList.empty())
      {
         msg->circularZones(circularList);
         msg->ellipticalZones(ellipticalList);
         msg->sphericalZones(sphericalList);
         msg->polygonalZones(polygonalList);
         bool isGlobal = msg->isGlobal();
         SendZoneMsg(isGlobal, aZonePtr->GetPlatform(), aSimTime, std::move(msg));
      }
   }
   // Handle zone references
   // For simplicity, since these can reference zone sets, treat every reference as a zone_set internally
   else if (zoneRefPtr)
   {
      std::unique_ptr<WsfEventPipe::MsgZoneSet> msg = ut::make_unique<WsfEventPipe::MsgZoneSet>();
      msg->zoneSetName(zoneRefPtr->GetName());
      if (zoneRefPtr->GetPlatform())
      {
         msg->parentPlatform(zoneRefPtr->GetPlatform()->GetName());
         msg->isGlobal(false);
      }
      else
      {
         msg->parentPlatform("");
         msg->isGlobal(true);
      }
      msg->lineColor(UtColorToColorF(zoneRefPtr->GetReferencedZone()->GetLineColor()));
      msg->fillColor(UtColorToColorF(zoneRefPtr->GetReferencedZone()->GetFillColor()));

      ZoneList zoneList;
      GetReferenceZones(zoneRefPtr, zoneList, msg->parentPlatform());
      if (zoneList.mCircularList.empty() || zoneList.mEllipticalList.empty() || zoneList.mSphericalList.empty() ||
          zoneList.mPolygonalList.empty())
      {
         msg->circularZones(zoneList.mCircularList);
         msg->ellipticalZones(zoneList.mEllipticalList);
         msg->sphericalZones(zoneList.mSphericalList);
         msg->polygonalZones(zoneList.mPolygonalList);
         bool isGlobal = msg->isGlobal();
         SendZoneMsg(isGlobal, aZonePtr->GetPlatform(), aSimTime, std::move(msg));
      }
   }
}

void WsfEventPipeInterface::RegisterZone(WsfZone*                                   aZonePtr,
                                         WsfEventPipe::CircularZoneList&            aCircularList,
                                         WsfEventPipe::EllipticalZoneList&          aEllipticalList,
                                         WsfEventPipe::SphericalZoneList&           aSphericalList,
                                         WsfEventPipe::PolygonalZoneList&           aPolygonalList,
                                         std::unique_ptr<WsfEventPipe::MsgZoneSet>& aMsgPtr)
{
   WsfZoneDefinition* zoneDefPtr = dynamic_cast<WsfZoneDefinition*>(aZonePtr);
   WsfZoneReference*  zoneRefPtr = dynamic_cast<WsfZoneReference*>(aZonePtr);

   if (zoneDefPtr)
   {
      WsfEventPipe::CommonZoneData commonData = GetCommonZoneData(zoneDefPtr, aMsgPtr->parentPlatform());
      WsfZoneDefinition::ShapeType shapeType  = zoneDefPtr->GetShapeType();
      if (shapeType == WsfZoneDefinition::ShapeType::cCIRCULAR)
      {
         aCircularList.push_back(GetCircularZone(zoneDefPtr, commonData));
      }
      else if (shapeType == WsfZoneDefinition::ShapeType::cELLIPTICAL)
      {
         aEllipticalList.push_back(GetEllipticalZone(zoneDefPtr, commonData));
      }
      else if (shapeType == WsfZoneDefinition::ShapeType::cSPHERICAL)
      {
         aSphericalList.push_back(GetSphericalZone(zoneDefPtr, commonData));
      }
      else if (shapeType == WsfZoneDefinition::ShapeType::cPOLYGONAL)
      {
         aPolygonalList.push_back(GetPolygonalZone(zoneDefPtr, commonData));
      }
   }
   else if (zoneRefPtr)
   {
      ZoneList zoneList;
      GetReferenceZones(zoneRefPtr, zoneList, aMsgPtr->parentPlatform());
      // Don't overwrite: Add to the existing lists
      for (const auto& circle : zoneList.mCircularList)
      {
         aCircularList.push_back(circle);
      }
      for (const auto& ellipse : zoneList.mEllipticalList)
      {
         aEllipticalList.push_back(ellipse);
      }
      for (const auto& sphere : zoneList.mSphericalList)
      {
         aSphericalList.push_back(sphere);
      }
      for (const auto& polygon : zoneList.mPolygonalList)
      {
         aPolygonalList.push_back(polygon);
      }
   }
}

void WsfEventPipeInterface::SendZoneMsg(bool                                   aGlobal,
                                        WsfPlatform*                           aPlatform,
                                        double                                 aSimTime,
                                        std::unique_ptr<WsfEventPipe::MsgBase> aMsg)
{
   if (aGlobal)
   {
      SendImmediate(aSimTime, false, std::move(aMsg));
   }
   else if (aPlatform)
   {
      SendDependent(aSimTime, *aPlatform, std::move(aMsg));
   }
}

void WsfEventPipeInterface::SendZoneDefMessage(WsfZoneDefinition* aZoneDefPtr, double aSimTime)
{
   WsfEventPipe::CommonZoneData commonData = GetCommonZoneData(aZoneDefPtr);
   bool                         isGlobal   = commonData.isGlobal();
   WsfZoneDefinition::ShapeType shapeType  = aZoneDefPtr->GetShapeType();
   if (shapeType == WsfZoneDefinition::ShapeType::cCIRCULAR)
   {
      std::unique_ptr<WsfEventPipe::MsgCircularZone> msg = ut::make_unique<WsfEventPipe::MsgCircularZone>();
      msg->zoneInfo(GetCircularZone(aZoneDefPtr, commonData));
      SendZoneMsg(isGlobal, aZoneDefPtr->GetPlatform(), aSimTime, std::move(msg));
   }
   else if (shapeType == WsfZoneDefinition::ShapeType::cELLIPTICAL)
   {
      std::unique_ptr<WsfEventPipe::MsgEllipticalZone> msg = ut::make_unique<WsfEventPipe::MsgEllipticalZone>();
      msg->zoneInfo(GetEllipticalZone(aZoneDefPtr, commonData));
      SendZoneMsg(isGlobal, aZoneDefPtr->GetPlatform(), aSimTime, std::move(msg));
   }
   else if (shapeType == WsfZoneDefinition::ShapeType::cSPHERICAL)
   {
      std::unique_ptr<WsfEventPipe::MsgSphericalZone> msg = ut::make_unique<WsfEventPipe::MsgSphericalZone>();
      msg->zoneInfo(GetSphericalZone(aZoneDefPtr, commonData));
      SendZoneMsg(isGlobal, aZoneDefPtr->GetPlatform(), aSimTime, std::move(msg));
   }
   else if (shapeType == WsfZoneDefinition::ShapeType::cPOLYGONAL)
   {
      std::unique_ptr<WsfEventPipe::MsgPolygonalZone> msg = ut::make_unique<WsfEventPipe::MsgPolygonalZone>();
      msg->zoneInfo(GetPolygonalZone(aZoneDefPtr, commonData));
      SendZoneMsg(isGlobal, aZoneDefPtr->GetPlatform(), aSimTime, std::move(msg));
   }
}

void WsfEventPipeInterface::DrawUpdate(const std::string&            aTarget,
                                       const std::string&            aLayer,
                                       const WsfDraw::DrawEventList& aDrawList)
{
   auto msgPtr = ut::make_unique<WsfEventPipe::MsgDrawCommand>();
   if (!aLayer.empty())
   {
      msgPtr->layer(aLayer);
   }
   WsfEventPipe::MsgDrawCommand_ItemList& items = msgPtr->items();
   for (const WsfDraw::DrawEvent& evt : aDrawList)
   {
      WsfEventPipe::MsgDrawCommand_Item item;
      item.id(evt.mID);
      switch (evt.mDrawType)
      {
      case WsfDraw::cLINE:
      case WsfDraw::cPOINT:
      case WsfDraw::cICON:
      case WsfDraw::cELLIPSE:
      case WsfDraw::cELLIPSOID:
      case WsfDraw::cQUADRILATERAL:
      case WsfDraw::cTEXT:
      case WsfDraw::cTIMER:
      {
         unsigned int rgba = ((unsigned int)std::min(255.0f, std::max(0.0f, evt.mColor.GetData()[0] * 255.0f))) |
                             ((unsigned int)std::min(255.0f, std::max(0.0f, evt.mColor.GetData()[1] * 255.0f)) << 8) |
                             ((unsigned int)std::min(255.0f, std::max(0.0f, evt.mColor.GetData()[2] * 255.0f)) << 16) |
                             ((unsigned int)std::min(255.0f, std::max(0.0f, evt.mColor.GetData()[3] * 255.0f)) << 24);
         item.color(rgba);
         item.duration(evt.mDuration);
         if (evt.mDrawType == WsfDraw::cLINE)
         {
            item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::line);
            PackDrawVertex(item.vertex1(), evt.mVerts[0]);
            item.vertex1Valid(true);
            PackDrawVertex(item.vertex2(), evt.mVerts[1]);
            item.vertex2Valid(true);
            item.drawSize((unsigned char)evt.mLineSize);
            item.drawStyle((unsigned char)evt.mLineStyle);
         }
         else if (evt.mDrawType == WsfDraw::cPOINT)
         {
            item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::point);
            PackDrawVertex(item.vertex1(), evt.mVerts[0]);
            item.vertex1Valid(true);
            item.drawSize((unsigned char)evt.mPointSize);
         }
         else if (evt.mDrawType == WsfDraw::cICON)
         {
            item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::icon);
            PackDrawVertex(item.vertex1(), evt.mVerts[0]);
            item.vertex1Valid(true);
            WsfEventPipe::Vec3f ori;
            ori.x(evt.mHeading);
            item.orientation(ori);
            item.text(evt.mIcon);
         }
         else if (evt.mDrawType == WsfDraw::cELLIPSE)
         {
            item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::ellipse);
            PackDrawVertex(item.vertex1(), evt.mVerts[0]);
            item.vertex1Valid(true);
            item.drawSize((unsigned char)evt.mLineSize);
            item.drawStyle((unsigned char)evt.mLineStyle);
            item.drawStyle2((unsigned char)evt.mEllipseMode);
            WsfEventPipe::Vec3f ori;
            ori.x(evt.mHeading);
            item.orientation(ori);
            WsfEventPipe::Vec3f axis;
            axis.x(evt.mAxisA);
            axis.y(evt.mAxisB);
            item.axis(axis);
         }
         else if (evt.mDrawType == WsfDraw::cELLIPSOID)
         {
            item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::ellipsoid);
            PackDrawVertex(item.vertex1(), evt.mVerts[0]);
            item.vertex1Valid(true);
            item.drawSize((unsigned char)evt.mLineSize);
            item.drawStyle((unsigned char)evt.mLineStyle);
            item.drawStyle2((unsigned char)evt.mEllipseMode);
            WsfEventPipe::Vec3f ori;
            ori.x(evt.mHeading);
            ori.y(evt.mPitch);
            ori.z(evt.mRoll);
            item.orientation(ori);
            WsfEventPipe::Vec3f axis;
            axis.x(evt.mAxisA);
            axis.y(evt.mAxisB);
            axis.z(evt.mAxisC);
            item.axis(axis);
         }
         else if (evt.mDrawType == WsfDraw::cQUADRILATERAL)
         {
            item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::quadrilateral);
            PackDrawVertex(item.vertex1(), evt.mVerts[0]);
            item.vertex1Valid(true);
            PackDrawVertex(item.vertex2(), evt.mVerts[1]);
            item.vertex2Valid(true);
            PackDrawVertex(item.vertex3(), evt.mVerts[2]);
            item.vertex3Valid(true);
            PackDrawVertex(item.vertex4(), evt.mVerts[3]);
            item.vertex4Valid(true);
         }
         else if (evt.mDrawType == WsfDraw::cTEXT)
         {
            item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::text);
            PackDrawVertex(item.vertex1(), evt.mVerts[0]);
            item.vertex1Valid(true);
            item.drawSize((unsigned char)evt.mTextSize);
            item.text(evt.mText);
         }
         else if (evt.mDrawType == WsfDraw::cTIMER)
         {
            item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::timer);
            PackDrawVertex(item.vertex1(), evt.mVerts[0]);
            item.vertex1Valid(true);
            item.drawSize((unsigned char)evt.mPointSize);
         }
         items.push_back(item);
      }
      break;
      case WsfDraw::cERASE:
         item.drawType(WsfEventPipe::MsgDrawCommand_DrawType::erase);
         items.push_back(item);
         break;
      default:
         break;
      }
   }

   SendImmediate(GetSimulation().GetSimTime(), false, std::move(msgPtr));
}

void WsfEventPipeInterface::MessageReceived(double             aSimTime,
                                            wsf::comm::Comm*   aXmtrPtr,
                                            wsf::comm::Comm*   aRcvrPtr,
                                            const WsfMessage&  aMessage,
                                            wsf::comm::Result& aResult)
{
   if (aRcvrPtr->GetPlatform() != nullptr)
   {
      WsfPlatform& rcvr = *aRcvrPtr->GetPlatform();
      if (!GetPlatformDetailLevel(rcvr).IsOptionEnabled(mEventIds.cMESSAGE_RECEIVED))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgMessageReceived>();
      msgPtr->xmtrPlatformIndex(Platform(*aXmtrPtr->GetPlatform()));
      msgPtr->xmtrCommName(aXmtrPtr->GetName());
      msgPtr->rcvrPlatformIndex(Platform(*aRcvrPtr->GetPlatform()));
      msgPtr->rcvrCommName(aXmtrPtr->GetName());
      msgPtr->messageType(aMessage.GetType());
      WsfStringId subType = aMessage.GetSubType();
      if (!subType.IsNull())
      {
         msgPtr->messageSubType(subType);
      }
      int sizeBits = aMessage.GetSizeBits();
      if (sizeBits != 0)
      {
         msgPtr->messageSize(sizeBits);
      }
      msgPtr->serialNumber(aMessage.GetSerialNumber());
      SendDependent(aSimTime, rcvr, std::move(msgPtr));
   }
}

void WsfEventPipeInterface::MessageHop(double            aSimTime,
                                       wsf::comm::Comm*  aRcvrPtr,
                                       wsf::comm::Comm*  aDestPtr,
                                       const WsfMessage& aMessage)
{
   if (aRcvrPtr->GetPlatform() != nullptr)
   {
      WsfPlatform& rcvr = *aRcvrPtr->GetPlatform();
      MessageId mid(aMessage.GetSerialNumber(), aMessage.GetSrcAddr().GetAddress(), aDestPtr->GetAddress().GetAddress());

      size_t      rcvrIdx  = Platform(rcvr);
      std::string rcvrName = aRcvrPtr->GetName();

      auto last = mMessageHopTracker.find(mid);

      if (GetPlatformDetailLevel(rcvr).IsOptionEnabled(mEventIds.cMESSAGE_HOP))
      {
         auto msgPtr = ut::make_unique<WsfEventPipe::MsgMessageHop>();
         if (last != mMessageHopTracker.end())
         {
            msgPtr->xmtrPlatformIndex(static_cast<unsigned int>(last->second.first));
            msgPtr->xmtrCommName(last->second.second);
         }
         else
         {
            msgPtr->xmtrPlatformIndex(static_cast<unsigned int>(aMessage.GetOriginatorIndex()));
            auto* origComm = GetSimulation().GetCommNetworkManager()->GetComm(aMessage.GetSrcAddr());
            if (origComm != nullptr)
            {
               msgPtr->xmtrCommName(origComm->GetName());
            }
         }
         msgPtr->rcvrPlatformIndex(static_cast<unsigned int>(rcvrIdx));
         msgPtr->rcvrCommName(rcvrName);
         msgPtr->messageType(aMessage.GetType());
         WsfStringId subType = aMessage.GetSubType();
         if (!subType.IsNull())
         {
            msgPtr->messageSubType(subType);
         }
         int sizeBits = aMessage.GetSizeBits();
         if (sizeBits != 0)
         {
            msgPtr->messageSize(sizeBits);
         }
         msgPtr->serialNumber(static_cast<unsigned int>(aMessage.GetSerialNumber()));
         SendDependent(aSimTime, rcvr, std::move(msgPtr));
      }

      mMessageHopTracker[mid] = std::make_pair(rcvrIdx, rcvrName);
   }
}

void WsfEventPipeInterface::MessageTransmitted(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage)
{
   if (aXmtrPtr->GetPlatform() != nullptr)
   {
      WsfPlatform& xmtr = *aXmtrPtr->GetPlatform();
      if (!GetPlatformDetailLevel(xmtr).IsOptionEnabled(mEventIds.cMESSAGE_TRANSMITTED))
      {
         return;
      }
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgMessageTransmitted>();
      msgPtr->xmtrPlatformIndex(Platform(*aXmtrPtr->GetPlatform()));
      msgPtr->xmtrCommName(aXmtrPtr->GetName());
      auto* rcvrComm = GetSimulation().GetCommNetworkManager()->GetComm(aMessage.GetDstAddr());
      if (rcvrComm)
      {
         auto* platformPtr = rcvrComm->GetPlatform();
         msgPtr->rcvrPlatformIndex(Platform(*platformPtr));
      }
      msgPtr->messageType(aMessage.GetType());
      WsfStringId subType = aMessage.GetSubType();
      if (!subType.IsNull())
      {
         msgPtr->messageSubType(subType);
      }
      int sizeBits = aMessage.GetSizeBits();
      if (sizeBits != 0)
      {
         msgPtr->messageSize(sizeBits);
      }
      msgPtr->serialNumber(aMessage.GetSerialNumber());
      SendDependent(aSimTime, xmtr, std::move(msgPtr));
   }
}

void WsfEventPipeInterface::MoverUpdated(double aSimTime, WsfMover* aMover)
{
   if (aSimTime > aMover->GetPlatform()->GetCreationTime())
   {
      SendEntityState(aSimTime, *aMover->GetPlatform());
   }
}

std::unique_ptr<UtScriptClass> WsfEventPipeInterface::CreateScriptClass(const std::string& aClassName,
                                                                        UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptEventPipeClass>(aClassName, aScriptTypesPtr);
}

void WsfEventPipeInterface::AddLogger(WsfEventPipeLogger* aLoggerPtr)
{
   mLoggers.push_back(aLoggerPtr);
   if (mStreamPtr != nullptr)
   {
      WsfEventPipeOptions empty = mCombinedOptions;
      empty.DisableAll();
      if (empty != mCombinedOptions)
      {
         aLoggerPtr->UpdateSubscriptions(mCombinedOptions, empty);
      }
   }
}

void WsfEventPipeInterface::RemoveLogger(WsfEventPipeLogger* aLoggerPtr)
{
   auto i = std::find(mLoggers.begin(), mLoggers.end(), aLoggerPtr);
   if (i != mLoggers.end())
   {
      mLoggers.erase(i);
   }
}

void WsfEventPipeInterface::PublishMode(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   mModeSet.insert(SensorModeIdentifier(aSensorPtr->GetUniqueId(), aModePtr->GetName()));

   if (aSensorPtr->GetPlatform() != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgEmitterModeDefinition>();

      msgPtr->ownerIndex(Platform(*aSensorPtr->GetPlatform()));
      msgPtr->partName(aSensorPtr->GetName());
      msgPtr->partType(WsfEventPipe::PartType::sensor);
      msgPtr->modeName(aModePtr->GetName());
      msgPtr->slewLimit().minAzimuth(static_cast<float>(aSensorPtr->GetMinAzSlew()));
      msgPtr->slewLimit().maxAzimuth(static_cast<float>(aSensorPtr->GetMaxAzSlew()));
      msgPtr->slewLimit().minElevation(static_cast<float>(aSensorPtr->GetMinElSlew()));
      msgPtr->slewLimit().maxElevation(static_cast<float>(aSensorPtr->GetMaxElSlew()));
      // the following max/min stuff is from the script methods
      msgPtr->cueLimit().minAzimuth(static_cast<float>(std::max(aSensorPtr->GetMinAzCue(), aSensorPtr->GetMinAzSlew())));
      msgPtr->cueLimit().maxAzimuth(static_cast<float>(std::min(aSensorPtr->GetMaxAzCue(), aSensorPtr->GetMaxAzSlew())));
      msgPtr->cueLimit().minElevation(static_cast<float>(std::max(aSensorPtr->GetMinElCue(), aSensorPtr->GetMinElSlew())));
      msgPtr->cueLimit().maxElevation(static_cast<float>(std::min(aSensorPtr->GetMaxElCue(), aSensorPtr->GetMaxElSlew())));
      msgPtr->slewMode(aSensorPtr->GetSlewMode());

      int rc = static_cast<int>(aSensorPtr->GetEM_RcvrCount());
      int bc = static_cast<int>(aModePtr->GetBeamCount());
      if (bc > 0)
      {
         for (int i = 0; i < bc; ++i)
         {
            WsfEventPipe::BeamDefinition bd;

            WsfSensorBeam* beamPtr = aModePtr->GetBeamEntry(i);
            bd.beamId(static_cast<int>(beamPtr->GetBeamIndex()));
            WsfEM_Antenna*     antennaPtr = beamPtr->GetEM_Rcvr()->GetAntenna();
            double             freq       = beamPtr->GetEM_Rcvr()->GetFrequency();
            double             btilt      = beamPtr->GetEM_Rcvr()->GetBeamTilt();
            WsfAntennaPattern* patt =
               beamPtr->GetEM_Rcvr()->GetAntennaPattern(beamPtr->GetEM_Rcvr()->GetPolarization(), freq);
            PublishAntennaData(bd, antennaPtr, aSensorPtr, patt, freq, btilt);
            msgPtr->beamList().push_back(bd);
         }
      }
      else if (rc > 0)
      {
         WsfEventPipe::BeamDefinition bd;

         bd.beamId(0);
         double             freq  = aSensorPtr->GetEM_Rcvr(0).GetFrequency();
         double             btilt = aSensorPtr->GetEM_Rcvr(0).GetBeamTilt();
         WsfAntennaPattern* patt =
            aSensorPtr->GetEM_Rcvr(0).GetAntennaPattern(aSensorPtr->GetEM_Rcvr(0).GetPolarization(), freq);
         WsfEM_Antenna* antennaPtr = aSensorPtr->GetEM_Rcvr(0).GetAntenna();
         PublishAntennaData(bd, antennaPtr, aSensorPtr, patt, freq, btilt);
         msgPtr->beamList().push_back(bd);
      }
      SendDependent(aSimTime, *aSensorPtr->GetPlatform(), std::move(msgPtr));
   }
}

void WsfEventPipeInterface::RouteChanged(WsfMover* aMoverPtr)
{
   if (aMoverPtr->GetRoute())
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgRouteChanged>();
      msgPtr->platformIndex(Platform(*aMoverPtr->GetPlatform()));
      double xyz[3];
      aMoverPtr->GetPlatform()->GetLocationWCS(xyz);
      msgPtr->location(toVec3d(xyz));
      double hpr[3];
      aMoverPtr->GetPlatform()->GetOrientationNED(hpr[0], hpr[1], hpr[2]);
      msgPtr->heading(hpr[0]);
      for (const auto& wp : aMoverPtr->GetRoute()->GetWaypoints())
      {
         WsfEventPipe::Waypoint msgwp;
         msgwp.locationType(wp.GetPointType() & WsfWaypoint::cLOCATION_MASK);
         if (wp.GetPointType() & WsfWaypoint::cHEADING_MASK)
         {
            msgwp.locationType(WsfWaypoint::cLATITUDE_AND_LONGITUDE);
         }
         if (!wp.GetLabelId().Empty())
         {
            msgwp.label(wp.GetLabelId());
         }
         msgwp.locationX(wp.GetLat());
         msgwp.locationY(wp.GetLon());
         if (wp.GetAlt() != WsfPath::cUSE_PREVIOUS)
         {
            msgwp.altitude(wp.GetAlt());
         }
         if (wp.GetHeading() != WsfPath::cUSE_PREVIOUS)
         {
            msgwp.heading(wp.GetHeading());
         }
         if (!wp.GetGotoId().Empty())
         {
            msgwp.gotoId(wp.GetGotoId());
         }
         msgPtr->route().push_back(msgwp);
      }

      SendDependent(GetSimulation().GetSimTime(), *aMoverPtr->GetPlatform(), std::move(msgPtr));
   }
}

void WsfEventPipeInterface::MoverChangedOnPlatform(WsfPlatform* aPlatformPtr)
{
   WsfMover* const moverPtr = aPlatformPtr->GetMover();

   // If the platform is not externally controlled or moved, check whether to change the mover's update interval.
   if (!aPlatformPtr->IsExternallyControlled() && !aPlatformPtr->IsExternallyMoved() && moverPtr)
   {
      moverPtr->SetUpdateInterval(MoverUpdateInterval(*moverPtr));
   }
}

void WsfEventPipeInterface::PublishAntennaData(WsfEventPipe::BeamDefinition& aBeamDefinition,
                                               WsfEM_Antenna*                aAntennaPtr,
                                               WsfArticulatedPart*           aPartPtr,
                                               WsfAntennaPattern*            aPatternPtr,
                                               double&                       aFrequency,
                                               const double                  aBeamTilt) const
{
   aBeamDefinition.minRange((float)aAntennaPtr->GetMinimumRange());
   aBeamDefinition.maxRange((float)aAntennaPtr->GetMaximumRange());
   aBeamDefinition.stabilized(aAntennaPtr->GetScanStabilization());
   aBeamDefinition.antennaTilt((float)aAntennaPtr->GetPitch());
   aBeamDefinition.scanLimit().minAzimuth(static_cast<float>(aAntennaPtr->GetMinimumAzimuthScanLimit()));
   aBeamDefinition.scanLimit().maxAzimuth(static_cast<float>(aAntennaPtr->GetMaximumAzimuthScanLimit()));
   aBeamDefinition.scanLimit().minElevation(static_cast<float>(aAntennaPtr->GetMinimumElevationScanLimit()));
   aBeamDefinition.scanLimit().maxElevation(static_cast<float>(aAntennaPtr->GetMaximumElevationScanLimit()));
   aBeamDefinition.scanMode(aAntennaPtr->GetScanMode());
   aBeamDefinition.beamTilt(static_cast<float>(aBeamTilt));
   if (aPatternPtr != nullptr)
   {
      aBeamDefinition.beamWidthAz(static_cast<float>(aPatternPtr->GetAzimuthBeamwidth(aFrequency, 0.0, 0.0)));
      aBeamDefinition.beamWidthEl(static_cast<float>(aPatternPtr->GetElevationBeamwidth(aFrequency, 0.0, 0.0)));
   }
   if (!aAntennaPtr->DefaultFieldOfView())
   {
      int                   scanMode = aAntennaPtr->GetScanMode();
      const WsfFieldOfView* fovPtr   = aAntennaPtr->GetFieldOfView();
      WsfStringId           fovtype  = fovPtr->GetType();
      if (fovtype == "circular")
      {
         aBeamDefinition.fov().shape(WsfEventPipe::FovShape::circular);
         const WsfCircularFieldOfView* cfovPtr = dynamic_cast<const WsfCircularFieldOfView*>(fovPtr);
         WsfEventPipe::Vec2f           point;
         point.x((float)cfovPtr->GetHalfAngle());
         point.y(0.0f);
         aBeamDefinition.fov().points().push_back(point);
      }
      else if (fovtype == "polygonal")
      {
         const WsfPolygonalFieldOfView* pFovPtr = dynamic_cast<const WsfPolygonalFieldOfView*>(fovPtr);

         const WsfZoneDefinition& zd =
            pFovPtr->GetPoints(); // I have been promised that this is only ever going to be a polygon,
                                  // so we will ignore everything else that zone-definition can do
         if (zd.GetShapeType() == WsfZoneDefinition::cPOLYGONAL)
         {
            aBeamDefinition.fov().shape(WsfEventPipe::FovShape::polygonal);
            const std::vector<WsfZone::Point>&          points = zd.GetPoints();
            std::vector<WsfZone::Point>::const_iterator it;
            for (it = points.begin(); it != points.end(); ++it)
            {
               WsfEventPipe::Vec2f point;
               point.x((float)it->mX);
               point.y((float)it->mY);
               aBeamDefinition.fov().points().push_back(point);
            }
         }
      }
      else if (fovtype == "equatorial")
      {
         aBeamDefinition.fov().shape(WsfEventPipe::FovShape::equatorial);
         const WsfEquatorialFieldOfView* pFovPtr = dynamic_cast<const WsfEquatorialFieldOfView*>(fovPtr);
         double                          equatorialMin;
         double                          equatorialMax;
         double                          polarMin;
         double                          polarMax;
         pFovPtr->GetEquatorialFieldOfView(equatorialMin, equatorialMax);
         pFovPtr->GetPolarFieldOfView(polarMin, polarMax);

         const double posX = tan(equatorialMax);
         const double negX = tan(equatorialMin);
         const double posY = tan(polarMax);
         const double negY = tan(polarMin);

         WsfEventPipe::Vec2f point;
         point.x((float)posX);
         point.y((float)posY);
         aBeamDefinition.fov().points().push_back(point);
         point.x((float)posX);
         point.y((float)negY);
         aBeamDefinition.fov().points().push_back(point);
         point.x((float)negX);
         point.y((float)negY);
         aBeamDefinition.fov().points().push_back(point);
         point.x((float)negX);
         point.y((float)posY);
         aBeamDefinition.fov().points().push_back(point);
      }
      else // rectangular
      {
         aBeamDefinition.fov().shape(WsfEventPipe::FovShape::rectangular);
         double minAzFOV;
         double maxAzFOV;
         aAntennaPtr->GetAzimuthFieldOfView(minAzFOV, maxAzFOV);
         double minElFOV;
         double maxElFOV;
         aAntennaPtr->GetElevationFieldOfView(minElFOV, maxElFOV);
         if (0 == (scanMode & WsfEM_Antenna::cSCAN_EL)) // antenna does not scan in elevation, the beam tilt may be applied
         {
            minElFOV += aBeamTilt;
            maxElFOV += aBeamTilt;
         }
         WsfEventPipe::Vec2f point;
         point.x((float)minAzFOV);
         point.y((float)minElFOV);
         aBeamDefinition.fov().points().push_back(point);
         point.x((float)maxAzFOV);
         point.y((float)maxElFOV);
         aBeamDefinition.fov().points().push_back(point);
      }
   }
}


// ================================================================================================

std::map<size_t, size_t> WsfEventPipeInterface::mExternalIdLookup;

bool WsfEventPipeInterface::RequiresEntityStateUpdate(double              aSimTime,
                                                      unsigned int        aPlatIdx,
                                                      const UtVec3d&      aLocV,
                                                      const UtQuaternion& aOriV) const
{
   if (mInput.mPositionThresholdSquared.has_value() || mInput.mAngleThreshold.has_value())
   {
      auto entData = mEntityStateData.find(aPlatIdx);
      if (entData == mEntityStateData.end())
      {
         return true;
      }
      if (mInput.mPositionThresholdSquared.has_value())
      {
         UtVec3d dist = aLocV - entData->second.Extrapolate(aSimTime);
         if (dist.MagnitudeSquared() >
             mInput.mPositionThresholdSquared) // how much does our extrapolation deviate from current state?
         {
            return true;
         }
      }
      if (mInput.mAngleThreshold.has_value())
      {
         UtQuaternion conversion = aOriV.Conjugate() * entData->second.Orientation();
         double sa = sqrt(conversion[1] * conversion[1] + conversion[2] * conversion[2] + conversion[3] * conversion[3]);
         double angle = 2.0 * atan2(sa, conversion[0]);
         if (angle > mInput.mAngleThreshold)
         {
            return true;
         }
      }
      return false;
   }
   // if there are no thresholds, every update from the movers will be published
   return true;
}

bool WsfEventPipeInterface::MessageId::operator<(const MessageId& aRHS) const
{
   if (aRHS.mSerialNumber == mSerialNumber)
   {
      if (aRHS.mDstAddr == mDstAddr)
      {
         return (mSrcAddr < aRHS.mSrcAddr);
      }
      else
      {
         return (mDstAddr < aRHS.mDstAddr);
      }
   }
   else
   {
      return mSerialNumber < aRHS.mSerialNumber;
   }
}

UtVec3d WsfEventPipeInterface::EntityStateData::Extrapolate(double aSimTime) const
{
   double elapsedTime = aSimTime - mSimTime;
   return mPosition + elapsedTime * mVelocity + 0.5 * elapsedTime * elapsedTime * mAcceleration;
}

WsfScriptEventPipeClass::WsfScriptEventPipeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   mCloneable     = false;

   SetClassName("WsfEventPipe");

   AddStaticMethod(ut::make_unique<Record_1>("Record"));
   AddStaticMethod(ut::make_unique<Record_2>("Record"));
   AddStaticMethod(ut::make_unique<Record_3>("Record"));
   AddStaticMethod(ut::make_unique<AddBookmark_1>("AddBookmark"));
   AddStaticMethod(ut::make_unique<AddBookmark_2>("AddBookmark"));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEventPipeClass, WsfEventPipeInterface, Record_1, 4, "void", "double, WsfPlatform, string, double")
{
   static bool sWarningReported = false;

   if (!sWarningReported)
   {
      auto msg =
         ut::log::warning() << "WsfEventPipe.Record with the sim_time argument has been deprecated and will be removed "
                            << "in a future release.  Use WsfEventPipe.Record without the sim_time argument instead."
                            << "This issue may occur multiple places in the scenario.";

      auto note = msg.AddNote() << "Callstack:";
      UtScriptExecutor::PrintAllCallStacks(note, true);
      sWarningReported = true;
   }

   WsfEventPipeInterface* iface = WsfScriptContext::GetEVENTPIPE(aContext);
   if (iface)
   {
      auto platformPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfPlatform>();
      if (platformPtr)
      {
         iface->RecordDouble(*platformPtr, aVarArgs[2].GetString(), aVarArgs[3].GetDouble());
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEventPipeClass, WsfEventPipeInterface, Record_2, 3, "void", "WsfPlatform, string, Object")
{
   WsfEventPipeInterface* iface = WsfScriptContext::GetEVENTPIPE(aContext);
   if (iface)
   {
      auto platformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
      if (platformPtr)
      {
         switch (aVarArgs[2].GetType())
         {
         case ut::script::Data::Type::cBOOL:
            iface->RecordBool(*platformPtr, aVarArgs[1].GetString(), aVarArgs[2].GetBool());
            break;
         case ut::script::Data::Type::cINT:
            iface->RecordInt(*platformPtr, aVarArgs[1].GetString(), aVarArgs[2].GetInt());
            break;
         case ut::script::Data::Type::cDOUBLE:
            iface->RecordDouble(*platformPtr, aVarArgs[1].GetString(), aVarArgs[2].GetDouble());
            break;
         case ut::script::Data::Type::cSTRING:
            iface->RecordString(*platformPtr, aVarArgs[1].GetString(), aVarArgs[2].GetString());
            break;
         case ut::script::Data::Type::cUNDEFINED:
         case ut::script::Data::Type::cPOINTER:
         default:
            break;
         }
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEventPipeClass, WsfEventPipeInterface, Record_3, 3, "void", "WsfPlatform, string, string")
{
   WsfEventPipeInterface* iface = WsfScriptContext::GetEVENTPIPE(aContext);
   if (iface)
   {
      auto platformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
      if (platformPtr)
      {
         iface->RecordString(*platformPtr, aVarArgs[1].GetString(), aVarArgs[2].GetString());
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEventPipeClass, WsfEventPipeInterface, AddBookmark_1, 2, "void", "string, string")
{
   WsfEventPipeInterface* iface = WsfScriptContext::GetEVENTPIPE(aContext);
   if (iface)
   {
      iface->Bookmark(iface->GetSimulation().GetSimTime(), aVarArgs[0].GetString(), aVarArgs[1].GetString());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEventPipeClass, WsfEventPipeInterface, AddBookmark_2, 3, "void", "double, string, string")
{
   WsfEventPipeInterface* iface = WsfScriptContext::GetEVENTPIPE(aContext);
   if (iface)
   {
      iface->Bookmark(aVarArgs[0].GetDouble(), aVarArgs[1].GetString(), aVarArgs[2].GetString());
   }
}
