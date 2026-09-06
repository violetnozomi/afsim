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
#ifndef UTCOMPILERVERSION_HPP
#define UTCOMPILERVERSION_HPP

#include <iostream>

extern "C"
{
#if defined(_WIN32)
   static const char* UtCompilerVersionString()
   {
      static char version[32];

      const char*  system   = "win";
      unsigned int compiler = _MSC_VER;
#ifdef _WIN64
      const char* bits = "64bit";
#else
      const char* bits  = "32bit";
#endif
#ifdef _DEBUG
      const char* build = "debug";
#else
      const char* build = "release";
#endif
      sprintf(version, "%s_%d_%s_%s", system, compiler, bits, build);
      return version;
   }
#elif defined(__linux__)
   static const char* UtCompilerVersionString()
   {
      static char version[32];

      const char*  system   = "lnx";
#ifdef __GNUC_PATCHLEVEL__
      unsigned int compiler = __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__;
#else
      unsigned int compiler = __GNUC__ * 10000 + __GNUC_MINOR__ * 100;
#endif
#ifdef __LP64__
      const char*  bits     = "64bit";
#else
      const char*  bits     = "32bit";
#endif
#ifdef __OPTIMIZE__
      const char*  build    = "release";
#else
      const char*  build    = "debug";
#endif
      sprintf(version, "%s_%d_%s_%s", system, compiler, bits, build);
      return version;
   }
#elif defined(sgi)
   static const char* UtCompilerVersionString()
   {
      static char version[32];

      const char*  system   = "sgi";
      unsigned int compiler = _COMPILER_VERSION;
#if (_MIPS_FPSET == 16) // number of registers.
      const char*  bits     = "32bit";
#else
      const char* bits  = "64bit";
#endif
#ifdef __OPTIMIZE__
      const char*  build    = "release";
#else
      const char* build = "debug";
#endif

      sprintf(version, "%s_%d_%s_%s", system, compiler, bits, build);
      return version;
   }
#else
   static const char* UtCompilerVersionString() { return "unknown"; }
#endif
}
#endif
