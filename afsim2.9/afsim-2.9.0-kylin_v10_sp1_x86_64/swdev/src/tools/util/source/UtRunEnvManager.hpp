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

// UtRunEnvManager.h: interface for the UtRunEnvManager class
//  This class collects a set of useful variables and a constructor to
//  collect their values.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef UTRUNENVMANAGER_HPP
#define UTRUNENVMANAGER_HPP

#include "ut_export.h"

#include <string>

//---------------------------------------------------------------------------
// UtRunEnvManager provides access to path data (home path, run path, etc.)
// Several of the support libraries (rts/geodata/models) rely on this class
// being setup, so make sure to call Setup(argv[0]) at the top of main().
//---------------------------------------------------------------------------

namespace UtRunEnvManager
{
//----------------------------------------------------------------------------------
// This method determines the version and runpath by the arguments, username and
// homepath by Environment variables;
//----------------------------------------------------------------------------------
UT_EXPORT void Setup(const char* aArgvZero, const char* aCME_REL_BASE = nullptr);

// Accessors

//--------------------------------
// This method returns the homepath
//--------------------------------
UT_EXPORT const std::string& GetHomePath();

//------------------------------------
// This method returns the running-path
//------------------------------------
UT_EXPORT const std::string& GetRunPath();

//--------------------------------------------------------------------
// This method returns the program version number from the running-path
//--------------------------------------------------------------------
UT_EXPORT const std::string& GetVersionNumber();

//---------------------------------------
// This method returns the user's username
//---------------------------------------
UT_EXPORT const std::string& GetUsername();

//---------------------------------------------------
// Returns the location of the CME base directory.
// If no environment variables are set the path is
//  determined based on the run directory.
//---------------------------------------------------
UT_EXPORT const std::string& GetCmeRelBase();

//---------------------------------------------
// Returns the CME version suffix (ex .v3.53).
//---------------------------------------------
UT_EXPORT const std::string& GetCmeRelSuffix();

//---------------------------------------------
// Returns the provided folder name appended to
// the CME base directory and appends the version.
//---------------------------------------------
UT_EXPORT std::string GetCmeFolder(const std::string& aFolderName);

//---------------------------------------------
// Returns '/' for linux and '\' for windows.
//---------------------------------------------
UT_EXPORT const std::string& GetPathSeparator();

}; // namespace UtRunEnvManager

#endif //_UTRUNENVMANAGER_HPP_
