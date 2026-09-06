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
#ifndef L16_MESSAGEACCESSOR_HPP
#define L16_MESSAGEACCESSOR_HPP

#include "wsf_l16_export.h"

#include <map>
#include <string>
#include <typeindex>
#include <vector>

#include "Messages.hpp"

//!
//! The Fields of the J-Messages are made without virtual functions so they
//! only take up the minimal number of bytes.
//! The classes here provide a way to access fields generically.  This requires
//! a 'hack' where we store the offset of a field from the beginning of the message
//! so we can access that field later.
//
namespace WsfL16
{
namespace Messages
{
class FieldAccessor;

// Contains information about a field type
class WSF_L16_EXPORT FieldAccessorType
{
public:
   enum AccessorType
   {
      cBOOLEAN,
      cINTEGER,
      cSTRING,
      cDOUBLE,
      cENUMERATION
   };
   friend class FieldAccessor;

   FieldAccessorType();
   static FieldAccessorType* Find(const std::type_index& aType);
   void                      FormatEnumNames();
   struct Variant
   {
      bool        mBoolean;
      int         mInteger;
      std::string mString;
      double      mDouble;
   };

   int                        mId;
   int                        mFieldSizeBits;
   std::map<int, std::string> mEnumMap;
   AccessorType               mAccessor;
   bool                       mUsingRawData;
   int                        mNoStatementValue;
   bool                       mIsSpare;
   bool                       mIsDisused;
   bool                       mHasUnits;
   std::string                mUnitsName;
   void (*mFieldSetter)(void* aFieldPtr, const Variant& aValue);
   void (*mFieldGetter)(void* aFieldPtr, Variant& aValue);

   static std::map<std::type_index, FieldAccessorType> sFieldAccessorTypes;
   static void                                         ResetState() { sFieldAccessorTypes.clear(); }
};

// Contains information about a message type
class MessageAccessorType
{
public:
   struct FieldData
   {
      int                mByteOffset;
      std::string        mName;
      FieldAccessorType* mAccessorType;
   };
   std::vector<FieldData>   mFields;
   Messages::Base::WordType mWordType;
   int                      mLabel;
   int                      mSubLabel;
   int                      mWordNumber;
   std::string              mMessageName;

   FieldAccessor GetField(Messages::Base* aMessage, int aFieldIndex);

   friend class MessageAccessor;
   static MessageAccessorType*               Find(Messages::Base* aMessage);
   static std::map<int, MessageAccessorType> sMessageAccessorTypes;
   static void                               ResetState() { sMessageAccessorTypes.clear(); }
};


// Provides access to a field
class WSF_L16_EXPORT FieldAccessor
{
public:
   FieldAccessor(void* aFieldPtr, FieldAccessorType* aAccessorType, const std::string* aNamePtr);

   enum DataType
   {
      cBOOLEAN,
      cINTEGER,
      cSTRING,
      cDOUBLE,
      cENUMERATION
   };
   DataType GetType() const;
   int      GetId() const { return mAccessorType->mId; }
   // Set and Get methods:
   // You must ensure you are calling the correct method
   // Check the value returned from GetType()
   void Set(bool aValue);
   void Set(int aValue);
   void Set(const std::string& aValue);
   void Set(double aValue);

   bool        GetBool() const;
   int         GetInt() const;
   std::string GetString() const;
   double      GetDouble() const;
   bool        GetEnumeration(std::string& aEnum) const;
   bool        GetUnits(std::string& aUnitsType) const;
   bool        IsNoStatement() const;
   bool        IsSpare() const;
   bool        IsDisused() const;

   void SetRawData(int aValue);
   int  GetRawData() const;
   void SetNoStatement();

   const std::string& GetName() const { return *mNamePtr; }

private:
   void*              mFieldPtr;
   FieldAccessorType* mAccessorType;
   const std::string* mNamePtr;
};

// Provides access to a message
class WSF_L16_EXPORT MessageAccessor
{
public:
   MessageAccessor(const Messages::Base* aMessage);

   FieldAccessor GetField(int aIndex);

   int FieldCount();

   const Messages::Base* mMessagePtr;
   int                   mCurrentField;
   MessageAccessorType*  mAccessorType;
};
} // namespace Messages
} // namespace WsfL16

#endif
