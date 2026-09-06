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

#ifndef DISENUMCONVERSION_HPP
#define DISENUMCONVERSION_HPP

#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

//! \headerfile ""
/*!
 *  This file sets up a template Converter class for use to handle
 *  quick conversions between enumerated values and strings
 *
 *  \note
 *  The string to enum conversion ignores punctuation and whitespace, so:
 *
 *  \code{.cpp}
 *  ToEnum("Monday");
 *  ToEnum("monday!");
 *  ToEnum("( Monday ? )");
 *  \endcode
 *
 *  Will all return the same enumeration.
 *
 *  Usage Example:
 *  In your header file:
 *
 *  \code{.cpp}
 *  namespace Day
 *  {
 *     enum Enum
 *     {
 *        Monday,
 *        Tuesday,
 *        Wednesday,
 *        Thursday,
 *        Friday,
 *        Saturday,
 *        Sunday
 *     };
 *     template<typename IntType> const bool IsValid(IntType day);
 *     const std::string& ToString(Enum day);
 *     const Enum ToEnum(const std::string& day);
 *
 *  } // Day
 *  \endcode
 *
 *  (C++98 Initialization)
 *
 *  \code{.cpp}
 *  #include "DisEnumManager.hpp"
 *
 *  namespace  // localized implementaion namespace
 *  {
 *     std::pair<Day::Enum, std::string> data[] = {
 *        std::make_pair(Day::Monday,    "Monday"),
 *        std::make_pair(Day::Tuesday,   "Tuesday"),
 *        std::make_pair(Day::Wednesday, "Wednesday"),
 *        std::make_pair(Day::Thursday,  "Thursday"),
 *        std::make_pair(Day::Friday,    "Friday"),
 *        std::make_pair(Day::Saturday,  "Saturday"),
 *        std::make_pair(Day::Sunday,    "Sunday"),
 *     };
 *
 *     DisEnum::Conversion<Day::Enum> conv(DisEnum::MakeVector(data));
 *  }
 *
 *  \endcode
 *
 *  (C++11 Initialization)
 *
 *  \code{.cpp}
 *  #include "DisEnumManager.hpp"
 *
 *  namespace  // localized implementaion namespace
 *  {
 *     const std::vector<std::pair<Day::Enum, std::string> > data = {
 *        {Monday,    "Monday"},
 *        {Tuesday,   "Tuesday"},
 *        {Wednesday, "Wednesday"},
 *        {Thursday,  "Thursday"},
 *        {Friday,    "Friday"},
 *        {Saturday,  "Saturday"},
 *        {Sunday,    "Sunday}
 *     };
 *     const DisEnum::Conversion<Day::Enum> conv(data);
 *  } // end localized implementation namespace
 *  \endcode
 *
 *  (Function Definitions - hides template mess from user)
 *
 *  \code{.cpp}
 *
 *  namespace Day
 *  {
 *     template<typename IntType> const bool IsValid(IntType day)
 *     {
 *        // This is quicker than a map lookup, so user can optimize the lookups
 *        return day >= Monday && day <= Sunday;
 *     }
 *     const std::string& ToString(Enum day)
 *     {
 *        return conv.ToString(day);
 *     }
 *     const Enum ToEnum(const std::string& day)
 *     {
 *        return conv.ToEnum(day);
 *     }
 *  }
 *  \endcode
 *
 *  Example usage:
 *  \code{.cpp}
 *  {
 *     Day::Enum de = Day::Wednesday;
 *     std::string ds = Day::ToString(de);
 *
 *     // Outputs "Wednesday = 2"
 *     std::cout << ds << " = " << de << std::endl;
 *
 *     ds = "Fri-Day !!!";
 *     de = Day::ToEnum(ds);
 *
 *     // Outputs "Fri-Day !!! = 4"
 *     std::cout << ds << " = " << de << std::endl;
 *  }
 *  \endcode
 */
namespace DisEnum
{
//! Helper function for setting up vector data
template<typename T, size_t N>
std::vector<T> MakeVector(const T (&data)[N])
{
   return std::vector<T>(data, data + N);
}

//! Removes punctuation, whitespace and special characters from a string,
//! then converts to lowercase.
std::string SimplifyString(const std::string& str);

//! User created class to set up an enumeration mapping
template<typename E>
class Conversion
{
public:
   typedef std::pair<E, std::string> PAIR;
   typedef std::vector<PAIR>         VECTOR;

   //! Initialize Converision utility off of input vector
   Conversion(const VECTOR& v);

   //! Returns true if the given enumeration is valid
   template<typename IntType>
   const bool IsValid(const IntType e) const;
   //! Returns a string based on the given enumerated value
   const std::string& ToString(const E e) const;
   //! Returns an enumerated value based on the supplied string
   const E ToEnum(const std::string& str) const;

private:
   typedef std::map<E, std::string> ToStringMap;
   typedef std::map<std::string, E> ToEnumMap;

   const ToStringMap mStringMap; //!< Converts Enums to Strings
   const ToEnumMap   mEnumMap;   //!< Converts Strings to Enums

   //! Initializer method for enum->string map
   static const ToStringMap InitializeToString(const VECTOR& v);
   //! Initializer method for string->enum map
   static const ToEnumMap InitializeToEnum(const VECTOR& v);
};

template<typename E>
Conversion<E>::Conversion(const VECTOR& v)
   : mStringMap(InitializeToString(v))
   , mEnumMap(InitializeToEnum(v))
{
}

template<typename E>
template<typename IntType>
const bool Conversion<E>::IsValid(const IntType e) const
{
   typename ToStringMap::const_iterator it = mStringMap.find(E(e));
   return (it != mStringMap.end());
}

template<typename E>
const std::string& Conversion<E>::ToString(const E e) const
{
   static const std::string             INVALID_STR = "-";
   typename ToStringMap::const_iterator it          = mStringMap.find(e);
   return (it != mStringMap.end()) ? it->second : INVALID_STR;
}

template<typename E>
const E Conversion<E>::ToEnum(const std::string& str) const
{
   // For most enums max value of an int should be bad...
   static const E INVALID_ENUM = static_cast<E>(std::numeric_limits<int>::max());

   std::string                        lookup = SimplifyString(str);
   typename ToEnumMap::const_iterator it     = mEnumMap.find(lookup);
   return (it != mEnumMap.end()) ? it->second : INVALID_ENUM;
}

template<typename E>
const typename Conversion<E>::ToStringMap Conversion<E>::InitializeToString(const typename Conversion<E>::VECTOR& v)
{
   return ToStringMap(v.begin(), v.end());
}

//! Helper class to create a reverse lookup map
/*!
 *  To be used as an operator for a for_each loop
 */
template<typename E>
class InverseMap
{
public:
   // Simplifies the string and maps to the enumeration
   void                     operator()(const std::pair<E, std::string>& p) { mMap[SimplifyString(p.second)] = p.first; }
   std::map<std::string, E> mMap;
};

template<typename E>
const typename Conversion<E>::ToEnumMap Conversion<E>::InitializeToEnum(const typename Conversion<E>::VECTOR& v)
{
   InverseMap<E> m = std::for_each(v.begin(), v.end(), InverseMap<E>());
   return m.mMap;
}
} // namespace DisEnum

#endif
