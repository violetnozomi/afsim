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

#ifndef WSFXIO_PUBLISHKEY_HPP
#define WSFXIO_PUBLISHKEY_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;

//! Defines the unique identifier for a publication.
//! Contains up to 4 fields. Each field can be either a string or an integer.
//! See WsfXIO_Publisher.
class WSF_EXPORT WsfXIO_PublishKey
{
public:
   static const int cMAX_FIELDS = 4;
   class WSF_EXPORT Field
   {
   public:
      friend class WsfXIO_PublishKey;
      Field(const char* aStr)
         : mValue(aStr)
      {
      }
      Field() = default;
      Field(const std::string& aString)
         : mValue(aString)
      {
      }
      Field(int aValue);
      Field(size_t aValue);
      bool   operator==(const Field& aField) const { return mValue == aField.mValue; }
      bool   operator!=(const Field& aField) const { return mValue != aField.mValue; }
      Field& operator=(int aValue);
      Field& operator=(const std::string& aValue)
      {
         mValue = aValue;
         return *this;
      }

      operator std::string() const { return mValue; }

      const std::string& GetString() const { return mValue; }

      operator const char*() const { return mValue.c_str(); }

      operator int() const;

      void SetAny() { mValue = '*'; }

      bool IsAny() const { return mValue == "*"; }

      bool operator<(const Field& aField) const { return mValue < aField.mValue; }
      bool operator>(const Field& aField) const { return mValue > aField.mValue; }

   private:
      std::string mValue;
   };

   WsfXIO_PublishKey(const Field& a1 = Field(), const Field& a2 = Field(), const Field& a3 = Field(), const Field& a4 = Field())
   {
      mFields[0] = a1;
      mFields[1] = a2;
      mFields[2] = a3;
      mFields[3] = a4;
   }

   bool operator<(const WsfXIO_PublishKey& aKey) const
   {
      if (mFields[0] < aKey[0])
      {
         return true;
      }
      if (mFields[0] > aKey[0])
      {
         return false;
      }
      if (mFields[1] < aKey[1])
      {
         return true;
      }
      if (mFields[1] > aKey[1])
      {
         return false;
      }
      if (mFields[2] < aKey[2])
      {
         return true;
      }
      if (mFields[2] > aKey[2])
      {
         return false;
      }
      if (mFields[3] < aKey[3])
      {
         return true;
      }
      return false;
   }
   Field&       operator[](int aIndex) { return mFields[aIndex]; }
   const Field& operator[](int aIndex) const { return mFields[aIndex]; }

   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mFields[0].mValue& mFields[1].mValue& mFields[2].mValue& mFields[3].mValue;
   }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   bool HasWildcards() const
   {
      return mFields[0].IsAny() || mFields[1].IsAny() || mFields[2].IsAny() || mFields[3].IsAny();
   }

   std::string ToString() const
   {
      return mFields[0].GetString() + '.' + mFields[1].GetString() + '.' + mFields[2].GetString() + '.' +
             mFields[3].GetString();
   }

   static UtScriptClass* GetScriptClass() { return sScriptClassPtr; }

   bool Match(const WsfXIO_PublishKey& aKey) const
   {
      for (int i = 0; i < cMAX_FIELDS; ++i)
      {
         std::string field = mFields[i].GetString();
         std::string key   = aKey[i].GetString();
         if (field != "*" && field != key)
         {
            return false;
         }
      }
      return true;
   }

protected:
   static UtScriptClass* sScriptClassPtr;
   Field                 mFields[cMAX_FIELDS];
};

//! Defines criteria for selecting a WsfXIO_PublishKey.  Each of the 4 fields must match
//! the key, but "*" may be used as a wildcard to match multiple keys.
class WSF_EXPORT WsfXIO_PublishFilter : public WsfXIO_PublishKey
{
public:
   WsfXIO_PublishFilter(const WsfXIO_PublishKey& aKey)
      : WsfXIO_PublishKey(aKey)
   {
   }
   WsfXIO_PublishFilter(const Field& a1 = Field(),
                        const Field& a2 = Field(),
                        const Field& a3 = Field(),
                        const Field& a4 = Field())
      : WsfXIO_PublishKey(a1, a2, a3, a4)
   {
   }

   //! Returns 'true' if the filter matches aKey
   bool operator==(const WsfXIO_PublishKey& aKey) const { return Match(aKey); }
};

#endif
