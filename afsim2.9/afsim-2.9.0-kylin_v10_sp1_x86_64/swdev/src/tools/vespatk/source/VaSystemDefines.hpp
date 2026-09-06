// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VaSystemDefines_hpp
#define VaSystemDefines_hpp

//! System-specific preprocessor definitions.
//!
//! This file provides system-specific definitions that cannot be provided on
//! the compiler command line.  It *MUST* be included from any header file that
//! uses the STL (Standard Template Library)

#ifdef _WIN32
// Prevent compiler warnings about debugging information getting truncated.
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#endif
