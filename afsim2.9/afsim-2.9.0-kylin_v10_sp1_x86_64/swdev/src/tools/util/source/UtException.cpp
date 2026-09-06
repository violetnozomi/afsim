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

#include "UtException.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <mutex>

#include "UtLog.hpp"
#include "UtOptional.hpp"
#include "UtPath.hpp"
#include "UtPlugin.hpp" // For ut::system strings
#include "UtTypeInfo.hpp"

#ifndef PROMOTE_HARDWARE_EXCEPTIONS /* if hardware exceptions are disabled */

void ut::PromoteHardwareExceptions(bool) noexcept
{
   // Do nothing.
}

#elif defined _WIN32 /* if hardware exceptions are enabled on Windows */

#include <sstream>

#include <windows.h>

static std::string GetWindowsExceptionText(unsigned int aExceptionCode, bool aWriteViolation, const void* aAddress) noexcept
{
   switch (aExceptionCode)
   {
   case EXCEPTION_ACCESS_VIOLATION:
   {
      std::stringstream ss;
      ss << "Hardware - ";
      ss << (aWriteViolation ? "Write" : "Read");
      ss << " access violation at address 0x";
      ss << aAddress;
      ss << ".";
      return ss.str();
   }
   case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      return "Hardware - Array bounds exceeded.";
   case EXCEPTION_BREAKPOINT:
      return "Hardware - Breakpoint hit.";
   case EXCEPTION_DATATYPE_MISALIGNMENT:
      return "Hardware - Data misalignment.";
   case EXCEPTION_FLT_DENORMAL_OPERAND:
      return "Hardware - Floating point operand too small.";
   case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      return "Hardware - Floating point division by zero.";
   case EXCEPTION_FLT_INEXACT_RESULT:
      return "Hardware - Floating point inexact result.";
   case EXCEPTION_FLT_INVALID_OPERATION:
      return "Hardware - Unknown floating point exception.";
   case EXCEPTION_FLT_OVERFLOW:
      return "Hardware - Floating point overflow.";
   case EXCEPTION_FLT_STACK_CHECK:
      return "Hardware - Stack error caused by floating point operation.";
   case EXCEPTION_FLT_UNDERFLOW:
      return "Hardware - Floating point underflow.";
   case EXCEPTION_GUARD_PAGE:
      return "Hardware - Page-guarded memory access.";
   case EXCEPTION_ILLEGAL_INSTRUCTION:
      return "Hardware - Illegal instruction.";
   case EXCEPTION_IN_PAGE_ERROR:
      return "Hardware - Page not present.";
   case EXCEPTION_INT_DIVIDE_BY_ZERO:
      return "Hardware - Integer division by zero.";
   case EXCEPTION_INT_OVERFLOW:
      return "Hardware - Integer overflow.";
   case EXCEPTION_INVALID_DISPOSITION:
      return "Hardware - Invalid disposition.";
   case EXCEPTION_INVALID_HANDLE:
      return "Hardware - Invalid kernel object handle.";
   case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      return "Hardware - Non-continuable exception.";
   case EXCEPTION_PRIV_INSTRUCTION:
      return "Hardware - Attempt to use kernel mode instruction.";
   case EXCEPTION_SINGLE_STEP:
      return "Hardware - Single instruction step.";
   case EXCEPTION_STACK_OVERFLOW:
      return "Hardware - Stack overflow.";
   case STATUS_UNWIND_CONSOLIDATE:
      return "Hardware - Frame consolidation.";
   default:
      return "Hardware - Unknown exception.";
   }
}

static void __cdecl WindowsExceptionTranslator(unsigned int aExceptionCode, EXCEPTION_POINTERS* aExceptionData)
{
   throw ut::HardwareException(
      GetWindowsExceptionText(aExceptionCode,
                              aExceptionData->ExceptionRecord->ExceptionInformation[0] != 0,
                              reinterpret_cast<const void*>(aExceptionData->ExceptionRecord->ExceptionInformation[1])));
}

void ut::PromoteHardwareExceptions(bool aEnable) noexcept
{
   _set_se_translator(aEnable ? WindowsExceptionTranslator : nullptr);
}

#else /* if hardware exceptions are enabled on Linux */

#include <csignal>
#include <sstream>

/*
A note on SIGEMT:

According to the Linux manual pages, SIGEMT should be defined, but it isn't.
It is being left in with preprocessor guards so that it will be available if
future GCC versions support it.
*/

static std::string ToHexString(const void* aPtr)
{
   std::stringstream ss;
   ss << aPtr;
   return ss.str();
}

static std::string GetLinuxExceptionText(int aSignal, const siginfo_t* aInfo)
{
   switch (aSignal)
   {
   case SIGBUS:
      return "Hardware - Bus error (bad memory access) at 0x" + ::ToHexString(aInfo->si_addr);
#ifdef SIGEMT
   case SIGEMT:
      return "Hardware - Emulator trap.";
#endif
   case SIGFPE:
      return "Hardware - Floating-point exception.";
   case SIGILL:
      return "Hardware - Illegal instruction.";
   case SIGSEGV:
      return "Hardware - Invalid memory reference at 0x" + ::ToHexString(aInfo->si_addr);
   case SIGSYS:
      return "Hardware - Bad system call.";
   case SIGTRAP:
      return "Hardware - Trace/breakpoint trap.";
   default:
      return "Hardware - Unknown exception.";
   }
}

extern "C"
{
   static void LinuxSignalTranslator(int aSignal, siginfo_t* aInfo, void* aContext)
   {
      //! DeferReset is used to reset the signal handling mechanism after the exception has been thrown.
      //! See the instance 'defer' declared below.
      struct DeferReset
      {
         ~DeferReset() noexcept
         {
            ::sigset_t actions;
            ::sigemptyset(&actions);
            ::pthread_sigmask(SIG_SETMASK, &actions, nullptr);
         }
      };
      DeferReset defer;

      throw ut::HardwareException(GetLinuxExceptionText(aSignal, aInfo));
   }
}

void ut::PromoteHardwareExceptions(bool aEnable) noexcept
{
   struct sigaction sa;
   if (aEnable)
   {
      sa.sa_sigaction = ::LinuxSignalTranslator;
      ::sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_SIGINFO | SA_RESTART;
   }
   else
   {
      sa.sa_handler = SIG_DFL;
      ::sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_RESTART;
   }

   ::sigaction(SIGBUS, &sa, nullptr);
#ifdef SIGEMT
   ::sigaction(SIGEMT, &sa, nullptr);
#endif
   ::sigaction(SIGFPE, &sa, nullptr);
   ::sigaction(SIGILL, &sa, nullptr);
   ::sigaction(SIGSEGV, &sa, nullptr);
   ::sigaction(SIGSYS, &sa, nullptr);
   ::sigaction(SIGTRAP, &sa, nullptr);
}

#endif

bool ut::ResetStackOverflow() noexcept
{
#ifdef _WIN32
   return _resetstkoflw();
#else
   return false;
#endif
}

namespace
{
using Lock = std::lock_guard<std::mutex>;

static const char* sApplicationName = nullptr;
static const char* sVersion         = nullptr;
static std::string sLogFilePath;
static std::mutex  sLogFileMutex;

//! Opens aStream and writes header information to it.
//! The Lock& parameter forces callers to have obtained a lock first.
//! Returns whether the stream could be opened.
bool WriteLogHeader(std::ofstream& aStream, Lock&)
{
   // The first time the file is opened, overwrite its contents.
   // If it is opened again, append to its contents.
   static std::ios::openmode openMode = std::ios::out | std::ios::trunc;
   aStream.open(sLogFilePath, openMode);
   if (aStream.is_open())
   {
      std::time_t currentTime = std::time(nullptr);
      openMode                = std::ios::app;

      aStream << "########################################################################\n";
      aStream << std::ctime(&currentTime) << "\n";
      if (sVersion && *sVersion)
      {
         aStream << "Version: " << sVersion << "\n";
      }
      aStream << "Compiler: " << UT_PLUGIN_API_COMPILER_STRING << "\n";
      return true;
   }
   return false;
}
} // namespace

ut::ExceptionDetails::ExceptionDetails(std::exception_ptr aException) noexcept
{
   try
   {
      std::rethrow_exception(aException);
   }
   catch (const ut::FatalException& e)
   {
      const char* what = e.what();
      mTypeName        = ut::TypeNameOf(e);
      mWhatMessage     = what ? what : std::string();
      mFatalException  = true;
      mStacktrace      = e.GetStacktrace();
   }
   catch (const UtException& e)
   {
      const char* what = e.what();
      mTypeName        = ut::TypeNameOf(e);
      mWhatMessage     = what ? what : std::string();
      mStacktrace      = e.GetStacktrace();
   }
   catch (const std::exception& e)
   {
      const char* what = e.what();
      mTypeName        = ut::TypeNameOf(e);
      mWhatMessage     = what ? what : std::string();
   }
   catch (...)
   {
      mFatalException = true;
      // Leave other values default.
   }
}

void ut::SetupApplicationLog(const char* aApplicationName, const char* aVersion, std::string aPath) noexcept
{
   Lock l(sLogFileMutex);
   sApplicationName = aApplicationName;
   sVersion         = aVersion;
   sLogFilePath.swap(aPath);
}

const char* ut::GetApplicationName() noexcept
{
   return sApplicationName;
}

std::string ut::GetApplicationLogPath() noexcept
{
   if (!sLogFilePath.empty())
   {
      std::string cwd = UtPath::WorkingDirectory().GetNormalizedPath();
      return cwd + "/" + sLogFilePath;
   }
   return std::string();
}

void ut::WriteTerminateLog() noexcept
{
   Lock          l{sLogFileMutex};
   std::ofstream file;

   if (::WriteLogHeader(file, l))
   {
      file << "Called std::terminate().\n";

      ut::PrintStacktrace(file, ut::stacktrace::current());

      file << "\n\n";
   }
}

void ut::WriteExceptionLog(const char* aSource, const ut::ExceptionDetails& aException) noexcept
{
   Lock          l{sLogFileMutex};
   std::ofstream file;

   if (::WriteLogHeader(file, l))
   {
      if (aException.mTypeName.empty())
      {
         file << "Caught unknown exception.\n";
      }
      else
      {
         const char* excSubType = aException.mFatalException ? "a fatal" : "an unhandled";
         const char* source     = aSource ? aSource : sApplicationName;
         file << source << " threw " << excSubType << " exception.\n";
         file << "Type: " << aException.mTypeName << "\n";
         file << "What: " << aException.mWhatMessage << "\n";

         if (aException.mStacktrace)
         {
            ut::PrintStacktrace(file, *aException.mStacktrace);
         }
         else
         {
            file << "No stacktrace available.\n";
         }
      }

      file << "\n\n";
   }
}

void ut::TerminateHandler()
{
   ut::WriteTerminateLog();

   const char* appName = sApplicationName ? sApplicationName : "The application";
   auto        out     = ut::log::fatal() << appName << " has terminated unexpectedly.";
   out.AddNote() << "See '" << ut::GetApplicationLogPath() << "' for more information.";
   out.AddNote() << "Please provide this log with a bug report to the AFSIM Development Team.";
}

void ut::ReportException(const char* aSource, std::exception_ptr aException, const char* aAdditionalInfo) noexcept
{
   ut::ExceptionDetails ex{aException};
   ut::WriteExceptionLog(aSource, ex);

   auto out = ut::log::fatal();
   if (ex.mTypeName.empty())
   {
      out << aSource << " terminated unexpectedly due to an unknown exception.";
   }
   else
   {
      const char* excSubType = ex.mFatalException ? "a fatal" : "an unhandled";
      out << aSource << " terminated unexpectedly due to " << excSubType << " exception.";
      out.AddNote() << "Type: " << ex.mTypeName;
      out.AddNote() << "What: " << ex.mWhatMessage;
   }

   if (aAdditionalInfo)
   {
      out.AddNote() << aAdditionalInfo;
   }
   out.AddNote() << "See '" << ut::GetApplicationLogPath() << "' for more information.";
   out.AddNote() << "Please provide this log with a bug report to the AFSIM Development Team.";
}

static std::terminate_handler sTerminateHandler = ut::TerminateHandler;

void ut::SetApplicationErrorHandling(std::terminate_handler aFunction, unsigned long aStackSize /* = 0x28000 */) noexcept
{
   sTerminateHandler = aFunction;
   ut::SetupThreadErrorHandling(aStackSize);
}

void ut::SetupThreadErrorHandling(unsigned long aStackSize /* = 0x28000 */) noexcept
{
   std::set_terminate(sTerminateHandler);
   ut::PromoteHardwareExceptions(true);
#if defined(PROMOTE_HARDWARE_EXCEPTIONS) && defined(_WIN32)
   ULONG reservedBytes = aStackSize;
   ::SetThreadStackGuarantee(&reservedBytes);
#endif
}
