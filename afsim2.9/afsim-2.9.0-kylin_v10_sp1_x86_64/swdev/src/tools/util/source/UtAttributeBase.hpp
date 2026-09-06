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

#ifndef UTATTRIBUTEBASE_HPP
#define UTATTRIBUTEBASE_HPP

#include "ut_export.h"

#include <map>
#include <memory>
#include <string>

template<class T>
class UtAttribute;

class UtDictionary;
#include "UtException.hpp"
#include "UtStringId.hpp"
#include "UtTypeTraits.hpp"
namespace ut
{
class Variant;
}
using UtVariant = ut::Variant;

//! UtAttributeBase is the base class for attributes.  It has a name, a type, and a designation
//! of whether the attribute is simple or complex (i.e., a composition of attributes).  Simple attributes
//! need to be registered by the method UtAttributeBase::RegisterPrototype, so that a type string can
//! be stored to look up prototypes of the attribute for cloning.
//! Type is necessary for a simple attribute to have meaning, although the name of the attribute can be left
//! blank.  Complex attributes do not necessarily need either type or name to be valid.  Simple attributes are
//! defined by the templated class UtAttribute, and complex attributes are defined by the class UtAttributeContainer.
class UT_EXPORT UtAttributeBase
{
public:
   virtual ~UtAttributeBase() = default;

   static void RegisterPrototype(const std::string& aTypeName, std::unique_ptr<UtAttributeBase> aPrototypePtr);

   static UtAttributeBase* GetPrototype(const std::string& aTypeName);

   //! Get the attribute, copying it into the given value.
   template<class T>
   void Get(T& aType) const
   {
      const UtAttribute<T>* ptr = dynamic_cast<const UtAttribute<T>*>(this);
      if (ptr != nullptr)
      {
         aType = ptr->Get();
      }
      else
      {
         throw NotConvertibleException(*this);
      }
   }

   //! Get a pointer to the attribute.
   template<class T>
   void Get(T*& aTypePtr)
   {
      auto* ptr = dynamic_cast<UtAttribute<ut::remove_const_t<T>>*>(this);
      if (ptr != nullptr)
      {
         aTypePtr = &ptr->Get();
      }
      else
      {
         throw NotConvertibleException(*this);
      }
   }

   //! Get a pointer to the attribute (const version)
   template<class T>
   void Get(const T*& aTypePtr) const
   {
      const UtAttribute<T>* ptr = dynamic_cast<const UtAttribute<T>*>(this);
      if (ptr != nullptr)
      {
         aTypePtr = &ptr->Get();
      }
      else
      {
         throw NotConvertibleException(*this);
      }
   }

   //! Returns true on success, false on failure.
   template<class T>
   bool TryGet(T& aType) const noexcept
   {
      const auto* ptr = dynamic_cast<const UtAttribute<T>*>(this);
      if (ptr != nullptr)
      {
         aType = ptr->Get();
         return true;
      }
      return false;
   }

   //! Get a pointer to the attribute.
   //! Returns true on success, false on failure.
   template<class T>
   bool TryGet(T*& aTypePtr)
   {
      auto* ptr = dynamic_cast<UtAttribute<ut::remove_const_t<T>>*>(this);
      if (ptr != nullptr)
      {
         aTypePtr = &ptr->Get();
         return true;
      }
      return false;
   }

   //! Get a pointer to the attribute (const version)
   //! Returns true on success, false on failure.
   template<class T>
   bool TryGet(const T*& aTypePtr) const
   {
      const UtAttribute<T>* ptr = dynamic_cast<const UtAttribute<T>*>(this);
      if (ptr != nullptr)
      {
         aTypePtr = &ptr->Get();
         return true;
      }
      return false;
   }

   //! Set the attribute, copying it from the given value.
   //! @note I use static_cast here, as I feel c++ is a little too strict with typing,
   //!    especially where enumerated types differ from integers.  Using static_cast
   //!    allows conversion from integers to c++ enumerated types.
   template<class T>
   void Set(T&& aType)
   {
      auto* ptr = dynamic_cast<UtAttribute<ut::decay_t<T>>*>(this);
      if (ptr != nullptr)
      {
         ptr->Set(std::forward<T>(aType));
      }
      else
      {
         throw NotConvertibleException(*this);
      }
   }

   //! Set the actual value in the derived class, using type information provided by polymorphism.
   virtual void SetAttribute(const UtAttributeBase& aSrc) = 0;

   // The following are specialized implementations of "Get(...)".
   // The intent is to provide optimized versions in derived classes (through partial template specialization).

   //! Return the attribute as a boolean.
   //! If the actual type is not "bool", an exception is thrown.
   virtual bool GetBool() const
   {
      bool result;
      Get(result);
      return result;
   }

   //! Return the attribute as a double.
   //! If the actual type is not "double", an exception is thrown.
   virtual double GetDouble() const
   {
      double result;
      Get(result);
      return result;
   }

   //! Return the attribute as an int.
   //! If the actual type is not "int", an exception is thrown.
   virtual int GetInt() const
   {
      int result;
      Get(result);
      return result;
   }

   //! Return the attribute as a string.
   //! If the actual type is not "string", an exception is thrown.
   virtual const std::string& GetString() const
   {
      const std::string* resultPtr;
      Get(resultPtr);
      return *resultPtr;
   }

   //! Create a copy of this attribute from a given prototype.
   virtual UtAttributeBase* Clone() const = 0;

   //! Return the string corresponding to the attribute's name
   UtStringId GetName() const { return mNameId; }

   //! Return the dictionary id corresponding to the attribute's dictionary id.
   UtStringId GetNameId() const { return mNameId; }

   //! Set the name of the attribute to the given string.
   void SetName(UtStringId aName) { mNameId = aName; }

   //! Return the dictionary type id corresponding to the given type.
   //! @note This method should be used with caution: it is not compatible
   //! with WsfString!
   static UtStringId GetTypeId(const std::string& aType) { return aType; }

   //! Return the string corresponding to the attribute's type.
   //! A default version is provided to maintain backward compatibility.
   virtual const std::string& GetType() const;

   //! Return the dictionary id corresponding to the attribute's dictionary id.
   //! @note This method should be used with caution: it is not compatible
   //! with WsfString!
   //! A default version is provided to maintain backward compatibility.
   virtual int GetTypeId() const { return 0; }

   //! Return whether this attribute is a container of other attributes.
   //! A default version is provided to maintain backward compatibility.
   virtual bool IsContainerType() const { return false; }

   //! Convert the attribute into a variant.  Returns 'true' if successful.
   virtual bool GetVariant(UtVariant& /*aVariant*/) const { return false; }

   //! Get the dictionary used to convert names and types to/from integers.
   static UtDictionary&      GetDictionary(); // Must be public for VS6
   static const std::string& GetDictionaryString(int aId);
   static int                GetDictionaryNumber(const std::string&);

   class UT_EXPORT NotConvertibleException : public UtException
   {
   public:
      explicit NotConvertibleException(const UtAttributeBase& aAttribute);
   };

   //! This is the base class for basic iterator functionality.
   //! Method names are closely aligned with their WSF script
   //! iterator counterparts.
   class IteratorP
   {
   public:
      virtual void             First()   = 0;
      virtual void             Next()    = 0;
      virtual bool             HasNext() = 0;
      virtual UtAttributeBase& Data()    = 0;

      virtual ~IteratorP() = default;

   protected:
      IteratorP() = default;
   };

   //! This is the iterator for users to instantiate.
   //! The interface is meant to align with STL iterators.
   class Iterator
   {
   public:
      explicit Iterator(UtAttributeBase& aAttr)
         : mIterPtr(aAttr.CreateIterator())
      {
      }
      // disallow copy and assignment to avoid
      // multiple deletions of mIterPtr
      Iterator(const Iterator& aSrc) = delete;
      Iterator& operator=(const Iterator& aRhs) = delete;
      ~Iterator() noexcept                      = default;

      IteratorP*       operator->() { return mIterPtr.get(); }
      UtAttributeBase& operator*() { return mIterPtr->Data(); }

   private:
      std::unique_ptr<IteratorP> mIterPtr;
   };

   //! This is the base class for basic const_iterator functionality.
   //! Method names are closely aligned with their WSF script
   //! iterator counterparts.
   class ConstIteratorP
   {
   public:
      virtual void                   First()   = 0;
      virtual void                   Next()    = 0;
      virtual bool                   HasNext() = 0;
      virtual const UtAttributeBase& Data()    = 0;

      virtual ~ConstIteratorP() = default;

   protected:
      ConstIteratorP() = default;
   };

   //! Const version of the above Iterator class.
   //! This is the const iterator for users to instantiate.
   //! The interface is meant to align with STL const iterators.
   class ConstIterator
   {
   public:
      explicit ConstIterator(const UtAttributeBase& aAttr)
         : mIterPtr(aAttr.CreateConstIterator())
      {
      }
      // disallow copy and assignment to avoid
      // multiple deletions of mIterPtr
      ConstIterator(const ConstIterator& aSrc) = delete;
      ConstIterator& operator=(const ConstIterator& aRhs) = delete;
      ~ConstIterator() noexcept                           = default;

      ConstIteratorP*        operator->() { return mIterPtr.get(); }
      const UtAttributeBase& operator*() { return mIterPtr->Data(); }

   private:
      std::unique_ptr<ConstIteratorP> mIterPtr;
   };

protected:
   //! Create an iterator to this object.
   //! Derived classes must provide an implementation for this method.
   virtual std::unique_ptr<IteratorP> CreateIterator() = 0;

   //! Create an iterator to this object.
   //! Derived classes must provide an implementation for this method.
   virtual std::unique_ptr<ConstIteratorP> CreateConstIterator() const = 0;

   explicit UtAttributeBase() = default;

   // Named and typed constructor is for UtAttribute<T> instances.
   explicit UtAttributeBase(UtStringId aName);

   //! Set the type of the attribute to the given string.
   //! Derived classes should implement this method as appropriate.
   //! A default version is provided for backward compatibility.
   virtual void SetType(const std::string& /*aType*/) {}

   class Prototypes
   {
   public:
      using PrototypeMap = std::map<int, std::unique_ptr<UtAttributeBase>>;

      UtAttributeBase* Get(int aType);

      void Add(const std::string& aTypeName, std::unique_ptr<UtAttributeBase> aPrototypePtr);

   private:
      PrototypeMap mPrototypeMap;
   };

   // static
   //! The "list" of attribute prototypes.
   static Prototypes& GetPrototypes() // singleton
   {
      if (mPrototypesInstancePtr == nullptr)
      {
         mPrototypesInstancePtr = new Prototypes();
      }
      return *mPrototypesInstancePtr;
   }

   static void ClearPrototypes()
   {
      delete mPrototypesInstancePtr;
      mPrototypesInstancePtr = nullptr;
   }

   static void ClearStatic();

private:
   UtStringId mNameId;

   static Prototypes*   mPrototypesInstancePtr;
   static UtDictionary* mDictionaryInstancePtr;
};

using UtData = UtAttributeBase;

#endif
