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

#ifndef UTSCRIPTBASICTYPES_HPP
#define UTSCRIPTBASICTYPES_HPP

#include "UtScriptClassDefine.hpp"

// void is a special case and can't be used as variable type.
class UTIL_SCRIPT_EXPORT UtScriptVoid : public UtScriptClass
{
public:
   UtScriptVoid(UtScriptTypes* aTypesPtr);

   bool IsVoid() const override { return true; }
};

class UTIL_SCRIPT_EXPORT UtScriptNull : public UtScriptClass
{
public:
   UtScriptNull(UtScriptTypes* aTypesPtr);

   bool IsNull() const override { return true; }
};

class UTIL_SCRIPT_EXPORT UtScriptAppPointerClass : public UtScriptClass
{
public:
   UtScriptAppPointerClass(UtScriptTypes* aTypesPtr);
};

//! UtScriptVariant, like 'Object', can be any type.  However, this class can be implicitly cast to any other type
class UTIL_SCRIPT_EXPORT UtScriptVariant : public UtScriptClass
{
public:
   UtScriptVariant(UtScriptTypes* aTypesPtr);

   bool CanImplicitCast(UtStringId aClassNameId) const override;
};

//! Define derived UtScriptClass' for all the basic types
//! (int, double, bool, string).
class UTIL_SCRIPT_EXPORT UtScriptBasicType : public UtScriptClass
{
protected:
   UtScriptBasicType(const std::string& aClassName, UtScriptTypes* aTypesPtr);
};

class UTIL_SCRIPT_EXPORT UtScriptInt : public UtScriptBasicType
{
public:
   UtScriptInt(UtScriptTypes* aTypesPtr);

   bool IsInt() const override;
   void SetDefaultValue(UtScriptData& aData) override;

   void* Create(const UtScriptContext&) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;
};

class UTIL_SCRIPT_EXPORT UtScriptDouble : public UtScriptBasicType
{
public:
   UtScriptDouble(UtScriptTypes* aTypesPtr);

   bool IsDouble() const override;
   void SetDefaultValue(UtScriptData& aData) override;

   void* Create(const UtScriptContext&) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;
};

class UTIL_SCRIPT_EXPORT UtScriptBool : public UtScriptBasicType
{
public:
   UtScriptBool(UtScriptTypes* aTypesPtr);

   bool IsBool() const override;
   void SetDefaultValue(UtScriptData& aData) override;

   void* Create(const UtScriptContext&) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;
};

class UTIL_SCRIPT_EXPORT UtScriptString : public UtScriptBasicType
{
public:
   UtScriptString(UtScriptTypes* aTypesPtr);

   bool IsString() const override { return true; }
   void SetDefaultValue(UtScriptData& aData) override;

   void* Create(const UtScriptContext&) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;
};

class UTIL_SCRIPT_EXPORT UtScriptArray : public UtScriptClass
{
public:
   friend class InterfaceMethod;

   using Array = std::vector<UtScriptData>;

   UtScriptArray(const std::string& aClassName, UtScriptTypes* aTypesPtr, const TemplateArgTypes& aTemplateArgTypes);

   void*       Create(const UtScriptContext&) override;
   void        Destroy(void* aObjectPtr) override;
   void*       Clone(void* aObjectPtr) override;
   std::string ToString(void* aDataPtr) const override;
   std::string ToStringN(void* aDataPtr, int aMaxStringSize) const override;
   bool        LessThan(void* aLHS, void* aRHS) override;
   bool        EqualTo(void* aLHS, void* aRHS) override;
};

class UTIL_SCRIPT_EXPORT UtScriptMap : public UtScriptClass
{
public:
   friend class InterfaceMethod;

   using Map = std::map<UtScriptData, UtScriptData>;

   UtScriptMap(const std::string& aClassName, UtScriptTypes* aTypesPtr, const TemplateArgTypes& aTemplateArgTypes);

   void*       Create(const UtScriptContext&) override;
   void        Destroy(void* aObjectPtr) override;
   void*       Clone(void* aObjectPtr) override;
   std::string ToString(void* aDataPtr) const override;
   std::string ToStringN(void* aDataPtr, int aMaxStringSize) const override;
   bool        EqualTo(void* aLHS, void* aRHS) override;
   bool        LessThan(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(Get);
};

class UTIL_SCRIPT_EXPORT UtScriptSet : public UtScriptClass
{
public:
   friend class InterfaceMethod;

   using Set = std::set<UtScriptData>;

   UtScriptSet(const std::string& aClassName, UtScriptTypes* aTypesPtr, const TemplateArgTypes& aTemplateArgTypes);

   void*       Create(const UtScriptContext&) override;
   void        Destroy(void* aObjectPtr) override;
   void*       Clone(void* aObjectPtr) override;
   std::string ToString(void* aDataPtr) const override;
   std::string ToStringN(void* aDataPtr, int aMaxStringSize) const override;
   bool        EqualTo(void* aLHS, void* aRHS) override;
   bool        LessThan(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(Union);
   UT_DECLARE_SCRIPT_METHOD(Difference);
   UT_DECLARE_SCRIPT_METHOD(Intersection);
   UT_DECLARE_SCRIPT_METHOD(__Insert);
};

//! Script base class for a container iterator.
class UTIL_SCRIPT_EXPORT UtScriptIterator : public UtScriptClass
{
public:
   UtScriptIterator(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void Destroy(void* aObjectPtr) override;

   // Base class for a container iterator.
   class UTIL_SCRIPT_EXPORT Iterator
   {
   public:
      virtual ~Iterator() = default;

      virtual bool                HasNext() = 0;
      virtual const UtScriptData& Next()    = 0;
   };
};

//! Script iterator for an array.
class UTIL_SCRIPT_EXPORT UtScriptArrayIterator : public UtScriptIterator
{
public:
   UtScriptArrayIterator(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void Destroy(void* aObjectPtr) override;

   //! Iterator for an array.
   class UTIL_SCRIPT_EXPORT ArrayIterator : public UtScriptIterator::Iterator
   {
   public:
      ArrayIterator(const UtScriptArray::Array& aArray);

      bool                        HasNext() override;
      const UtScriptData&         Next() override;
      virtual bool                HasPrev();
      virtual const UtScriptData& Prev();
      virtual const UtScriptData& Key() const;
      virtual const UtScriptData& Data() const;

   private:
      const UtScriptArray::Array& mArrayRef;
      UtScriptData                mCurrentKey;
      UtScriptData                mCurrentData;
   };
};

//! Script iterator for a map.
class UTIL_SCRIPT_EXPORT UtScriptMapIterator : public UtScriptIterator
{
public:
   UtScriptMapIterator(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void Destroy(void* aObjectPtr) override;

   //! Iterator for a map.
   class UTIL_SCRIPT_EXPORT MapIterator : public UtScriptIterator::Iterator
   {
   public:
      MapIterator(const UtScriptMap::Map& aMap);

      bool                        HasNext() override;
      const UtScriptData&         Next() override;
      virtual bool                HasPrev();
      virtual const UtScriptData& Prev();
      virtual const UtScriptData& Key() const;
      virtual const UtScriptData& Data() const;

   private:
      const UtScriptMap::Map&          mMapRef;
      UtScriptMap::Map::const_iterator mMapIter;
      UtScriptData                     mCurrentKey;
      UtScriptData                     mCurrentData;
   };
};

//! Script iterator for a set.
class UTIL_SCRIPT_EXPORT UtScriptSetIterator : public UtScriptIterator
{
public:
   UtScriptSetIterator(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void Destroy(void* aObjectPtr) override;

   //! Iterator for a set.
   class UTIL_SCRIPT_EXPORT SetIterator : public UtScriptIterator::Iterator
   {
   public:
      SetIterator(const UtScriptSet::Set& aSet);

      bool                        HasNext() override;
      const UtScriptData&         Next() override;
      virtual bool                HasPrev();
      virtual const UtScriptData& Prev();
      virtual const UtScriptData& Data() const;

   private:
      const UtScriptSet::Set&          mSetRef;
      UtScriptSet::Set::const_iterator mSetIter;
      UtScriptData                     mCurrentData;
   };
};

#endif
