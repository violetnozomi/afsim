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

#ifndef UTATTRIBUTECONTAINER_HPP
#define UTATTRIBUTECONTAINER_HPP

#include "ut_export.h"

#include <string>
#include <unordered_map>
#include <vector>

#include "UtAttribute.hpp"
#include "UtCloneablePtr.hpp"
#include "UtMemory.hpp"

/** An interface supporting a collection of attribute objects. See UtAttribute for more information.
   Note: Treat this class as an interface, to be used in the context of multiple inheritance, as follows:
   class MyClass : public MyBaseClass:
                   public UtAttributeContainer
*/
class UT_EXPORT UtAttributeContainer : public UtAttributeBase
{
public:
   using AttributeMap = std::unordered_map<std::string, UtCloneablePtr<UtAttributeBase>>;

   UtAttributeContainer()                                 = default;
   UtAttributeContainer(const UtAttributeContainer& aSrc) = default;
   UtAttributeContainer(UtAttributeContainer&& aSrc)      = default;

   UtAttributeContainer& operator=(const UtAttributeContainer& aSrc) = default;
   UtAttributeContainer& operator=(UtAttributeContainer&& aSrc) = default;

   ~UtAttributeContainer() noexcept override = default;

   //! Create a copy (clone) of "this" object.
   //! Instead of calling this directly, users should use ut::clone.
   UtAttributeContainer* Clone() const override { return new UtAttributeContainer(*this); }

   //! @deprecated Removing in AFSIM 2.9
   //! Takes ownership of aAttributePtr
   void UT_DEPRECATED AddAttribute(UtAttributeBase* aAttributePtr)
   {
      AddAttribute(std::unique_ptr<UtAttributeBase>{aAttributePtr});
   }
   void AddAttribute(std::unique_ptr<UtAttributeBase> aAttributePtr);

   UtAttributeBase& GetAttribute(const std::string& aString) const;
   bool             AttributeExists(const std::string& aString) const;

   // quiet warnings
   using UtAttributeBase::GetBool;
   using UtAttributeBase::GetDouble;
   using UtAttributeBase::GetInt;
   using UtAttributeBase::GetString;

   bool         GetBool(const std::string& aName) const;
   int          GetInt(const std::string& aName) const;
   double       GetDouble(const std::string& aName) const;
   std::string& GetString(const std::string& aName) const;
   UtVariant    GetVar(const std::string& aName) const;

   //! @deprecated Removing in AFSIM 2.9
   //! Takes ownership of aPrototypePtr
   static void UT_DEPRECATED RegisterPrototype(const std::string& aTypeName, UtAttributeBase* aPrototypePtr)
   {
      RegisterPrototype(aTypeName, std::unique_ptr<UtAttributeBase>{aPrototypePtr});
   }
   static void RegisterPrototype(const std::string& aTypeName, std::unique_ptr<UtAttributeBase> aPrototypePtr);

   //! Assign an attribute of the given type.  If an existing attribute
   //! exists with that name, it will be reassigned.  If no attribute exists,
   //! one will be created.  Throws an exception if the existing attribute is
   //! not the same type as the assigning type.
   template<class T>
   void Assign(const std::string& aName, T&& aValue)
   {
      using AttributeType = UtAttribute<ut::decay_t<T>>;
      auto itersuccess    = mAttributeMap.emplace(aName, UtCloneablePtr<UtAttributeBase>{});
      if (!itersuccess.second)
      {
         // attribute with name already existed, no insertion occurred.
         AttributeType* attributePtr = dynamic_cast<AttributeType*>(itersuccess.first->second.get());
         if (attributePtr != nullptr)
         {
            attributePtr->Set(std::forward<T>(aValue));
         }
         else
         {
            throw UtException("Assignment to different types.");
         }
      }
      else
      {
         // insertion of newly named attribute occured. now initialize the ptr.
         itersuccess.first->second = ut::make_unique<AttributeType>(aName);
         itersuccess.first->second->Set(std::forward<T>(aValue));
      }
   }

   void AssignBool(const std::string& aName, bool aInitialValue = false);
   void AssignInt(const std::string& aName, int aInitialValue = 0);
   void AssignDouble(const std::string& aName, double aInitialValue = 0.0);
   void AssignString(const std::string& aName, const std::string& aInitialValue = "");
   void AssignVar(const std::string& aName, const UtVariant& aVariant);

   //! Add an attribute of the given type.
   //! The new attribute object is cloned from the type,
   //! so the initial value is that of the input value.
   //! Use this method to build up a prototype that can then be cloned.
   template<class T>
   void Add(const std::string& aName, T&& aInitialValue)
   {
      auto attrPtr = ut::make_unique<UtAttribute<ut::decay_t<T>>>(aName);
      attrPtr->Set(std::forward<T>(aInitialValue));
      AddAttribute(std::move(attrPtr));
   }

   void AddBool(UtStringId aName, bool aInitialValue = false);
   void AddInt(UtStringId aName, int aInitialValue = 0);
   void AddDouble(UtStringId aName, double aInitialValue = 0.0);
   void AddString(UtStringId aName, const std::string& aInitialValue = "");

   bool Delete(const std::string& aName);

   //! Fill the given variable with the value of the attribute corresponding
   //! with the given name.
   //! @param aName The name of the attribute.
   //! @param aOutValue The returned value of the attribute.
   template<class T>
   void Get(const std::string& aName, T& aOutValue) const
   {
      GetAttribute(aName).Get(aOutValue);
   }

   //! Fills the given variable with the value of the attribute with the given name.
   //! @returns true on success, false on failure.
   //! @param aName The name of the attribute.
   //! @param aOutValue The returned value of the attribute.
   template<class T>
   bool TryGet(const std::string& aName, T& aOutValue) const
   {
      auto iter = mAttributeMap.find(aName);
      if (iter != mAttributeMap.end())
      {
         return iter->second->TryGet(aOutValue);
      }
      return false;
   }

   //! Set the value of the attribute with the given name to the given value.
   //! @param aName The name of the attribute.
   //! @param aValue The value to which the attribute will be set.
   template<class T>
   void Set(const std::string& aName, T&& aValue)
   {
      GetAttribute(aName).Set(std::forward<T>(aValue));
   }

   void SetAttribute(const UtAttributeBase& aSrc) override;

   //! Return a const pointer to the value of the attribute corresponding
   //! with the given name.
   //! @param aName The name of the attribute.
   //! @param aOutValuePtr A const pointer to the named attribute.
   template<class T>
   void Get(const std::string& aName, const T*& aOutValuePtr) const
   {
      GetAttribute(aName).Get(aOutValuePtr);
   }

   //! Return a (non-const) pointer to the value of the attribute corresponding
   //! with the given name.
   //! @param aName The name of the attribute.
   //! @param aOutValuePtr  A pointer to the named attribute.
   template<class T>
   void Get(const std::string& aName, T*& aOutValuePtr)
   {
      GetAttribute(aName).Get(aOutValuePtr);
   }

   int GetTypeId() const override { return mType; }

   const std::string& GetType() const override { return GetDictionaryString(mType); }

   bool     IsContainerType() const override { return true; }
   bool     HasAttributes() const;
   unsigned GetNumberOfAttributes() const;
   void     GetAttributeNames(std::vector<std::string>& aNames) const;

   void Merge(const UtAttributeContainer& aSrc);
   void Update(const UtAttributeContainer& aSrc);

   //! Set the type; made public so that containers may be used without the prototypes factory.
   void SetType(const std::string& aType) override { mType = GetDictionaryNumber(aType); }

   //! Registers bool, int, double, and string.
   static void RegisterAtomicTypes();
   //! Registers atomic types and UtMatrixd as "Matrix".
   static void RegisterStandardTypes();

   //! Returns the map of all attributes in this container
   const AttributeMap& GetAttributeMap() const { return mAttributeMap; }

   class IteratorP : public UtAttributeBase::IteratorP
   {
   public:
      IteratorP(UtAttributeContainer& aAttribute)
         : mMapPtr(&aAttribute.mAttributeMap)
         , mIter(aAttribute.mAttributeMap.begin())
      {
      }

      void             First() override { mIter = mMapPtr->begin(); }
      void             Next() override { ++mIter; }
      bool             HasNext() override { return (mIter != mMapPtr->end()); }
      UtAttributeBase& Data() override { return (*(mIter->second)); }

      ~IteratorP() override = default;

   protected:
      IteratorP() = default;

   private:
      AttributeMap*          mMapPtr = nullptr;
      AttributeMap::iterator mIter;
   };

   class ConstIteratorP : public UtAttributeBase::ConstIteratorP
   {
   public:
      ConstIteratorP(const UtAttributeContainer& aAttribute)
         : mMapPtr(&aAttribute.mAttributeMap)
         , mIter(aAttribute.mAttributeMap.begin())
      {
      }

      void                   First() override { mIter = mMapPtr->begin(); }
      void                   Next() override { ++mIter; }
      bool                   HasNext() override { return (mIter != mMapPtr->end()); }
      const UtAttributeBase& Data() override { return (*(mIter->second)); }

      ~ConstIteratorP() override = default;

   protected:
      ConstIteratorP() = default;

   private:
      const AttributeMap*          mMapPtr = nullptr;
      AttributeMap::const_iterator mIter;
   };

   std::unique_ptr<UtAttributeBase::IteratorP> CreateIterator() override { return ut::make_unique<IteratorP>(*this); }

   std::unique_ptr<UtAttributeBase::ConstIteratorP> CreateConstIterator() const override
   {
      return ut::make_unique<ConstIteratorP>(*this);
   }

   void Clear();

   UtAttributeBase* FindAttribute(const std::string& aName) const;

protected:
   mutable AttributeMap mAttributeMap;

private:
   int mType = 0;

   void SetAttributes(const AttributeMap& aSrc);
};

#endif
