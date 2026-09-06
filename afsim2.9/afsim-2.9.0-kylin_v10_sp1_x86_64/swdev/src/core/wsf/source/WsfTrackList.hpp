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

#ifndef WSFTRACKLIST_HPP
#define WSFTRACKLIST_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include "UtScriptAccessible.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"

/** A 'Track List' is a container for a list of 'Track' objects.
   This class is templatized; WsfTrackListFwd.hpp includes the
   following standard track lists (typedefs):
   <ul>
   <li>  WsfTrackList      (type is WsfTrack)
   <li>  WsfLocalTrackList (type is WsfLocalTrack)
   </ul>
   Please "#include" this file (WsfTrackListFwd.hpp) in header files
   where you only need pointers or references to either of these track list
   types.  "#include" this file (WsfTrackList.hpp) wherever the track lists
   themselves are declared or defined.
*/
template<class T>
class WsfTrackListT
{
public:
   WsfTrackListT() = default;

   WsfTrackListT(const WsfTrackListT<T>& aSrc);
   WsfTrackListT<T>& operator=(const WsfTrackListT<T>& aRhs) = delete;
   virtual ~WsfTrackListT()                                  = default;

   void AddTrack(std::unique_ptr<T> aTrackPtr);

   //! Get the number of tracks in the track list.
   //!   This method, along with GetTrackEntry() can be used to iterate through
   //!   the track list.
   //!   @return The number of entries in the track list.
   unsigned int GetTrackCount() const { return static_cast<unsigned int>(mTrackList.size()); }

   //! Get a track from the track list by its entry index (offset) into the list.
   //!   This version returns a non-const pointer to a track so the track can be modified.
   //!   @param aEntryIndex Index of the desired entry. The index must be in the range
   //!   0 <= aEntryIndex < GetTrackCount().
   T* GetTrackEntry(unsigned int aEntryIndex) { return mTrackList[aEntryIndex].get(); }

   //! Get a track from the track list by its entry index (offset) into the list.
   //!   This version returns a const-pointer to the track.
   //!   @param aEntryIndex Index of the desired entry. The index must be in the range
   //!   0 <= aEntryIndex < GetTrackCount().
   const T* GetTrackEntry(unsigned int aEntryIndex) const { return mTrackList[aEntryIndex].get(); }

   //! Clone this object.
   WsfTrackListT<T>* Clone() const { return new WsfTrackListT<T>(*this); }

   bool DeleteTrack(const WsfTrackId& aTrackId);

   unsigned int DeleteTrackEntry(unsigned int aEntryIndex);

   void DeleteAllTracks();

   T* FindTrack(const WsfTrackId& aTrackId);

   const T* FindTrack(const WsfTrackId& aTrackId) const;

   std::unique_ptr<T> RemoveTrack(const WsfTrackId& aTrackId);

private:
   using TrackIdMap = std::unordered_map<WsfTrackId, T*, WsfTrackId>;

   std::vector<std::unique_ptr<T>> mTrackList;
   TrackIdMap                      mTrackIdMap;
};

using WsfTrackList      = WsfTrackListT<WsfTrack>;
using WsfLocalTrackList = WsfTrackListT<WsfLocalTrack>;

template<>
constexpr const char* UtScriptClassName<WsfTrackList>()
{
   return "WsfTrackList";
}
template<>
constexpr const char* UtScriptClassName<WsfLocalTrackList>()
{
   return "WsfLocalTrackList";
}

// **************************************************************************
// Implementation
// **************************************************************************

#include <algorithm>
#include <cassert>

template<class T>
WsfTrackListT<T>::WsfTrackListT(const WsfTrackListT<T>& aSrc)
{
   for (const auto& trackPtr : aSrc.mTrackList)
   {
      AddTrack(ut::clone(trackPtr));
   }
}

// virtual
/** Add a track to the track list.
  @param aTrackPtr Pointer to the track to be added.  This class takes ownership
   of the track object.
  @note It is the callers responsibility to ensure that another track with the
   same track ID does not already exist in the list.
*/
template<class T>
void WsfTrackListT<T>::AddTrack(std::unique_ptr<T> aTrackPtr)
{
   mTrackIdMap[aTrackPtr->GetTrackId()] = aTrackPtr.get();
   mTrackList.push_back(std::move(aTrackPtr));
}

// virtual
/** Delete the indicated track from the track list and delete the track object.
   @param aTrackId The track ID of the track to be deleted.
   @return true if requested track was found and deleted.
   @return false if the requested track was not found.
*/
template<class T>
bool WsfTrackListT<T>::DeleteTrack(const WsfTrackId& aTrackId)
{
   bool trackDeleted = false;

   // Find the track, remove its entry in the track list and delete the track object.
   auto trackIter =
      std::find_if(mTrackList.begin(),
                   mTrackList.end(),
                   [aTrackId](const std::unique_ptr<T>& aTrackPtr) { return aTrackPtr->GetTrackId() == aTrackId; });

   // Search the track id map before the track is deleted, as the track id may be part of the deleted track.
   auto idIter = mTrackIdMap.find(aTrackId);

   if (trackIter != mTrackList.end())
   {
      mTrackList.erase(trackIter);
      trackDeleted = true;
   }

   // Find and remove the entry from the track ID map.
   // The assertions are to check for consistency between the track list and the ID map.
   if (idIter != mTrackIdMap.end())
   {
      mTrackIdMap.erase(idIter);
      assert(trackDeleted);
   }
   else
   {
      assert(!trackDeleted);
   }
   return trackDeleted;
}

// virtual
/** Delete all the tracks in the track list.
 */
template<class T>
void WsfTrackListT<T>::DeleteAllTracks()
{
   // Clear out the container that points to all the tracks
   mTrackList.clear();
   mTrackIdMap.clear();
}

// virtual
/** Find the requested track.
    This version returns a non-const pointer to the track so it can be modified.
    @param aTrackId The track ID of the track to be located.
    @return Pointer to track or nullptr if the requested track could not be found.
*/
template<class T>
T* WsfTrackListT<T>::FindTrack(const WsfTrackId& aTrackId)
{
   auto iter = mTrackIdMap.find(aTrackId);
   if (iter != mTrackIdMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

// virtual
/** Find the requested track.
    This version returns a const pointer to the track.
   @param aTrackId The track ID of the track to be located.
   @return Pointer to track or nullptr if the requested track could not be found.
*/
template<class T>
const T* WsfTrackListT<T>::FindTrack(const WsfTrackId& aTrackId) const
{
   return const_cast<WsfTrackListT<T>*>(this)->FindTrack(aTrackId);
}

// virtual
/** Remove the indicated track from the track list but don't delete the track object itself.
   @param aTrackId The track ID of the track to be removed.
   @return Pointer to track or nullptr if the requested track could not be found.
*/
template<class T>
std::unique_ptr<T> WsfTrackListT<T>::RemoveTrack(const WsfTrackId& aTrackId)
{
   std::unique_ptr<T> trackPtr{nullptr};

   // Find the track and remove its entry in the track list.
   auto trackIter =
      std::find_if(mTrackList.begin(),
                   mTrackList.end(),
                   [aTrackId](const std::unique_ptr<T>& aTrackPtr) { return aTrackPtr->GetTrackId() == aTrackId; });
   if (trackIter != mTrackList.end())
   {
      trackPtr = std::move(*trackIter);
      mTrackList.erase(trackIter);
   }

   // Find and remove the entry from the track ID map.
   // The assertions are to check for consistency between the track list and the ID map.
   auto idIter = mTrackIdMap.find(aTrackId);
   if (idIter != mTrackIdMap.end())
   {
      mTrackIdMap.erase(idIter);
      assert(trackPtr != nullptr);
   }
   else
   {
      assert(trackPtr == nullptr);
   }

   return trackPtr;
}

//! Delete a track entry from the track list, returning the next valid entry.
//! If there is no next valid entry, or if the entry to be deleted is out-of-bounds,
//! the value of GetTrackCount() will be returned.
template<class T>
unsigned int WsfTrackListT<T>::DeleteTrackEntry(unsigned int aEntryIndex)
{
   unsigned int nextIndex = static_cast<unsigned int>(mTrackList.size());
   if (aEntryIndex < static_cast<unsigned int>(mTrackList.size()))
   {
      // Directly access the desired element, eliminating the overhead of the find_if call in DeleteTrack.
      auto trackIter = mTrackList.begin();
      std::advance(trackIter, aEntryIndex);
      assert(trackIter != mTrackList.end());

      // Find the entry in the track ID map.
      auto idIter = mTrackIdMap.find((*trackIter)->GetTrackId());
      assert(idIter != mTrackIdMap.end());

      // Remove the track from the track list and from the id map.
      DeleteTrack(idIter->first);

      // If this was the last entry in the list, return "end" as the next element.
      if (aEntryIndex != static_cast<unsigned int>(mTrackList.size()))
      {
         nextIndex = aEntryIndex;
      }
   }
   return nextIndex;
}

#endif
