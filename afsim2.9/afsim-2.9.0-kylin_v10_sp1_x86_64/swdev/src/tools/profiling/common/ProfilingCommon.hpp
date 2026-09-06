// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef PROFILINGCOMMON_HPP
#define PROFILINGCOMMON_HPP

/// @brief Definition of utility struct and function used by the profiling targets

#include <cstdint>
#include <string>

#include "UtException.hpp"

namespace profiling
{
//! profiling exception class
class Exception : public UtException
{
public:
   Exception(const std::string& aWhat, int aErrorCode)
      : UtException(aWhat)
      , mReturnCode(aErrorCode)
   {
   }

   int GetReturnCode() const { return mReturnCode; }

private:
   //! Error code for the exception
   int mReturnCode;
};
/// @brief structure defining the values used for semantic versioning of the profiling system
struct VersionInfo
{
   //! Incrementing EPOCH version indicates a resetting of versioning system.
   //! No comparison is valid between VersionInfo instances with unmatched epochs.
   uint16_t mEpoch;
   //! incrementing MAJOR version indicates when incompatible API changes are made.
   uint16_t mMajor;
   //! incrementing MINOR version indicates when functionality is added in a backwards compatible manner.
   uint16_t mMinor;
   //! incrementing PATCH version indicates when backwards compatible bug fixes are made.
   uint16_t mPatch;
};

/// @brief Combines four uint16_t versioning values into one uint64_t. Each value takes up
///    16 bits (disjointed) in the following order [Epoch][Major][Minor][Patch].
///
/// @param aEpoch - version value which when changed indicates a reset of the versioning system.
///
/// @param aMajor - version value which when changed indicates incompatible API changes.
///
/// @param aMinor - version value which when changed indicates a change in the implementation
///    that maintains compatibility. When a library indicates a non-zero minor value, that
///    represents a minimum version of the profiling system with which the library is compatible.
///
/// @param aPatch - version value which when changed indicates bug fixes that maintain compatibility.
///
/// @returns packed uint64_t value encoding versioning values
inline uint64_t PackVersionInfo(uint16_t aEpoch, uint16_t aMajor, uint16_t aMinor, uint16_t aPatch)
{
   const uint64_t epoch = static_cast<uint64_t>(aEpoch);
   const uint64_t major = static_cast<uint64_t>(aMajor);
   const uint64_t minor = static_cast<uint64_t>(aMinor);
   const uint64_t patch = static_cast<uint64_t>(aPatch);

   return (epoch << 48) | (major << 32) | (minor << 16) | patch;
}

/// @brief Unpacks the profiling library's version information.
///
/// @param packedVersionInfo Return value of the profiling library's
///    @c AfsimProfilingInterfaceVersion function.
inline VersionInfo UnpackVersionInfo(const uint64_t packedVersionInfo)
{
   constexpr auto epochShift = 48u;
   constexpr auto majorShift = 32u;
   constexpr auto minorShift = 16u;

   constexpr uint64_t epochMask = 0xFFFF000000000000ull;
   constexpr uint64_t majorMask = 0x0000FFFF00000000ull;
   constexpr uint64_t minorMask = 0x00000000FFFF0000ull;
   constexpr uint64_t patchMask = 0x000000000000FFFFull;

   VersionInfo versionInfo;
   versionInfo.mEpoch = static_cast<uint16_t>((packedVersionInfo & epochMask) >> epochShift);
   versionInfo.mMajor = static_cast<uint16_t>((packedVersionInfo & majorMask) >> majorShift);
   versionInfo.mMinor = static_cast<uint16_t>((packedVersionInfo & minorMask) >> minorShift);
   versionInfo.mPatch = static_cast<uint16_t>(packedVersionInfo & patchMask);
   return versionInfo;
}

} // namespace profiling
#endif // PROFILINGCOMMON_HPP
