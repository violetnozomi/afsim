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

#ifndef UTATTRIBUTE_HPP
#define UTATTRIBUTE_HPP

#include <iosfwd>
#include <string>

template<class T>
class UtAttribute;
#include "UtAttributeBase.hpp"
#include "UtException.hpp"
#include "UtMemory.hpp"
#include "UtVariant.hpp"

// typedefs
using UtIntAttribute     = UtAttribute<int>;
using UtBoolAttribute    = UtAttribute<bool>;
using UtDoubleAttribute  = UtAttribute<double>;
using UtStringAttribute  = UtAttribute<std::string>;
using UtVariantAttribute = UtAttribute<UtVariant>;

/** A UtAttribute embodies an attribute of arbitrary type.  Standard types have typedefs for easier use.
   Use UtAttributes when one does not know what attribute to get/set until runtime (it is string-based,
   typically read from a file.)  This use is analogous to being able to update records in a database.
   The interface is of the form:
      template <class T> void Get(string, T& aValue) const  and
      template <class T> void Set(string, const T& aValue).
*/
template<class T>
class UtAttribute : public UtAttributeBase
{
public:
   UtAttribute() = default;

   using UtAttributeBase::RegisterPrototype;

   static void RegisterPrototype(const std::string& aTypeName)
   {
      RegisterPrototype(aTypeName, ut::make_unique<UtAttribute<T>>());
   }

   explicit UtAttribute(UtStringId aName)
      : UtAttributeBase(aName)
      , mAttribute()
   {
   }

   explicit UtAttribute(UtStringId aName, const T& aInitialValue)
      : UtAttributeBase(aName)
      , mAttribute(aInitialValue)
   {
   }

   ~UtAttribute() override = default;

   //! Get the value of this attribute.
   T& Get() { return mAttribute; }

   //! Get the value of this attribute (const version).
   const T& Get() const { return mAttribute; }

   //! Store the given value in this attribute.
   void Set(const T& aValue) { mAttribute = aValue; }
   void Set(T&& aValue) { mAttribute = std::move(aValue); }

   void SetAttribute(const UtAttributeBase& aSrc) override
   {
      const UtAttribute<T>* derivedPtr = dynamic_cast<const UtAttribute<T>*>(&aSrc);
      if (derivedPtr != nullptr)
      {
         Set(derivedPtr->Get());
      }
      else
      {
         throw UtAttributeBase::NotConvertibleException(aSrc);
      }
   }

   //! Make a copy of this attribute.
   //! Instead of calling this directly, users should use ut::clone.
   UtAttribute<T>* Clone() const override { return new UtAttribute<T>(*this); }

   template<class Type>
   friend std::ostream& operator<<(std::ostream& os, const UtAttribute<Type>& aAttribute);

   //! Allow this object to be used as a functor.
   const T& operator()() const { return mAttribute; }

   bool operator==(const UtAttribute<T>& aRhs) const
   {
      return ((GetNameId() == aRhs.GetNameId()) && (mAttribute == aRhs.mAttribute));
   }

   bool operator!=(const UtAttribute<T>& aRhs) const
   {
      return ((GetNameId() != aRhs.GetNameId()) || (mAttribute != aRhs.mAttribute));
   }

   int GetTypeId() const override { return mClassTypeId; } // can be called by an instance

   static int GetClassTypeId() { return mClassTypeId; }

   const std::string& GetType() const override { return mClassType; }

   bool IsContainerType() const override { return false; }

   bool GetVariant(UtVariant& aVariant) const override;

protected:
   T                  mAttribute;
   static std::string mClassType;
   static int         mClassTypeId;

   void SetType(const std::string& aClassType) override
   {
      mClassType   = aClassType;
      mClassTypeId = GetDictionaryNumber(mClassType);
   }

   template<class Q>
   class IteratorP : public UtAttributeBase::IteratorP
   {
   public:
      explicit IteratorP(UtAttribute<Q>& aAttribute)
         : mAttribute(aAttribute)
      {
      }

      void             First() override {}
      void             Next() override {}
      bool             HasNext() override { return false; }
      UtAttributeBase& Data() override { return mAttribute; }

      ~IteratorP() override = default;

   protected:
      IteratorP() = default;

   private:
      IteratorP& operator=(const IteratorP& aSrc) = delete;

      UtAttribute<Q>& mAttribute;
   };

   template<class Q>
   class ConstIteratorP : public UtAttributeBase::ConstIteratorP
   {
   public:
      explicit ConstIteratorP(const UtAttribute<Q>& aAttribute)
         : mAttribute(aAttribute)
      {
      }

      void                   First() override {}
      void                   Next() override {}
      bool                   HasNext() override { return false; }
      const UtAttributeBase& Data() override { return mAttribute; }

      ~ConstIteratorP() override = default;

   protected:
      ConstIteratorP() = default;

   private:
      ConstIteratorP& operator=(const ConstIteratorP& aSrc) = delete;

      const UtAttribute<Q>& mAttribute;
   };

   std::unique_ptr<UtAttributeBase::IteratorP> CreateIterator() override
   {
      return ut::make_unique<IteratorP<T>>(*this);
   }

   std::unique_ptr<UtAttributeBase::ConstIteratorP> CreateConstIterator() const override
   {
      return ut::make_unique<ConstIteratorP<T>>(*this);
   }
};

template class UT_EXPORT UtAttribute<int>;
template class UT_EXPORT UtAttribute<bool>;
template class UT_EXPORT UtAttribute<double>;
template class UT_EXPORT UtAttribute<std::string>;

// static
template<class T>
std::string UtAttribute<T>::mClassType = "";
template<class T>
int UtAttribute<T>::mClassTypeId = 0;

//! Define a translation to UtVariant for basic types, and to allow copy.
//!  Return false for any other type.
template<typename T>
inline bool UtAttributeGetVariant(const T& /*aValue*/, UtVariant& /*aVariant*/)
{
   return false;
}
template<>
inline bool UtAttributeGetVariant(const int& aValue, UtVariant& aVariant)
{
   aVariant = aValue;
   return true;
}
template<>
inline bool UtAttributeGetVariant(const bool& aValue, UtVariant& aVariant)
{
   aVariant = aValue;
   return true;
}
template<>
inline bool UtAttributeGetVariant(const double& aValue, UtVariant& aVariant)
{
   aVariant = aValue;
   return true;
}
template<>
inline bool UtAttributeGetVariant(const std::string& aValue, UtVariant& aVariant)
{
   aVariant = aValue;
   return true;
}
template<>
inline bool UtAttributeGetVariant(const UtVariant& aValue, UtVariant& aVariant)
{
   aVariant = aValue;
   return true;
}

// virtual
template<class T>
inline bool UtAttribute<T>::GetVariant(UtVariant& aVariant) const
{
   return UtAttributeGetVariant(mAttribute, aVariant);
}

// friend
//! An overloaded << operator for use with streams.
template<class Type>
std::ostream& operator<<(std::ostream& os, const UtAttribute<Type>& aAttribute)
{
   os << aAttribute.mAttribute;
   return os;
}

#endif
