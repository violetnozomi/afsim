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
#ifndef L16_FIELDACCESSORTYPES_HPP
#define L16_FIELDACCESSORTYPES_HPP

#include <typeinfo>

#include "FieldTypes.hpp"
#include "MessageAccessor.hpp"

namespace WsfL16
{
namespace Messages
{
namespace Accessors
{
template<typename DATA_TYPE>
struct VariantTypeSelect
{
   typedef int                                  variant_type;
   static const FieldAccessorType::AccessorType cACCESSOR_TYPE = FieldAccessorType::cINTEGER;
   static int&                                  Select(FieldAccessorType::Variant& aVal) { return aVal.mInteger; }
   static int                                   Select(const FieldAccessorType::Variant& aVal) { return aVal.mInteger; }
};
template<>
struct VariantTypeSelect<std::string>
{
   typedef std::string                          variant_type;
   static const FieldAccessorType::AccessorType cACCESSOR_TYPE = FieldAccessorType::cSTRING;
   static std::string&                          Select(FieldAccessorType::Variant& aVal) { return aVal.mString; }
   static const std::string&                    Select(const FieldAccessorType::Variant& aVal) { return aVal.mString; }
};
template<>
struct VariantTypeSelect<double>
{
   typedef double                               variant_type;
   static const FieldAccessorType::AccessorType cACCESSOR_TYPE = FieldAccessorType::cDOUBLE;
   static double&                               Select(FieldAccessorType::Variant& aVal) { return aVal.mDouble; }
   static double                                Select(const FieldAccessorType::Variant& aVal) { return aVal.mDouble; }
};
template<>
struct VariantTypeSelect<bool>
{
   typedef bool                                 variant_type;
   static const FieldAccessorType::AccessorType cACCESSOR_TYPE = FieldAccessorType::cBOOLEAN;
   static bool&                                 Select(FieldAccessorType::Variant& aVal) { return aVal.mBoolean; }
   static bool                                  Select(const FieldAccessorType::Variant& aVal) { return aVal.mBoolean; }
};

template<typename FIELD, typename DATA_TYPE>
struct FillerAccessors
{
   typedef VariantTypeSelect<DATA_TYPE> VariantSelector;
   static void                          Fill(FieldAccessorType& aAccessor)
   {
      aAccessor.mFieldGetter  = &FillerAccessors<FIELD, DATA_TYPE>::Get;
      aAccessor.mFieldSetter  = &FillerAccessors<FIELD, DATA_TYPE>::Set;
      aAccessor.mUsingRawData = false;
      aAccessor.mAccessor     = VariantSelector::cACCESSOR_TYPE;
   }
   static void Set(void* aField, const FieldAccessorType::Variant& aVal)
   {
      FIELD& field = *((FIELD*)aField);
      field        = (typename FIELD::accessor_type)VariantSelector::Select(aVal);
   }
   static void Get(void* aField, FieldAccessorType::Variant& aVal)
   {
      FIELD& field                  = *((FIELD*)aField);
      VariantSelector::Select(aVal) = (typename VariantSelector::variant_type)field;
   }
};

template<typename FIELD>
struct FillerAccessors<FIELD, Messages::NoAccessorTypeTag>
{
   typedef VariantTypeSelect<int> VariantSelector;
   static void                    Fill(FieldAccessorType& aAccessor)
   {
      aAccessor.mFieldGetter  = &FillerAccessors<FIELD, Messages::NoAccessorTypeTag>::Get;
      aAccessor.mFieldSetter  = &FillerAccessors<FIELD, Messages::NoAccessorTypeTag>::Set;
      aAccessor.mUsingRawData = true;
      aAccessor.mAccessor     = VariantSelector::cACCESSOR_TYPE;
   }
   static void Set(void* aField, const FieldAccessorType::Variant& aVal)
   {
      FIELD& field = *((FIELD*)aField);
      field.SetRawData(aVal.mInteger);
   }
   static void Get(void* aField, FieldAccessorType::Variant& aVal)
   {
      FIELD& field  = *((FIELD*)aField);
      aVal.mInteger = field.GetRawData();
   }
};

template<typename FIELD, typename ENUM_TAG>
struct FillerEnum
{
   static void Fill(FieldAccessorType& /*aAccessor*/) {}
};

template<typename FIELD>
struct FillerEnum<FIELD, EnumStringMapTag>
{
   static void Fill(FieldAccessorType& aAccessor)
   {
      FIELD::CreateEnumMap();
      aAccessor.mEnumMap.insert(FIELD::sEnumMap.begin(), FIELD::sEnumMap.end());
      aAccessor.mAccessor = FieldAccessorType::cENUMERATION;
   }
};

template<typename FIELD, typename UNITS_TAG>
struct FillerUnits
{
   static void Fill(FieldAccessorType& aAccessor)
   {
      aAccessor.mUnitsName = UNITS_TAG::GetName();
      aAccessor.mHasUnits  = true;
   }
};

template<typename FIELD>
struct FillerUnits<FIELD, NoAccessorUnitsTag>
{
   static void Fill(FieldAccessorType& aAccessor) { aAccessor.mHasUnits = false; }
};

template<typename FIELD>
struct FillerSpare
{
   static void Fill(FieldAccessorType& /*aAccessor*/) {}
};

template<unsigned BITS>
struct FillerSpare<SpareField<BITS>>
{
   static void Fill(FieldAccessorType& aAccessor) { aAccessor.mIsSpare = true; }
};

template<unsigned BITS>
struct FillerSpare<DisusedField<BITS>>
{
   static void Fill(FieldAccessorType& aAccessor) { aAccessor.mIsDisused = true; }
};

template<typename FIELD, typename NO_STATEMENT_TAG>
struct FillerNoStatement
{
   static void Fill(FieldAccessorType& aAccessor) { aAccessor.mNoStatementValue = -1; }
};

template<typename FIELD>
struct FillerNoStatement<FIELD, FieldNoStatementValidTag>
{
   static void Fill(FieldAccessorType& aAccessor) { aAccessor.mNoStatementValue = FIELD::cNO_STATEMENT_VALUE; }
};

template<typename FIELD>
struct WSF_L16_EXPORT Filler
{
   static void Fill(FieldAccessorType& aAccessor)
   {
      FillerAccessors<FIELD, typename FIELD::accessor_type>::Fill(aAccessor);
      aAccessor.mFieldSizeBits = FIELD::cFIELD_SIZE;
      FillerEnum<FIELD, typename FIELD::enum_map_type>::Fill(aAccessor);
      FillerUnits<FIELD, typename FIELD::accessor_units>::Fill(aAccessor);
      FillerNoStatement<FIELD, typename FIELD::no_statement_type>::Fill(aAccessor);
      FillerSpare<FIELD>::Fill(aAccessor);
   }
};
} // namespace Accessors

class WSF_L16_EXPORT MessageAccessorFiller
{
public:
   MessageAccessorFiller()
      : mFieldIndex(0)
   {
   }
   void                 ProcessMessage(Messages::Base* aMessage);
   void                 AddField(int aOffset, FieldAccessorType* aAccessor);
   int                  mFieldIndex;
   Messages::Base*      mMessagePtr;
   MessageAccessorType* mMessageAccessor;
};

template<typename FIELD>
MessageAccessorFiller& operator<<(MessageAccessorFiller& aC, FIELD& aField)
{
   const auto&        type              = typeid(FIELD);
   FieldAccessorType* fieldAccessorType = FieldAccessorType::Find(type);
   if (!fieldAccessorType)
   {
      int                id       = (int)FieldAccessorType::sFieldAccessorTypes.size();
      FieldAccessorType& accessor = FieldAccessorType::sFieldAccessorTypes[type];
      accessor.mId                = id;
      Accessors::Filler<FIELD>::Fill(accessor);
      accessor.FormatEnumNames();
      fieldAccessorType = &accessor;
   }
   char* msgBeginPtr = ((char*)aC.mMessagePtr);
   char* fieldPtr    = ((char*)&aField);
   aC.AddField((int)(fieldPtr - msgBeginPtr), fieldAccessorType);
   return aC;
}
} // namespace Messages
} // namespace WsfL16

#endif
