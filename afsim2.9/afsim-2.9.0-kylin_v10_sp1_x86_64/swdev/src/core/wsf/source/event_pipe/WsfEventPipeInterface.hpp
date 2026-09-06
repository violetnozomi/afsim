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

#ifndef WSFEVENTPIPEINTERFACE_HPP
#define WSFEVENTPIPEINTERFACE_HPP

#include <mutex>
#include <set>

#include "WsfSimulationExtension.hpp"

#if _WIN32
#include <windows.h>
#include <UtUndefineWindows.hpp>
#else
#include <sys/file.h>
#include <unistd.h>
#endif

#include "DisEntityId.hpp"
#include "UtCallbackHolder.hpp"
class UtCovariance;
#include "UtPack.hpp"
#include "UtQuaternion.hpp"
#include "UtScriptClassDefine.hpp"
class WsfAdvancedBehaviorTree;
class WsfAdvancedBehaviorTreeNode;
class WsfAntennaPattern;
class WsfArticulatedPart;
class WsfAuxDataEnabled;
class WsfDisEmission;
class WsfDisInterface;
#include "WsfDraw.hpp"
class WsfEM_Antenna;
#include "WsfEM_Interaction.hpp"
#include "WsfEventPipeClasses.hpp"
class WsfEventPipeExtension;
#include "WsfEventPipeEventIds.hpp"
#include "WsfEventPipeInput.hpp"
#include "WsfEventPipeOptions.hpp"
class WsfEventPipeLogger;
class WsfLocalTrack;
class WsfMessage;
class WsfMover;
class WsfPlatform;
#include "WsfPlatformObserver.hpp"
class WsfPlatformPart;
class WsfProcessor;
class WsfScriptStateMachine;
#include "WsfScenarioExtension.hpp"
class WsfSensor;
class WsfSensorMode;
class WsfSensorResult;
#include "WsfStringId.hpp"
class WsfTrack;
class WsfTrackId;
class WsfVisualPart;
class WsfZone;
class WsfZoneDefinition;

namespace wsf
{
namespace comm
{
class Comm;
class Result;
} // namespace comm
namespace eventpipe
{
class FileWriterWorker;
class PartUpdateEvent;
} // namespace eventpipe
} // namespace wsf

#define DETECTION_CHANGE_WORKAROUND_NECESSARY 1

// Similar to event output, but faster, more information, configurable by platform, and more computer-readable.
class WSF_EXPORT WsfEventPipeInterface : public WsfSimulationExtension
{
public:
   static WsfEventPipeInterface* Find(const WsfSimulation& aSimulation);

   WsfEventPipeInterface(WsfEventPipeExtension& aExtension, const WsfEventPipeInput& aInput);
   ~WsfEventPipeInterface() override;

   WsfEventPipeInterface(const WsfEventPipeInterface& aPipeInterface) = delete;
   WsfEventPipeInterface& operator=(const WsfEventPipeInterface& aPipeInterface) = delete;

   void AddedToSimulation() override;

   int GetEventId(const std::string& aEventName) const;

   const std::string& GetOutputFileName() const;

   // send a message with no dependency on a platform immediately
   // Interface takes ownership of message.
   void SendImmediate(double aSimTime, bool aExternal, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr);
   // send a message with a platform dependency when possible
   // Interface takes ownership of message.
   void SendDependent(double aSimTime, const WsfPlatform& aPlatform, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr);

   void SendPartStatus(double aSimTime, WsfPlatformPart* aPartPtr, int aType);

   void SendModeActive(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, bool aActive);

   void SendVisualPartDefinition(double aSimTime, WsfVisualPart* aPartPtr, int aType);

   // Returns an options object representing the union of all options currently enabled for at least one platform
   // this set of options is useful for determining which observers need to be connected
   const WsfEventPipeOptions&            GetCombinedOptions() const { return mCombinedOptions; }
   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void AddLogger(WsfEventPipeLogger* aLoggerPtr);
   void RemoveLogger(WsfEventPipeLogger* aLoggerPtr);

   static void Pack(WsfEventPipe::Track_Id& aData, const WsfTrackId& aId);
   static void Pack(WsfEventPipe::Covariance& aData, const UtCovariance& aCovariance);
   static void PackTrack(WsfEventPipe::Track& aTrackData, const WsfTrack& aTrack);
   static void Pack(WsfEventPipe::EM_Interaction_RelativeData& aData, const WsfEM_Interaction::RelativeData& aRelative);
   static void Pack(WsfEventPipe::EM_Interaction_BeamData& aData, const WsfEM_Interaction::BeamData& aBeam);
   static void Pack(WsfEventPipe::EM_Interaction& aData, const WsfEM_Interaction& aInteraction);

   void UpdateArticulation(double              aSimTime,
                           WsfArticulatedPart* aPartPtr,
                           int                 aPartType,
                           double              aAdditionalAz = 0.0,
                           double              aAdditionalEl = 0.0);

   static unsigned int Platform(const WsfPlatform& aPlatform);
   static unsigned int Platform(size_t aPlatformIndex);

   void Bookmark(double aSimTime, const std::string& aMsgType, const std::string& aMsgText);


   WSF_DEPRECATED void Record(double aSimTime, WsfPlatform* aPlatform, const std::string& aKey, double aValue);

   void RecordDouble(const WsfPlatform& aPlatform, const std::string& aKey, double aValue);

   void RecordInt(const WsfPlatform& aPlatform, const std::string& aKey, int aValue);

   void RecordBool(const WsfPlatform& aPlatform, const std::string& aKey, bool aValue);

   void RecordString(const WsfPlatform& aPlatform, const std::string& aKey, const std::string& aValue);

   //! @param aTime is the current time
   //! @param aPlatformId is the platform of interest
   //! if the entity no longer exists we delete the event
   //! if an update occurred within the interval, no message is sent and the event is rescheduled to meet the interval
   //! if an update hasn't occurred, a message is sent and the event is rescheduled to meet the interval
   //! @return 0.0 if the event should be deleted, and the time of the next event otherwise
   double HandleScheduledEntityStateRequest(double aTime, size_t aPlatformId);

   void UpdatePartArticulation(double aSimTime, size_t aPlatformId, WsfStringId& aPartNameId);

private:
   void AdvancedBehaviorTree(double aSimTime, WsfAdvancedBehaviorTree* aTreePtr);
   void PopulateAdvancedBehaviorTreeNodeList(WsfEventPipe::BehaviorTreeNodeList& aList,
                                             WsfAdvancedBehaviorTreeNode*        aNodePtr) const;
   void AdvancedBehaviorTreeState(double aSimTime, WsfAdvancedBehaviorTreeNode* aNodePtr);
   void PopulateAdvancedBehaviorTreeStateLists(WsfEventPipe::BehaviorTreeNodeExecList&       aExecList,
                                               WsfEventPipe::BehaviorTreeBlackboardDataList& aBlackboardList,
                                               WsfAdvancedBehaviorTreeNode*                  aNodePtr) const;
   void StateMachine(double aSimTime, WsfScriptStateMachine* aStateMachinePtr);
   void PopulateStateList(WsfEventPipe::StateList& aList, WsfScriptStateMachine* aStateMachinePtr);
   void StateMachineState(double                 aSimTime,
                          WsfScriptStateMachine* aStateMachinePtr,
                          const int              aOldStateIndex,
                          const int              aNewStateIndex);

   void OpenOutput();

   void UpdateSubscriptions(const WsfEventPipeOptions& aPreviousOptions);

   //! Considers sending an entity state, the decision to send is affected by
   //! the entity_state input values.
   void SendEntityState(double aSimTime, WsfPlatform& aPlatform, bool aForce = false);

   //! Creates and sends a MsgAuxData for all relevant platforms.
   void SendAllPlatformAuxData(double aSimTime);

   //! Creates and sends a MsgAuxData for the specified platform.
   void SendPlatformAuxData(double aSimTime, const WsfPlatform* aPlatform);

   //! Callback for WsfAuxDataEnabled::AuxDataAccessed.
   //! Adds aSource to mAuxDataAccessed.
   //! The next time SendAllPlatfomrAuxData is called, a MsgAuxData will be created for *aSource.
   void AuxDataAccessed(const WsfAuxDataEnabled* aSource);

   //! Callback for WsfAuxDataEnabled::AuxDataDestroyed.
   //! Removes aDestroyed from mAuxDataAccessed.
   //! Required to prevent use-after-free errors.
   //! @note aDestroyed CANNOT be checked using dynamic_cast!
   void AuxDataDestroyed(const WsfAuxDataEnabled* aDestroyed);

   //! Check the update interval of @param aMover against the maximum mover update interval
   //! known to the instance of WsfEventPipeInterface and returns the value that @param aMover
   //! should use between the two values.
   double MoverUpdateInterval(const WsfMover& aMover) const;

   void DrawUpdate(const std::string& aTarget, const std::string& aLayer, const WsfDraw::DrawEventList& aDrawList);

   void Comment(double aSimTime, WsfPlatform* aPlatformPtr, const std::string& aComment);

   void DisEmissionReceived(WsfDisInterface* aDisInterface, const WsfDisEmission& aEmissionPduPtr);

   void LocalTrackCorrelation(double            aSimTime,
                              WsfPlatform*      aPlatformPtr,
                              const WsfTrackId& aLocalTrackId,
                              const WsfTrackId& aNonLocalTrackId);

   void LocalTrackDecorrelation(double            aSimTime,
                                WsfPlatform*      aPlatformPtr,
                                const WsfTrackId& aLocalTrackId,
                                const WsfTrackId& aNonLocalTrackId);

   void LocalTrackDropped(double aSimTime, WsfPlatform* aPlatformPtr, const WsfLocalTrack* aTrackPtr);

   void LocalTrackInitiated(double               aSimTime,
                            WsfPlatform*         aPlatformPtr,
                            const WsfLocalTrack* aTrackPtr,
                            const WsfTrack*      aSourcePtr);

   void LocalTrackUpdated(double aSimTime, WsfPlatform* aPlatformPtr, const WsfLocalTrack* aTrackPtr, const WsfTrack* aSourcePtr);

   void MessageReceived(double             aSimTime,
                        wsf::comm::Comm*   aXmtrPtr,
                        wsf::comm::Comm*   aRcvrPtr,
                        const WsfMessage&  aMessage,
                        wsf::comm::Result& aResult);

   void MessageHop(double aSimTime, wsf::comm::Comm* aRcvrPtr, wsf::comm::Comm* aDestPtr, const WsfMessage& aMessage);

   void MessageTransmitted(double aSimTime, wsf::comm::Comm* aXmtrPtr, const WsfMessage& aMessage);

   void MoverChangedOnPlatform(WsfPlatform* aPlatformPtr);

   void MoverUpdated(double aSimTime, WsfMover* aMover);

   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformAppearanceChanged(double aSimTime, WsfPlatform* aPlatformPtr, WsfObserver::AppearanceType aAppearanceType);

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformBroken(double aSimTime, WsfPlatform* aPlatformPtr);

   void PublishAntennaData(WsfEventPipe::BeamDefinition& aBeamDef,
                           WsfEM_Antenna*                aAntennaPtr,
                           WsfArticulatedPart*           aPartPtr,
                           WsfAntennaPattern*            aPatternPtr,
                           double&                       aFrequency,
                           const double                  aBeamTilt) const;

   void PublishMode(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void RegisterZone(WsfZone*                                   aZonePtr,
                     WsfEventPipe::CircularZoneList&            aCircularList,
                     WsfEventPipe::EllipticalZoneList&          aEllipticalList,
                     WsfEventPipe::SphericalZoneList&           aSphericalList,
                     WsfEventPipe::PolygonalZoneList&           aPolygonalList,
                     std::unique_ptr<WsfEventPipe::MsgZoneSet>& aMsgPtr);

   void RouteChanged(WsfMover* aMoverPtr);

   void SensorDetectionAttempt(double aSimTime, WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   void SensorDetectionChanged(double aSimTime, WsfSensor* aSensorPtr, size_t aTargetIndex, WsfSensorResult& aResult);

   void SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr);

   void SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr);

   void SimulationStarting();

   void UpdateFilters();

   void VisualPartTurnedOn(double aSimTime, WsfVisualPart* aPartPtr);

   void VisualPartTurnedOff(double aSimTime, WsfVisualPart* aPartPtr);

   void SendExecData();

   void SendScenarioData();

   static void PackDrawVertex(WsfEventPipe::MsgDrawCommand_Vertex& aMsg, const WsfDraw::VertexObject& aVertex);

   bool RequiresEntityStateUpdate(double aSimTime, unsigned int aPlatIdx, const UtVec3d& aLocV, const UtQuaternion& aOriV) const;

   struct DisBeamAddress
   {
      DisBeamAddress(unsigned int aPlatformId, unsigned int aSystemId, unsigned int aBeamNumber)
         : mPlatformId(aPlatformId)
         , mSystemId(aSystemId)
         , mBeamNumber(aBeamNumber)
      {
      }
      bool operator<(const DisBeamAddress& aRHS) const
      {
         if (mPlatformId < aRHS.mPlatformId)
         {
            return true;
         }
         else if (mPlatformId > aRHS.mPlatformId)
         {
            return false;
         }
         else if (mSystemId < aRHS.mSystemId)
         {
            return true;
         }
         else if (mSystemId > aRHS.mSystemId)
         {
            return false;
         }
         else if (mBeamNumber < aRHS.mBeamNumber)
         {
            return true;
         }
         else
         {
            return false;
         }
      }
      unsigned int mPlatformId;
      unsigned int mSystemId;
      unsigned int mBeamNumber;
   };

   void UpdateDisTrackJamSet(const DisBeamAddress&        aAddress,
                             const std::set<DisEntityId>& aNewList,
                             std::set<DisEntityId>&       aAddSet,
                             std::set<DisEntityId>&       aRemSet);

   const WsfEventPipeOptions& GetPlatformDetailLevel(const WsfPlatform& aPlatform);

   void AddZoneSet(double aSimTime, WsfZone* aZonePtr);
   void SendZoneMsg(bool aGlobal, WsfPlatform* aPlatform, double aSimTime, std::unique_ptr<WsfEventPipe::MsgBase> aMsg);
   void SendZoneDefMessage(WsfZoneDefinition* aZoneDefPtr, double aSimTime);

   // A reference back to the scenario extension for accessing some lesser used input data.
   const WsfEventPipeExtension& mExtension;

   UtCallbackHolder mCallbacks;

   UtPackMessageStdStreamO*          mStreamPtr;
   std::unique_ptr<std::ofstream>    mFileStreamPtr;
   UtPackSerializer*                 mSerializerPtr;
   UtPackSchema*                     mSchemaPtr;
   wsf::eventpipe::FileWriterWorker* mWriteThreadPtr;

   //! @name Copies of data from the scenario extension
   //@{
   const WsfEventPipeInput  mInput;
   wsf::eventpipe::EventIds mEventIds;
   //@}

   using FilterMap = std::unordered_map<std::string, WsfEventPipeOptions>;

   // A filter to determine if a platform belongs to a detail level
   struct Filter
   {
      WsfEventPipeOptionsCriteria mCritiera;
      unsigned int                mDetailSetting;
   };

   // A detail level
   struct DetailSettingData
   {
      DetailSettingData()
         : mActivePlatformCount(0)
      {
      }
      DetailSettingData(const WsfEventPipeOptions& aOptions)
         : mOptions(aOptions)
         , mActivePlatformCount(0)
      {
      }
      WsfEventPipeOptions mOptions;
      // the number of alive platforms which use this detail level
      int mActivePlatformCount;
   };

   class ArticulatedPartId
   {
   public:
      ArticulatedPartId(int aOwner, std::string& aName, int aType)
         : mOwner(aOwner)
         , mName(aName)
         , mType(aType)
      {
      }
      virtual ~ArticulatedPartId() = default;

      bool operator<(const ArticulatedPartId& aRHS) const
      {
         return (mOwner == aRHS.mOwner) ? (mName == aRHS.mName) ? (mType < aRHS.mType) : (mName < aRHS.mName) :
                                          (mOwner < aRHS.mOwner);
      }

   private:
      int         mOwner;
      std::string mName;
      int         mType;
   };

   struct ArticulationData
   {
      ArticulationData() = default;
      ArticulationData(const UtVec3d& aPos, const UtVec3d& aSlew, const UtVec3d& aCue)
         : mPosition(aPos)
         , mSlew(aSlew)
         , mCue(aCue)
      {
      }
      bool operator!=(const ArticulationData& aRHS) const noexcept
      {
         return (mPosition != aRHS.mPosition) || (mSlew != aRHS.mSlew) || (mCue != aRHS.mCue);
      }
      UtVec3d mPosition;
      UtVec3d mSlew;
      UtVec3d mCue;
   };

   template<typename OBSERVER>
   void ConnectPartStatusCallback(OBSERVER& aObserver, WsfEventPipe::PartType::Value aType)
   {
      mBaseCallbacks +=
         aObserver(&GetSimulation())
            .Connect([this, aType](double aSimTime, WsfPlatformPart* aPart) { SendPartStatus(aSimTime, aPart, aType); });
   }

   std::map<ArticulatedPartId, ArticulationData> mArticulationTracker;

   std::vector<unsigned int>          mPlatformIndexToSettingIndex;
   std::vector<Filter>                mFilters;
   std::vector<DetailSettingData>     mDetailSettingsArray;
   std::vector<WsfEventPipeLogger*>   mLoggers;
   WsfEventPipeOptions                mCombinedOptions;
   std::set<const WsfAuxDataEnabled*> mAuxDataAccessed;
   std::mutex                         mAuxDataAccessedMutex;

   UtCallbackHolder mDetectionChangeCallbacks;
   UtCallbackHolder mLocalTrackEventCallbacks;
   UtCallbackHolder mLocalTrackUpdatedCallbacks;
   UtCallbackHolder mEntityStateCallbacks;
   UtCallbackHolder mBaseCallbacks;
   UtCallbackHolder mDetectionAttemptCallbacks;
   UtCallbackHolder mDrawCallbacks;
   UtCallbackHolder mMessageReceivedCallback;
   UtCallbackHolder mMessageHopCallback;
   UtCallbackHolder mMessageTransmittedCallback;
   UtCallbackHolder mCommentCallback;
   UtCallbackHolder mBehaviorToolCallbacks;
   UtCallbackHolder mAuxDataCallbacks;
   UtCallbackHolder mRouteCallbacks;

   bool         mTrackEventsFiltered;
   bool         mTrackUpdatesFiltered;
   unsigned int mFilteredFlags;

   std::string mOutputFileName;

   using SensorModeIdentifier =
      std::pair<unsigned int, std::string>; // identify a sensor mode with sensor unique id and mode name
   std::set<SensorModeIdentifier> mModeSet;

   std::map<unsigned int, wsf::eventpipe::PartUpdateEvent*>              mPartUpdateEvents;
   std::map<size_t, std::vector<std::unique_ptr<WsfEventPipe::MsgBase>>> mCachedMessages; // sometimes platform-related
                                                                                          // messages are received prior
                                                                                          // to platform initialization,
                                                                                          // hang on to them until they
                                                                                          // can actually be procesed

#ifdef DETECTION_CHANGE_WORKAROUND_NECESSARY
   std::map<std::pair<size_t, std::string>, std::map<size_t, int>> mDetectionList;
#endif

   // This is needed to provide a unique id for messages.
   class MessageId
   {
   public:
      MessageId(size_t aSerialNumber, std::string aSrcAddr, std::string aDstAddr)
         : mSerialNumber(aSerialNumber)
         , mSrcAddr(aSrcAddr)
         , mDstAddr(aDstAddr)
      {
      }
      bool operator<(const MessageId& aRHS) const;

   private:
      size_t      mSerialNumber;
      std::string mSrcAddr;
      std::string mDstAddr;
   };
   // maps message serial number to previous hop data
   std::map<MessageId, std::pair<size_t, std::string>> mMessageHopTracker;

   std::map<std::string, size_t>   mExternalNameMap;
   static std::map<size_t, size_t> mExternalIdLookup;

   std::map<DisBeamAddress, std::set<DisEntityId>> mCurrentTrackJamList;

   class EntityStateData
   {
   public:
      EntityStateData(double              aSimTime,
                      const UtVec3d&      aPosition,
                      const UtVec3d&      aVelocity,
                      const UtVec3d&      aAcceleration,
                      const UtQuaternion& aOrientation)
         : mSimTime(aSimTime)
         , mPosition(aPosition)
         , mVelocity(aVelocity)
         , mAcceleration(aAcceleration)
         , mQuaternion(aOrientation)
      {
      }

      UtVec3d             Extrapolate(double aSimTime) const;
      const UtQuaternion& Orientation() const { return mQuaternion; }
      double              Time() const { return mSimTime; }

   private:
      double       mSimTime{0.0};
      UtVec3d      mPosition;
      UtVec3d      mVelocity;
      UtVec3d      mAcceleration;
      UtQuaternion mQuaternion;
   };

   std::map<unsigned int, EntityStateData> mEntityStateData;
};

//! Define script methods for WsfEventPipeInterface
class WsfScriptEventPipeClass : public UtScriptClass
{
public:
   WsfScriptEventPipeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptEventPipeClass() override = default;

   UT_DECLARE_SCRIPT_METHOD(Record_1);
   UT_DECLARE_SCRIPT_METHOD(Record_2);
   UT_DECLARE_SCRIPT_METHOD(Record_3);
   UT_DECLARE_SCRIPT_METHOD(AddBookmark_1);
   UT_DECLARE_SCRIPT_METHOD(AddBookmark_2);
};

#endif
