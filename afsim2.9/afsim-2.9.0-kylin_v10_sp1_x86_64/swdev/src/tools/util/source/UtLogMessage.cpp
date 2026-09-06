// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtLogMessage.hpp"

#include <algorithm>
#include <cassert>

#include "UtLog.hpp"

std::string ut::log::Message::Fatal()
{
   return "fatal";
}
std::string ut::log::Message::Error()
{
   return "error";
}
std::string ut::log::Message::Warning()
{
   return "warning";
}
std::string ut::log::Message::Debug()
{
   return "debug";
}
std::string ut::log::Message::Info()
{
   return "info";
}
std::string ut::log::Message::Developer()
{
   return "developer";
}

std::string ut::log::Message::Format()
{
   return "format";
}
std::string ut::log::Message::Raw()
{
   return "raw";
}

ut::log::TypeList ut::log::Message::PresetErrors()
{
   return {Message::Fatal(), Message::Error()};
}
ut::log::TypeList ut::log::Message::PresetWarnings()
{
   return {Message::Fatal(), Message::Error(), Message::Warning()};
}
ut::log::TypeList ut::log::Message::PresetRelease()
{
   return {Message::Fatal(), Message::Error(), Message::Warning(), Message::Info()};
}
ut::log::TypeList ut::log::Message::PresetDebug()
{
   return {Message::Fatal(), Message::Error(), Message::Warning(), Message::Info(), Message::Debug()};
}
ut::log::TypeList ut::log::Message::PresetDeveloper()
{
   return {Message::Fatal(), Message::Error(), Message::Warning(), Message::Info(), Message::Debug(), Message::Developer()};
}

struct CStringEquals final
{
   const char* const mValue;

   bool operator()(const std::string& aValue) const noexcept { return mValue == aValue; }
};

ut::log::TypeList::TypeList(const char* aString) noexcept
{
   mData.emplace_back(aString);
}

ut::log::TypeList::TypeList(std::string aString) noexcept
{
   mData.emplace_back(std::move(aString));
}

ut::log::TypeList::TypeList(std::initializer_list<TypeList> aList) noexcept
{
   for (const auto& item : aList)
   {
      Insert(item);
   }
}

bool ut::log::TypeList::Contains(const char* aString) const noexcept
{
   return std::any_of(mData.begin(), mData.end(), CStringEquals{aString});
}

bool ut::log::TypeList::Contains(const std::string& aString) const noexcept
{
   return std::find(mData.begin(), mData.end(), aString) != mData.end();
}

bool ut::log::TypeList::Overlaps(const TypeList& aList) const noexcept
{
   for (const auto& type : aList)
   {
      if (Contains(type))
      {
         return true;
      }
   }
   return false;
}

bool ut::log::TypeList::Insert(const char* aString) noexcept
{
   if (!Contains(aString))
   {
      mData.emplace_back(aString);
      return true;
   }
   return false;
}

bool ut::log::TypeList::Insert(const std::string& aString) noexcept
{
   if (!Contains(aString))
   {
      mData.emplace_back(aString);
      return true;
   }
   return false;
}

bool ut::log::TypeList::Insert(std::string&& aString) noexcept
{
   if (!Contains(aString))
   {
      mData.emplace_back(std::move(aString));
      return true;
   }
   return false;
}

size_t ut::log::TypeList::Insert(const TypeList& aList) noexcept
{
   if (mData.empty())
   {
      mData = aList.mData;
      return mData.size();
   }
   else
   {
      size_t retval = 0;

      for (const auto& item : aList)
      {
         retval += Insert(item);
      }

      return retval;
   }
}

size_t ut::log::TypeList::Insert(TypeList&& aList) noexcept
{
   if (mData.empty())
   {
      mData = std::move(aList.mData);
      return mData.size();
   }
   else
   {
      size_t retval = 0;

      for (auto& item : aList)
      {
         retval += Insert(std::move(item));
      }

      return retval;
   }
}

bool ut::log::TypeList::Remove(const char* aString) noexcept
{
   auto it = std::find_if(mData.begin(), mData.end(), CStringEquals{aString});

   if (it != mData.end())
   {
      std::swap(*it, mData.back());
      mData.pop_back();
   }

   return false;
}

bool ut::log::TypeList::Remove(const std::string& aString) noexcept
{
   auto it = std::find(mData.begin(), mData.end(), aString);

   if (it != mData.end())
   {
      std::swap(*it, mData.back());
      mData.pop_back();
      return true;
   }

   return false;
}

size_t ut::log::TypeList::Remove(const TypeList& aList) noexcept
{
   size_t retval = 0;

   for (const auto& item : aList)
   {
      retval += Remove(item);
   }

   return retval;
}

ut::log::TypeList::iterator ut::log::TypeList::begin() const noexcept
{
   return mData.begin();
}

ut::log::TypeList::iterator ut::log::TypeList::end() const noexcept
{
   return mData.end();
}

size_t ut::log::TypeList::size() const noexcept
{
   return mData.size();
}

bool ut::log::TypeList::empty() const noexcept
{
   return mData.empty();
}

void ut::log::TypeList::clear() noexcept
{
   mData.clear();
}
