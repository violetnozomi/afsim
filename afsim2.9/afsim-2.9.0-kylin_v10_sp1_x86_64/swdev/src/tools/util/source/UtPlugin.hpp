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

#ifndef UTPLUGIN_HPP
#define UTPLUGIN_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

//! This is a 'C' interface for registering and creating dynamic plugins.
//!
//! The following example illustrates how to this interface to register objects in your plugin:
//!
//! ---------------------------------------------------------------------------------------------------------
//! Begin ExamplePlugin.hpp
//!
//! // EXAMPLE_PLUGIN_EXPORTS should be defined in your project's preprocessor definitions
//! // (i.e. Properties->C/C++->Preprocessor Definitions).
//! #if defined(_WIN32)
//! #if defined(EXAMPLE_PLUGIN_EXPORTS)
//! # define UT_PLUGIN_API __declspec(dllexport)
//! #else
//! # define UT_PLUGIN_API __declspec(dllimport)
//! #endif
//! #endif
//!
//! End ExamplePlugin.hpp
//!
//! ---------------------------------------------------------------------------------------------------------
//! Begin ExamplePlugin.cpp
//!
//! #include "ExamplePlugin.hpp"
//! #include "UtPlugin.hpp"
//! #include "WsfMyGeometricSensor.hpp"
//!
//! extern "C" UT_PLUGIN_API int ExitFunc()
//! {
//!    // perform any cleanup that may be needed.
//!    return 0;
//! }
//!
//! extern "C" UT_PLUGIN_API UtPluginExitFunc UtPluginInitPlugin(const UtPluginRegistrationService* aRegistrationServicePtr)
//! {
//!    // Create the plugin version object using the provided macros.
//!    const UtPluginVersion version(UT_PLUGIN_API_MAJOR_VERSION, UT_PLUGIN_API_MINOR_VERSION, UT_PLUGIN_API_COMPILER_STRING);
//!
//!    // Note: 'Create' (i.e. UtPluginCreateFunc) and 'Destroy' (i.e. UtPluginDestroyFunc) must be provided for each
//!    //       type that is registered. (see WsfMyGeometricSensor.hpp/.cpp below).
//!    // Note: Each plugin can register more than one type. Simply call aRegistrationServicePtr->RegisterObject(...) with
//!    //       each type that you want to register.
//!
//!    // Register "WSF_MY_GEOMETRIC_SENSOR".
//!    UtPluginRegistration regSensor1(version,
//!       WsfMyGeometricSensor::Create, WsfMyGeometricSensor::Destroy,
//!       "WsfSensor", "WSF_MY_GEOMETRIC_SENSOR");
//!    int result = aRegistrationServicePtr->RegisterObject(regSensor1);
//!    if (result < 0)
//!    {
//!       return 0;
//!    }
//!    return ExitFunc;
//! }
//!
//! End ExamplePlugin.cpp
//!
//! ---------------------------------------------------------------------------------------------------------
//! Begin WsfMyGeometricSensor.hpp
//!
//! class WsfMyGeometricSensor : public WsfSensor
//! {
//! public:
//!  // Plugin Interface
//!    static void* Create(const UtPluginObjectParameters& aObjectParameters);
//!    static int Destroy(void* aObjectPtr);
//!    ...
//! };
//!
//! End WsfMyGeometricSensor.hpp
//!
//! ---------------------------------------------------------------------------------------------------------
//! Begin WsfMyGeometricSensor.cpp
//!
//! // New a WsfMyGeometricSensor object.
//! // The aObjectParameters object can be used to pass arguments if needed.
//! void* WsfMyGeometricSensor::Create(const UtPluginObjectParameters& aObjectParameters)
//! {
//!    return new WsfMyGeometricSensor();
//! }
//!
//! // Delete the WsfMyGeometricSensor object.
//! int WsfMyGeometricSensor::Destroy(void* aObjectPtr)
//! {
//!    int deleted = -1;
//!    if (aObjectPtr != 0)
//!    {
//!       delete (WsfMyGeometricSensor*)aObjectPtr;
//!       deleted = 0;
//!    }
//!    return deleted;
//! }
//!
//! End WsfMyGeometricSensor.cpp

extern "C"
{
   namespace ut
   {
   namespace system
   {
#ifdef PROMOTE_HARDWARE_EXCEPTIONS
#define HWE_SUFFIX "-hwe"
#else
#define HWE_SUFFIX
#endif

#ifdef _WIN32
   static constexpr const char*  platform = "win";
   static constexpr unsigned int compiler = _MSC_VER;
#ifdef _WIN64
   static constexpr const char* bits = "64bit";
#else
   static constexpr const char* bits  = "32bit";
#endif

// The combination of _DEBUG and PROMOTE_HARDWARE_EXCEPTIONS creates 4 distinct build types:
//    "debug", "debug-hwe", "release", "release-hwe"
#ifdef _DEBUG
   static constexpr const char* build = "debug" HWE_SUFFIX;
#else
   static constexpr const char* build = "release" HWE_SUFFIX;
#endif

#elif defined(__linux__)
   static constexpr const char*  platform = "lnx";
#ifdef __GNUC_PATCHLEVEL__
   static constexpr unsigned int compiler = __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__;
#else
   static constexpr unsigned int compiler = __GNUC__ * 10000 + __GNUC_MINOR__ * 100;
#endif
#ifdef __LP64__
   static constexpr const char*  bits     = "64bit";
#else
   static constexpr const char*  bits     = "32bit";
#endif

// The combination of __OPTIMIZE__ and PROMOTE_HARDWARE_EXCEPTIONS creates 4 distinct build types:
//    "debug", "debug-hwe", "release", "release-hwe"
#ifdef __OPTIMIZE__
   static constexpr const char* build = "release" HWE_SUFFIX;
#else
   static constexpr const char*  build    = "debug" HWE_SUFFIX;
#endif

#endif

#undef HWE_SUFFIX
   } // namespace system
   } // namespace ut

   static const char* UtPluginCompilerVersionString()
   {
#if defined(_WIN32) || defined(__linux__)
      using namespace ut::system;
      std::ostringstream oss;
      oss << platform << '_' << compiler << '_' << bits << '_' << build;
      static std::string version = oss.str();
      return version.c_str();
#else
      return "unknown";
#endif
   }

#define UT_PLUGIN_API_MAJOR_VERSION 1
#define UT_PLUGIN_API_MINOR_VERSION 0

   static const char* UT_PLUGIN_API_COMPILER_STRING = UtPluginCompilerVersionString();

#define UT_PLUGIN_VERSION_CREATE \
   UtPluginVersion(UT_PLUGIN_API_MAJOR_VERSION, UT_PLUGIN_API_MINOR_VERSION, UT_PLUGIN_API_COMPILER_STRING)

   struct UtPluginRegistrationService;

   //! UtPluginVersion maintains the current plugin API major and minor version.

   struct UtPluginVersion
   {
      UtPluginVersion(unsigned int aMajor, unsigned int aMinor, const char* aCompilerVersion)
         : mMajor(aMajor)
         , mMinor(aMinor)
         , mCompilerVersion(aCompilerVersion)
      {
      }

      unsigned int mMajor;
      unsigned int mMinor;

      //! <win | lnx>_<compiler-version>_<32bit | 64bit>_<release | debug | release-hwe | debug-hwe> (e.g.
      //! win_1400_32bit_release-hwe). The provided 'UtPluginCompilerVersionString()' method or
      //! 'UT_PLUGIN_API_COMPILER_STRING' macro can be used to build this string.
      const char* mCompilerVersion;
   };

   //! UtPluginObjectParameters is the type passed to the plugin's registration
   //! object (UtPluginRegistration) when requesting the creation of an object.
   //! As an example, we may create an object of this type with aObjectType='WsfSensor'
   //! and aObjectName='WsfMySensor'.

   struct UtPluginObjectParameters
   {
      UtPluginObjectParameters(const char*                        aObjectType,
                               const char*                        aObjectName,
                               void*                              aObjectDataPtr,
                               const UtPluginRegistrationService* aRegistrationServicePtr)
         : mObjectType(aObjectType)
         , mObjectName(aObjectName)
         , mObjectDataPtr(aObjectDataPtr)
         , mRegistrationServicePtr(aRegistrationServicePtr)
      {
      }

      const char*                        mObjectType;
      const char*                        mObjectName;
      mutable void*                      mObjectDataPtr;
      const UtPluginRegistrationService* mRegistrationServicePtr;
   };

   using UtPluginCreateFunc  = void* (*)(const UtPluginObjectParameters&);
   using UtPluginDestroyFunc = int (*)(void*);

   //! UtPluginRegistration is the interface to a type that was loaded from a dynamic library.
   //! This class provides methods for creating and destroying objects of the type held in
   //! 'aObjectType' and 'aObjectName'. As an example, if we loaded a dynamic library it may register
   //! a custom sensor type aObjectType='WsfSensor' and aObjectName='WsfMySensor'. The creation
   //! method will create an object of type 'WsfMySensor' in the plugin and return it. The destroy
   //! method takes the object that was allocated in the creation method and deallocates it.

   struct UtPluginRegistration
   {
      UtPluginRegistration(const UtPluginVersion& aVersion,
                           UtPluginCreateFunc     aCreateFuncPtr,
                           UtPluginDestroyFunc    aDestroyFuncPtr,
                           const char*            aObjectType,
                           const char*            aObjectName)
         : mVersion(aVersion)
         , mCreateFuncPtr(aCreateFuncPtr)
         , mDestroyFuncPtr(aDestroyFuncPtr)
         , mObjectType(aObjectType)
         , mObjectName(aObjectName)
      {
      }

      //! Dispatches to the plugin for allocation of an object.
      //! @param aObjectParameters Any arguments needed to allocate the object.
      //! @return A void pointer to the created object.
      //!         If the object couldn't be create, NULL is returned.
      void* CreateObject(const UtPluginObjectParameters& aObjectParameters) const
      {
         return mCreateFuncPtr(aObjectParameters);
      }

      //! Dispatches to the plugin for deallocation of the provided object.
      //! @param aObjectPtr The object to deallocate.
      //! @return  0 - Indicates the object was successfully deallocated.
      //!         -1 - Indicates the object was not deallocated.
      int DestroyObject(void* aObjectPtr) const { return mDestroyFuncPtr(aObjectPtr); }

      UtPluginVersion     mVersion;
      UtPluginCreateFunc  mCreateFuncPtr;
      UtPluginDestroyFunc mDestroyFuncPtr;
      const char*         mObjectType;
      const char*         mObjectName;
   };

   using UtPluginRegisterFunc = int (*)(void*, const UtPluginRegistration&);

   //! UtPluginRegistrationService is the class used by plugin clients to register the types of objects that they
   //! support (i.e. create/destroy). This is handled by the application layer, which calls the plugin initialization
   //! function (i.e. UtPluginInitFunc) and passes an object of this type. The client then creates a
   //! UtPluginRegistration objects for each object type that they support and then calls RegisterObject.

   struct UtPluginRegistrationService
   {
      UtPluginRegistrationService(const UtPluginVersion& aVersion, void* aManagerPtr, UtPluginRegisterFunc aRegistrationFuncPtr)
         : mVersion(aVersion)
         , mManagerPtr(aManagerPtr)
         , mRegistrationFuncPtr(aRegistrationFuncPtr)
      {
      }

      //! The plugin object registration method.
      //! @return 1 - Indicates a successful registration.
      //!         0 - Indicates failure because object type/name already exists.
      //!        -1 - Indicates failure for unknown reason.
      //!        -2 - Indicates failure because the compiler version doesn't match.
      //!        -3 - Indicates failure because the major version doesn't match.
      int RegisterObject(const UtPluginRegistration& aRegistration) const
      {
         return mRegistrationFuncPtr(mManagerPtr, aRegistration);
      }

      UtPluginVersion      mVersion;
      void*                mManagerPtr;
      UtPluginRegisterFunc mRegistrationFuncPtr;
   };

   using UtPluginExitFunc = int (*)();

   using UtPluginInitFunc = UtPluginExitFunc (*)(const UtPluginRegistrationService*);

#ifndef UT_PLUGIN_API
#ifdef _WIN32
#define UT_PLUGIN_API __declspec(dllimport)
#define UT_PLUGIN_EXPORT __declspec(dllexport)
#else
#define UT_PLUGIN_API
#define UT_PLUGIN_EXPORT
#endif
#endif
}

//! Print plugin version information to ostream.
std::ostream& operator<<(std::ostream& aOut, const UtPluginVersion& aVersion);

#endif
