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

#ifndef UTDICTIONARY_HPP
#define UTDICTIONARY_HPP

#include "ut_export.h"

#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class UtDictionaryMemoryPool;

//!   Provide a mechanism to equate a string with a unique integer.
//!
//!   The ability to equate a string with a unique integer is useful because it
//!   allows comparisons of strings to be replaced by comparisons of integers
//!   (the later of course being much faster).
//!
//!   It also allows a std::string variable to be replaced with an integer,
//!   thus saving space and reducing the time needed for assignment or copy
//!   construction.

class UT_EXPORT UtDictionary
{
public:
   // Wrap a string pointer for use as a key in a map or hash map
   struct StringPtr
   {
      StringPtr()
         : mStringPtr(nullptr)
      {
      }
      StringPtr(const std::string* aStringPtr)
         : mStringPtr(aStringPtr)
      {
      }

      bool               operator<(const StringPtr& aRhs) const { return *mStringPtr < *aRhs.mStringPtr; }
      bool               operator==(const StringPtr& aRhs) const { return *mStringPtr == *aRhs.mStringPtr; }
      const std::string* mStringPtr;
   };
   struct StringHash
   {
      //! Microsoft Developer Studio requires the following definitions.
      //! They cannot be defined with Developer Studio 6, however, because it causes a compile error.
#if defined(_WIN32) && _MSC_VER >= 1300
      static const int bucket_size = 8;
      static const int min_buckets = 4;
#endif

      size_t operator()(const StringPtr aValue) const
      {
         size_t value = 0;
         for (unsigned int i = 0; i < aValue.mStringPtr->size(); ++i)
         {
            value += value * 5 + (*aValue.mStringPtr)[i];
         }
         return value;
      }
      bool operator()(const StringPtr aLhs, const StringPtr aRhs) const { return *aLhs.mStringPtr < *aRhs.mStringPtr; }
   };

   explicit UtDictionary(std::size_t aInitialPoolSize = 500);
   virtual ~UtDictionary();

   int  GetNumber(const std::string& aString);
   bool Exists(const std::string& aString) const;

   const std::string& GetString(int aNumber) const;

   int                  GetCount() const;
   static void          SetDictionary(UtDictionary* aDictionaryPtr);
   static bool          HasGlobalDictionary() { return mInstancePtr != nullptr; }
   static UtDictionary& GetDictionary()
   {
      // if UT_DICTIONARY_NO_INSTANCE is defined,
      // don't create the dictionary automatically
#ifdef UT_DICTIONARY_NO_INSTANCE
      return *mInstancePtr;
#else
      if (mInstancePtr != nullptr)
      {
         return *mInstancePtr;
      }
      return GetDictionaryP();
#endif
   }


   static void Destroy();

private:
   int                  InsertNew(const std::string& aString);
   static UtDictionary& GetDictionaryP();

   static std::unique_ptr<UtDictionary> mInstancePtr;

   // Maps string to string integer ID
   using MapType = std::unordered_map<const StringPtr, int, StringHash>;
   MapType mStringToNumber;

   // Array of strings by string ID number
   std::vector<const std::string*> mNumberToString;

   // using a spinlock instead of a mutex.
   mutable std::atomic<int32_t> mReadWriteSpinner;

   // New strings are created here
   std::unique_ptr<UtDictionaryMemoryPool> mPoolPtr;
};

#endif
