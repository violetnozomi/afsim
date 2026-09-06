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

#if !defined(WsfXIO_Export_hpp)
#define WsfXIO_Export_hpp

#if defined(_WIN32)

#pragma warning(disable : 4251)
#pragma warning(disable : 4786)

#ifdef xio_EXPORTS
#define XIO_BUILD_DLL
#endif
#if !defined(XIO_BUILD_DLL) && defined(SWDEV_ALL_USE_DLL)
#define XIO_USE_DLL
#endif

#if defined(XIO_BUILD_DLL) // Creating DLL (i.e. exporting)
#define WSF_EXPORT __declspec(dllexport)
#elif defined(XIO_USE_DLL) // Using DLL (i.e. importing).
#define WSF_EXPORT __declspec(dllimport)
#else
#define WSF_EXPORT // Compiling statically.
#endif
#else
#define WSF_EXPORT
#endif

#endif // WsfExport_hpp
