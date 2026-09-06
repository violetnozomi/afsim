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

#ifndef WSFOBJECTTYPELIST_HPP
#define WSFOBJECTTYPELIST_HPP

#include "wsf_export.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "WsfObject.hpp"
#include "WsfObjectTypeListBase.hpp"
#include "WsfStringId.hpp"
#include "WsfTypes.hpp"

//! A template class for defining a list of 'type' objects.
//!
//! A 'type' object is one that can be cloned to  create specific 'instances'
//! of the type. For instance, a 'platform_type' of 'Honda_Civic' can be
//! created added to a 'platform_type' list.  Instances of 'Honda_Civic' can
//! then cloned and added to the simulation.
//!
//! Each class the supports object types instantiates one of these.
//! The user or the input factories can populate these type lists and instantiate objects from them.

template<class T>
class WsfObjectTypeList : public WsfObjectTypeListBase
{
public:
   typedef T               ObjectType;
   typedef std::vector<T*> ObjectPtrList;

protected:
   template<typename DEFAULT_TYPE>
   static WsfObject* CreateDefaultFunction()
   {
      return new DEFAULT_TYPE;
   }

   static WsfObject* CreateFunction()
   {
      WsfObject* objectPtr = new ObjectType();
      return objectPtr;
   }

   template<typename FACTORY_LIST>
   static WsfObject* FactoryCreateFunction(void* aListPtr, const std::string& aTypeName)
   {
      const FACTORY_LIST& aList = *((FACTORY_LIST*)aListPtr);
      for (typename FACTORY_LIST::const_iterator i = aList.begin(); i != aList.end(); ++i)
      {
         WsfObject* newObjectPtr = (*i)(aTypeName);
         if (newObjectPtr != nullptr)
         {
            return newObjectPtr;
         }
      }
      return nullptr;
   }

public:
   WsfObjectTypeList(WsfScenario& aScenario, unsigned int aFlags, const std::string& aBlockName)
      : WsfObjectTypeListBase(aFlags, aBlockName, &aScenario)
   {
   }

   WsfObjectTypeList(WsfScenario& aScenario, const std::string& aBlockName)
      : WsfObjectTypeListBase(0, aBlockName, &aScenario)
   {
   }

   //! Add a new type with the given name.
   //!
   //! The return value will be 'true' if the type was successfully added
   //! and 'false' if not (i.e.: it attempts to duplicate a type that
   //! already exists. This will take ownership of aDefinitionPtr.
   bool Add(WsfStringId aId, std::unique_ptr<T> aDefinitionPtr)
   {
      static_assert(std::is_base_of<WsfObject, T>::value, "Add requires a WsfObject");
      return AddP(aId, std::move(aDefinitionPtr));
   }

   //! Add a new core type with the given name. Core types are defined by C++ code, and cannot be overwritten.
   //!
   //! The return value will be 'true' if the type was successfully added
   //! and 'false' if no (i.e.: it attempts to duplicate a type that
   //! already exists). This takes ownership of the aDefinitionPtr.
   bool AddCoreType(WsfStringId aId, std::unique_ptr<T> aDefinitionPtr)
   {
      static_assert(std::is_base_of<WsfObject, T>::value, "AddCoreType requires a WsfObject");
      if (AddP(aId, std::move(aDefinitionPtr)))
      {
         mCoreTypes.insert(aId);
         return true;
      }
      return false;
   }

   //! Clone an instance of the requested type. The returned instance
   //! can either be used as an object of the specified type or a base
   //! on which to build a new derived type.
   //!
   //! The return pointer will be zero if the requested type is not present.
   T* Clone(WsfStringId aId) const override
   {
      static_assert(std::is_base_of<WsfObject, ObjectType>::value, "Type does not support Clone()");
      return static_cast<T*>(CloneP(aId));
   }

   //! Return the definition of the given type.
   //! The return value will be zero if the requested type is not present.

   T* Find(WsfStringId aId) const override { return static_cast<ObjectType*>(FindP(aId)); }

   //! Returns a list of core types, i.e. types added via AddCoreType
   ObjectPtrList GetCoreTypes() const
   {
      ObjectPtrList typePtrVec;
      for (const auto& typeEntry : mCoreTypes)
      {
         auto it = mTypeMap.find(typeEntry);
         if (it != std::end(mTypeMap))
         {
            typePtrVec.push_back(static_cast<ObjectType*>(it->second.get()));
         }
      }

      return typePtrVec;
   }

   void GetCurrentTypes(ObjectPtrList& aListOfPtrs) const
   {
      aListOfPtrs.clear();

      for (TypeMap::const_iterator iter = mTypeMap.begin(); iter != mTypeMap.end(); ++iter)
      {
         aListOfPtrs.push_back(static_cast<ObjectType*>(iter->second.get()));
      }
   }

   //! Returns the list of types defined by the user (no core / builtin types)
   void GetCurrentUserTypes(ObjectPtrList& aListOfPtrs) const
   {
      aListOfPtrs.clear();

      for (TypeMap::const_iterator iter = mTypeMap.begin(); iter != mTypeMap.end(); ++iter)
      {
         if (mCoreTypes.find(iter->first) == mCoreTypes.end())
         {
            aListOfPtrs.push_back(static_cast<ObjectType*>(iter->second.get()));
         }
      }
   }

   bool IsType(WsfStringId aTypeName) const { return Find(aTypeName) != nullptr; }

   //! Result of a LoadType() operation.
   struct LoadResult
   {
      LoadResult()
         : mIsCommandProcessed(false)
         , mObjectTypePtr(nullptr)
      {
      }
      //! Returns 'true' if the input was processed by this type list
           operator bool() const { return mIsCommandProcessed; }
      bool mIsCommandProcessed;
      //! The object loaded.  Null if no object was loaded.
      ObjectType* mObjectTypePtr;
   };

   void SetSingularBaseType()
   {
      mCreateSingularTypeFunction = &WsfObjectTypeList<T>::CreateFunction;
      assert(mFactoryCreateFunction == nullptr);
      mFlags |= cSINGULAR_BASE_TYPE;
   }

   template<typename FACTORY_LIST>
   void SetObjectFactory(const FACTORY_LIST* aFactoryListPtr)
   {
      mFactoryPtr = (void*)aFactoryListPtr;
      // mFactoryCreateFunction = &WsfObjectTypeList<T>::FactoryCreateFunction<FACTORY_LIST>;
      mFactoryCreateFunction = &FactoryCreateFunction<FACTORY_LIST>;
      assert(mCreateSingularTypeFunction == nullptr);
   }

   template<typename DEFAULT_TYPE>
   void SetObjectFactoryDefault()
   {
      mFactoryCreateDefault = &CreateDefaultFunction<DEFAULT_TYPE>;
   }

   // =================================================================================================
   //! Load a new 'type object' from an input stream.
   //! Process the current command if it is one that defines a new 'type object' of this class.
   //! @param aInput The input stream.
   //! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
   virtual LoadResult LoadType(UtInput& aInput)
   {
      LoadResult result;
      WsfObject* loadedObjectPtr = nullptr;
      result.mIsCommandProcessed = LoadTypeP(aInput, loadedObjectPtr);
      result.mObjectTypePtr      = static_cast<ObjectType*>(loadedObjectPtr);
      return result;
   }

   bool ProcessInput(UtInput& aInput) override { return LoadType(aInput); }

   virtual bool InitializeType(ObjectType* aTypePtr) { return true; }

   //! @name Helper methods for components that utilize type lists.
   //! These are invoked from the component factories to facilitate the adding, editing and deleting
   //! of component instances on platforms.
   //@{
   bool LoadNamedComponent(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding, int aRole)
   {
      return LoadComponentP(aInput, aPlatform, aIsAdding, aRole, true, true);
   }

   bool LoadUnnamedComponent(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding, int aRole)
   {
      return LoadComponentP(aInput, aPlatform, aIsAdding, aRole, false, true);
   }

   bool LoadUnnamedComponentWithoutEdit(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding, int aRole)
   {
      return LoadComponentP(aInput, aPlatform, aIsAdding, aRole, false, false);
   }

   bool DeleteNamedComponent(UtInput& aInput, WsfPlatform& aPlatform, int aRole)
   {
      return DeleteComponentP(aInput, aPlatform, aRole, true);
   }

   bool DeleteUnnamedComponent(UtInput& aInput, WsfPlatform& aPlatform, int aRole)
   {
      return DeleteComponentP(aInput, aPlatform, aRole, false);
   }
   //@}

protected:
   bool InitializeTypeP(WsfObject* aObjectPtr) override { return InitializeType(static_cast<ObjectType*>(aObjectPtr)); }

private:
   // Copy constructor is declared but not defined in order to prevent its use.
   WsfObjectTypeList(const WsfObjectTypeList& aSrc) = delete;
   // Assignment operator is declared but not defined in order to prevent its use.
   WsfObjectTypeList& operator=(const WsfObjectTypeList& aRhs) = delete;
};

#endif
