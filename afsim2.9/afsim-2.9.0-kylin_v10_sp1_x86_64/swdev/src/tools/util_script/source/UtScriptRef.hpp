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

#ifndef UTSCRIPTREF_HPP
#define UTSCRIPTREF_HPP

#include "util_script_export.h"

#include <vector>

#include "UtReferenceCount.hpp"
class UtScriptAccessible;
class UtScriptClass;
class UtScriptTypes;
#include "UtStringId.hpp"

//--------------------------------------------------------------
//! The UtScriptRef class is the type that the interpreter will
//! work with to dispatch calls down to the application layer.
//! UtScriptRef holds a pointer to the actual application layer
//! object and the UtScriptClass that describes the class. Method
//! calls are made via the Call method, which calls the associated
//! UtScriptClass object passing in the arguments and the application
//! layer object pointer, which will be used to make the actual calls.
//--------------------------------------------------------------

class UTIL_SCRIPT_EXPORT UtScriptRef
{
public:
   enum MemManagement
   {
      cMANAGE,            // Indicates that this UtScriptRef will manage the memory.
      cDONT_MANAGE,       // Indicates that this UtScriptRef will not manage the memory (default).
      cEXTERNALLY_MANAGED // Indicates that this UtScriptRef will contains a reference managed externally
   };

   UtScriptRef(void* aAppObjectPtr, UtScriptClass* aScriptClassPtr, MemManagement aMemory = cDONT_MANAGE);
   UtScriptRef(void*             aAppObjectPtr,
               UtScriptClass*    aScriptClassPtr,
               UtReferenceCount* aReferenceCount,
               MemManagement     aMemory = cEXTERNALLY_MANAGED);

   static UtScriptRef* Ref(UtScriptAccessible* aAccessibleValuePtr,
                           UtScriptTypes*      aTypesPtr,
                           MemManagement       aMemory = cDONT_MANAGE);

   static UtScriptRef* Ref(UtScriptAccessible* aAccessibleValuePtr,
                           UtScriptClass*      aDefaultClassPtr,
                           MemManagement       aMemory = cDONT_MANAGE);

   UtScriptRef(const UtScriptRef& aSrc);

   virtual ~UtScriptRef();

   UtScriptRef& operator=(const UtScriptRef& aRhs);

   UtScriptRef* Clone();

   bool Cast(UtStringId aClassNameId);
   bool Cast(UtScriptClass* aClassPtr);

   void SetAppObject(void* aAppObjectPtr, UtReferenceCount* aReferenceCount = nullptr);
   void SetExternalManaged(void* aAppObjectPtr);

   //! Returns a pointer to the application object.
   template<class T = void>
   T* GetAppObject() const
   {
      return static_cast<T*>(mAppObjectPtr);
   }

   //! Returns a pointer to the script class object.
   UtScriptClass* GetScriptClass() const { return mScriptClassPtr; }

   void SetScriptClass(UtScriptClass* aScriptClassPtr) { mScriptClassPtr = aScriptClassPtr; }

   //! Sets the auxiliary data associated with this reference.
   void SetAuxData(int aAuxData);

   //! Returns the auxiliary data associated with this reference.
   int GetAuxData() const;

   //! Returns true if the reference points to a valid object
   bool IsValid() const
   {
      return (mAppObjectPtr != nullptr) && ((mMemory != cEXTERNALLY_MANAGED) || mCounterPtr->IsValid());
   }

   void SetExternalReference(UtReferenceCount* aRefCountPtr);

   MemManagement GetMemManagement() const { return mMemory; }

   UtReferenceCount* GetReferenceCounter() { return mCounterPtr; }

   void Set(void* aAppObjectPtr, UtScriptClass* aScriptClassPtr, UtReferenceCount* aReferenceCount, MemManagement aMemory);

private:
   UtScriptRef(); // Prohibit use.

   void Acquire(void* aAppObjectPtr, UtReferenceCount* aCounterPtr);

   void Release();

   UtReferenceCount* mCounterPtr;
   UtScriptClass*    mScriptClassPtr; // A pointer to the class type of the application object.
   void*             mAppObjectPtr;   // The application object.
   MemManagement     mMemory;         // Indicates how to manage the memory.
   int               mAuxData[1];     // Auxiliary data that can be associated with the script reference.
};

template<>
inline void* UtScriptRef::GetAppObject<void>() const
{
   return mAppObjectPtr;
}

#endif
