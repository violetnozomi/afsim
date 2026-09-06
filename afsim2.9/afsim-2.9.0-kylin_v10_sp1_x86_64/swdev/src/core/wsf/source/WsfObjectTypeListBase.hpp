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

#ifndef WSFOBJECTTYPELISTBASE_HPP
#define WSFOBJECTTYPELISTBASE_HPP

#include <map>
#include <memory>
#include <set>
#include <vector>

class UtInput;
class WsfDeferredInput;
class WsfObject;
class WsfPlatform;
class WsfScenario;
#include "WsfStringId.hpp"
#include "WsfTypes.hpp"

template<typename T>
class WsfObjectTypeList;

namespace WsfObjectTypeListDetail
{
class LoadTypeP_Later;
}

//! The interface an object type list must support
class WSF_EXPORT WsfObjectTypeListBaseI
{
public:
   typedef std::vector<WsfStringId> TypeIdList;

   virtual ~WsfObjectTypeListBaseI() = default;
   virtual void Clear()              = 0;

   virtual bool ProcessInput(UtInput& aInput) = 0;
   virtual void CompleteLoad() {}

   virtual WsfObject*         Clone(WsfStringId aId) const             = 0;
   virtual WsfObject*         Find(WsfStringId aId) const              = 0;
   virtual const std::string& GetBlockName() const                     = 0;
   virtual void               GetTypeIds(TypeIdList& aListOfIds) const = 0;
};

//! Provides the interface of WsfObjectTypeListBaseI by forwarding calls to an underlying implementation.
//! This is used to provide an inline, non-dll-exported interface.
template<typename T>
class WsfObjectTypeListFwd : public WsfObjectTypeListBaseI
{
public:
   WsfObjectTypeListFwd(WsfObjectTypeListBaseI* aActualTypeListPtr)
      : mActualTypeListPtr(aActualTypeListPtr)
   {
   }

   ~WsfObjectTypeListFwd() override { delete mActualTypeListPtr; }

   void Clear() override { mActualTypeListPtr->Clear(); }

   bool ProcessInput(UtInput& aInput) override { return mActualTypeListPtr->ProcessInput(aInput); }

   void CompleteLoad() override { return mActualTypeListPtr->CompleteLoad(); }

   T* Clone(WsfStringId aId) const override { return (T*)mActualTypeListPtr->Clone(aId); }

   T* Find(WsfStringId aId) const override { return (T*)mActualTypeListPtr->Find(aId); }

   const std::string& GetBlockName() const override { return mActualTypeListPtr->GetBlockName(); }

   void GetTypeIds(TypeIdList& aListOfIds) const override { return mActualTypeListPtr->GetTypeIds(aListOfIds); }

protected:
   WsfObjectTypeList<T>* TypeList() { return (WsfObjectTypeList<T>*)mActualTypeListPtr; }

   WsfObjectTypeListBaseI* mActualTypeListPtr;
};

//! Base class for WsfObjectTypeList.  Provides most of the implementation.
class WSF_EXPORT WsfObjectTypeListBase : public WsfObjectTypeListBaseI
{
public:
   WsfObjectTypeListBase(const WsfObjectTypeListBase&) = delete;
   WsfObjectTypeListBase& operator=(const WsfObjectTypeListBase&) = delete;

   friend class WsfObjectTypeListDetail::LoadTypeP_Later;

   enum Flags
   {
      // A type may be defined more than once; only the last definition remains.
      cREDEFINITION_ALLOWED = 1,
      // There is no base type, 'new BaseType' is done for each instance
      cSINGULAR_BASE_TYPE = 2,
      // The base type is found in an object factory
      cOBJECT_FACTORY_BASE = 4,
      // Disable deferred loading. This should be used for types that do not depend on other types
      cNO_DELAY_LOAD = 8,
      // Flags used for signature type lists
      cSIGNATURE_FLAGS = cREDEFINITION_ALLOWED | cNO_DELAY_LOAD
   };

   WsfObjectTypeListBase(unsigned int       aFlags,
                         const std::string& aBlockName   = std::string(),
                         WsfScenario*       aScenarioPtr = nullptr);

   ~WsfObjectTypeListBase() override;

   //! Clear (Erase) all type entries
   void Clear() override;

   //! Return a vector of the type identifiers currently being controlled by this list.
   void GetTypeIds(TypeIdList& aListOfIds) const override;

   bool ProcessInput(UtInput& aInput) override;

   //! Return the current number of types being maintained by the list
   int Size() const { return (int)mTypeMap.size(); }

   WsfObject* Clone(WsfStringId aId) const override = 0;
   WsfObject* Find(WsfStringId aId) const override  = 0;

   const std::string& GetBlockName() const override { return mBlockName; }

   WsfScenario* GetScenario() const { return mScenarioPtr; }

protected:
   WsfDeferredInput& GetDeferredInput() const;

   bool CanDelayLoad() const { return (mFlags & cNO_DELAY_LOAD) == 0; }

   bool AddP(WsfStringId aId, std::unique_ptr<WsfObject> aDefinitionPtr);

   WsfObject* CloneP(WsfStringId aId) const;

   virtual WsfObject* FindP(WsfStringId aId) const;

   virtual bool InitializeTypeP(WsfObject* aObjectPtr) = 0;

   bool LoadTypeP(UtInput& aInput, WsfObject*& aLoadedObjectPtr);

   virtual bool DeferredLoadTypeP(UtInput& aInput, WsfObject*& aLoadedObjectPtr);

   bool DeleteComponentP(UtInput& aInput, WsfPlatform& aPlatform, int aRole, bool aIsNamed);

   bool LoadComponentP(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding, int aRole, bool aIsNamed, bool aIsEditable);

   typedef std::map<WsfStringId, std::unique_ptr<WsfObject>> TypeMap;
   TypeMap                                                   mTypeMap;

   //! A list of type objects that have been redefined.
   std::vector<std::unique_ptr<WsfObject>> mRedefinedTypes;

   std::set<WsfStringId> mCoreTypes;

   unsigned int mFlags;

   const std::string mBlockName;

   WsfObject* (*mCreateSingularTypeFunction)();
   WsfObject* (*mFactoryCreateFunction)(void* aListPtr, const std::string& aTypeName);
   WsfObject* (*mFactoryCreateDefault)();
   void*        mFactoryPtr;
   WsfScenario* mScenarioPtr;
};

#endif
