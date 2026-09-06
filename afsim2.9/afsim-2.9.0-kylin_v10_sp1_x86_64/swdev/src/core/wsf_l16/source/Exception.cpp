// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "Exception.hpp"

#include <cstdio>

namespace WsfL16
{
namespace Messages
{

ReadException::ReadException(std::string aError)
{
   mError = "Error reading JMessage: " + aError;
}

ReadException::ReadException()
{
   mError = "Error reading JMessage";
}

InterpretException::InterpretException()
{
   mError = "Error Interpreting Message";
}

InterpretException::InterpretException(std::string aError)
{
   mError = "Error Interpreting Message: " + aError;
}

} // namespace Messages
} // namespace WsfL16
