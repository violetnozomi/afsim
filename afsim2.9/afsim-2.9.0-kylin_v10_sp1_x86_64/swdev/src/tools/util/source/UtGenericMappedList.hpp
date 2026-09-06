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

#ifndef UTGENERICMAPPEDLIST_HPP
#define UTGENERICMAPPEDLIST_HPP

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "UtException.hpp"
#include "UtMemory.hpp"

/** A UtGenericMappedList is a container of objects (Type T), indexed by 'Id' objects (type Id).
   The implementation is parameterized by "MapType."  Existing implementations are
   UtMappedList (using a hash map), and UtStdMappedList (using std::map).
   This list maintains the 'flavor' of the standard Wsf 'lists' (e.g., WsfTrackList).
*/
template<class MapType, class T, class Id>
class UtGenericMappedList
{
   using SpecializedType = UtGenericMappedList<MapType, T, Id>;

public:
   UtGenericMappedList() = default;

   UtGenericMappedList(const SpecializedType& aSrc) { CopyMemberData(aSrc); }

   UtGenericMappedList<MapType, T, Id>& operator=(const SpecializedType& aRhs)
   {
      if (this != &aRhs)
      {
         CopyMemberData(aRhs);
      }
      return *this;
   }

   virtual ~UtGenericMappedList() = default;

   void Add(std::unique_ptr<T> aPtr); //"T" must support the GetId() interface for this method to compile.
   void AddAt(std::unique_ptr<T> aPtr, unsigned int aPos);

   void Add(std::unique_ptr<T> aPtr, const Id& aId);
   void AddAt(std::unique_ptr<T> aPtr, const Id& aId, unsigned int aPos);

   void Add(const T& aObjectRef); //"T" must support the GetId() interface for this method to compile.
   void Add(const T& aObjectRef, const Id& aId);
   void AddAt(const T& aObjectRef, const Id& aId, unsigned int aPos);

   //! Get the number of objects in the object list.
   //!   This method, along with GetEntry() can be used to iterate through
   //!   the object list.
   //!   @return The number of entries in the object list.
   //
   // NOTE: The return value used to be size_t but that was inconsistent with the argument for
   //       GetEntry, which caused warnings when compiled with 64-bit compiles where size_t !=
   //       unsigned int. In addition it is more compatible with our other classes which use
   //       unsigned int for sizes and indices.
   unsigned int GetCount() const { return static_cast<unsigned int>(mList.size()); }

   //! Get a object from the object list by its entry index (offset) into the list.
   //!   This version returns a non-const pointer to a object so the object can be modified.
   //!   @param aEntryIndex Index of the desired entry. The index must be in the range
   //!   0 <= aEntryIndex < GetCount().
   T* GetEntry(unsigned int aEntryIndex) { return mList[aEntryIndex].get(); }

   //! Get an object from the object list by its entry index (offset) into the list.
   //!   This version returns a const-pointer to the object.
   //!   @param aEntryIndex Index of the desired entry. The index must be in the range
   //!   0 <= aEntryIndex < GetCount().
   const T* GetEntry(unsigned int aEntryIndex) const { return mList[aEntryIndex].get(); }

   //! Clone this object.
   SpecializedType* Clone() const { return new SpecializedType(*this); }

   bool Delete(const Id& aId);

   void DeleteAll();

   T* Find(const Id& aId);

   const T* Find(const Id& aId) const;

   std::unique_ptr<T> Remove(const Id& aId);

private:
   void CopyMemberData(const SpecializedType& aSrc);

   std::vector<std::unique_ptr<T>> mList;
   MapType                         mIdMap;
};

// Aliases for UtGenericMappedList using specific map types
template<class T, class Id>
using UtMappedList = UtGenericMappedList<std::unordered_map<Id, T*, Id>, T, Id>;

template<class T, class Id>
using UtStdMappedList = UtGenericMappedList<std::map<Id, T*>, T, Id>;

// **************************************************************************
// Implementation
// **************************************************************************

#include <algorithm>

// private
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::CopyMemberData(const UtGenericMappedList<MapType, T, Id>& aSrc)
{
   for (const auto& mapEntry : aSrc.mIdMap)
   {
      Add(ut::make_unique<T>(*mapEntry.second), mapEntry.first);
   }
}

// virtual
//! Add an object pointer to the end of the object list.
//!  @param aPtr Pointer to the object to be added. This class instance takes ownership of the object.
//!  @param aId The id used to reference the object.
//!  @note It is the callers responsibility to ensure that another object with the
//!   same object ID does not already exist in the list.
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::Add(std::unique_ptr<T> aPtr, const Id& aId)
{
   mIdMap[aId] = aPtr.get();
   mList.push_back(std::move(aPtr));
}

// virtual
//! Add an object pointer to the object list.
//!  @param aPtr Pointer to the object to be added. This class instance takes ownership of the object.
//!  @param aId The id used to reference the object.
//!  @param aPos The new (zero-based) position of the object in the list (Defaults to the end)
//!  @note It is the callers responsibility to ensure that another object with the
//!   same object ID does not already exist in the list.
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::AddAt(std::unique_ptr<T> aPtr, const Id& aId, unsigned int aPos)
{
   mIdMap[aId] = aPtr.get();
   if (aPos >= mList.size())
   {
      mList.push_back(std::move(aPtr));
   }
   else
   {
      auto it = std::begin(mList);
      std::advance(it, aPos);
      mList.insert(it, std::move(aPtr));
   }
}

// virtual
//! Add a pointer to an object, supporting the GetId() interface, to the end of the object list.
//!  @param aPtr Pointer to the object to be added.  This class instance takes ownership of the object.
//!  @note It is the callers responsibility to ensure that another object with the
//!   same object ID does not already exist in the list.
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::Add(std::unique_ptr<T> aPtr)
{
   Add(std::move(aPtr), aPtr->GetId());
}

// virtual
//! Add an object, supporting the GetId() interface, to the end of the object list.
//!  @param aObjectRef reference to the object to be added.
//!  @param aId The id used to reference the object.
//!  @note It is the callers responsibility to ensure that another object with the
//!   same object ID does not already exist in the list.
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::Add(const T& aObjectRef)
{
   Add(aObjectRef, aObjectRef.GetId());
}

// virtual
//! Add a pointer to an object, supporting the GetId() interface, to the object list.
//!  @param aPtr Pointer to the object to be added.  This class instance takes ownership of the object.
//!  @param aPos The new (zero-based) position of the object in the list (Defaults to the end)
//!  @note It is the callers responsibility to ensure that another object with the
//!   same object ID does not already exist in the list.
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::AddAt(std::unique_ptr<T> aPtr, unsigned int aPos)
{
   AddAt(std::move(aPtr), aPtr->GetId(), aPos);
}

// virtual
//! Add an object to the end of the object list.
//!  @param aObjectRef reference to the object to be added.
//!  @param aId The id used to reference the object.
//!  @note It is the callers responsibility to ensure that another object with the
//!   same object ID does not already exist in the list.
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::Add(const T& aObjectRef, const Id& aId)
{
   Add(ut::make_unique<T>(aObjectRef), aId);
}

// virtual
//! Add an object to the object list.
//!  @param aObjectRef reference to the object to be added.
//!  @param aId The id used to reference the object.
//!  @param aPos The new (zero-based) position of the object in the list (Defaults to the end)
//!  @note It is the callers responsibility to ensure that another object with the
//!   same object ID does not already exist in the list.
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::AddAt(const T& aObjectRef, const Id& aId, unsigned int aPos)
{
   AddAt(ut::make_unique<T>(aObjectRef), aId, aPos);
}

// virtual
//! Delete the indicated object from the object list and delete the object.
//!   @param aId The objectID of the object to be deleted.
//!   @return true if requested object was found and deleted.
//!   @return false if the requested object was not found.
template<class MapType, class T, class Id>
bool UtGenericMappedList<MapType, T, Id>::Delete(const Id& aId)
{
   bool deleted = false;

   auto idIter = mIdMap.find(aId);
   if (idIter != mIdMap.end())
   {
      auto iter = std::find_if(std::begin(mList),
                               std::end(mList),
                               [idIter](const std::unique_ptr<T>& aTypePtr) { return aTypePtr.get() == idIter->second; });
      if (iter == std::end(mList))
      {
         throw UtException("Object not found in UtGenericMappedList::Delete");
      }
      mList.erase(iter);
      mIdMap.erase(idIter);
      deleted = true;
   }

   return deleted;
}

// virtual
//! Delete all objects in the list.
template<class MapType, class T, class Id>
void UtGenericMappedList<MapType, T, Id>::DeleteAll()
{
   // Clear out the container that points to all the objects
   mList.clear();
   mIdMap.clear();
}

// virtual
//! Find the requested object.
//!    This version returns a non-const pointer to the object so it can be modified.
//!    @param aId The object ID of the object to be located.
//!    @return Pointer to object or nullptr if the requested object could not be found.
template<class MapType, class T, class Id>
T* UtGenericMappedList<MapType, T, Id>::Find(const Id& aId)
{
   auto iter = mIdMap.find(aId);
   if (iter != mIdMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

// virtual
//! Find the requested .
//!    This version returns a const pointer to the object.
//!   @param aId The object ID of the  to be located.
//!   @return Pointer to object or nullptr if the requested object could not be found.
template<class MapType, class T, class Id>
const T* UtGenericMappedList<MapType, T, Id>::Find(const Id& aId) const
{
   return const_cast<SpecializedType*>(this)->Find(aId);
}

// virtual
//! Remove the indicated ID from the object list but don't delete the object itself.
//!   @param aId The object ID of the object to be removed.
//!   @return Pointer to object or nullptr if the requested object could not be found.
template<class MapType, class T, class Id>
std::unique_ptr<T> UtGenericMappedList<MapType, T, Id>::Remove(const Id& aId)
{
   std::unique_ptr<T> typePtr{nullptr};

   auto idIter = mIdMap.find(aId);
   if (idIter != mIdMap.end())
   {
      auto iter = std::find_if(std::begin(mList),
                               std::end(mList),
                               [idIter](const std::unique_ptr<T>& aTypePtr) { return aTypePtr.get() == idIter->second; });
      if (iter == mList.end())
      {
         throw UtException("Object not found in UtGenericMappedList::Remove");
      }
      typePtr = std::move(*iter);
      mList.erase(iter);
      mIdMap.erase(idIter);
   }

   return typePtr;
}

#endif
