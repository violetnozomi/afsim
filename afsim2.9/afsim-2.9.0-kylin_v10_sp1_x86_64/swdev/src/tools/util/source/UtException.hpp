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

#ifndef UTEXCEPTION_HPP
#define UTEXCEPTION_HPP

#include "ut_export.h"

#include <exception>
#include <string>

#include "UtOptional.hpp"
#include "UtStacktrace.hpp"

//! A base class for exceptions.
class UT_EXPORT UtException : public std::exception
{
public:
   explicit UtException(std::string aWhat, ut::stacktrace aStacktrace = ut::stacktrace::current()) noexcept
      : mWhat(std::move(aWhat))
      , mStacktrace(std::move(aStacktrace))
   {
   }
   ~UtException() override = default;

   const char*           what() const noexcept override { return mWhat.c_str(); }
   const ut::stacktrace& GetStacktrace() const noexcept { return mStacktrace; }

private:
   std::string    mWhat;
   ut::stacktrace mStacktrace;
};

namespace ut
{
using Exception = ::UtException;

//! On Windows with the PROMOTE_HARDWARE_EXCEPTIONS CMake flag set,
//!    sets the _se_translator_function to map SE to ut::HardwareException.
//! Once set, hardware exceptions, such as division-by-zero and null-pointer-dereference,
//!    can be caught as a std::exception in the current thread.
//! MUST BE CALLED ON EVERY THREAD THAT USES MICROSOFT STRUCTURED EXCEPTIONS!
//! https://docs.microsoft.com/en-us/windows/win32/debug/structured-exception-handling

//! On Linux with the PROMOTE_HARDWARE_EXCEPTIONS CMake flag set,
//!    sets signal handlers to throw exceptions that can be caught via ut::HardwareException.

//! If the PROMOTE_HARDWARE_EXCEPTIONS CMake flag is not set,
//!    this is still safe to call, and does nothing.
UT_EXPORT void PromoteHardwareExceptions(bool aEnable) noexcept;

//! On Windows, calls _resetstkoflw().
//! On Linux, does nothing.
//! @par details
//!    If a stack overflow occurs and is thrown as a ut::HardwareException,
//!    ResetStackOverflow() should be called AFTER exiting the catch{} block.
//!    This resets the stack page guards that raise the stack overflow exception.
//!    This function CAN be called if no stack overflow has occurred.
UT_EXPORT bool ResetStackOverflow() noexcept;

//! To be passed to std::set_terminate() or ut::SetApplicationErrorHandling().
//! Logs that the application is terminating.
UT_EXPORT void TerminateHandler();

//! Sets global variables that are used by ut::SetupThreadErrorHandling().
//! @param aFunction is the terminate handler
//! @param aStackSize is the stack space reserved by windows for the main thread
//! Then calls ut::SetupThreadErrorHandling().
//! @par Terminate handler
//!    This is necessary because MSVC maintains a separate terminate handler for each thread.
//!    When running from a visual application, a custom terminate handler may be desired.
//! @par Usage
//!    ut::SetApplicationErrorHandling() should be called once at the beginning of the application before any additional
//!    threads have been spawned. Afterwards, threads should call ut::SetupThreadErrorHandling().
//! @par Windows error handling
//!    Windows can reserve some additional stack space to allow the error handling to display a pop-up in case of a
//!    stack overflow. The `aStackSize` parameter determines how much space Windows reserves.
UT_EXPORT void SetApplicationErrorHandling(std::terminate_handler aFunction, unsigned long aStackSize = 0x28000) noexcept;

//! Sets the terminate handler and hardware exceptions policy on the current thread.
//! @par details
//!    If ut::SetApplicationErrorHandling() has not been called,
//!    uses ut::TerminateHandler() for the terminate handler,
//!    and enables hardware exceptions.
//! @note Concurrent calls to ut::SetupThreadErrorHandling() are safe
//! @param aStackSize is the stack space reserved by windows for the current thread
UT_EXPORT void SetupThreadErrorHandling(unsigned long aStackSize = 0x28000) noexcept;

//! FatalException is an exception that should only be caught at application scope.
class UT_EXPORT FatalException // Intentionally does NOT inherit from anything
{
public:
   FatalException(std::string aWhat, ut::stacktrace aStacktrace) noexcept
      : mWhat(std::move(aWhat))
      , mStacktrace(std::move(aStacktrace))
   {
   }

   virtual ~FatalException() noexcept = default;

   const char*           what() const noexcept { return mWhat.c_str(); }
   const ut::stacktrace& GetStacktrace() const noexcept { return mStacktrace; }

private:
   std::string    mWhat;
   ut::stacktrace mStacktrace;
};

//! ut::HardwareException is the type thrown when a hardware exception occurs.
//! This only happens when the CMake variable PROMOTE_HARDWARE_EXCEPTIONS is enabled.
//! This type is visible when the variable is unset, but will not be thrown.
struct UT_EXPORT HardwareException final : FatalException
{
   explicit HardwareException(std::string aWhat) noexcept
      : FatalException(std::move(aWhat), ut::stacktrace::current())
   {
   }
};

//! Extracts details from different exception types and makes them available for other methods to use.
struct UT_EXPORT ExceptionDetails final
{
   //! Retrieves the exception details from aException.
   explicit ExceptionDetails(std::exception_ptr aException) noexcept;

   bool        mFatalException = false;      //!< True if the exception is a ut::FatalException or sub-class.
   std::string mTypeName;                    //!< Contains the name of the thrown exception type.
   std::string mWhatMessage;                 //!< Contains the what() message of the exception.
   ut::optional<ut::stacktrace> mStacktrace; //!< Contains the stack trace of where the exception was thrown, if applicable.
};

//! To be passed to std::set_terminate().
//! Logs that the application is terminating.
UT_EXPORT void TerminateHandler();

//! Should be called once at the beginning of the application.
//! Should be combined with SetApplicationErrorHandling() from AFSIM-1565.
UT_EXPORT void SetupApplicationLog(const char* aApplicationName, const char* aVersion, std::string aPath) noexcept;

//! Returns the application name set by @see SetupApplicationLog.
//! @retval nullptr if SetupApplicationLog has not been called
UT_EXPORT const char* GetApplicationName() noexcept;

//! Returns the full path to the application log set by @see SetupApplicationLog.
//! If SetupApplicationLog has not been called, returns an empty string.
UT_EXPORT std::string GetApplicationLogPath() noexcept;

//! Writes an entry to the application crash log.
//! Should be called from terminate handlers.
UT_EXPORT void WriteTerminateLog() noexcept;

//! Writes an entry to the application crash log.
//! Called from ut::ReportException.
UT_EXPORT void WriteExceptionLog(const char* aSource, const ExceptionDetails& aException) noexcept;

//! Reports that aException was thrown from aSource as a fatal error.
//! Usage:
//! @code
//! catch (...)
//! {
//!    ut::ReportException("AFSIM", std::current_exception());
//! }
//! @endcode
UT_EXPORT void ReportException(const char*        aSource,
                               std::exception_ptr aException,
                               const char*        aAdditionalInfo = nullptr) noexcept;
} // namespace ut

#endif
