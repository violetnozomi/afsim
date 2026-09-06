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

#include "UtProcess.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPathParser.hpp"
#include "UtStringUtil.hpp"

#ifdef _WIN32
#include "windows.h"
// Windows implementation using CreateProcess()
class UtProcessImpl
{
private:
   // Ensure the path uses all back-slashes
   void FixPath(std::string& aPath) { std::replace(aPath.begin(), aPath.end(), '/', '\\'); }

   bool SetupRedirectIO()
   {
      HANDLE currentProcess = ::GetCurrentProcess();
      HANDLE stdOutReadTemp = nullptr;
      HANDLE stdErrReadTemp = nullptr;

      SECURITY_ATTRIBUTES sa;
      memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
      sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
      sa.lpSecurityDescriptor = nullptr;
      sa.bInheritHandle       = true;

      // Create the child stdout pipe.
      if (::CreatePipe(&stdOutReadTemp, &mStdOutWrite, &sa, 0) == 0)
      {
         return false;
      }

      // Create the child stderr pipe.
      if (::CreatePipe(&stdErrReadTemp, &mStdErrWrite, &sa, 0) == 0)
      {
         return false;
      }

      // Create new output read handle and the input write handles. Set
      // the Properties to FALSE. Otherwise, the child inherits the
      // properties and, as a result, non-closeable handles to the pipes
      // are created.
      if (::DuplicateHandle(currentProcess, stdOutReadTemp, currentProcess, &mStdOutRead, 0, FALSE, DUPLICATE_SAME_ACCESS) ==
          0)
      {
         ::CloseHandle(stdOutReadTemp);
         return false;
      }

      if (::DuplicateHandle(currentProcess, stdErrReadTemp, currentProcess, &mStdErrRead, 0, FALSE, DUPLICATE_SAME_ACCESS) ==
          0)
      {
         ::CloseHandle(stdOutReadTemp);
         return false;
      }

      // Close inheritable copies of the handles you do not want to be inherited
      ::CloseHandle(stdOutReadTemp);
      ::CloseHandle(stdErrReadTemp);

      return true;
   }

   bool   mRedirectIO;
   HANDLE mStdOutWrite;
   HANDLE mStdOutRead;
   HANDLE mStdErrWrite;
   HANDLE mStdErrRead;


public:
   UtProcessImpl()
      : mHandle(nullptr)
      , mID(0)
      , mHasExited(false)
      , mExitCode(0)
      , mRedirectIO(false)
      , mStdOutRead(nullptr)
      , mStdOutWrite(nullptr)
      , mStdErrWrite(nullptr)
      , mStdErrRead(nullptr)
      , mFileRedirect1(nullptr)
      , mFileRedirect2(nullptr)
   {
   }

   ~UtProcessImpl()
   {
      if (mFileRedirect1 != nullptr)
      {
         ::CloseHandle(mFileRedirect1);
      }
      if (mFileRedirect2 != nullptr)
      {
         ::CloseHandle(mFileRedirect2);
      }
   }

   // MS provides CommandLineToArgvW(), but not the inverse.  Because we usually have an argument
   // list, we need to translate a list of strings into a command line.
   //
   // From: http://blogs.msdn.com/b/twistylittlepassagesallalike/archive/2011/04/23/everyone-quotes-arguments-the-wrong-way.aspx
   // This routine appends the given argument to a command line such
   // that CommandLineToArgvW will return the argument string unchanged.
   // Arguments in a command line should be separated by spaces; this
   // function does not add these spaces.
   //
   // Force - Supplies an indication of whether we should quote
   // the argument even if it does not contain any characters that would
   // ordinarily require quoting.
   std::string ArgvToCommandLine(const std::string& aArgument, bool aForce = false)
   {
      std::string commandLine;
      // Unless we're told otherwise, don't quote unless we actually
      // need to do so --- hopefully avoid problems if programs won't
      // parse quotes properly

      if (aForce == false && aArgument.empty() == false && aArgument.find_first_of(" \t\n\v\"") == std::string::npos)
      {
         commandLine.append(aArgument);
      }
      else
      {
         commandLine.push_back(L'"');

         for (std::string::const_iterator i = aArgument.begin();; ++i)
         {
            unsigned numberBackslashes = 0;

            while (i != aArgument.end() && *i == L'\\')
            {
               ++i;
               ++numberBackslashes;
            }

            if (i == aArgument.end())
            {
               // Escape all backslashes, but let the terminating double quotation mark
               // we add below be interpreted as a meta-character.
               commandLine.append(numberBackslashes * 2, L'\\');
               break;
            }
            else if (*i == L'"')
            {
               // Escape all backslashes and the following double quotation mark.
               commandLine.append(numberBackslashes * 2 + 1, L'\\');
               commandLine.push_back(*i);
            }
            else
            {
               // Backslashes aren't special here.
               commandLine.append(numberBackslashes, L'\\');
               commandLine.push_back(*i);
            }
         }

         commandLine.push_back(L'"');
      }
      return commandLine;
   }

   std::string ArgvToCommandLine(const std::vector<std::string>& aArgument)
   {
      std::string cmdLine;
      for (size_t i = 0; i < aArgument.size(); ++i)
      {
         if (aArgument[i].length() > MAX_PATH)
         {
            cmdLine = "";
            break;
         }

         if (i != 0)
         {
            cmdLine += ' ';
         }
         cmdLine += ArgvToCommandLine(aArgument[i]);
      }
      return cmdLine;
   }

   bool Execute(const std::string& aPath, const std::vector<std::string>& aArgs, const UtProcess::ExecOptions& aOptions)
   {
      if (aOptions.mStdOutToPipe)
      {
         mRedirectIO = SetupRedirectIO();
      }

      bool        ok;
      std::string path = aPath;
      FixPath(path);
      PROCESS_INFORMATION pinfo;
      STARTUPINFO         startupInfo;
      memset(&startupInfo, 0, sizeof(startupInfo));
      startupInfo.cb = sizeof(startupInfo);
      if (mRedirectIO)
      {
         startupInfo.dwFlags     = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
         startupInfo.hStdOutput  = mStdOutWrite;
         startupInfo.hStdError   = mStdErrWrite;
         startupInfo.wShowWindow = SW_HIDE;
      }
      else if (!aOptions.mStdErrFile.empty() || !aOptions.mStdOutFile.empty())
      {
         startupInfo.dwFlags     = STARTF_USESTDHANDLES;
         HANDLE stdoutFileHandle = nullptr;
         HANDLE stderrFileHandle = nullptr;
         if (!aOptions.mStdErrFile.empty())
         {
            SECURITY_ATTRIBUTES sa;
            sa.nLength              = sizeof(sa);
            sa.lpSecurityDescriptor = nullptr;
            sa.bInheritHandle       = TRUE;
            mFileRedirect1 = stderrFileHandle = CreateFile(aOptions.mStdErrFile.c_str(),
                                                           FILE_APPEND_DATA,
                                                           FILE_SHARE_WRITE | FILE_SHARE_READ,
                                                           &sa,
                                                           OPEN_ALWAYS,
                                                           FILE_ATTRIBUTE_NORMAL,
                                                           nullptr);
            if (aOptions.mStdErrFile == aOptions.mStdOutFile)
            {
               stdoutFileHandle = stderrFileHandle;
            }
         }
         if (!aOptions.mStdOutFile.empty() && (stdoutFileHandle == nullptr))
         {
            SECURITY_ATTRIBUTES sa;
            sa.nLength              = sizeof(sa);
            sa.lpSecurityDescriptor = nullptr;
            sa.bInheritHandle       = TRUE;
            stdoutFileHandle        = CreateFile(aOptions.mStdOutFile.c_str(),
                                          FILE_APPEND_DATA,
                                          FILE_SHARE_WRITE | FILE_SHARE_READ,
                                          &sa,
                                          OPEN_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL,
                                          nullptr);
            if (mFileRedirect1 == nullptr)
            {
               mFileRedirect2 = stdoutFileHandle;
            }
            else
            {
               mFileRedirect1 = stdoutFileHandle;
            }
         }
         startupInfo.hStdOutput = stdoutFileHandle;
         startupInfo.hStdError  = stderrFileHandle;
      }

      std::vector<std::string> cmdLineList = aArgs;
      cmdLineList.insert(cmdLineList.begin(), path);
      std::string cmdLineString = ArgvToCommandLine(cmdLineList);

      if (cmdLineString.empty())
      {
         return false;
      }

      char* cmdLineArray = new char[cmdLineString.length() + 1];
      cmdLineString.copy(cmdLineArray, cmdLineString.length());
      cmdLineArray[cmdLineString.length()] = 0;

      const char* workingDir = nullptr;
      if (!aOptions.mWorkingDirectory.empty())
      {
         workingDir = aOptions.mWorkingDirectory.c_str();
      }

      int flags = 0;
      if (aOptions.mCreateNewConsole)
      {
         flags |= CREATE_NEW_CONSOLE;
      }
      else if (!aOptions.mShowConsoleWindow)
      {
         flags |= CREATE_NO_WINDOW;
      }

      if (CreateProcess(nullptr,
                        const_cast<char*>(cmdLineString.c_str()),
                        nullptr,
                        nullptr,
                        true,
                        flags,
                        nullptr,
                        workingDir,
                        &startupInfo,
                        &pinfo))
      {
         mHandle = pinfo.hProcess;
         mID     = pinfo.dwProcessId;
         ok      = true;
         ::CloseHandle(pinfo.hThread);
      }
      else
      {
         ok = false;
      }
      delete[] cmdLineArray;
      if (mRedirectIO)
      {
         // Close pipe handles (do not continue to modify the parent).
         // You need to make sure that no handles to the write end of the
         // output pipe are maintained in this process or else the pipe will
         // not close when the child process exits and the ReadFile will hang.
         ::CloseHandle(mStdOutWrite);
         ::CloseHandle(mStdErrWrite);
      }
      return ok;
   }

   void SetCurrent()
   {
      mHandle = GetCurrentProcess();
      mID     = GetCurrentProcessId();
   }

   void Terminate() { TerminateProcess(mHandle, 0); }

   bool HasExited()
   {
      if (mHasExited)
      {
         return true;
      }
      DWORD exitCode;
      BOOL  status = GetExitCodeProcess(mHandle, &exitCode);
      if (STILL_ACTIVE == exitCode)
      {
         mHasExited = false;
      }
      else
      {
         mExitCode  = (int)exitCode;
         mHasExited = true;
      }
      return mHasExited;
   }

   void Wait()
   {
      WaitForSingleObject(mHandle, INFINITE);
      HasExited();
   }

   bool Read(std::string& aBuffer)
   {
      bool read = false;

      if (mStdOutRead != nullptr)
      {
         CHAR  buffer[32];
         DWORD bytesRead;
         if ((::ReadFile(mStdOutRead, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) == 0) || (bytesRead == 0))
         {
            if (::GetLastError() == ERROR_BROKEN_PIPE)
            {
               mStdOutRead = nullptr;
               mStdErrRead = nullptr;
            }
         }
         if (bytesRead > 0)
         {
            buffer[bytesRead] = '\0';
            aBuffer           = std::string(buffer);
            read              = true;
         }
      }
      return read;
   }

   int GetExitCode() { return mExitCode; }

   unsigned int GetProcessId() const { return mID; }

   uintptr_t GetProcessHandle() const { return reinterpret_cast<uintptr_t>(mHandle); }

   HANDLE       mHandle;
   unsigned int mID;
   bool         mHasExited;
   int          mExitCode;

   HANDLE mFileRedirect1;
   HANDLE mFileRedirect2;
};

#else

#include <signal.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Unix implementation using fork() and execv()
class UtProcessImpl
{
private:
   // Ensure the path uses all forward slashes
   void FixPath(std::string& aPath) { std::replace(aPath.begin(), aPath.end(), '\\', '/'); }

public:
   UtProcessImpl()
      : mHandle(0)
      , mHasExited(false)
      , mExitCode(0)
      , mRedirectIO(false)
      , mStdOutRead(0)
   {
   }

   ~UtProcessImpl() {}

   bool Execute(const std::string& aPath, const std::vector<std::string>& aArgs, const UtProcess::ExecOptions& aOptions)
   {
      mHandle          = 0;
      mHasExited       = true;
      mRedirectIO      = aOptions.mStdOutToPipe;
      mStdOutRead      = 0;
      bool        ok   = true;
      std::string path = aPath;

      FixPath(path);

      // We don't know if execv will succeed until after the fork.
      // Unless we force the ran program to signal that it ran ok,
      // the best way I can think of to make sure the execlp will work
      // is to do a fstat() on the .exe
      bool foundFile = false;
      {
         struct stat file_stat;
         if (0 == stat(path.c_str(), &file_stat))
         {
            // TODO: Improve this code to ensure
            //       that we specifically have access to execute

            // If someone can execute, go ahead an try it
            if ((S_IXUSR | S_IXGRP | S_IXOTH) & file_stat.st_mode)
            {
               foundFile = true;
            }
         }
      }

      if (foundFile)
      {
         int out[2];
         if (mRedirectIO)
         {
            if (pipe(out) != 0)
            {
               auto out = ut::log::error() << "pipe()";
               out.AddNote() << "Errno: " << std::strerror(errno);
            }
         }

         int rv = fork();
         if (rv == 0) // Child process.
         {
            if (mRedirectIO)
            {
               close(out[0]);
               dup2(out[1], 1);
               close(out[1]);
            }
            else
            {
               if (!aOptions.mStdOutFile.empty())
               {
                  FILE* f = fopen(aOptions.mStdOutFile.c_str(), "w");
                  dup2(fileno(f), STDOUT_FILENO);
                  if (aOptions.mStdErrFile == aOptions.mStdOutFile)
                  {
                     dup2(fileno(f), STDERR_FILENO);
                  }
                  fclose(f);
               }
               if (!aOptions.mStdErrFile.empty() && aOptions.mStdErrFile != aOptions.mStdOutFile)
               {
                  FILE* f = fopen(aOptions.mStdOutFile.c_str(), "w");
                  dup2(fileno(f), STDERR_FILENO);
                  fclose(f);
               }
            }
            if (!aOptions.mWorkingDirectory.empty())
            {
               if (chdir(aOptions.mWorkingDirectory.c_str()) != 0)
               {
                  auto out = ut::log::error() << "chdir() to " << aOptions.mWorkingDirectory << " failed";
                  out.AddNote() << "Errno: " << std::strerror(errno);
               }
            }
            if (aOptions.mUseShell)
            {
               // Execute from shell
               path += " ";
               path += UtStringUtil::Join(" ", aArgs);
               execlp("/bin/sh", "sh", "-c", path.c_str(), (char*)nullptr);
            }
            else
            {
               // Execute directly
               char**      argv    = new char*[aArgs.size() + 2];
               std::string exeName = UtPathParser::GetNameAndExtension(path);
               argv[0]             = (char*)exeName.c_str();
               for (size_t i = 0; i < aArgs.size(); ++i)
               {
                  argv[i + 1] = (char*)aArgs[i].c_str();
               }
               argv[aArgs.size() + 1] = nullptr;
               execvp(path.c_str(), argv);
            }

            // If we get here, bad things happened,
            // so exit the forked process
            exit(1);
         }
         else if (rv == -1) // Error creating child process.
         {
            // fork() failed
            if (mRedirectIO)
            {
               close(out[0]);
               close(out[1]);
            }
            ok = false;
         }
         else // Parent process.
         {
            if (mRedirectIO)
            {
               mStdOutRead = out[0];
               close(out[1]);
            }
            mHandle = rv;
         }
      }
      else
      {
         ok = false;
      }

      mHasExited = !ok;
      return ok;
   }

   void SetCurrent() { mHandle = getpid(); }

   bool HasExited()
   {
      if (mHasExited)
      {
         return true;
      }
      int status;
      if (mHandle == waitpid(mHandle, &status, WNOHANG))
      {
         if (WIFEXITED(status))
         {
            mExitCode = WEXITSTATUS(status);
         }
         mHasExited = true;
      }
      return mHasExited;
   }

   int GetExitCode() { return mExitCode; }

   unsigned int GetProcessId() const { return static_cast<unsigned int>(mHandle); }
   unsigned int GetProcessHandle() const { return static_cast<unsigned int>(mHandle); }

   void Terminate()
   {
      if (!mHasExited)
      {
         kill(mHandle, 9);
      }
   }

   void Wait()
   {
      int   status;
      pid_t rv = waitpid(mHandle, &status, 0);
      if (rv == mHandle)
      {
         if (WIFEXITED(status))
         {
            mExitCode = WEXITSTATUS(status);
         }
         mHasExited = true;
      }
   }

   bool Read(std::string& aBuffer)
   {
      if (mStdOutRead != 0)
      {
         char buffer[32];
         int  bytesRead = read(mStdOutRead, buffer, sizeof(buffer) - 1);
         if (bytesRead > 0)
         {
            buffer[bytesRead] = 0;
            aBuffer           = buffer;
            return true;
         }
         else
         {
            mStdOutRead = 0;
         }
      }
      return false;
   }

   pid_t mHandle;
   bool  mHasExited;
   int   mExitCode;
   bool  mRedirectIO;
   int   mStdOutRead;
};

#endif

UtProcess::UtProcess()
{
   mImpl = new UtProcessImpl();
}


UtProcess::~UtProcess()
{
   delete mImpl;
   mImpl = nullptr;
}

//! Execute a child process.
//! @param aPath relative or full path to the process to execute.
//! @param aArgs String of arguments to pass to the new process.
//! @param aRedirectIO Redirects the spawned process's standard
//!                    output and error streams to this process
//!                    making their contents available via 'Read'.
std::unique_ptr<UtProcess> UtProcess::Execute(const std::string& aPath, const std::string& aArgs, bool aRedirectIO)
{
   ExecOptions options;
   options.mStdOutToPipe         = aRedirectIO;
   std::vector<std::string> args = SplitByQuotes(aArgs);
   return Execute(aPath, args, options);
}

std::unique_ptr<UtProcess> UtProcess::Execute(const std::string&              aProcessPath,
                                              const std::vector<std::string>& aArgs,
                                              const ExecOptions&              aOptions)
{
   auto newProcess = std::unique_ptr<UtProcess>(new UtProcess());
   if (!newProcess->mImpl->Execute(aProcessPath, aArgs, aOptions))
   {
      return nullptr;
   }
   return newProcess;
}

UtProcess* UtProcess::GetCurrent()
{
   UtProcess* thisProcess = new UtProcess();
   thisProcess->mImpl->SetCurrent();
   return thisProcess;
}
//! Returns 'true' if the process has exited.
bool UtProcess::HasExited()
{
   return mImpl->HasExited();
}


//! Returns the exit code of the process
//! Only valid after HasExited() returns true
int UtProcess::GetExitCode()
{
   return mImpl->GetExitCode();
}

//! Attempts to terminate the process.
//! @note There is no guarantee that the process will terminate.
void UtProcess::Terminate()
{
   mImpl->Terminate();
}

//! Waits for the process to terminate.
void UtProcess::Wait()
{
   mImpl->Wait();
}

//! Reads from the process's standard output stream.
//! This method is valid to call if the 'aRedirectIO' parameter was passed to 'Execute'.
//! @note Read is non-blocking.
bool UtProcess::Read(std::string& aBuffer)
{
   return mImpl->Read(aBuffer);
}

//! Returns the ID of the process.
//! Returns 0 if the process does not exist.
unsigned int UtProcess::GetProcessId() const
{
   return mImpl->GetProcessId();
}

//! Returns the Handle of the process.  In windows, the handle is
//! different than the process ID.  In other OS's, this returns the
//! process's ID.
//! Returns 0 if the process does not exist.
uintptr_t UtProcess::GetProcessHandle() const
{
   return mImpl->GetProcessHandle();
}


// Divide a command-line string into pieces using quotes and spaces
// ex:    'quoted strings' "with either type are joined together" but spaces divide
//   returns:  ["quoted strings", "with either type are joined together", "but", "spaces", "divide"]
std::vector<std::string> UtProcess::SplitByQuotes(const std::string& aCommandLine)
{
   std::vector<std::string> words;
   int                      state    = 0;
   char                     endQuote = 0;
   std::string              word;
   for (size_t i = 0; i <= aCommandLine.size(); ++i)
   {
      char c = i < aCommandLine.size() ? aCommandLine[i] : '\0';
      if (state == 0)
      {
         switch (c)
         {
         case '\'':
            endQuote = '\'';
            state    = 1;
            break;
         case '"':
            endQuote = '"';
            state    = 1;
            break;
         case ' ':
            if (!word.empty())
            {
               words.push_back(word);
            }
            word = std::string();
            break;
         case '\0':
            words.push_back(word);
            break;
         case '\\':
            state = 2;
            break;
         default:
            word += c;
            break;
         }
      }
      else if (state == 1)
      {
         if (c == endQuote)
         {
            words.push_back(word);
            state = 0;
            word.clear();
         }
         else
         {
            word += c;
         }
      }
      else if (state == 2) // allow escaping spaces only
      {
         if (c == ' ')
         {
            word += ' ';
         }
         else if (c == '\0')
         {
            word += '\\';
         }
         else
         {
            word += '\\';
            word += c;
         }
         state = 0;
      }
   }
   return words;
}

namespace
{
// Returns the indices at the start and end of the variable
// a variable has this format:
//    $(VARNAME)
//    ^        ^     <- the reported indices
// Returns pair(npos, npos) if no variable is found
std::pair<size_t, size_t> FindVar(std::string& aText)
{
   for (size_t i = 0; i + 1 < aText.size(); ++i)
   {
      if (aText[i] == '$' && aText[i + 1] == '(')
      {
         size_t endIndex = std::string::npos;
         for (size_t j = i + 2; j < aText.size(); ++j)
         {
            if ((isalnum(aText[j]) != 0) || aText[j] == '_')
            {
               continue;
            }
            if (aText[j] == ')')
            {
               endIndex = j;
            }
            break;
         }
         if (endIndex != std::string::npos)
         {
            return std::make_pair(i, endIndex);
         }
      }
   }
   return std::make_pair(std::string::npos, std::string::npos);
}
} // namespace

// Splits a command line string
//  ie:    -option "hi you" -option2 'abc $(VAR1) def'
// -> {"-option", "hi you", "-option2", "abc MyVar1Value def"}
//  Quote escapes aren't handled yet
// Note: It is expected that a shell will not be processing these arguments, therefore quotes are removed
std::vector<std::string> UtProcess::SplitCommandLine(const std::string&                        aCommandLine,
                                                     const std::map<std::string, std::string>& aVariables)
{
   std::string expanded = aCommandLine;
   // Now apply variable substitution
   // expand variables
   {
      bool foundMatches = true;
      // Right now, it's possible to reference variables inside other variables.
      // if $(x)="$(y)" and $(y)="$(x)", we could do this forever.
      // Limit number of substitutions
      const int cSUBST_LIMIT = 500;
      for (int i = 0; i < cSUBST_LIMIT && foundMatches; ++i)
      {
         foundMatches                               = false;
         std::pair<std::size_t, std::size_t> varInd = FindVar(expanded);
         if (varInd.first != std::string::npos)
         {
            std::string varName = expanded.substr(varInd.first + 2, varInd.second - varInd.first - 2);
            if (aVariables.find(varName) != aVariables.end())
            {
               std::string varValue = aVariables.find(varName)->second;
               expanded             = expanded.substr(0, varInd.first) + varValue + expanded.substr(varInd.second + 1);
               foundMatches         = true;
            }
         }
      }
   }

   std::vector<std::string> args = SplitByQuotes(expanded);

   // remove any empty strings
   while (std::find(args.begin(), args.end(), "") != args.end())
   {
      args.erase(std::find(args.begin(), args.end(), ""));
   }

   return args;
}
