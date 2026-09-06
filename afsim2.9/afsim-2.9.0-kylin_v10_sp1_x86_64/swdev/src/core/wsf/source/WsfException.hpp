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

#ifndef WSFEXCEPTION_HPP
#define WSFEXCEPTION_HPP

#include "wsf_export.h"

#include "UtException.hpp"

//! WsfException is a base class for exceptions used by the WSF
//! core framework.
class WSF_EXPORT WsfException : public UtException
{
public:
   WsfException(const std::string& aWhat) noexcept
      : UtException(aWhat)
   {
   }

   ~WsfException() override {}
};

//! An exception that gets thrown when an attempt is made to Clone() an unclonable object.
class WSF_EXPORT WsfUncloneableException : public WsfException
{
public:
   WsfUncloneableException()
      : WsfException("WsfUnclonableException")
   {
   }
   WsfUncloneableException(const std::string& aWhat)
      : WsfException(aWhat)
   {
   }
};

#endif
