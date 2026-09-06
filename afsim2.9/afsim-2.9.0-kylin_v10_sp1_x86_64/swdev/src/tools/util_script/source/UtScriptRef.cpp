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

#include "UtScriptRef.hpp"

#include "UtScriptClass.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptData.hpp"
#include "UtScriptRefDiagnostic.hpp"
#include "UtScriptTypes.hpp"
#include "UtStringIdLiteral.hpp"

// global counter for tracking nullptrs, so that a reference counter doesn't need to be allocated everytime
// a UtScriptRef of a nullptr is created
static UtReferenceCount sNullptrCounter(1, 0);
//--------------------------------------------------------------
//! UtScriptRef constructor
//! @param aAppObjectPtr:   A pointer to the application object.
//! @param aScriptClassPtr: A pointer to the class type of the
//!                         application object.
//! @param aMemory:         By default it is set to cDONT_MANAGED,
//!                         means it will not be deleted by
//!                         the UtScriptRef.
//--------------------------------------------------------------

UtScriptRef::UtScriptRef(void* aAppObjectPtr, UtScriptClass* aScriptClassPtr, MemManagement aMemory)
   : mCounterPtr(nullptr)
   , mScriptClassPtr(aScriptClassPtr)
   , mAppObjectPtr(aAppObjectPtr)
   , mMemory(aMemory)
{
   mAuxData[0] = 0;
   if (aAppObjectPtr != nullptr)
   {
      if (aMemory == cMANAGE)
      {
         // initialize with a strong count of one
         mCounterPtr = new UtReferenceCount(1, 0);
         UT_SCRIPT_REF_DIAGNOSTIC_INCREMENT()
      }
      else
      {
         // initialize with a weak count of one
         mCounterPtr = new UtReferenceCount(0, 1);
      }
      aScriptClassPtr->OnNewScriptRef(*this);
   }
   else
   {
      // avoid allocating a new ref-counter for nullptr data,
      // by using a global counter that will never reach 0,
      // since nullptr types never actually need to be destroyed.
      Acquire(nullptr, &sNullptrCounter);
   }
}

UtScriptRef::UtScriptRef(void*             aAppObjectPtr,
                         UtScriptClass*    aScriptClassPtr,
                         UtReferenceCount* aReferenceCount,
                         MemManagement     aMemory)
   : mCounterPtr(aReferenceCount)
   , mScriptClassPtr(aScriptClassPtr)
   , mAppObjectPtr(nullptr)
   , mMemory(aMemory)
{
   Acquire(aAppObjectPtr, aReferenceCount);
   mAuxData[0] = 0;
   if (aAppObjectPtr != nullptr)
   {
      aScriptClassPtr->OnNewScriptRef(*this);
   }
}

UtScriptRef::UtScriptRef(const UtScriptRef& aSrc)
   : mCounterPtr(nullptr)
   , mScriptClassPtr(aSrc.mScriptClassPtr)
   , mAppObjectPtr(nullptr)
   , mMemory(aSrc.mMemory)
{
   Acquire(aSrc.mAppObjectPtr, aSrc.mCounterPtr);

   mAuxData[0] = aSrc.mAuxData[0];
}

UtScriptRef::~UtScriptRef()
{
   Release();
}

UtScriptRef& UtScriptRef::operator=(const UtScriptRef& aRhs)
{
   if (this != &aRhs)
   {
      Release();
      mScriptClassPtr = aRhs.mScriptClassPtr;
      mMemory         = aRhs.mMemory;
      mAuxData[0]     = aRhs.mAuxData[0];
      Acquire(aRhs.mAppObjectPtr, aRhs.mCounterPtr);
   }
   return *this;
}

//--------------------------------------------------------------
//! Clones a copy of this UtScriptRef and returns a pointer to
//! it. This actually creates another application layer object.
//! Note: The caller is responsible for the memory.
//--------------------------------------------------------------

UtScriptRef* UtScriptRef::Clone()
{
   return new UtScriptRef(mScriptClassPtr->Clone(mAppObjectPtr), mScriptClassPtr, cMANAGE);
}

//--------------------------------------------------------------
//! Casts this reference to the provided type if the cast is
//! appropriate. It basically just swaps the UtScriptClass
//! pointer.  If the cast fails, the app object is set to null.
//--------------------------------------------------------------

bool UtScriptRef::Cast(UtStringId aClassNameId)
{
   if (!mScriptClassPtr)
      return false;

   bool success = mScriptClassPtr->IsOfType(aClassNameId);
   if (!success)
   {
      // try to up-cast
      UtScriptClass* scriptClassPtr = mScriptClassPtr->GetTypes()->GetClass(aClassNameId);
      success                       = scriptClassPtr->UpcastFromType(this);
   }
   return success;
}

bool UtScriptRef::Cast(UtScriptClass* aClassPtr)
{
   if (!mScriptClassPtr)
      return false;

   bool success = mScriptClassPtr->IsOfType(aClassPtr->GetClassName());
   if (!success)
   {
      // try to up-cast
      success = aClassPtr->UpcastFromType(this);
   }
   return success;
}

//! Change the reference to point to a different
//! application object.
//!
//! @param aAppObjectPtr The application object to reference.
//! @param aReferenceCountPtr The reference count to use for the script ref.

// public
void UtScriptRef::SetAppObject(void* aAppObjectPtr, UtReferenceCount* aReferenceCountPtr)
{
   Release();

   UtReferenceCount* refCount = aReferenceCountPtr;
   if (aReferenceCountPtr == nullptr)
   {
      refCount = new UtReferenceCount;
      mMemory  = cDONT_MANAGE;
   }
   else
   {
      mMemory = cEXTERNALLY_MANAGED;
   }

   Acquire(aAppObjectPtr, refCount);
   mAuxData[0] = 0;
   if ((aAppObjectPtr != nullptr) && (mScriptClassPtr != nullptr))
   {
      mScriptClassPtr->OnNewScriptRef(*this);
   }
}

//! Sets the app object pointer as an externally managed value
//! The application should handle management of the reference count
//! by implementing OnNewScriptRef().
void UtScriptRef::SetExternalManaged(void* aAppObjectPtr)
{
   Release();

   mMemory = cEXTERNALLY_MANAGED;

   Acquire(aAppObjectPtr, new UtReferenceCount);
   mAuxData[0] = 0;
   if ((aAppObjectPtr != nullptr) && (mScriptClassPtr != nullptr))
   {
      mScriptClassPtr->OnNewScriptRef(*this);
   }
}

//! Acquire a reference to the application object.
//!
//! @param aAppObjectPtr The application object to reference.
//! @param aCounterPtr The reference counter.

// private
void UtScriptRef::Acquire(void* aAppObjectPtr, UtReferenceCount* aCounterPtr)
{
   mAppObjectPtr = aAppObjectPtr;
   mCounterPtr   = aCounterPtr;

   // Increment the reference count.
   if (mMemory == cMANAGE)
   {
      mCounterPtr->AddStrongRef();
      UT_SCRIPT_REF_DIAGNOSTIC_INCREMENT()
   }
   else
   {
      mCounterPtr->AddWeakRef();
   }
}

//! Release reference to the application object.

// private
void UtScriptRef::Release()
{
   if (mCounterPtr != nullptr)
   {
      // Decrement the reference count and delete the
      // memory if it is managed memory.
      if (mMemory == cMANAGE)
      {
         if (mCounterPtr->RemoveStrongRef())
         {
            mScriptClassPtr->Destroy(mAppObjectPtr);
            UT_SCRIPT_REF_DIAGNOSTIC_DECREMENT()
         }
      }
      else
      {
         mCounterPtr->RemoveWeakRef();
      }
      mCounterPtr = nullptr;
   }
}

int UtScriptRef::GetAuxData() const
{
   return mAuxData[0];
}

void UtScriptRef::SetAuxData(int aAuxData)
{
   mAuxData[0] = aAuxData;
}

void UtScriptRef::SetExternalReference(UtReferenceCount* aRefCountPtr)
{
   Release();
   mCounterPtr = aRefCountPtr;
   mCounterPtr->AddWeakRef();
   mMemory = cEXTERNALLY_MANAGED;
}

UtScriptRef* UtScriptRef::Ref(UtScriptAccessible* aAccessibleValuePtr,
                              UtScriptTypes*      aTypesPtr,
                              MemManagement       aMemory /*= cDONT_MANAGE*/)
{
   if (aAccessibleValuePtr != nullptr)
   {
      UtScriptClass* classPtr = aTypesPtr->GetClass(aAccessibleValuePtr);
      if (classPtr)
      {
         return new UtScriptRef(aAccessibleValuePtr, classPtr, aMemory);
      }
   }
   return new UtScriptRef(nullptr, aTypesPtr->GetClass(UtStringIdLiteral("null")));
}

UtScriptRef* UtScriptRef::Ref(UtScriptAccessible* aAccessibleValuePtr, UtScriptClass* aDefaultClassPtr, MemManagement aMemory)
{
   if (aAccessibleValuePtr != nullptr)
   {
      UtScriptClass* classPtr = aDefaultClassPtr->GetTypes()->GetClass(aAccessibleValuePtr);
      if (!classPtr)
      {
         classPtr = aDefaultClassPtr;
      }
      return new UtScriptRef(aAccessibleValuePtr, classPtr, aMemory);
   }
   else
   {
      return new UtScriptRef(nullptr, aDefaultClassPtr);
   }
}

void UtScriptRef::Set(void* aAppObjectPtr, UtScriptClass* aScriptClassPtr, UtReferenceCount* aReferenceCount, MemManagement aMemory)
{
   if (mCounterPtr != aReferenceCount)
   {
      Release();
   }
   mAppObjectPtr   = aAppObjectPtr;
   mScriptClassPtr = aScriptClassPtr;
   mCounterPtr     = aReferenceCount;
   mMemory         = aMemory;
}
