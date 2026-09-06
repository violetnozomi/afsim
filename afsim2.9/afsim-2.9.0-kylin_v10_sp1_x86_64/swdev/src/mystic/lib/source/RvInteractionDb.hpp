// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVWSFRESULTINTERACTIONDB_HPP
#define RVWSFRESULTINTERACTIONDB_HPP
#include "RvExport.hpp"
#include "RvMilEventPipeClasses.hpp"
#include "RvResultDb.hpp"

namespace rv
{
class RV_EXPORT InteractionDb
{
public:
   struct Interaction
   {
      Interaction(float              aTime,
                  const std::string& aType,
                  unsigned int       aIndex,
                  bool               aStart,
                  unsigned int       aSource,
                  unsigned int       aTarget,
                  const std::string& aData,
                  unsigned int       aId)
         : mTime(aTime)
         , mType(aType)
         , mIndex(aIndex)
         , mStart(aStart)
         , mSource(aSource)
         , mTarget(aTarget)
         , mData(aData)
         , mId(aId)
      {
      }
      float        mTime;
      std::string  mType;
      unsigned int mIndex;
      bool         mStart;
      unsigned int mSource;
      unsigned int mTarget;
      std::string  mData;
      unsigned int mId; // this will be used to correlate start/stop events
   };


   // probably not the right nomenclature here, but this manages its own memory.  push will new, and pop will delete.
   class InteractionArray
   {
   public:
      // TODO: want to use a circular buffer here, instead
      typedef std::deque<Interaction*>                  Array;
      typedef Array::iterator                           iterator;
      typedef Array::const_iterator                     const_iterator;
      typedef std::pair<const_iterator, const_iterator> range_pair;

      InteractionArray();
      virtual ~InteractionArray();

      void           push(unsigned int       aIndex,
                          bool               aAtBegin,
                          const std::string& aType,
                          float              aTime,
                          bool               aStart,
                          int                aSource,
                          int                aTarget,
                          const std::string& aData,
                          bool               aCorrelateByDataDictionary = false);
      void           pop(unsigned int aIndex, bool aAtBegin);
      const_iterator FindFirstBefore(float aTime) const;
      const_iterator FindFirstAfter(float aTime) const;
      range_pair GetDataInRange(float aStart, float aEnd, bool& aValid) const; // find first after start, find first before end

      struct ComparePred
      {
         bool operator()(const Interaction* aInteraction, float aTime) const { return aInteraction->mTime > aTime; }
         bool operator()(float aTime, const Interaction* aInteraction) const { return aTime < aInteraction->mTime; }
      };


   private:
      float                               mMinTime;
      float                               mMaxTime;
      unsigned int                        mMinMessageIndex;
      unsigned int                        mMaxMessageIndex;
      Array                               mData;
      std::map<std::string, unsigned int> mCorrelationDictionary;
   };

   // in AddPairedStartInteraction and AddPairedStopInteraction the aCorrelateByData parameter will add the aData value
   // to a dictionary so that start/stop pairs may be correlated.  If used, it is essential that the aData provided by
   // the start and stop messages matches
   void AddPairedStartInteraction(unsigned int       aOwner,
                                  unsigned int       aInteractor,
                                  unsigned int       aInteractee,
                                  unsigned int       aMessageIndex,
                                  bool               aAtBegin,
                                  const std::string& aMessageType,
                                  double             aSimTime,
                                  const std::string& aData,
                                  bool               aCorrelateByData = false);
   void AddPairedStopInteraction(unsigned int       aOwner,
                                 unsigned int       aInteractor,
                                 unsigned int       aInteractee,
                                 unsigned int       aMessageIndex,
                                 bool               aAtBegin,
                                 const std::string& aMessageType,
                                 double             aSimTime,
                                 const std::string& aAttackType,
                                 bool               aCorrelateByData = false);
   void AddUnpairedInteraction(unsigned int       aOwner,
                               unsigned int       aInteractor,
                               unsigned int       aInteractee,
                               unsigned int       aMessageIndex,
                               bool               aAtBegin,
                               const std::string& aMessageType,
                               double             aSimTime,
                               const std::string& aAttackType);

   void RemovePairedInteraction(unsigned int aInteractor, unsigned int aInteractee, unsigned int aMessageIndex, bool aAtBegin);

   void RemoveUnpairedInteraction(unsigned int aInteractor, unsigned int aInteractee, unsigned int aMessageIndex, bool aAtBegin);

   void AddMessage(MsgSensorDetectionChange* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorDetectionChange* aMsg, bool aAtBegin);
   void AddMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin);
   void AddMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin);
   void AddMessage(MsgLocalTrackCreated* aMsg, bool aAtBegin);
   void RemoveMessage(MsgLocalTrackCreated* aMsg, bool aAtBegin);
   void AddMessage(MsgLocalTrackDrop* aMsg, bool aAtBegin);
   void RemoveMessage(MsgLocalTrackDrop* aMsg, bool aAtBegin);
   void AddMessage(MsgTaskUpdate* aMsg, bool aAtBegin);
   void RemoveMessage(MsgTaskUpdate* aMsg, bool aAtBegin);
   void AddMessage(MsgWeaponFired* aMsg, bool aAtBegin);
   void AddMessage(MsgWeaponTerminated* aMsg, bool aAtBegin);
   void AddMessage(MsgMessageReceived* aMsg, bool aAtBegin);
   void RemoveMessage(MsgMessageReceived* aMsg, bool aAtBegin);
   void AddMessage(MsgMessageHop* aMsg, bool aAtBegin);
   void RemoveMessage(MsgMessageHop* aMsg, bool aAtBegin);
   void AddMessage(MsgJammingRequestInitiated* aMsg, bool aAtBegin);
   void RemoveMessage(MsgJammingRequestInitiated* aMsg, bool aAtBegin);
   void AddMessage(MsgJammingRequestCanceled* aMsg, bool aAtBegin);
   void RemoveMessage(MsgJammingRequestCanceled* aMsg, bool aAtBegin);

   unsigned int GetChangeNumber() const { return mChangeNumber; }

   InteractionArray::range_pair GetRangeData(int aPlatform, float aStart, float aEnd, bool& aValid) const;
   InteractionArray::range_pair GetOneTimeRangeData(int aPlatform, float aStart, float aEnd, bool& aValid) const;
   InteractionArray::range_pair GetUnpairedRangeData(int aPlatform, float aStart, float aEnd, bool& aValid) const;

private:
   void RemoveMessagePrivate(std::map<int, InteractionArray>& aArray,
                             unsigned int                     aPlatformIndex1,
                             unsigned int                     aPlatformIndex2,
                             unsigned int                     aMessageIndex,
                             bool                             aAtBegin);

   unsigned int mChangeNumber;

   mutable std::map<int, InteractionArray> mInteractionArrayMap; // paged interactions with a start and end event
   mutable std::map<int, InteractionArray> mOneTimeInteractionArrayMap; // one time load interactions with a start and end event
   mutable std::map<int, InteractionArray> mUnpairedInteractionArrayMap; // interactions we expect to turn off with a time-out

   // interactions with a start and multiple update events, but one end event, can use this to ensure that only the
   // first start event is added to the interaction map. A message group consists of a message type and a group ID. For
   // instance "Task" and a taskId.
   using MessageGroup    = std::pair<std::string, int>;
   using MessageIndex    = unsigned int;
   using MessageBookends = std::map<MessageGroup, std::pair<MessageIndex, MessageIndex>>;

   std::map<int, MessageBookends> mMultiInteractionArrayMap;
};
} // namespace rv
#endif
