// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVRESULTPLATFORM_HPP
#define RVRESULTPLATFORM_HPP

#include <map>
#include <memory>
#include <string>

#include <QMutex>

#include "RvEventPipeMessages.hpp"
#include "RvExport.hpp"
#include "RvMilEventPipeClasses.hpp"
#include "RvResultMessageArray.hpp"
#include "UtEntity.hpp"
#include "UtException.hpp"
#include "UtOptional.hpp"

namespace rv
{
// Manages reference counting and locking
class ResultObject
{
public:
   void BeginRead() { mLock.lock(); }
   void EndRead() { mLock.unlock(); }
   void BeginWrite() { mLock.lock(); }
   void EndWrite() { mLock.unlock(); }

protected:
   virtual ~ResultObject() = default;
   ResultObject()
      : mLock(QMutex::Recursive)
   {
   }

private:
   QMutex mLock;
};

// Container for data related to a platform
class RV_EXPORT ResultPlatform : public ResultObject
{
public:
   friend class ResultDb;

   ResultPlatform(const std::string& aName, unsigned int aPlatformIndex, double aTime);

   ~ResultPlatform() override = default;

   unsigned int       GetPlatformIndex() const { return mPlatformIndex; }
   const std::string& GetName() const { return mName; }

   //////////////////////////////////////////////////////////////////////////
   // These methods require a lock before calling
   //////////////////////////////////////////////////////////////////////////

   void AddBaseMessage(MsgBase* aMsg, bool aAtBegin);
   void RemoveBaseMessage(MsgBase* aMsg, bool aAtBegin);
   void AddStringKeyedMessage(MsgBase* aMsg, const std::string& aKey, bool aAtBegin);
   void RemoveStringKeyedMessage(MsgBase* aMsg, const std::string& aKey, bool aAtBegin);

   void AddPartMessage(MsgBase* aMsg, char aPartType, const std::string& aPartName, bool aAtBegin);
   void RemovePartMessage(MsgBase* aMsg, char aPartType, const std::string& aPartName, bool aAtBegin);


   const MsgEmitterModeDefinition* FindSensorMode(const std::string& aModeName, const std::string& aSensorName) const;
   const MsgEmitterModeDefinition* FindWeaponMode(const std::string& aModeName, const std::string& aWeaponName) const;

   void AddSensorModeDefinition(MsgEmitterModeDefinition* aDefPtr);

   void AddWeaponModeDefinition(MsgEmitterModeDefinition* aDefPtr);

   // 2 is tracked
   // 1 is detected
   // 0 is no vulnerability state
   int GetStartState();

   using BaseMessageArray = MessageArray<MsgBase>;
   class MessageMap
   {
   public:
      void Add(MsgBase* aMessage, bool aAtBegin) { mMap[aMessage->GetMessageId()].push(aMessage, aAtBegin); }
      void Remove(MsgBase* aMessage, bool aAtBegin) { mMap[aMessage->GetMessageId()].pop(aMessage, aAtBegin); }
      void UseDeferredPopping(int aMessageId) { mMap[aMessageId].use_deferred_popping(); }
      bool HasMessageOfType(int aMessageId) const;
      template<class T>
      const rv::ResultPlatform::BaseMessageArray* GetArray();
      template<class T>
      bool HasData() const;
      template<class T>
      const T* FindFirstBefore(double aTime) const;
      template<class T>
      const T* FindFirstAfter(double aTime) const;

   private:
      std::map<int, BaseMessageArray> mMap;
   };

   using StringKeyedMessageMap = std::map<int, std::map<std::string, BaseMessageArray>>;

   class PartMap
   {
   public:
      PartMap()  = default;
      ~PartMap() = default;

      void AddMessage(MsgBase* aMessage, rv::PartType::Value aPartType, const std::string& aPartName, bool aAtBegin);
      void RemoveMessage(MsgBase* aMessage, rv::PartType::Value aPartType, const std::string& aPartName, bool aAtBegin);
      using NameMap = std::map<std::string, MessageMap>;
      using TypeMap = std::map<rv::PartType::Value, NameMap>;

      const NameMap* GetParts(rv::PartType::Value& aType) const;

      // this iterator lets us traverse the multimap with a single iterator
      class GeneralPartIterator
      {
      public:
         GeneralPartIterator(const TypeMap& aContainer, bool aEnd = false)
            : mContainer(aContainer)
            , mIterator1(aContainer.begin())
         {
            if (aEnd)
            {
               mIterator1 = mContainer.end();
               mIterator2.reset();
            }
            else
            {
               mIterator1 = aContainer.begin();
               if (mIterator1 != aContainer.end())
               {
                  mIterator2 = mIterator1->second.begin();
                  if (mIterator2 == mIterator1->second.end())
                  {
                     mIterator2.reset();
                  }
               }
               else
               {
                  mIterator2.reset();
               }
            }
         }
         bool operator!=(const GeneralPartIterator& aRHS)
         {
            return (mIterator1 != aRHS.mIterator1) || (mIterator2 != aRHS.mIterator2);
         }
         GeneralPartIterator& operator++()
         {
            if (mIterator2.has_value())
            {
               mIterator2.value()++;
               if (mIterator2 == mIterator1->second.end())
               {
                  mIterator1++;
                  if (mIterator1 == mContainer.end())
                  {
                     mIterator2.reset();
                  }
                  else
                  {
                     mIterator2 = mIterator1->second.begin();
                  }
               }
            }
            return *this;
         }

         const MessageMap& operator*() const
         {
            if (!mIterator2.has_value())
            {
               throw(UtException("Dereferencing an invalid GeneralPartIterator"));
            }
            return mIterator2.value()->second;
         }

         const MessageMap* operator->()
         {
            if (mIterator2.has_value())
            {
               return &(mIterator2.value()->second);
            }
            else
            {
               return nullptr;
            }
         }

      private:
         const TypeMap&                        mContainer;
         TypeMap::const_iterator               mIterator1;
         ut::optional<NameMap::const_iterator> mIterator2;
      };

      GeneralPartIterator begin() const { return GeneralPartIterator(mPartMap); }
      GeneralPartIterator end() const { return GeneralPartIterator(mPartMap, true); }

   private:
      TypeMap mPartMap;
   };

   using DetectionAttemptArray = MessageArray<MsgDetectAttempt>;
   using DetectionChangeArray  = MessageArray<MsgSensorDetectionChange>;
   using DetectionChangeIndex  = std::pair<int, std::string>;
   using TrackChangeIndex      = std::pair<int, int>;
   using SensorModeMap       = std::map<std::pair<std::string, std::string>, std::unique_ptr<MsgEmitterModeDefinition>>;
   using WeaponModeMap       = std::map<std::pair<std::string, std::string>, std::unique_ptr<MsgEmitterModeDefinition>>;
   using DetectionAttemptMap = std::map<std::string, std::map<int, DetectionAttemptArray>>;

   // returns the state changes that occur in the requested range
   // also files aStartState with the state at aStart
   std::map<float, int> GetStateChanges(float aStart, float aStop, int& aStartState);

   const DetectionAttemptMap& GetDetectionAttemptMap() const { return mDetectionAttemptMap; }
   bool                       HasMessageOfType(int aMessageType) { return mMessageMap.HasMessageOfType(aMessageType); }

   template<class T>
   const T* FindFirstBefore(double aTime) const
   {
      return mMessageMap.FindFirstBefore<T>(aTime);
   }

   template<class T>
   const T* FindFirstAfter(double aTime) const
   {
      return mMessageMap.FindFirstAfter<T>(aTime);
   }

   template<class T>
   const T* FindFirstBefore(double aTime, const std::string& aKey) const;

   template<class T>
   bool HasData() const
   {
      return mMessageMap.HasData<T>();
   }

   template<class T>
   const BaseMessageArray* GetArray()
   {
      return mMessageMap.GetArray<T>();
   }

   template<class T>
   const BaseMessageArray* GetArray(const std::string& aKey) const;

   template<class T>
   const std::map<std::string, rv::ResultPlatform::BaseMessageArray>* GetStringKeyedMap() const;

   const double GetStartTime() const { return mStartTime; }
   const double GetEndTime() const { return mEndTime; }
   bool         IsActive(double aTime) const;

   const PartMap&          GetParts() const { return mPartMap; }
   const PartMap::NameMap* GetParts(rv::PartType::Value aType) const { return mPartMap.GetParts(aType); }

private:
   void AddMessage(MsgDetectAttempt* aMsg, bool aAtBegin);
   void RemoveMessage(MsgDetectAttempt* aMsg, bool aAtBegin);

   void AddMessage(MsgPlatformStatus* aMsg, bool aAtBegin);

   void AddStartState(MsgSensorDetectionChange* aMsg);
   void RemoveStartState(MsgSensorDetectionChange* aMsg);

   void AddStartState(MsgSensorTrackCreated* aMsg);
   void RemoveStartState(MsgSensorTrackCreated* aMsg);

   void AddMessage(MsgSensorDetectionChange* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorDetectionChange* aMsg, bool aAtBegin);

   void AddMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorTrackCreated* aMsg, bool aAtBegin);

   void AddMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin);
   void RemoveMessage(MsgSensorTrackDrop* aMsg, bool aAtBegin);

   void AddMessage(MsgWeaponFired* aMsg);
   void AddMessage(MsgWeaponTerminated* aMsg);

   // here we hold messages that don't sort into additional categories
   MessageMap mMessageMap;
   // these messages are sorted by a string
   StringKeyedMessageMap mStringKeyedMessageMap;
   // these messages are sorted by a part id... usually a general component
   PartMap mPartMap;

   unsigned int mChangeNumber;

   unsigned int mPlatformIndex;
   std::string  mName;

   DetectionAttemptMap                                      mDetectionAttemptMap;
   DetectionChangeArray                                     mDetectedChangeArray;
   BaseMessageArray                                         mTrackedArray;
   std::map<double, int>                                    mFiredStateMap;
   std::map<DetectionChangeIndex, MsgSensorDetectionChange> mDetectChangeStartState;
   std::map<TrackChangeIndex, MsgSensorTrackCreated>        mTrackChangeStartState;
   SensorModeMap                                            mSensorModeDefinition;
   WeaponModeMap                                            mWeaponModeDefinition;
   double                                                   mStartTime;
   double                                                   mEndTime{-1.0};
};

template<class T>
const T* rv::ResultPlatform::MessageMap::FindFirstBefore(double aTime) const
{
   static_assert(std::is_base_of<rv::MsgBase, T>::value, "Template type must be a subclass of rv::MsgBase");

   auto messageMapEntry = mMap.find(T::cMESSAGE_ID);
   if (messageMapEntry != mMap.end())
   {
      return dynamic_cast<T*>(messageMapEntry->second.FindFirstBefore(aTime));
   }
   return nullptr;
}

template<class T>
const T* rv::ResultPlatform::MessageMap::FindFirstAfter(double aTime) const
{
   static_assert(std::is_base_of<rv::MsgBase, T>::value, "Template type must be a subclass of rv::MsgBase");

   auto messageMapEntry = mMap.find(T::cMESSAGE_ID);
   if (messageMapEntry != mMap.end())
   {
      return dynamic_cast<T*>(messageMapEntry->second.FindFirstAfter(aTime));
   }
   return nullptr;
}

template<class T>
bool rv::ResultPlatform::MessageMap::HasData() const
{
   static_assert(std::is_base_of<rv::MsgBase, T>::value, "Template type must be a subclass of rv::MsgBase");

   auto messageMapEntry = mMap.find(T::cMESSAGE_ID);
   if (messageMapEntry != mMap.end())
   {
      return !messageMapEntry->second.empty();
   }
   return false;
}

template<class T>
const rv::ResultPlatform::BaseMessageArray* rv::ResultPlatform::MessageMap::GetArray()
{
   static_assert(std::is_base_of<rv::MsgBase, T>::value, "Template type must be a subclass of rv::MsgBase");

   auto messageMapEntry = mMap.find(T::cMESSAGE_ID);
   if (messageMapEntry != mMap.end())
   {
      return &messageMapEntry->second;
   }
   return nullptr;
}

template<class T>
const T* rv::ResultPlatform::FindFirstBefore(double aTime, const std::string& aKey) const
{
   static_assert(std::is_base_of<rv::MsgBase, T>::value, "Template type must be a subclass of rv::MsgBase");

   auto keyedMapEntry = mStringKeyedMessageMap.find(T::cMESSAGE_ID);
   if (keyedMapEntry != mStringKeyedMessageMap.end())
   {
      auto messageMapEntry = keyedMapEntry->second.find(aKey);
      if (messageMapEntry != keyedMapEntry->second.end())
      {
         return dynamic_cast<T*>(messageMapEntry->second.FindFirstBefore(aTime));
      }
   }
   return nullptr;
}
} // namespace rv

template<class T>
const rv::ResultPlatform::BaseMessageArray* rv::ResultPlatform::GetArray(const std::string& aKey) const
{
   static_assert(std::is_base_of<rv::MsgBase, T>::value, "Template type must be a subclass of rv::MsgBase");

   auto keyedMapEntry = mStringKeyedMessageMap.find(T::cMESSAGE_ID);
   if (keyedMapEntry != mStringKeyedMessageMap.end())
   {
      auto dataArray = keyedMapEntry->second.find(aKey);
      if (dataArray != keyedMapEntry->second.end())
      {
         return &(dataArray->second);
      }
   }
   return nullptr;
}

template<class T>
const std::map<std::string, rv::ResultPlatform::BaseMessageArray>* rv::ResultPlatform::GetStringKeyedMap() const
{
   static_assert(std::is_base_of<rv::MsgBase, T>::value, "Template type must be a subclass of rv::MsgBase");

   auto keyedMapEntry = mStringKeyedMessageMap.find(T::cMESSAGE_ID);
   if (keyedMapEntry != mStringKeyedMessageMap.end())
   {
      return &keyedMapEntry->second;
   }
   return nullptr;
}

#endif
