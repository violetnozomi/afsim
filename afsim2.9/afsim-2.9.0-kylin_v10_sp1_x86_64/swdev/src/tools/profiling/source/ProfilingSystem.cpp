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

#include "ProfilingSystem.hpp"

#include <memory>
#include <sstream>

#include "ProfilingCommon.hpp"
#include "ProfilingHooks.hpp"
#include "ProfilingMacros.hpp"
#include "UtBuildInfo.hpp"
#include "UtBuildInfoMacros.hpp"
#include "UtDynamicLibrary.hpp"
#include "UtLog.hpp"
#include "UtOptional.hpp"
#include "UtPath.hpp"

namespace profiling
{

ProfilingSystemArguments& GlobalProfilingSystemArguments()
{
   static ProfilingSystemArguments args;
   return args;
}

namespace
{
constexpr const char cDEFAULT_PROFILING_LIB_NAME[] = "afperf";

// Current ProfilingSystem version
constexpr uint16_t    cEPOCH = 0;
constexpr uint16_t    cMAJOR = 1;
constexpr uint16_t    cMINOR = 0;
constexpr uint16_t    cPATCH = 0;
constexpr VersionInfo cPROFILING_SYSTEM_VERSION{cEPOCH, cMAJOR, cMINOR, cPATCH};

/// @brief Returns version info as a string for output purposes.
///
/// @param aVersion Unpacked version value.
///
/// @param aIncludeEpoch Flag to include epoch version value.
std::string GetFormattedVersionInfo(VersionInfo aVersionInfo, bool aIncludeEpoch = false)
{
   std::stringstream ss;

   if (aIncludeEpoch)
   {
      ss << aVersionInfo.mEpoch << ':';
   }
   ss << aVersionInfo.mMajor << '.' << aVersionInfo.mMinor << '.' << aVersionInfo.mPatch;

   return ss.str();
}

/// @brief Returns the name of the profiler that is being loaded
std::string GetProfilerName()
{
   if (GlobalProfilingSystemArguments().mProfilingLibraryPath.has_value())
   {
      const UtPath path = GlobalProfilingSystemArguments().mProfilingLibraryPath.value();
      return path.GetFileName();
   }
   else
   {
      return cDEFAULT_PROFILING_LIB_NAME;
   }
}

/// @brief Conversion function to extract versioning values
///
/// @param aVersion Unpacked version value.
VersionInfo UnpackVersionInfo(uint64_t aVersion)
{
   VersionInfo result;
   result.mEpoch = static_cast<uint16_t>(aVersion >> 48);
   result.mMajor = static_cast<uint16_t>(aVersion >> 32);
   result.mMinor = static_cast<uint16_t>(aVersion >> 16);
   result.mPatch = static_cast<uint16_t>(aVersion);
   return result;
}

/// @brief Checks the profiling library's version against the ProfilingSystem version.
///
/// @param aVersion Unpacked return value of ProfilingInterfaceVersionFunc.
bool IsLibraryCompatible(VersionInfo aVersion)
{
   return cPROFILING_SYSTEM_VERSION.mEpoch == aVersion.mEpoch && cPROFILING_SYSTEM_VERSION.mMajor == aVersion.mMajor &&
          cPROFILING_SYSTEM_VERSION.mMinor >= aVersion.mMinor;
}

/// @brief Creates formatted error message based on version conflict.
///
/// @param aVersion Unpacked return value of ProfilingInterfaceVersionFunc.
std::string IncompatibilityMessage(VersionInfo aVersion)
{
   std::stringstream ss;
   const bool        bad_epoch = cPROFILING_SYSTEM_VERSION.mEpoch != aVersion.mEpoch;

   ss << "Attempted to load profiling library that is incompatible with the current profiling system (";
   ss << "system: " + GetFormattedVersionInfo(cPROFILING_SYSTEM_VERSION, bad_epoch);
   ss << ", library: " + GetFormattedVersionInfo(aVersion, bad_epoch) << ")";

   return ss.str();
}

/// @brief Load and return profiling library, or return @a nullptr if unsuccessful.
///
/// @param aCandidatePath Path in which to look for the library.
std::unique_ptr<UtDynamicLibrary> LoadProfilingHooksLibrary(const std::string& aCandidatePath)
{
   {
      ut::log::debug() << (std::string("Attempting to load profiling library \"") + aCandidatePath + "\"");
   }
   std::string                       errorString;
   std::unique_ptr<UtDynamicLibrary> lib(UtDynamicLibrary::Load(aCandidatePath, errorString));
   if (!lib)
   {
      std::string errMsg = std::string("Failed to load profiling library \"") + aCandidatePath + "\"";
      if (errorString != "")
      {
         errMsg += ": " + errorString;
      }
      ut::log::error() << errMsg;
   }
   else
   {
      ut::log::debug() << (std::string("Loaded profiling library \"") + aCandidatePath + "\"");
   }
   return lib;
}

/// @brief Load and return the default profiling library,
///    or return @a nullopt if the load fails.
std::unique_ptr<UtDynamicLibrary> LoadDefaultProfilingHooksLibrary()
{
   const std::string libPath = ut::BuildInfo::GetSharedLibraryFilePath(cDEFAULT_PROFILING_LIB_NAME);

   if (libPath.empty())
   {
      return nullptr;
   }
   else
   {
      return LoadProfilingHooksLibrary(libPath);
   }
}

//! Result of loading profiling library; return type of @a LoadProfilingHooks (which see).
struct LoadProfilingHooksResult
{
   //! Whether loading the profiling library succeeded, or how it failed.
   enum class ErrorCode
   {
      SUCCESS,
      NO_LIBRARY,
      MISSING_SYMBOL,
      BAD_VERSION,
      MISSING_OUTPUT
   } mErrorCode = ErrorCode::NO_LIBRARY;

   /// @brief Additional information from the version check when loading
   ///    a profiler
   std::string mErrorMessage;
   /// @brief Either nullptr, or the loaded library, kept open.
   ///
   /// If loading the dynamic library succeeded, then we need to keep it loaded
   /// for as long as we might use its functions -- that is, until after
   /// the library's @a finalize function is called and returns.
   std::unique_ptr<UtDynamicLibrary> mLib;
   /// @brief The actual profiling functions ("hooks").
   ///
   /// We only expose these to clients, but it's important that the hooks
   /// not be allowed to persist past closing of the library (which happens
   /// when @a mLib goes out of scope).
   ProfilingHooks mHooks;
};

/// Load a symbol from an opened dynamic shared library.
///
/// @tparam The function pointer type.
///    The caller is responsible for knowing the correct type.
///    Using an incorrect type will result in undefined behavior.
///
/// @param aLib The opened dynamic shared library.
///
/// @param aSymbol The symbol of the function to load.
///
/// @returns If the symbol can be loaded, then a pointer
///    to the loaded function (cast to @a FunctionPointer ),
///    else @a nullptr .
template<class FunctionPointer>
FunctionPointer LoadSymbol(UtDynamicLibrary& aLib, const char* aSymbol)
{
   // GetSymbol isn't const, so we can't take lib as const.
   return reinterpret_cast<FunctionPointer>(aLib.GetSymbol(aSymbol));
}

/// @brief Load and return profiling hooks from the given dynamic shared library.
///
/// @param aLib If @a nullptr, return the default hooks, else attempt to load the hooks.
LoadProfilingHooksResult LoadProfilingHooks(std::unique_ptr<UtDynamicLibrary> aLib)
{
   if (!aLib)
   {
      return std::move(LoadProfilingHooksResult{});
   }
   else
   {
      // C++17 would let us write all the loads at once,
      // using a fold expression over a tuple of all the function types,
      // and a std::array<std::string_view, numFunctions> of the symbol names.
      bool        missingSymbol = false;
      bool        badVersion    = false;
      std::string versionCheckMessage;
      auto interfaceVersionFunc = LoadSymbol<ProfilingInterfaceVersionFunc>(*aLib, cPROFILING_INTERFACE_VERSION_SYMBOL);
      if (!interfaceVersionFunc)
      {
         missingSymbol = true;
      }
      else
      {
         const VersionInfo profilingLibVersion = UnpackVersionInfo(interfaceVersionFunc());
         if (!IsLibraryCompatible(profilingLibVersion))
         {
            versionCheckMessage = IncompatibilityMessage(profilingLibVersion);
            badVersion          = true;
         }
         else
         {
            versionCheckMessage = GetProfilerName() + " version " + GetFormattedVersionInfo(profilingLibVersion);
         }
      }
      auto initializeFunc = LoadSymbol<ProfilingInitializeFunc>(*aLib, cPROFILING_INITIALIZE_SYMBOL);
      if (!initializeFunc)
      {
         missingSymbol = true;
      }
      auto beginRegionFunc = LoadSymbol<BeginProfilingRegionFunc>(*aLib, cPROFILING_BEGIN_REGION_SYMBOL);
      if (!beginRegionFunc)
      {
         missingSymbol = true;
      }
      auto endRegionFunc = LoadSymbol<EndProfilingRegionFunc>(*aLib, cPROFILING_END_REGION_SYMBOL);
      if (!endRegionFunc)
      {
         missingSymbol = true;
      }
      auto createSectionFunc = LoadSymbol<CreateProfilingSectionFunc>(*aLib, cPROFILING_CREATE_SECTION_SYMBOL);
      if (!createSectionFunc)
      {
         missingSymbol = true;
      }
      auto destroySectionFunc = LoadSymbol<DestroyProfilingSectionFunc>(*aLib, cPROFILING_DESTROY_SECTION_SYMBOL);
      if (!destroySectionFunc)
      {
         missingSymbol = true;
      }
      auto beginSectionFunc = LoadSymbol<BeginProfilingSectionFunc>(*aLib, cPROFILING_BEGIN_SECTION_SYMBOL);
      if (!beginSectionFunc)
      {
         missingSymbol = true;
      }
      auto endSectionFunc = LoadSymbol<EndProfilingSectionFunc>(*aLib, cPROFILING_END_SECTION_SYMBOL);
      if (!endSectionFunc)
      {
         missingSymbol = true;
      }
      auto finalizeFunc = LoadSymbol<ProfilingFinalizeFunc>(*aLib, cPROFILING_FINALIZE_SYMBOL);
      if (!finalizeFunc)
      {
         missingSymbol = true;
      }

      // C++20 would let us use designated initializers (commented out in the code below).
      // This would prevent future errors due to incorrect order.
      if (badVersion)
      {
         LoadProfilingHooksResult result;
         result.mErrorCode    = LoadProfilingHooksResult::ErrorCode::BAD_VERSION;
         result.mErrorMessage = versionCheckMessage;
         return result;
      }
      else if (missingSymbol)
      {
         // GCC 4.8 has bugs relating to C++11 aggregate initialization.
         // The commented-out line of code below is valid C++11,
         // but GCC 4.8 does not accept it.
         //
         // return { LoadProfilingHooksResult::ErrorCode::MISSING_SYMBOL };
         LoadProfilingHooksResult result;
         result.mErrorCode = LoadProfilingHooksResult::ErrorCode::MISSING_SYMBOL;
         return result;
      }
      else
      {
#if 1
         // This code works around GCC 4.8's C++11 aggregate initialization bugs.
         ProfilingHooks hooks;
         hooks.mInterfaceVersion = interfaceVersionFunc;
         hooks.mInitialize       = initializeFunc;
         hooks.mBeginRegion      = beginRegionFunc;
         hooks.mEndRegion        = endRegionFunc;
         hooks.mCreateSection    = createSectionFunc;
         hooks.mDestroySection   = destroySectionFunc;
         hooks.mBeginSection     = beginSectionFunc;
         hooks.mEndSection       = endSectionFunc;
         hooks.mFinalize         = finalizeFunc;

         LoadProfilingHooksResult result;
         result.mErrorCode    = LoadProfilingHooksResult::ErrorCode::SUCCESS;
         result.mErrorMessage = versionCheckMessage;
         result.mLib          = std::move(aLib);
         result.mHooks        = hooks;
         return result;
#else
         // GCC 4.8 does not accept the following valid C++11 code.
         return {/* .mErrorCode = */ LoadProfilingHooksResult::ErrorCode::SUCCESS,
                 /* .mErrorMessage = */ mErrorMessage,
                 /* .mLib = */ std::move(aLib),
                 /* .mHooks = */
                 ProfilingHooks{/* .mInterfaceVersion = */ interfaceVersionFunc,
                                /* .mInitialize = */ initializeFunc,
                                /* .mBeginRegion = */ beginRegionFunc,
                                /* .mEndRegion = */ endRegionFunc,
                                /* .mCreateSection = */ createSectionFunc,
                                /* .mDestroySection = */ destroySectionFunc,
                                /* .mBeginSection = */ beginSectionFunc,
                                /* .mEndSection = */ endSectionFunc,
                                /* .mFinalize = */ finalizeFunc}};
#endif // 1
      }
   }
}

//! Load and return profiling hooks from the given dynamic shared library file.
LoadProfilingHooksResult LoadProfilingHooks(const std::string& aInputLibraryPath)
{
   return LoadProfilingHooks(LoadProfilingHooksLibrary(aInputLibraryPath.c_str()));
}

LoadProfilingHooksResult LoadDefaultProfilingHooks()
{
   return LoadProfilingHooks(LoadDefaultProfilingHooksLibrary());
}

//! The dynamic shared library containing the global profiling hooks currently in use.
std::unique_ptr<UtDynamicLibrary> globalProfilingLibrary;
//! The global profiling hooks currently in use.
ProfilingHooks globalProfilingHooks;

//! Whether the global profiling hooks' initialize function has been called.
bool globalProfilingHooksInitialized = false;

/// @brief Initialize the profiling library (if loaded),
///    and set @a globalProfilingLibrary, @a globalProfilingHooks,
///    and @a globalProfilingInitialized.
///
/// @param aResult Result of @a LoadProfilingHooks or @a LoadDefaultProfilingHooks
///    (this function consumes it).
///
/// @param aOutputDestination The destination to which to write profiling data.
///    The format is a URI, with supported schemes and final interpretation defined by
///    the profiling library.
///
/// @return @a true if loading and initializing the profiling hooks was successful,
///    else @a false.
void InitializeGlobalProfilingHooks(LoadProfilingHooksResult&& aResult, const std::string& aOutputDestination)
{
   const auto errorCode = aResult.mErrorCode;

   if (errorCode == LoadProfilingHooksResult::ErrorCode::NO_LIBRARY)
   {
      throw profiling::Exception(
         "Failed to initialize ProfilingHooks - profiling library \"" + GetProfilerName() + "\" not detected.",
         static_cast<int>(errorCode));
   }
   if (errorCode == LoadProfilingHooksResult::ErrorCode::MISSING_SYMBOL)
   {
      throw profiling::Exception("Failed to initialize ProfilingHooks - Profiling library \"" + GetProfilerName() +
                                    "\" was loaded, but has one or more missing symbols.  "
                                    "This means either that the path does not point to a library, "
                                    "or that the symbols have changed since the library was built.",
                                 static_cast<int>(errorCode));
   }
   if (errorCode == LoadProfilingHooksResult::ErrorCode::BAD_VERSION)
   {
      throw profiling::Exception("Failed to initialize ProfilingHooks - " + aResult.mErrorMessage,
                                 static_cast<int>(errorCode));
   }

   {
      ut::log::info() << "Successfully loaded profiling library";
   }
   if (aResult.mHooks.mInitialize != nullptr)
   {
      const int32_t initErrCode = aResult.mHooks.mInitialize(aOutputDestination.c_str());
      if (initErrCode == 0)
      {
         globalProfilingLibrary          = std::move(aResult.mLib);
         globalProfilingHooks            = aResult.mHooks;
         globalProfilingHooksInitialized = true;
         ut::log::info() << "Successfully initialized profiling library: " << aResult.mErrorMessage;
      }
      else
      {
         throw profiling::Exception("Failed to initialize profiling library \"" + GetProfilerName() +
                                       "\" with output destination " + aOutputDestination +
                                       "ProfilingInitializeFunc returned nonzero error code.",
                                    static_cast<int>(initErrCode));
      }
   }
}

} // namespace

namespace detail
{
//! Get the current global profiling hooks.
const ProfilingHooks& GetGlobalProfilingHooks()
{
   return globalProfilingHooks;
}

} // namespace detail

void ProfilingSystem::initializeWithAlternateLibrary(const std::string& aLibraryPath,
                                                     const std::string& aOutputDestination)
{
   InitializeGlobalProfilingHooks(LoadProfilingHooks(aLibraryPath), aOutputDestination);
}

void ProfilingSystem::initializeWithDefaultLibrary(const std::string& aOutputDestination)
{
   InitializeGlobalProfilingHooks(LoadDefaultProfilingHooks(), aOutputDestination);
}

ProfilingSystem::ProfilingSystem(const ProfilingSystemArguments& aArgs)
{
   if (aArgs.mProfilingOutputDest.has_value())
   {
      const auto& outputPath = aArgs.mProfilingOutputDest.value();

      if (aArgs.mProfilingLibraryPath.has_value())
      {
         const auto& libPath = aArgs.mProfilingLibraryPath.value();
         initializeWithAlternateLibrary(libPath, outputPath);
      }
      else
      {
         initializeWithDefaultLibrary(outputPath);
      }
   }
   else if (aArgs.mProfilingLibraryPath.has_value())
   {
      throw profiling::Exception("Failed to instantiate ProfilingSystem - " +
                                    std::string("Attempting to use profiling library [") +
                                    aArgs.mProfilingLibraryPath.value() + "] without setting the output destination argument.",
                                 static_cast<int>(LoadProfilingHooksResult::ErrorCode::MISSING_OUTPUT));
   }
}

ProfilingSystem::~ProfilingSystem()
{
   if (globalProfilingHooksInitialized && globalProfilingHooks.mFinalize != nullptr)
   {
      globalProfilingHooks.mFinalize();
   }
   globalProfilingHooks = {};
   globalProfilingLibrary.reset();
   globalProfilingHooksInitialized = false;
}

} // namespace profiling
