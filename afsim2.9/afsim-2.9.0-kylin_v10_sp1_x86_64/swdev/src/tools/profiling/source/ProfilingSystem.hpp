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

#ifndef PROFILINGSYSTEM_HPP
#define PROFILINGSYSTEM_HPP

/// @brief Declaration of scope guard for enabling profiling.
///
/// The profiling system loads a shared library at run time.
/// For the interface that the shared library must implement,
/// please see ProfilingHooks.hpp.
/// For the interface for profiling a region of C++ code,
/// please see ProfilingRegion.hpp.

#include "profiling_export.h"

#include <string>

#include "UtOptional.hpp"

namespace profiling
{

/// @brief Command-line arguments for profiling,
///    as they may be parsed by code that uses profiling.
struct PROFILING_EXPORT ProfilingSystemArguments
{
   /// @brief If the value exists, the path of a shared library
   ///    containing profiling hooks.
   ///
   /// If the value does not exist, then the profiling system will load
   /// the AFPerf profiling library.  If this value exists,
   /// then the value of @c mProfilingOutputDest (see below) must exist.
   ut::optional<std::string> mProfilingLibraryPath;

   /// @brief If the value exists, the destination (as a URI)
   ///    to which the profiling system may write output.
   ///
   /// The destination is a URI (Uniform Resource Indicator),
   /// with supported schemes and final interpretation
   /// defined by the profiling library.
   ///
   /// If the value does not exist, then profiling will not occur.
   /// That is, the profiling system will do nothing.
   ut::optional<std::string> mProfilingOutputDest;
};

/// @brief Reference to the command-line arguments for profiling.
PROFILING_EXPORT ProfilingSystemArguments& GlobalProfilingSystemArguments();

/// @brief Scope guard, whose constructor starts the profiling system,
///    and whose destructor stops the profiling system.
///
/// This class is meant to be used with @c ProfilingRegion,
/// which marks a scope of C++ code for profiling.
///
/// Always name the variable representing the @c ProfilingSystem instance.
/// Don't just invoke the constructor without giving the variable a name.
/// If you forget to name the variable, then the region will close right away,
/// since the unnamed temporary object's destructor
/// will be invoked at the end of the statement.
///
/// @warning Do NOT use this at @c main scope!
///    Always make sure the object is out of scope before @c main returns.
///
/// @warning Please only have one instance of @c ProfilingSystem active at a time.
///    Please also see the thread parallelism discussion
///    in the documentation of @c ProfilingRegion.
class PROFILING_EXPORT ProfilingSystem
{
public:
   /// @brief Use command-line arguments, as they would be parsed
   ///    by code that uses profiling, to start the profiling system.
   ///
   /// @param aArgs Command-line arguments, as they would be parsed
   ///    by code that uses profiling.
   ///
   /// @pre Either <tt>aArgs.mProfilingLibraryPath.has_value()</tt> is @c false,
   ///    or <tt>aArgs.mProfilingOutputDest.has_value()</tt> is @c true.
   ///
   /// "Start the profiling system" means to call the function in the library
   /// that initializes it, thus preparing it for profiling.
   /// Once the profiling system is started, you may use profiling regions
   /// (see ProfilingRegion.hpp) and sections to profile regions of C++ code,
   /// as long as the ProfilingSystem object is in scope.
   ///
   /// If @c aArgs.mProfilingOutputDest is not set (does not have a value),
   /// then profiling will not occur.  You may still use
   /// profiling regions and sections, but they will have no effect.
   ///
   /// If @c aArgs.mProfilingOutputDest is set (has a value), profiling will occur.
   /// In that case, if @c aArgs.mProfilingLibraryPath is not set (does not have a value),
   /// then profiling will use the AFPerf library of profiling hooks.
   /// This is the primary (default) library that AFSIM builds and installs.
   /// It's what you want if you don't know otherwise.
   /// If @c aArgs.mProfilingLibraryPath is set (has a value),
   /// then profiling will use the alternate library of profiling hooks.
   /// An alternate shared library of profiling hooks may be any
   /// shared library that implements the interface described
   /// in ProfilingHooks.hpp, and need not be built or installed by AFSIM.
   ProfilingSystem(const ProfilingSystemArguments& aArgs);

   /// @brief Finalize the global profiling hooks if they were initialized,
   ///    and then unload the shared library containing the hooks.
   ///
   /// Once this destructor has been invoked,
   /// you may no longer use profiling regions or sections.
   ///
   /// @warning Do not invoke this in an @c atexit handler.
   ///   Doing so can cause segmentation violations or other errors,
   ///   as any state in the shared library might become invalid
   ///   by the time @c exit is called or @c main returns.
   ///   It's important that the destructor be called before @c main returns.
   ~ProfilingSystem();

   ProfilingSystem()                       = delete;
   ProfilingSystem(const ProfilingSystem&) = delete;
   ProfilingSystem& operator=(const ProfilingSystem&) = delete;
   ProfilingSystem(ProfilingSystem&&)                 = delete;
   ProfilingSystem& operator=(ProfilingSystem&&) = delete;

private:
   void initializeWithDefaultLibrary(const std::string& aOutputDestination);
   void initializeWithAlternateLibrary(const std::string& aLibraryPath, const std::string& aOutputDestination);
};

} // namespace profiling

#endif // PROFILINGSYSTEM_HPP
