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

#ifndef WsfParseSourceProvider_HPP
#define WsfParseSourceProvider_HPP
#include "wsf_parser_export.h"

#include "UtPath.hpp"
class UtTextDocument;

//! Interface providing source files to the WsfParser
class WSF_PARSER_EXPORT WsfParseSourceProvider
{
public:
   virtual ~WsfParseSourceProvider() {}
   //! Derived classes should override this method to return
   //! a source if it exists at aPath
   virtual UtTextDocument* FindSource(const UtPath& aPath, bool aReadAccess) = 0;

   virtual UtTextDocument* CreateSource(const UtPath& aPath) { return nullptr; }
};
#endif
