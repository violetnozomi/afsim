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

#ifndef RVMERGER_UTILS_HPP
#define RVMERGER_UTILS_HPP

#include <fstream>
#include <limits>
#include <unordered_map>
#include <vector>

#include "GenUniqueId.hpp"
#include "UtPack.hpp"
#include "UtPackSchema.hpp"

// Note: Include order dependency
#include "RvEventPipeClasses.hpp"
#include "RvEventPipeMessages.hpp"

inline bool operator==(const rv::DisEntityId& aLeft, const rv::DisEntityId& aRight)
{
   return (aLeft.site() == aRight.site()) && (aLeft.application() == aRight.application()) &&
          (aLeft.entity() == aRight.entity());
}

inline bool operator==(const rv::ApplicationUniqueId& aLeft, const rv::ApplicationUniqueId& aRight)
{
   return (aLeft.a() == aRight.a()) && (aLeft.b() == aRight.b()) && (aLeft.c() == aRight.c());
}

// Specializations for rv::DisEntityId.
namespace std
{
// For some reason, just overloading operator== for DisEntityId was not working.
template<>
struct equal_to<rv::DisEntityId>
{
   bool operator()(const rv::DisEntityId& aLeft, const rv::DisEntityId& aRight) const
   {
      return ::operator==(aLeft, aRight);
   }
};

template<>
struct hash<rv::DisEntityId>
{
   size_t operator()(const rv::DisEntityId& aValue) const
   {
      return (static_cast<size_t>(aValue.site()) << 24) + (static_cast<size_t>(aValue.application()) << 16) +
             (static_cast<size_t>(aValue.entity()));
   }
};
} // namespace std

namespace RvMerger
{
//! Initializes a serializer with details from a recording file.
void InitializeSerializer(const std::string& aFileName, UtPackSerializer& aSerializer);

//! A wrapper around a file stream that reads messages.
class RecordingFileReader
{
public:
   //! Opens the file and reads its contents.
   //! This will fill in UniqueId, but not TimeOffset.
   //! Throws if file could not be opened.
   //! Throws if no UniqueId could be found.
   RecordingFileReader(const std::string& aFileName, UtPackSerializer& aSerializer);

   //! Returns the file name.
   const std::string& GetFileName() const noexcept;

   //! Returns the recording's UniqueId.
   rv::ApplicationUniqueId GetUniqueId() const noexcept;

   //! Calculates the recording's time offset from a reference recording.
   //! Scanning always starts from the beginning of the recording.
   //! Throws if no offset could be determined.
   //! Returns the offset.
   double DetermineTimeOffset(rv::ApplicationUniqueId aReferenceId);

   //! Returns the recording's time offset from the reference recording.
   //! Returns 0 if the offset has not been calculated yet.
   double GetTimeOffset() const noexcept;

   //! Returns a pointer to the next message (the one that would be returned by PeekNext)
   //!    and moves the recording forward one message.
   //! Returns nullptr if there are no more messages.
   std::unique_ptr<rv::MsgBase> TakeNext() noexcept;

   //! Returns a pointer to the next message.
   //! If there are no more messages, returns nullptr.
   const rv::MsgBase* PeekNext() const noexcept;

   //! If the 2 recording files have the same schema, returns true.
   //! Otherwise returns false.
   bool CompareSchema(const RecordingFileReader& aReferenceRecording) const noexcept;

   //! Returns the schema string read from the beginning of the file.
   const std::string& GetSchema() const noexcept;

   //! Seeks to the beginning of events.
   void Restart() noexcept;

private:
   void DetermineUniqueId();

   std::string    mFileName;
   std::ifstream  mFileStream;
   std::streampos mBeginningOfMessageData;

   std::string       mAerSchema;
   UtPackSerializer& mSerializer;

   rv::ApplicationUniqueId      mUniqueId;
   double                       mTimeOffset;
   std::unique_ptr<rv::MsgBase> mNextMessage;
};

class RecordingFileWriter
{
public:
   RecordingFileWriter(const std::string& aFileName, const std::string& aAerSchema, UtPackSerializer& aSerializer);

   void Write(const rv::MsgBase& aMessage);

private:
   std::string   mFileName;
   std::ofstream mFileStream;

   UtPackSerializer& mSerializer;
};

//! A database for storing platform indices across multiple simulations.
//! Maps local platforms to a "final index" to be used in the constructed recording file.
//! Final indexes begin counting at 1.
class PlatformDatabase
{
public:
   explicit PlatformDatabase(size_t aSimCount);

   //! Adds a platform from a given sim, index, and DIS Id.
   //! If a platform with that sim and index already exists, does nothing.
   //! If a platform from another sim has that DIS Id, uses that platform's final index.
   //! Returns the final index.
   unsigned int Add(size_t aSimIndex, unsigned int aPlatformIndex, rv::DisEntityId aDisIndex);

   //! Adds a platform from a given sim and index.
   //! If a platform with that sim and index already exists, does nothing.
   //! Returns the final index.
   unsigned int Add(size_t aSimIndex, unsigned int aPlatformIndex);

   //! Returns the final index of a platform from a given sim and index.
   //! If not found, returns 0.
   unsigned int Find(size_t aSimIndex, unsigned int aPlatformIndex) const;

private:
   unsigned int mCounter;

   std::vector<std::unordered_map<unsigned int, unsigned int>> mData;
   std::unordered_map<rv::DisEntityId, unsigned int>           mDisIds;
};
} // namespace RvMerger

#endif
