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

#ifndef WSFUTEXPORT_HPP
#define WSFUTEXPORT_HPP

#if defined(_WIN32)

// Prevent compiler warnings about dll-interface with templates.
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#if !defined(wsf_util_EXPORTS) && defined(SWDEV_ALL_USE_DLL)
#define WSF_UT_USE_DLL
#endif

#if defined(wsf_util_EXPORTS) // Creating DLL (i.e. exporting)
#define WSF_UT_EXPORT __declspec(dllexport)
#elif defined(WSF_UT_USE_DLL) // Using DLL (i.e. importing).
#define WSF_UT_EXPORT __declspec(dllimport)
#else
#define WSF_UT_EXPORT // Compiling statically.
#endif
#else
#define WSF_UT_EXPORT
#endif

#endif // WSFUTEXPORT_HPP
