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

#if !defined(WsfRIPR_Export_hpp)
#define WsfRIPR_Export_hpp

#if defined(_WIN32)

#pragma warning(disable : 4251)
#pragma warning(disable : 4786)
#ifdef wsf_ripr_EXPORTS
#define RIPR_BUILD_DLL
#endif
#if !defined(RIPR_BUILD_DLL) && defined(SWDEV_ALL_USE_DLL)
#define RIPR_USE_DLL
#endif

#if defined(RIPR_BUILD_DLL) // Creating DLL (i.e. exporting)
#define RIPR_EXPORT __declspec(dllexport)
#elif defined(RIPR_USE_DLL) // Using DLL (i.e. importing).
#define RIPR_EXPORT __declspec(dllimport)
#else
#define RIPR_EXPORT // Compiling statically.
#endif
#else
#define RIPR_EXPORT
#endif

#endif // WsfRIPR_Export_hpp
