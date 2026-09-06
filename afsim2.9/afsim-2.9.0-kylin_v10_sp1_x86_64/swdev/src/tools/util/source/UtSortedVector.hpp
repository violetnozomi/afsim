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

#ifndef UTSORTEDVECTOR_HPP
#define UTSORTEDVECTOR_HPP

#include <algorithm>
#include <vector>

// Uses a sorted vector to implement a container similar to std::map.
// Has better performance and memory usage, but should only be used if the contents rarely change.
// sort() must be called before using any lookup methods.
template<typename KEY, typename VALUE, typename PREDICATE = std::less<KEY>>
class UtSortedVector
{
public:
   typedef KEY                                  key_type;
   typedef std::pair<KEY, VALUE>                value_type;
   typedef value_type&                          reference;
   typedef const value_type&                    const_reference;
   typedef std::vector<value_type>              vector_type;
   typedef typename vector_type::iterator       iterator;
   typedef typename vector_type::const_iterator const_iterator;

   struct PredicateAdapter
   {
      PredicateAdapter(const PREDICATE& pred)
         : mPred(pred)
      {
      }
      bool operator()(const value_type& aLhs, const value_type& aRhs) const { return mPred(aLhs.first, aRhs.first); }
      bool operator()(const KEY& aLhs, const value_type& aRhs) const { return mPred(aLhs, aRhs.first); }
      bool operator()(const value_type& aLhs, const KEY& aRhs) const { return mPred(aLhs.first, aRhs); }
      PREDICATE mPred;
   };
   UtSortedVector()
      : mPredicate(PREDICATE())
   {
   }
   explicit UtSortedVector(const PREDICATE& aPred)
      : mPredicate(aPred)
   {
   }

   UtSortedVector(const UtSortedVector& aRhs)
      : mValues(aRhs.mValues)
      , mPredicate(aRhs.mPredicate)
   {
   }

   UtSortedVector& operator=(const UtSortedVector& aRhs)
   {
      if (this != &aRhs)
      {
         mPredicate = aRhs.mPredicate;
         mValues    = aRhs.mValues;
      }
      return *this;
   }

   void push_back(const KEY& k, const VALUE& v) { mValues.push_back(value_type(k, v)); }

   // Sorts the vector.  This must be called before any lookup methods.
   void sort() { std::sort(mValues.begin(), mValues.end(), mPredicate); }

   iterator find(const KEY& aKey)
   {
      iterator iter = std::lower_bound(mValues.begin(), mValues.end(), aKey, mPredicate);
      if (iter != mValues.end() && iter->first == aKey)
      {
         return iter;
      }
      return mValues.end();
   }
   const_iterator find(const KEY& aKey) const
   {
      iterator iter = std::lower_bound(mValues.begin(), mValues.end(), aKey, mPredicate);
      if (iter != mValues.end() && iter->first == aKey)
      {
         return iter;
      }
      return mValues.end();
   }
   // sort() must have been called becore executing any of these methods
   // {
   iterator lower_bound(const KEY& aKey) { return std::lower_bound(mValues.begin(), mValues.end(), aKey, mPredicate); }
   const_iterator lower_bound(const KEY& aKey) const
   {
      return std::lower_bound(mValues.begin(), mValues.end(), aKey, mPredicate);
   }
   iterator upper_bound(const KEY& aKey) { return std::upper_bound(mValues.begin(), mValues.end(), aKey, mPredicate); }
   const_iterator upper_bound(const KEY& aKey) const
   {
      return std::upper_bound(mValues.begin(), mValues.end(), aKey, mPredicate);
   }
   // }

   iterator       end() { return mValues.end(); }
   const_iterator end() const { return mValues.end(); }

   iterator       begin() { return mValues.begin(); }
   const_iterator begin() const { return mValues.begin(); }

   vector_type        values() { return mValues; }
   const vector_type& values() const { return mValues; }

   void swap(UtSortedVector& aRhs)
   {
      std::swap(mValues, aRhs.mValues);
      std::swap(mPredicate, aRhs.mPredicate);
   }

protected:
   vector_type      mValues;
   PredicateAdapter mPredicate;
};

namespace std
{
template<typename KEY, typename VALUE>
inline void swap(UtSortedVector<KEY, VALUE>& aLhs, UtSortedVector<KEY, VALUE>& aRhs)
{
   aLhs.swap(aRhs);
}
} // namespace std

#endif
