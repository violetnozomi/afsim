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
#ifndef UTPROCESS_HPP
#define UTPROCESS_HPP

#include "ut_export.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class UtProcessImpl;

//! Allows execution and keeping track of child processes
class UT_EXPORT UtProcess
{
public:
   virtual ~UtProcess();

   //! Extended options for process execution
   struct ExecOptions
   {
      //! When true, stdout is redirected to a pipe
      //! The pipe may be read using UtProcess::Read()
      bool mStdOutToPipe = false;
      //! Use the OS's shell when executing the process
      //! Has no effect on windows
      bool mUseShell = true;
      //! Forces the process to open in a new command prompt window
      //! Has no effect on linux
      bool mCreateNewConsole = false;
      //! Show the command prompt window when executing the process
      //! Ignored if mCreateNewConsole is true
      //! Has no effect on linux
      bool mShowConsoleWindow = true;
      //! When not empty, triggers redirection of STDOUT to a file
      std::string mStdOutFile{};
      //! When not empty, triggers redirection of STDERR to a file
      std::string mStdErrFile{};
      //! When not empty, sets the working directory for the process
      std::string mWorkingDirectory{};
   };

   static std::unique_ptr<UtProcess> Execute(const std::string& aProcessPath,
                                             const std::string& aArgs,
                                             bool               aRedirectIO = false);

   static std::unique_ptr<UtProcess> Execute(const std::string&              aProcessPath,
                                             const std::vector<std::string>& aArgs,
                                             const ExecOptions&              aOptions);

   static UtProcess* GetCurrent();

   static std::vector<std::string> SplitByQuotes(const std::string& aCommandLine);

   static std::vector<std::string> SplitCommandLine(const std::string&                        aCommandLine,
                                                    const std::map<std::string, std::string>& aVariables);


   bool HasExited();

   int GetExitCode();

   void Terminate();

   void Wait();

   bool Read(std::string& aBuffer);

   unsigned int GetProcessId() const;

   uintptr_t GetProcessHandle() const;

private:
   UtProcess();

   UtProcessImpl* mImpl;
};

#endif
