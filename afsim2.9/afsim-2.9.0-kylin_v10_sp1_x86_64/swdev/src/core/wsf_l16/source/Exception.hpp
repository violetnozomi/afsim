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
#ifndef L16_EXCEPTION_HPP
#define L16_EXCEPTION_HPP

#include <exception>
#include <string>

namespace WsfL16
{
namespace Messages
{
class Exception : public std::exception
{
public:
   const std::string& GetError() { return mError; }
   ~Exception() override {}

protected:
   std::string mError;
};

class ReadException : public Exception
{
public:
   ReadException(std::string aError);
   ReadException();
};
class InterpretException : public Exception
{
public:
   InterpretException();
   InterpretException(std::string aError);
};
} // namespace Messages
} // namespace WsfL16

#endif
