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

#include "UtQtMemoryUsage.hpp"

#if defined(_WIN32)
#include <stdio.h>

#include <windows.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#else
#include <QProcess>
#include <sys/types.h>
#include <unistd.h>
#endif

//! Returns the amount of physical memory being consumed in bytes.
//! @return Returns the amount of physical memory being consumed in bytes.
double UtQtMemoryUsage::GetMemoryUsageInBytes() const
{
   return GetMemoryUsageInBytesPrivate();
}

//! Returns the amount of physical memory being consumed in KBytes.
//! @return Returns the amount of physical memory being consumed in KBytes.
double UtQtMemoryUsage::GetMemoryUsageInKBytes() const
{
   return GetMemoryUsageInBytes() / 1024.0;
}

//! Returns the amount of physical memory being consumed in MBytes.
//! @return Returns the amount of physical memory being consumed in MBytes.
double UtQtMemoryUsage::GetMemoryUsageInMBytes() const
{
   return GetMemoryUsageInBytes() / 1048576.0;
}

//! Returns the amount of physical memory being consumed in GBytes.
//! @return Returns the amount of physical memory being consumed in GBytes.
double UtQtMemoryUsage::GetMemoryUsageInGBytes() const
{
   return GetMemoryUsageInBytes() / 1073741824.0;
}

//! Returns the memory usage as a string. It converts units of memory use
//! based on much memory is being consumed.
//! @return Returns a string that indicates how much memory is being consumed and the units.
QString UtQtMemoryUsage::GetMemoryUsageString() const
{
   return GetMemoryUsageString(GetMemoryUsageInBytes());
}

// static
QString UtQtMemoryUsage::GetMemoryUsageString(double aMemoryInBytes)
{
   QString memoryUsageString;

   double memoryUsage = aMemoryInBytes;
   if (memoryUsage >= 1073741824.0)
   {
      memoryUsageString = QString::number(memoryUsage / 1073741824.0) + " GB";
   }
   else if (memoryUsage >= 1048576.0)
   {
      memoryUsageString = QString::number(memoryUsage / 1048576.0) + " MB";
   }
   else if (memoryUsage >= 1024.0)
   {
      memoryUsageString = QString::number(memoryUsage / 1024.0) + " KB";
   }
   else
   {
      memoryUsageString = QString::number(memoryUsage) + " B";
   }
   return memoryUsageString;
}

// private
double UtQtMemoryUsage::GetMemoryUsageInBytesPrivate() const
{
   double memoryUsageInBytes = 0;

#if defined(_WIN32)
   DWORD  processID     = GetCurrentProcessId();
   HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
   if (processHandle != nullptr)
   {
      PROCESS_MEMORY_COUNTERS processMemoryCounters;
      if (GetProcessMemoryInfo(processHandle, &processMemoryCounters, sizeof(processMemoryCounters)))
      {
         memoryUsageInBytes = (double)processMemoryCounters.WorkingSetSize;
      }
      CloseHandle(processHandle);
   }
#else
   // On unix based platforms invoke the 'ps' system command and read the rss value.
   pid_t    pid = getpid();
   QProcess ps;
   ps.start("ps -p " + QString::number(pid) + " -o rss=");
   if (ps.waitForFinished(1000))
   {
      QString rss        = ps.readAll();
      memoryUsageInBytes = rss.toDouble() * 1024.0;
   }
#endif

   return memoryUsageInBytes;
}
