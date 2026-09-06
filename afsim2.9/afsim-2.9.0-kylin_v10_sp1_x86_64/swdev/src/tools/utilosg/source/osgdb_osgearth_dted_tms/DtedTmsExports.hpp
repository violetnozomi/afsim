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
#pragma once

#if defined(_WIN32)

#ifdef osgdb_osgearth_dted_tms_EXPORTS
#define DTED_TMS_BUILD_DLL
#endif
#if !defined(DTED_TMS_BUILD_DLL) && defined(SWDEV_ALL_USE_DLL)
#define DTED_TMS_USE_DLL
#endif

#if defined(DTED_TMS_BUILD_DLL) // Creating DLL (i.e. exporting)
#define DTED_TMS_EXPORT __declspec(dllexport)
#elif defined(DTED_TMS_USE_DLL) // Using DLL (i.e. importing).
#define DTED_TMS_EXPORT __declspec(dllimport)
#else
#define DTED_TMS_EXPORT // Compiling statically.
#endif

#else
#define DTED_TMS_EXPORT
#endif
