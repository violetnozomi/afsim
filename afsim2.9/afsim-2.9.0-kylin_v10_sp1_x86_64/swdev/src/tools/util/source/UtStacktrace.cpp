// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtStacktrace.hpp"

#include <iomanip>
#include <ostream>

#include "UtLogStream.hpp"

namespace
{
using NativeHandleType = ut::stacktrace_entry::native_handle_type;
}

#ifdef _WIN32 // Windows implementation

#ifdef _M_X64 // This code must be run on x64 architecture.

#include <algorithm>
#include <mutex>

#include <windows.h>

#include <Psapi.h>

#include "UtStringUtil.hpp"

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "dbghelp.lib")

// Some versions of imagehlp.dll lack the proper packing directives themselves
// so we need to do it.
#pragma pack(push, before_imagehlp, 8)
#include <imagehlp.h>
#pragma pack(pop, before_imagehlp)

namespace
{
constexpr std::size_t cMAX_NAME        = 512;
constexpr DWORD       cMACHINE_TYPE    = IMAGE_FILE_MACHINE_AMD64;
constexpr const char* cPDB_SEARCH_PATH = nullptr;

//! DebugEnvironment is an implementation class that wraps the dbghlp API.
//! Provides thread-safe wrappers for accessing the stacktrace
//!    and retrieving stack frame details.
//! SymInitialize is called once on application startup,
//!    and SymCleanup is called on application shutdown.
class DebugEnvironment
{
public:
   //! Initializes the dbghlp symbol handler for the current process.
   DebugEnvironment() noexcept
      : mProcess(::GetCurrentProcess())
      , mSetupSuccess(::SymInitialize(mProcess, cPDB_SEARCH_PATH, TRUE))
   {
      if (mSetupSuccess)
      {
         ::SymSetOptions(::SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
      }
   }

   //! Tears down the dbghlp symbol handler for the current process.
   //! If setup failed, does nothing.
   ~DebugEnvironment() noexcept
   {
      if (mSetupSuccess)
      {
         ::SymCleanup(mProcess);
      }
   }

   //! Accesses the source information for the native handle.
   //! Safe to call from multiple threads.
   //! @param aNativeHandle is the handle to the stack frame.
   ut::detail::StackFrameDetails GetDetails(DWORD64 aNativeHandle)
   {
      if (mSetupSuccess)
      {
         std::lock_guard<std::mutex> lock(mMutex);
         DWORD                       offsetFromSymbol = 0;
         IMAGEHLP_LINE64             line             = {0};
         line.SizeOfStruct                            = sizeof(IMAGEHLP_LINE64);

         std::string description = GetDescription(aNativeHandle);
         ::SymGetLineFromAddr64(mProcess, aNativeHandle, &offsetFromSymbol, &line);
         std::string sourceFile = line.FileName ? line.FileName : "";

         return {std::move(description), std::move(sourceFile), line.LineNumber};
      }
      return {};
   }

   //! Retrieves a list of stack frames for the current call stack.
   //! @param aSkip is the number of frames to skip.
   //! @param aMaxDepth is the maximum number of entries to report.
   //! If aSkip is 0, the first entry is the calling context of this function.
   __declspec(noinline) std::vector<ut::stacktrace_entry> GetTrace(std::size_t aSkip, std::size_t aMaxDepth) noexcept
   {
      if (mSetupSuccess)
      {
         ::SymRefreshModuleList(mProcess);

         const std::size_t skipFrames = aSkip + 3;
         const std::size_t maxFrames  = std::min(aMaxDepth + skipFrames, ut::detail::cMAX_STACKTRACE_SIZE + skipFrames);

         CONTEXT      context;
         STACKFRAME64 stack;
         const HANDLE currentThread = ::GetCurrentThread();

         // Initialize context and stack.
         PrepareStacktrace(&context, &stack);
         std::lock_guard<std::mutex> lock(mMutex);

         std::vector<ut::stacktrace_entry> retval;
         for (std::size_t frameIdx = 0; frameIdx < maxFrames; frameIdx++)
         {
            ::SetLastError(0);
            const BOOL result = ::StackWalk64(cMACHINE_TYPE,
                                              mProcess,
                                              currentThread,
                                              &stack,
                                              &context,
                                              nullptr,
                                              ::SymFunctionTableAccess64,
                                              ::SymGetModuleBase64,
                                              nullptr);
            if (!result || stack.AddrPC.Offset == 0)
            {
               // Read failed or base reached. No good way to tell which.
               break;
            }
            else if (frameIdx >= skipFrames)
            {
               retval.emplace_back(static_cast<NativeHandleType>(stack.AddrPC.Offset));
            }
         }
         return retval;
      }
      return {};
   }

private:
   //! Looks up the name of the symbol that the handle refers to.
   //! Assumes mSetupSuccess is true and mMutex is locked.
   std::string GetDescription(DWORD64 aNativeHandle) const
   {
      // IMAGEHLP_SYMBOL64 requires more stack space than it allocates.
      // To get around this, we allocate a char buffer and use that.
      using SymType                    = IMAGEHLP_SYMBOL64;
      constexpr std::size_t bufferSize = sizeof(SymType) + cMAX_NAME;
      alignas(SymType) char buffer[bufferSize];

      SymType* sym = reinterpret_cast<SymType*>(&buffer);
      std::memset(sym, 0, bufferSize);
      sym->SizeOfStruct    = sizeof(SymType);
      sym->MaxNameLength   = cMAX_NAME;
      DWORD64 displacement = 0;

      if (::SymGetSymFromAddr64(mProcess, aNativeHandle, &displacement, sym))
      {
         char name[cMAX_NAME];
         ::UnDecorateSymbolName(sym->Name, name, cMAX_NAME, UNDNAME_COMPLETE);
         return name;
      }
      return ErrorToString(GetLastError());
   }

   //! Converts aErrorCode to a string.
   static std::string ErrorToString(DWORD aErrorCode)
   {
      char*       messageBuffer = nullptr;
      std::size_t size =
         ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          nullptr,
                          aErrorCode,
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          reinterpret_cast<char*>(&messageBuffer),
                          0,
                          nullptr);

      std::string message(messageBuffer, size);
      ::LocalFree(messageBuffer);

      UtStringUtil::TrimWhiteSpace(message);
      return message;
   }

   //! Initializes aContext and aStack.
   __declspec(noinline) void PrepareStacktrace(CONTEXT* aContext, STACKFRAME64* aStack) const
   {
      aContext->ContextFlags = CONTEXT_FULL;
      ::RtlCaptureContext(aContext);
      std::memset(aStack, 0, sizeof(STACKFRAME64));

      aStack->AddrPC.Offset    = aContext->Rip;
      aStack->AddrPC.Mode      = AddrModeFlat;
      aStack->AddrFrame.Offset = aContext->Rsp;
      aStack->AddrFrame.Mode   = AddrModeFlat;
      aStack->AddrStack.Offset = aContext->Rbp;
      aStack->AddrStack.Mode   = AddrModeFlat;
   }

   const HANDLE mProcess;
   const BOOL   mSetupSuccess;

   std::mutex mMutex;
};

static DebugEnvironment sDebugEnv;
} // namespace

void ut::stacktrace_entry::InitializeDetails() const
{
   if (!mDetails)
   {
      const DWORD64 nativeHandle = static_cast<DWORD64>(mNativeHandle);
      mDetails                   = sDebugEnv.GetDetails(nativeHandle);
   }
}

// static
ut::stacktrace ut::stacktrace::current(size_type aSkip, size_type aMaxDepth) noexcept
{
   auto frames = sDebugEnv.GetTrace(aSkip, aMaxDepth);
   return ut::stacktrace(std::move(frames));
}

#else // if not x64 architecture

#pragma message("WARNING: Stacktraces are only available on x64 architecture.")

// static
ut::stacktrace ut::stacktrace::current(size_type aSkip, size_type aMaxDepth) noexcept
{
   return ut::stacktrace();
}

#endif // x64 architecture

#else // Linux implementation missing source file and line

#include <cstdlib>
#include <vector>

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <unistd.h>

namespace
{
std::string Unmangle(const char* aName)
{
   char* unmangledName = abi::__cxa_demangle(aName, nullptr, nullptr, nullptr);
   if (unmangledName)
   {
      std::string retval = unmangledName;
      std::free(unmangledName);
      return retval;
   }
   return aName;
}
} // namespace

void ut::stacktrace_entry::InitializeDetails() const
{
   if (!mDetails)
   {
      const void* addr = reinterpret_cast<const void*>(mNativeHandle);
      Dl_info     info;
      ::dladdr(addr, &info);
      detail::StackFrameDetails& frame = mDetails.emplace();
      frame.mDescription               = info.dli_sname ? ::Unmangle(info.dli_sname) : std::string();
      frame.mSourceFile                = info.dli_fname ? info.dli_fname : std::string();
      frame.mSourceLine                = 0;
   }
}

ut::stacktrace ut::stacktrace::current(size_type aSkip, size_type aMaxDepth) noexcept
{
   ut::stacktrace retval;
   const int      skipFrames = static_cast<int>(aSkip + 1);
   const int maxFrames = static_cast<int>(std::min(aMaxDepth + skipFrames, detail::cMAX_STACKTRACE_SIZE + skipFrames));

   std::vector<void*> frameBuffer(maxFrames);

   const int count = ::backtrace(frameBuffer.data(), maxFrames);
   for (int i = 0; i < count; i++)
   {
      if (i >= skipFrames)
      {
         const NativeHandleType nativeHandle = reinterpret_cast<NativeHandleType>(frameBuffer[i]);
         retval.mContainer.emplace_back(nativeHandle);
      }
   }

   return retval;
}

#endif

void ut::PrintStacktrace(std::ostream& aStream, const ut::stacktrace& aTrace)
{
   if (aTrace.empty())
   {
      aStream << "Stacktrace is empty.\n"
                 "    Stacktraces are only available on supported architectures.\n";
#ifdef _WIN32
      aStream << "    Check that you are running on an x64 architecture and that PDB files are in the installation "
                 "folder.\n";
#endif
   }
   else
   {
      aStream << "Stacktrace:\n";
      for (std::size_t i = 0; i < aTrace.size(); i++)
      {
         aStream << "    " << std::setw(2) << i << "# " << aTrace[i] << "\n";
      }
   }
}

void ut::AppendStacktrace(ut::log::MessageStream& aStream, const ut::stacktrace& aTrace)
{
   if (aTrace.empty())
   {
      auto note = aStream.AddNote() << "Stacktrace is empty.";
      note.AddNote() << "Stacktraces are only available on supported architectures.";
#ifdef _WIN32
      note.AddNote()
         << "Check that you are running on an x64 architecture and that PDB files are in the installation folder.";
#endif
   }
   else
   {
      for (std::size_t i = 0; i < aTrace.size(); i++)
      {
         aStream.AddNote() << std::setw(2) << i << "# " << aTrace[i] << "\n";
      }
   }
}

constexpr const char* cUNKNOWN = R"(??)";

std::ostream& ut::operator<<(std::ostream& aStream, const ut::stacktrace_entry& aEntry)
{
   if (aEntry)
   {
      aStream << "0x" << std::hex << aEntry.native_handle() << std::dec << " (";
      if (aEntry.description().empty())
      {
         aStream << cUNKNOWN;
      }
      else
      {
         aStream << aEntry.description();
      }
      aStream << ") at ";
      if (aEntry.source_file().empty())
      {
         aStream << cUNKNOWN;
      }
      else
      {
         aStream << aEntry.source_file();
         if (aEntry.source_line() > 0)
         {
            aStream << ": " << aEntry.source_line();
         }
      }
   }
   else
   {
      aStream << cUNKNOWN;
   }
   return aStream;
}

std::ostream& ut::operator<<(std::ostream& aStream, const ut::stacktrace& aTrace)
{
   for (std::size_t i = 0; i < aTrace.size(); i++)
   {
      aStream << std::setw(2) << i << "# " << aTrace[i] << "\n";
   }
   return aStream;
}
