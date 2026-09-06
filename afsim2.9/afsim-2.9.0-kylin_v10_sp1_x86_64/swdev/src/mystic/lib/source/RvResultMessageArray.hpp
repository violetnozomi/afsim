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

#ifndef RVRESULTMESSAGEARRAY_HPP
#define RVRESULTMESSAGEARRAY_HPP

#include "RvEventPipeClasses.hpp"
#include "UtCircularBuffer.hpp"

namespace rv
{
// An array of messages ordered by id ( and therefore time )
class MessageBaseArray
{
public:
   using Array          = UtCircularBuffer<MsgBase*>;
   using iterator       = Array::iterator;
   using const_iterator = Array::const_iterator;

   MessageBaseArray() = default;
   /*      MessageBaseArray(const MessageBaseArray& other) = delete;
         MessageBaseArray(MessageBaseArray&& other) = delete;
         MessageBaseArray& operator=(const MessageBaseArray& other) = delete;
         MessageBaseArray& operator=(MessageBaseArray&& other) = delete;*/
   ~MessageBaseArray()
   {
      if (mBackIsPopped)
      {
         mData.back()->Unref();
      }
      if (mFrontIsPopped)
      {
         mData.front()->Unref();
      }
   }
   void use_deferred_popping() { mDeferredPopping = true; }

   void push(MsgBase* aNewValue, bool aInFront)
   {
      if (InCachedRange(aNewValue->simTime()))
      {
         mCachedValue.first  = std::make_pair(0.0, 0.0);
         mCachedValue.second = std::make_pair(nullptr, nullptr);
      }
      // singular item special case for popped items
      if (mData.size() == 1)
      {
         // Can't have two items popped when only 1 item is in list
         assert(!(mBackIsPopped && mFrontIsPopped));
         if (aInFront)
         {
            if (mBackIsPopped)
               aInFront = false;
         }
         else
         {
            if (mFrontIsPopped)
               aInFront = true;
         }
      }

      if (aInFront)
      {
         if (mFrontIsPopped)
         {
            mData.front()->Unref();
            mData.pop_front();
            mData.push_front(aNewValue);
            mFrontIsPopped = false;
            return;
         }
      }
      else
      {
         if (mBackIsPopped)
         {
            mData.back()->Unref();
            mData.pop_back();
            mData.push_back(aNewValue);
            mBackIsPopped = false;
            return;
         }
      }

      unsigned int msgIndex = aNewValue->GetMessageIndex();
      if (mData.empty())
      {
         mData.push_back(aNewValue);
         mMinTime = mMaxTime = aNewValue->simTime();
         mMinMessageIndex = mMaxMessageIndex = msgIndex;
      }
      else if (aInFront)
      {
         mMinTime         = aNewValue->simTime();
         mMinMessageIndex = msgIndex;
         mData.push_front(aNewValue);
      }
      else
      {
         assert(msgIndex > mMaxMessageIndex);
         double rTime = aNewValue->simTime();
         // this used to just be a straight assert, but the WsfExtEntityDeadReckon is not that reliable
         // I am trying five hundredths of a second threshold on it
         if (rTime < mMaxTime)
         {
            if (mMaxTime - rTime < 0.05)
            {
               rTime = mMaxTime;
            }
            else
            {
               assert(mMaxTime <= rTime);
            }
         }
         mMaxTime         = rTime;
         mMaxMessageIndex = msgIndex;
         mData.push_back(aNewValue);
      }
   }

   void push(MsgBase* aNewValue)
   {
      if (InCachedRange(aNewValue->simTime()))
      {
         mCachedValue.first  = std::make_pair(0.0, 0.0);
         mCachedValue.second = std::make_pair(nullptr, nullptr);
      }
      unsigned int msgIndex = aNewValue->GetMessageIndex();
      if (mData.empty())
      {
         mData.push_back(aNewValue);
         mMinTime = mMaxTime = aNewValue->simTime();
         mMinMessageIndex = mMaxMessageIndex = msgIndex;
      }
      else if (msgIndex > mMinMessageIndex)
      {
         assert(msgIndex > mMaxMessageIndex);
         assert(mMaxTime <= aNewValue->simTime());
         mMaxTime         = aNewValue->simTime();
         mMaxMessageIndex = msgIndex;
         mData.push_back(aNewValue);
      }
      else
      {
         mMinTime         = aNewValue->simTime();
         mMinMessageIndex = msgIndex;
         mData.push_front(aNewValue);
      }
   }

   void pop(MsgBase* aRemovedValue, bool aAtFront)
   {
      if (InCachedRange(aRemovedValue->simTime(), true))
      {
         mCachedValue.first  = std::make_pair(0.0, 0.0);
         mCachedValue.second = std::make_pair(nullptr, nullptr);
      }
      if (!mDeferredPopping)
         return popP(aRemovedValue, aAtFront);

      if (aAtFront)
      {
         if (mFrontIsPopped)
         {
            mData.front()->Unref();
            popP(mData.front(), true);
            mData.front()->Ref();
         }
         else
         {
            mData.front()->Ref();
            mFrontIsPopped = true;
         }
      }
      else
      {
         if (mBackIsPopped)
         {
            mData.back()->Unref();
            popP(mData.back(), false);
            mData.back()->Ref();
         }
         else
         {
            mData.back()->Ref();
            mBackIsPopped = true;
         }
      }
   }
   struct ComparePred
   {
      bool operator()(const MsgBase* aMessage, float aTime) const { return aMessage->simTime() > aTime; }
      bool operator()(float aTime, const MsgBase* aMessage) const { return aTime < aMessage->simTime(); }
   };

   struct CompareMessageTime
   {
      bool operator()(const MsgBase* aMessage, float aTime) const { return aMessage->simTime() < aTime; }
      bool operator()(float aTime, const MsgBase* aMessage) const { return aTime < aMessage->simTime(); }
   };
   struct CompareMessageId
   {
      bool operator()(const MsgBase* aMessage, unsigned int aMessageId) const
      {
         return aMessage->GetMessageIndex() < aMessageId;
      }
      bool operator()(unsigned int aMessageId, const MsgBase* aMessage) const
      {
         return aMessageId < aMessage->GetMessageIndex();
      }
   };

   MsgBase* FindFirstBefore(float aTime) const
   {
      if (!InCachedRange(aTime))
      {
         UpdateCache(aTime);
      }
      return mCachedValue.second.first;
   }

   MsgBase* FindFirstAfter(float aTime) const
   {
      if (!InCachedRange(aTime))
      {
         UpdateCache(aTime);
      }
      return mCachedValue.second.second;
   }

   const_iterator FindFirstIteratorBefore(float aTime) const
   {
      Array::const_iterator i = std::upper_bound(mData.begin(), mData.end(), aTime, ComparePred());
      if (i != mData.begin())
      {
         --i;
         return i;
      }
      return mData.end();
   }
   // TODO: implement a faster version...
   const_iterator FindFirstIteratorAfter(float aTime) const
   {
      if (mData.empty())
         return mData.end();
      const_iterator i = mData.end();
      --i;
      while (true)
      {
         if (aTime < (*i)->simTime())
         {
            if (i == mData.begin())
            {
               break;
            }
            --i;
         }
         else
         {
            ++i;
            break;
         }
      }
      return i;
   }

   const_iterator FindById(unsigned int aMessageId) const
   {
      if (mData.empty())
         return mData.end();
      const_iterator iter = std::lower_bound(mData.begin(), mData.end(), aMessageId, CompareMessageId());
      if (iter != mData.end())
      {
         if ((*iter)->GetMessageIndex() != aMessageId)
         {
            return mData.end();
         }
      }
      return iter;
   }

   const_iterator LowerBoundById(unsigned int aMessageId) const
   {
      if (mData.empty())
         return mData.end();
      return std::lower_bound(mData.begin(), mData.end(), aMessageId, CompareMessageId());
   }

   const_iterator LowerBoundByTime(float aTime) const
   {
      if (mData.empty())
         return mData.end();
      return std::lower_bound(mData.begin(), mData.end(), aTime, CompareMessageTime());
   }
   const_iterator UpperBoundByTime(float aTime) const
   {
      if (mData.empty())
         return mData.end();
      return std::upper_bound(mData.begin(), mData.end(), aTime, CompareMessageTime());
   }
   const_iterator begin() const { return mData.begin(); }
   const_iterator end() const { return mData.end(); }

   size_t size() const { return mData.size(); }
   void   clear()
   {
      mCachedValue.second = std::make_pair(nullptr, nullptr);
      mCachedValue.first  = std::make_pair(0.0, 0.0);
      mData.clear();
   }
   bool InCachedRange(double aTime, bool aEndInclusive = false) const
   {
      if ((mCachedValue.first.first == mCachedValue.first.second) && (mCachedValue.first.first == 0.0F))
      {
         return false; // no data
      }
      if (mCachedValue.first.first <= aTime)
      {
         if (aEndInclusive && ((mCachedValue.first.second >= aTime)))
         {
            return true;
         }
         else if (!aEndInclusive && ((mCachedValue.first.second > aTime)))
         {
            return true;
         }
      }
      return false;
   }
   void UpdateCache(double aTime) const
   {
      if (!mData.empty())
      {
         Array::const_iterator beginIter = mData.begin();
         Array::const_iterator endIter   = mData.end();
         Array::const_iterator i         = std::upper_bound(beginIter, endIter, aTime, ComparePred());
         if (i != beginIter)
         {
            std::pair<float, float> range;
            Array::const_iterator   j = i;
            --j;
            if (i != mData.end()) // we found messages surrounding the time queried
            {
               mCachedValue.first  = std::make_pair((*j)->simTime(), (*i)->simTime());
               mCachedValue.second = std::make_pair(*j, *i);
            }
            else // we found a message before the time query, but nothing after
            {
               mCachedValue.first  = std::make_pair((*j)->simTime(), std::numeric_limits<float>::max());
               mCachedValue.second = std::make_pair(*j, nullptr);
            }
         }
         else
         {
            if (i != endIter) // we found a message after the time query, but nothing before
            {
               mCachedValue.first  = std::make_pair(0.0F, (*i)->simTime());
               mCachedValue.second = std::make_pair(nullptr, *i);
            }
            else // nothing, empty...
            {
               mCachedValue.first  = std::make_pair(0.0F, std::numeric_limits<float>::max());
               mCachedValue.second = std::make_pair(nullptr, nullptr);
            }
         }
      }
      else
      {
         mCachedValue.first  = std::make_pair(0.0F, std::numeric_limits<float>::max());
         mCachedValue.second = std::make_pair(nullptr, nullptr);
      }
   }

   const MsgBase* front() const { return mData.front(); }
   const MsgBase* back() const { return mData.back(); }
   bool           empty() const { return mData.empty(); }

   float GetMinTime() const { return mMinTime; }
   float GetMaxTime() const { return mMaxTime; }

   unsigned int GetMinMessageIndex() const { return mMinMessageIndex; }
   unsigned int GetMaxMessageIndex() const { return mMaxMessageIndex; }

   MsgBase* operator[](unsigned int aIndex) { return mData[aIndex]; }

protected:
   void popP(MsgBase* aRemovedValue, bool aAtBegin)
   {
      if (aAtBegin)
      {
         assert(mData.front() == aRemovedValue);
         mData.pop_front();
         if (!mData.empty())
         {
            mMinTime         = mData.front()->simTime();
            mMinMessageIndex = mData.front()->GetMessageIndex();
         }
      }
      else
      {
         assert(mData.back() == aRemovedValue);
         mData.pop_back();
         if (!mData.empty())
         {
            mMaxTime         = mData.back()->simTime();
            mMaxMessageIndex = mData.back()->GetMessageIndex();
         }
      }
   }

   float        mMinTime{0.0F};
   float        mMaxTime{0.0F};
   unsigned int mMinMessageIndex{0};
   unsigned int mMaxMessageIndex{0};
   // in 'deferred popping' mode, we keep around one message at the front and back at all times
   // after two pops, the message is finally removed.
   // Reference counts are assumed to be managed for us in the normal case,
   // but in deferred popping mode, the deferred popping item needs to be add-ref'd
   // to prevent it from being deleted prematurely.
   bool                                                                      mDeferredPopping{false};
   bool                                                                      mFrontIsPopped{false};
   bool                                                                      mBackIsPopped{false};
   Array                                                                     mData;
   mutable std::pair<std::pair<float, float>, std::pair<MsgBase*, MsgBase*>> mCachedValue;
};

// Wrapper around MessageBaseArray for type safety
template<typename T>
class MessageArray : public MessageBaseArray
{
public:
   MessageArray() {}
   void push(T* aNewValue) { MessageBaseArray::push(aNewValue); }
   void push(T* aNewValue, bool aAtBegin) { MessageBaseArray::push(aNewValue, aAtBegin); }
   void pop(T* aRemovedValue) { MessageBaseArray::pop(aRemovedValue); }
   void pop(T* aNewValue, bool aAtBegin) { MessageBaseArray::pop(aNewValue, aAtBegin); }
   T*   FindFirstBefore(float aTime) const { return (T*)MessageBaseArray::FindFirstBefore(aTime); }
   T*   FindFirstAfter(float aTime) const { return (T*)MessageBaseArray::FindFirstAfter(aTime); }

   const T* front() const { return (T*)mData.front(); }
   const T* back() const { return (T*)mData.back(); }
   const T* operator[](size_t aIndex) const { return (const T*)mData[aIndex]; }
   bool     empty() const { return mData.empty(); }
};
} // namespace rv

#endif
