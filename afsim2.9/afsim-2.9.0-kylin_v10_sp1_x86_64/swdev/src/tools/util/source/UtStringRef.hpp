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
#ifndef UTSTRINGREF_HPP
#define UTSTRINGREF_HPP
#include "ut_export.h"

#include <ostream>
#include <string>

//! Safely allows references to a const std::string without copying.
//! Uses a reference count to delete the string when no references remain.
//! Can usually be a drop-in replacement for std::string in STL containers.
class UT_EXPORT UtStringRef
{
public:
   using size_type = std::string::size_type;
   static std::string emptyStr;

   //! Construct a new string reference from another UtStringRef.
   UtStringRef(const UtStringRef& aSrc)
   {
      mRefCt = aSrc.mRefCt;
      if (mRefCt != nullptr)
         ++(*mRefCt);
      mStringPtr = aSrc.mStringPtr;
   }

   //! Construct a reference to a string given a string pointer.
   //! This class takes ownership of the string.
   UtStringRef(const std::string* aSrcPtr)
   {
      mRefCt     = new int(1);
      mStringPtr = aSrcPtr;
   }
   //! Construct a new string reference from a copy of a string.
   UtStringRef(const std::string& aStr = emptyStr)
   {
      if (&aStr == &emptyStr)
      {
         mStringPtr = &aStr;
         mRefCt     = nullptr;
      }
      else
      {
         mStringPtr = new std::string(aStr);
         mRefCt     = new int(1);
      }
   }
   //! Construct a new string reference from a C string
   explicit UtStringRef(const char* aStrPtr)
   {
      mStringPtr = new std::string(aStrPtr);
      mRefCt     = new int(1);
   }
   //! Construct a new string reference from a C string
   explicit UtStringRef(char aChar)
   {
      mStringPtr = new std::string(1, aChar);
      mRefCt     = new int(1);
   }

   ~UtStringRef() { Release(); }

   UtStringRef& operator=(const UtStringRef& aRhs)
   {
      if (&aRhs != this)
      {
         Release();
         mRefCt = aRhs.mRefCt;
         if (mRefCt != nullptr)
            ++(*mRefCt);
         mStringPtr = aRhs.mStringPtr;
      }
      return *this;
   }

   UtStringRef& operator=(const std::string& aRhs)
   {
      Release();
      mStringPtr = new std::string(aRhs);
      mRefCt     = new int(1);
      return *this;
   }

   bool                        operator<(const std::string& aRhs) const { return *mStringPtr < aRhs; }
   bool                        operator>(const std::string& aRhs) const { return *mStringPtr > aRhs; }
   bool                        operator==(const std::string& aRhs) const { return *mStringPtr == aRhs; }
   bool                        operator<(const UtStringRef& aRhs) const { return *mStringPtr < *aRhs.mStringPtr; }
   bool                        operator>(const UtStringRef& aRhs) const { return *mStringPtr > *aRhs.mStringPtr; }
   bool                        operator==(const UtStringRef& aRhs) const { return *mStringPtr == *aRhs.mStringPtr; }
                               operator const std::string&() const { return *mStringPtr; }
   char                        operator[](int offset) const { return (*mStringPtr)[offset]; }
   const std::string&          Get() const { return *mStringPtr; }
   bool                        empty() const { return mStringPtr->empty(); }
   std::string::const_iterator begin() const { return mStringPtr->begin(); }
   std::string::const_iterator end() const { return mStringPtr->end(); }
   size_t                      length() const { return mStringPtr->length(); }
   void                        Swap(UtStringRef& aOther)
   {
      std::swap(mRefCt, aOther.mRefCt);
      std::swap(mStringPtr, aOther.mStringPtr);
   }
   size_t      size() const { return Get().size(); }
   std::string substr(size_type aOffset = 0, size_type aCount = std::string::npos) const
   {
      return Get().substr(aOffset, aCount);
   }

private:
   void Release()
   {
      if (mRefCt != nullptr)
      {
         if (--(*mRefCt) == 0)
         {
            delete mRefCt;
            delete mStringPtr;
         }
         mRefCt     = nullptr;
         mStringPtr = &emptyStr;
      }
   }
   mutable int*       mRefCt;
   const std::string* mStringPtr;
};

inline bool operator==(const std::string& aLhs, const UtStringRef& aRhs)
{
   return aLhs == aRhs.Get();
}
inline bool operator<(const std::string& aLhs, const UtStringRef& aRhs)
{
   return aLhs < aRhs.Get();
}

inline std::ostream& operator<<(std::ostream& os, const UtStringRef& aString)
{
   os << aString.Get();
   return os;
}

namespace std
{
template<>
inline void swap(UtStringRef& a, UtStringRef& b)
{
   a.Swap(b);
}
} // namespace std

#endif
