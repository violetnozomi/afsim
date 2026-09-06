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
#ifndef UTHISTORYMAP_HPP
#define UTHISTORYMAP_HPP
#include <map>

//! The history map is like a std::map which maintains a history of previous states.
//! Modification of old states triggers a copy, otherwise copies share the same underlying structure.
template<typename KEY, typename VALUE, typename COMPARE = std::less<KEY>>
class UtHistoryMap
{
private:
   struct Change
   {
      int    mTransactionNumber;
      VALUE* mValuePtr;
   };
   typedef std::vector<Change>                   ValueSequence;
   typedef std::map<KEY, ValueSequence, COMPARE> Map;
   struct PrivateData
   {
      ~PrivateData()
      {
         for (typename Map::iterator iter = mMap.begin(); iter != mMap.end(); ++iter)
         {
            for (size_t i = 0; i < iter->second.size(); ++i)
            {
               delete iter->second[i].mValuePtr;
            }
         }
      }
      int mTransactionNumber;
      int mReferenceCount;
      Map mMap;
   };

   static Change* FindLastChange(ValueSequence& aSequence, int aTransactionNumber)
   {
      Change* rVal = 0;
      for (size_t i = 0; i < aSequence.size(); ++i)
      {
         if (aSequence[i].mTransactionNumber <= aTransactionNumber)
         {
            rVal = &aSequence[i];
         }
      }
      return rVal;
   }
   static const Change* FindLastChange(const ValueSequence& aSequence, int aTransactionNumber)
   {
      return const_cast<Change*>(FindLastChange(const_cast<ValueSequence&>(aSequence), aTransactionNumber));
   }
   PrivateData* mDataPtr;
   mutable int  mFirstTransaction;
   int          mStoredTransaction;

   UtHistoryMap(const UtHistoryMap& aRhs); // not implemented
public:
   typedef UtHistoryMap<KEY, VALUE, COMPARE> HistoryMap;
   UtHistoryMap()
   {
      mDataPtr                     = new PrivateData;
      mDataPtr->mTransactionNumber = 0;
      mDataPtr->mReferenceCount    = 1;
      mStoredTransaction           = 0;
      mFirstTransaction            = 0;
   }
   ~UtHistoryMap() { Release(); }
   //! Set this map as a copy of another.
   //! Time complexity is O(1).
   HistoryMap& operator=(const UtHistoryMap& aRhs)
   {
      if (aRhs.mDataPtr != mDataPtr)
      {
         Release();
         mDataPtr = aRhs.mDataPtr;
         ++mDataPtr->mReferenceCount;
      }
      aRhs.mFirstTransaction = mFirstTransaction = mDataPtr->mTransactionNumber;
      mStoredTransaction                         = aRhs.mStoredTransaction;
      return *this;
   }
   //! Either inserts a new value, or changes an existing value.
   //! Equivalent to std::map [aKey] = aValue;
   VALUE* Set(const KEY& aKey, const VALUE& aValue)
   {
      ReadyForChange();
      ValueSequence& seq = mDataPtr->mMap[aKey];
      seq.push_back(Change());
      Change& c            = seq.back();
      c.mTransactionNumber = ++mDataPtr->mTransactionNumber;
      c.mValuePtr          = new VALUE(aValue);
      mStoredTransaction   = c.mTransactionNumber;
      return c.mValuePtr;
   }

   //! Erase a key/value from the map.
   bool Erase(const KEY& aKey)
   {
      typename Map::iterator iter = mDataPtr->mMap.find(aKey);
      if (iter != mDataPtr->mMap.end())
      {
         if (iter->second.back().mValuePtr != 0)
         {
            if (ReadyForChange())
            {
               return Erase(aKey);
            }
            else
            {
               iter->second.push_back(Change());
               Change& c            = iter->second.back();
               c.mTransactionNumber = ++mDataPtr->mTransactionNumber;
               c.mValuePtr          = 0;
               mStoredTransaction   = c.mTransactionNumber;
               return true;
            }
         }
      }
      return false;
   }
   //! Finds a value by key.  Returns null if the key is not found.
   //! Returns a pointer to the value if it is found.
   const VALUE* Find(const KEY& aKey) const
   {
      typename Map::iterator iter = mDataPtr->mMap.find(aKey);
      if (iter != mDataPtr->mMap.end())
      {
         Change* c = FindLastChange(iter->second, mStoredTransaction);
         if (c)
         {
            return c->mValuePtr;
         }
      }
      return 0;
   }

   // Returns a value which may be modified any time prior to modifying this datastructure.
   // Returns null if the value does not exist.
   // This method avoids copying the value when possible
   VALUE* FindAndModify(const KEY& aKey)
   {
      ReadyForChange();
      typename Map::iterator iter = mDataPtr->mMap.find(aKey);
      if (iter != mDataPtr->mMap.end())
      {
         Change& lastChange = iter->second.back();
         if (lastChange.mValuePtr == 0)
            return 0;
         // If the value has been added after any new references of this datastructure,
         // then it is safe to let the user modify values.
         if (lastChange.mTransactionNumber > mFirstTransaction)
         {
            return lastChange.mValuePtr;
         }
         // Otherwise, we must copy the value.  Future calls to FindAndModify() may not need to copy.
         iter->second.push_back(Change());
         Change& newChange            = iter->second.back();
         newChange.mTransactionNumber = ++mStoredTransaction;
         newChange.mValuePtr          = new VALUE(*lastChange.mValuePtr);
         return newChange.mValuePtr;
      }
      return 0;
   }

   void Clear()
   {
      Release();
      mDataPtr                     = new PrivateData;
      mDataPtr->mTransactionNumber = 0;
      mDataPtr->mReferenceCount    = 1;
      mStoredTransaction           = 0;
      mFirstTransaction            = 0;
   }

   //! Returns 'true' if the data contained in this map is equivalent to the other map.
   bool Equal(const HistoryMap& aRhs) const
   {
      if (mDataPtr == aRhs.mDataPtr)
      {
         return mStoredTransaction == aRhs.mStoredTransaction;
      }
      const Map&                   lhsMap = mDataPtr->mMap;
      const Map&                   rhsMap = aRhs.mDataPtr->mMap;
      typename Map::const_iterator li = lhsMap.begin(), ri = rhsMap.begin();
      while (1)
      {
         if (li != lhsMap.end() && ri != rhsMap.end())
         {
            if (li->first < ri->first)
            {
               // lhs value must be erased
               const Change* c = FindLastChange(li->second, mStoredTransaction);
               if (c && c->mValuePtr != 0)
                  return false;
               ++li;
            }
            else if (ri->first < li->first)
            {
               // rhs value must be erased
               const Change* c = FindLastChange(ri->second, aRhs.mStoredTransaction);
               if (c && c->mValuePtr != 0)
                  return false;
               ++ri;
            }
            else
            {
               // lhs must equal rhs
               const Change* lc   = FindLastChange(li->second, mStoredTransaction);
               const Change* rc   = FindLastChange(ri->second, aRhs.mStoredTransaction);
               VALUE*        lval = lc ? lc->mValuePtr : 0;
               VALUE*        rval = rc ? rc->mValuePtr : 0;
               if (lval && rval)
               {
                  if (!(*lval == *rval))
                     return false;
               }
               else
               {
                  if (!lval != !rval)
                     return false;
               }
               ++li;
               ++ri;
            }
         }
         else if (li != lhsMap.end())
         {
            // lhs must be erased
            if (li->second.back().mTransactionNumber >= mStoredTransaction)
            {
               const Change* c = FindLastChange(li->second, mStoredTransaction);
               if (c && c->mValuePtr != 0)
                  return false;
            }
            ++li;
         }
         else if (ri != rhsMap.end())
         {
            // rhs must be erased
            if (ri->second.back().mTransactionNumber >= aRhs.mStoredTransaction)
            {
               const Change* c = FindLastChange(ri->second, mStoredTransaction);
               if (c && c->mValuePtr != 0)
                  return false;
            }
            ++ri;
         }
         else
         {
            break;
         }
      }
      return true;
   }

   //! Returns 'true' if both maps represent the same historic state.  If LazyEqual() is true,
   //! Equal() must be true.
   bool LazyEqual(const HistoryMap& aRhs) const
   {
      if (mDataPtr == aRhs.mDataPtr)
      {
         return mStoredTransaction == aRhs.mStoredTransaction;
      }
      return false;
   }

   //! Extracts a copy of this map.
   void Copy(std::map<KEY, VALUE, COMPARE>& aMap) const
   {
      aMap.clear();
      for (typename Map::iterator iter = mDataPtr->mMap.begin(); iter != mDataPtr->mMap.end(); ++iter)
      {
         Change* c = FindLastChange(iter->second, mStoredTransaction);
         if (c && c->mValuePtr != 0)
         {
            aMap[iter->first] = *c->mValuePtr;
         }
      }
   }

private:
   bool operator==(const HistoryMap& aRhs) const; // not implemented
protected:
   void Release()
   {
      if (0 == --mDataPtr->mReferenceCount)
      {
         delete mDataPtr;
      }
      mDataPtr = 0;
   }
   bool ReadyForChange()
   {
      if (mStoredTransaction < mDataPtr->mTransactionNumber)
      {
         PrivateData* newDataPtr = new PrivateData;
         for (typename Map::iterator iter = mDataPtr->mMap.begin(); iter != mDataPtr->mMap.end(); ++iter)
         {
            Change* cPtr = FindLastChange(iter->second, mStoredTransaction);
            if (cPtr && cPtr->mValuePtr != 0)
            {
               Change newChange    = *cPtr;
               newChange.mValuePtr = new VALUE(*newChange.mValuePtr);
               newDataPtr->mMap[iter->first].push_back(newChange);
            }
         }
         newDataPtr->mReferenceCount    = 1;
         newDataPtr->mTransactionNumber = mStoredTransaction;
         mFirstTransaction              = mStoredTransaction;
         Release();
         mDataPtr = newDataPtr;
         return true;
      }
      return false;
   }
};
#endif
