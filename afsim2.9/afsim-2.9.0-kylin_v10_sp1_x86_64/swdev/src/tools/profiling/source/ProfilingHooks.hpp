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

#ifndef PROFILINGHOOKS_HPP
#define PROFILINGHOOKS_HPP

/// @brief Interface required by a shared library
///    that implements profiling "hooks" for the profiling system.
///
/// For the interface that starts and stops the entire profiling system,
/// please see ProfilingSystem.hpp.
/// For the interface that identifies a region to profile,
/// please see ProfilingRegion.hpp.
///
/// The profiling system loads a shared library at run time.
/// That may be the AFPerf library that AFSIM builds and installs,
/// or it may be an alternate library.
/// The shared library must contain the five symbols
/// which are the string constants defined in this header file.
/// Loading a symbol must result in a function pointer
/// that is compatible with the corresponding function pointer alias
/// in this header file.
///
/// Using profiling hooks with C89-style parameters
/// (e.g., no @a std::string or other C++ types)
/// avoids potential issues with ABI incompatibility,
/// due to different compiler versions or C++ language versions.
/// The hope is that an alternate profiling library need not be built
/// with the same compiler version as AFSIM.
/// The profiling library should still be built
/// with the same compiler brand (e.g., GCC or MSVC) as AFSIM.

#include "profiling_export.h"

#include <cstdint>
#include <string>

#include <UtOptional.hpp>

#include "ProfilingCommon.hpp"

namespace profiling
{
/// @brief Type of a callback for getting the version number
///    of the profiling interface that the library provides.
///
/// Loading the @a cPROFILING_INTERFACE_VERSION_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using ProfilingInterfaceVersionFunc = uint64_t (*)();

/// @brief Name of symbol (to load from shared library) for the callback
///    that gets the version number of the profiling interface.
constexpr const char cPROFILING_INTERFACE_VERSION_SYMBOL[] = "AfsimProfilingInterfaceVersion";

/// Type of a callback for initializing the profiling library.
///
/// Loading the @a cPROFILING_INITIALIZE_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using ProfilingInitializeFunc = int32_t (*)(const char*);

/// @brief Name of symbol (to load from shared library) for the callback
///    that initializes the profiling library.
constexpr const char cPROFILING_INITIALIZE_SYMBOL[] = "AfsimProfilingInitialize";

/// Type of a callback for beginning a profiling region.
///
/// Loading the @a cPROFILING_BEGIN_REGION_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using BeginProfilingRegionFunc = void (*)(const char*);

/// @brief Name of symbol (to load from shared library) for the callback
///    that begins a profiling region.
constexpr const char cPROFILING_BEGIN_REGION_SYMBOL[] = "AfsimProfilingBeginRegion";

/// Type of a callback for ending a profiling region that was previously begun.
///
/// Loading the @a cPROFILING_END_REGION_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using EndProfilingRegionFunc = void (*)();

/// @brief Name of symbol (to load from shared library) for the callback
///    that ends a profiling region that was previously begun.
constexpr const char cPROFILING_END_REGION_SYMBOL[] = "AfsimProfilingEndRegion";

/// Type of a callback for creating a profiling section.
///
/// Loading the @a cPROFILING_CREATE_SECTION_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using CreateProfilingSectionFunc = uint32_t (*)(const char*);

/// @brief Name of symbol (to load from shared library) for the callback
///    that creates a profiling section.
constexpr const char cPROFILING_CREATE_SECTION_SYMBOL[] = "AfsimProfilingCreateSection";

/// Type of a callback for destroying a profiling section.
///
/// Loading the @a cPROFILING_DESTROY_SECTION_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using DestroyProfilingSectionFunc = void (*)(uint32_t);

/// @brief Name of symbol (to load from shared library) for the callback
///    that destroys a profiling section.
constexpr const char cPROFILING_DESTROY_SECTION_SYMBOL[] = "AfsimProfilingDestroySection";

/// Type of a callback for beginning a profiling section.
///
/// Loading the @a cPROFILING_BEGIN_SECTION_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using BeginProfilingSectionFunc = void (*)(uint32_t);

/// @brief Name of symbol (to load from shared library) for the callback
///    that begins a profiling section.
constexpr const char cPROFILING_BEGIN_SECTION_SYMBOL[] = "AfsimProfilingBeginSection";

/// Type of a callback for ending a profiling section that was previously begun.
///
/// Loading the @a cPROFILING_END_SECTION_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using EndProfilingSectionFunc = void (*)(uint32_t);

/// @brief Name of symbol (to load from shared library) for the callback
///    that ends a profiling section that was previously begun.
constexpr const char cPROFILING_END_SECTION_SYMBOL[] = "AfsimProfilingEndSection";

/// Type of a callback for finalizing the profiling library.
///
/// Loading the @a cPROFILING_FINALIZE_SYMBOL symbol
/// must produce a function pointer that is compatible
/// with this function pointer alias.
///
/// Please see @a ProfilingHooks for details.
using ProfilingFinalizeFunc = void (*)();

/// @brief Name of symbol (to load from shared library) for the callback
///    that finalizes the profiling library.
constexpr const char cPROFILING_FINALIZE_SYMBOL[] = "AfsimProfilingFinalize";

/// Set of all function pointers used for profiling.
///
/// The caller must check that any of these function pointers
/// is not null before invoking the function.
/// If either of @a mInitialize or @a mFinalize is null,
/// then none of the function pointers may be called.
/// None of these functions may be called
/// by multiple threads concurrently.
///
/// Here are the rules governing which functions may be called when,
/// relative to library initialization and finalization.
///
/// 1. It is always legal to call @a mInterfaceVersion,
///    at any time after loading the library.
/// 2. No function in this struct other than @a mInterfaceVersion
///    may be called before the call to @a mInitialize succeeds.
/// 3. @a mInitialize may be called at most once.
/// 4. If the call to @a mInitialize fails,
///    then no function may be called,
///    other than @a mInterfaceVersion.
/// 5. If @a mInitialize succeeds,
///    then @a mFinalize must be called exactly once,
///    at some point after @a mInitialize returns.
/// 6. No function other than @a mInterfaceVersion
///    may be called after @a mFinalize returns.
struct PROFILING_EXPORT ProfilingHooks
{
   /// @brief Callback for getting the version number
   ///    of the profiling interface that the library provides.
   ///
   /// The caller (the application) is responsible for checking
   /// that this matches (is equal to) the version number of the interface
   /// that the caller expects.  If the version numbers are not the same,
   /// then invoking any symbol in the library other than this one
   /// is undefined behavior.
   ProfilingInterfaceVersionFunc mInterfaceVersion = nullptr;

   /// Callback for initializing the profiling library.
   ///
   /// The initialization function takes one argument,
   /// a string specifying the output destination.
   /// The format is a URI, with supported schemes and final interpretation
   /// defined by the profiling library.
   ///
   /// Its return value is a POSIX-style signed integer error code
   /// (zero means success).
   ProfilingInitializeFunc mInitialize = nullptr;

   /// Callback for beginning a profiling region.
   ///
   /// The one argument is the name of the profiling region.
   /// Regions must be nested; that is, ending a region
   /// always ends the last region that was begun.
   BeginProfilingRegionFunc mBeginRegion = nullptr;

   //! Callback for ending the last profiling region that was begun.
   EndProfilingRegionFunc mEndRegion = nullptr;

   /// Callback for creating a profiling section.
   ///
   /// The one argument is the (string) name of the profiling section.
   /// Section labels need not be unique.  Calling this function twice
   /// with the same label shall result in distinct keys, referring to
   /// distinct sections.
   ///
   /// Its return value is the section "key."  Keys are unique.
   ///
   /// Sections differ from regions (see above) in the following ways.
   ///
   /// 1. Sections are allowed to overlap partially.
   /// 2. Section creation happens separately from beginning a section.
   /// 3. Section destruction happens separately from ending a section.
   CreateProfilingSectionFunc mCreateSection = nullptr;

   /// Callback for destroying a profiling section.
   ///
   /// The one argument is the key for the section to destroy.
   /// For each unique key created by @a mCreateSection,
   /// @a mDestroySection must be called exactly once with that key.
   DestroyProfilingSectionFunc mDestroySection = nullptr;

   /// Callback for beginning a profiling section.
   ///
   /// The one argument is the key (from creating the section).
   /// Every section that has been begun must be ended.
   /// After being ended, a section may be begun again.
   ///
   /// We permit multiple threads to call this function concurrently
   /// with different keys.  We do <i>not</i> permit multiple threads
   /// to call this function concurrently with the <i>same</i> key.
   BeginProfilingSectionFunc mBeginSection = nullptr;

   /// Callback for ending a profiling section.
   ///
   /// The one argument is the key (from creating the section).
   /// The key must refer to a section that has been begun.
   /// Every section that has been begun must be ended.
   /// After being ended, a section may be begun again.
   ///
   /// We permit multiple threads to call this function concurrently
   /// with different keys.  We do <i>not</i> permit multiple threads
   /// to call this function concurrently with the <i>same</i> key.
   EndProfilingSectionFunc mEndSection = nullptr;

   /// Callback for finalizing the profiling library.
   ///
   /// Calling any profiling library function (including this one)
   /// after this function has been called,
   /// other than the interface version function,
   /// is undefined behavior.
   ProfilingFinalizeFunc mFinalize = nullptr;
};

} // namespace profiling

#endif // PROFILINGHOOKS_HPP
