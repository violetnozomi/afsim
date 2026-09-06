// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVEXPORT_HPP
#define RVEXPORT_HPP

#if defined(_WIN32)

#pragma warning(disable : 4251)
#pragma warning(disable : 4786)

#ifdef mystic_lib_EXPORTS
#define RV_BUILD_DLL
#endif
#if !defined(RV_BUILD_DLL) && defined(SWDEV_ALL_USE_DLL)
#define RV_USE_DLL
#endif

#if defined(RV_BUILD_DLL) // Creating DLL (i.e. exporting)
#define RV_EXPORT __declspec(dllexport)
#elif defined(RV_USE_DLL) // Using DLL (i.e. importing).
#define RV_EXPORT __declspec(dllimport)
#else
// Plugin build only, cannot link statically.
#endif
#else
#define RV_EXPORT
#endif

#endif // RVEXPORT_HPP
