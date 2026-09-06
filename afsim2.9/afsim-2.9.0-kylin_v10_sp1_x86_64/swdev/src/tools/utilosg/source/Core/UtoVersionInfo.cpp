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

/////////////////////////////////////////////////////////////////////////////
// UtoVersionInfo.cpp : implementation file

#include "UtoVersionInfo.hpp"

#include <sstream>

#include "UtoVersion.hpp"

// Define an RCS keyword for the program ident to look for.
//{secret}
static char const rcsidVersionInfoConstant[] = "$Id: UtoVersionInfo.cpp,v 1.0 2013/12/20 18:43:01 b1078230 Exp $";

/**
\ingroup diag
\class UtoVersionInfo
\brief Accesses version information about UTO.

The version info is used to access UTO version information.  The Comment
method can be used to retrieve special information about the version.  The
Compare method can be used to verify a particular version of UTO.  The
parts of the version information can be accessed using the Major, Minor,
Update, and Build methods.
*/
class UtoVersionInfo;

const UtoVersionInfo::VerId UtoVersionInfo::any_id = (short)-1;

/** @name Constructor & Destructor */
//@{

/**
\brief Creates an instance of the object.

Creates an instance of the object.
*/
UtoVersionInfo::UtoVersionInfo() {}

/**
\brief Destroys the object.

This destroys all the subordinate objects for this object.
*/
UtoVersionInfo::~UtoVersionInfo() {}
//@}

/** @name Type Definitions */
//@{
/**
\typedef unsigned short UtoVersionInfo::VerId
\brief Version ID
*/
//@}

/** @name Data Members */
//@{
/**
\var static const UtoVersionInfo::VerId UtoVersionInfo::any_id;
\brief Represents a wildcard.

Represents a wildcard.
*/
//@}

/** @name Accessors */
//@{

/**
\brief Returns the major version value.

Returns the major version value.
\return UtoVersionInfo::VerId - the major version value.
*/
UtoVersionInfo::VerId UtoVersionInfo::Major() const
{
   return UTO_MAJOR_VERSION;
}

/**
\brief Returns the minor version value.

Returns the minor version value.
\return UtoVersionInfo::VerId - the minor version value.
*/
UtoVersionInfo::VerId UtoVersionInfo::Minor() const
{
   return UTO_MINOR_VERSION;
}

/**
\brief Returns the update version value.

Returns the update version value.
\return UtoVersionInfo::VerId - the update version value.
*/
UtoVersionInfo::VerId UtoVersionInfo::Update() const
{
   return UTO_PATCH_VERSION;
}

/**
\brief Returns the build version value.

Returns the build version value.
\return UtoVersionInfo::VerId - the build version value.
*/
UtoVersionInfo::VerId UtoVersionInfo::Build() const
{
   return UTO_BUILD_VERSION;
}

/**
\brief Returns the version string.

Returns the version string. The string is in the form: "{major}.{minor}.{update}.{build}"
\return std::string - the version string.
*/
std::string UtoVersionInfo::Version() const
{
   std::ostringstream sstr;
   sstr << UTO_MAJOR_VERSION << "." << UTO_MINOR_VERSION << "." << UTO_PATCH_VERSION << "." << UTO_BUILD_VERSION;
   std::string version = sstr.str();
   // rcsidVersionInfoConstant needs to be used or the compiler might
   // not include the string and this is about as good of way as any.
   if (version.empty())
      return rcsidVersionInfoConstant;
   return version;
}

/**
\brief Returns the comment string.

Returns the comment string.
std::string - the comment string.
*/
std::string UtoVersionInfo::Comment() const
{
   return UTO_COMMENT_VERSION;
}
//@}

/** @name Operations */
//@{

/**
\brief Compares version paremeters to the current UTO version.

Compares version paremeters to the current UTO version.  Using any_id for
a version parameter enables a wildcard.
\param major the major value to compare.
\param minor the minor value to compare.
\param update the update value to compare.
\param build the build value to compare.
\return int - <table>
                  <tr><td>0</td><td>Version parameters compare equal to the current version.</td></tr>
                  <tr><td>1</td><td>Version parameters compare less thanthe current version.</td></tr>
                  <tr><td>-1</td><td>Version parameters compare greater than the current version.</td></tr>
              </table>
*/
int UtoVersionInfo::Compare(VerId major, VerId minor, VerId update, VerId build)
{
   if (major == any_id)
   {
      return 0;
   }
   else if (major != UTO_MAJOR_VERSION)
   {
      return (major < UTO_MAJOR_VERSION) ? 1 : -1;
   }
   else if (minor == any_id)
   {
      return 0;
   }
   else if (minor != UTO_MINOR_VERSION)
   {
      return (minor < UTO_MINOR_VERSION) ? 1 : -1;
   }
   else if (update == any_id)
   {
      return 0;
   }
   else if (update != UTO_PATCH_VERSION)
   {
      return (update < UTO_PATCH_VERSION) ? 1 : -1;
   }
   else if (build == any_id)
   {
      return 0;
   }
   else if (build != UTO_BUILD_VERSION)
   {
      return (build < UTO_BUILD_VERSION) ? 1 : -1;
   }
   return 0;
}
//@}
