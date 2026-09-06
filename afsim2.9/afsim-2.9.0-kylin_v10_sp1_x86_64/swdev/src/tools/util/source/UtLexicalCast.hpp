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

#ifndef UTLEXICALCAST_HPP
#define UTLEXICALCAST_HPP

#include <cctype>
#include <exception>
#include <sstream>
#include <string>
#include <typeinfo>

namespace ut
{
namespace conversion
{
template<class T, class U>
bool try_lexical_convert(const U& arg, T& result)
{
   std::stringstream stream;
   stream << arg;
   if (stream >> result)
   {
      // make sure there is nothing but white space following the value just read
      int nextChar;
      while ((nextChar = stream.get()) != std::istream::traits_type::eof())
      {
         if (!std::isspace(nextChar))
         {
            return false;
         }
      }
      return true;
   }
   return false;
}
} // namespace conversion

class bad_lexical_cast : public std::bad_cast
{
public:
   bad_lexical_cast()
      : bad_lexical_cast(typeid(void), typeid(void))
   {
   }
   bad_lexical_cast(const std::type_info& aSourceType, const std::type_info& aTargetType)
      : mSourceType(aSourceType)
      , mTargetType(aTargetType)
   {
   }

   const char* what() const noexcept override
   {
      return "bad lexical cast: unable to convert source type to target type";
   }

   const std::type_info& source_type() const noexcept { return mSourceType; }

   const std::type_info& target_type() const noexcept { return mTargetType; }

   ~bad_lexical_cast() override = default;

private:
   const std::type_info& mSourceType;
   const std::type_info& mTargetType;
};

template<class T, class U>
T lexical_cast(const U& arg)
{
   T result;
   if (!conversion::try_lexical_convert(arg, result))
   {
      throw bad_lexical_cast(typeid(U), typeid(T));
   }
   return result;
}
} // namespace ut

#endif
