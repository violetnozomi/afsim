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
// ****************************************************************************
// Updated by Radiance Technologies
// ****************************************************************************

#include "UtScriptClass.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptCore.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptException.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptFunction.hpp"
#include "UtScriptTypes.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtStringUtil.hpp"

namespace // anonymous
{

struct MethodPtrCompare
{
   bool operator()(const UtScriptClass::InterfaceMethod* aLhs, const UtScriptClass::InterfaceMethod* aRhs) const
   {
      return *aLhs < *aRhs;
   }
   bool operator()(const UtScriptClass::InterfaceMethod* aLhs, UtStringId aRhs) const { return aLhs->GetName() < aRhs; }
   bool operator()(UtStringId aLhs, const UtScriptClass::InterfaceMethod* aRhs) const { return aLhs < aRhs->GetName(); }
};

} // end anonymous namespace

bool UtScriptClass::InterfaceMethod::sAbortOnScriptCallError(false);

//--------------------------------------------------------------
//! Derived classes should provide a class name as a string.
//--------------------------------------------------------------

UtScriptClass::UtScriptClass(UtStringId              aClassName,
                             UtScriptTypes*          aScriptTypesPtr,
                             const TemplateArgTypes& aTemplateArgTypes,
                             bool                    aBasicType)
   : mClassName(aClassName)
   , mTemplateArgTypes(aTemplateArgTypes)
   , mScriptTypesPtr(aScriptTypesPtr)
{
   mBasicType          = aBasicType;
   mConstructible      = false;
   mCloneable          = false;
   mContainer          = false;
   mLessThanComparable = false;
   mEqualityComparable = false;
   mIsScriptAccessible = false;
   mIsPseudoClass      = false;

   // As of AFSIM 2.9, script methods from Object are no longer available on basic types.
   // To support legacy compatibility, basic types can be implicitly cast to Object (see
   // UtScriptBasicType), and Object can be explicitly cast to the basic types.
   if (!mBasicType)
   {
      SetClassName(UtStringIdLiteral("Object"));
      AddMethod(ut::make_unique<IsValid>());
      AddMethod(ut::make_unique<IsNull1>());
      AddMethod(ut::make_unique<ConvertToString>());
   }

   if (mClassName == UtStringIdLiteral("Object"))
   {
      AddExplicitCastType("int");
      AddExplicitCastType("double");
      AddExplicitCastType("bool");
      AddExplicitCastType("string");
   }
}

//--------------------------------------------------------------
//! Calls the appropriate InterfaceMethod based on aMethodId,
//! passing in aAppObjectPtr, aVarArgs, and aReturnVal.
//--------------------------------------------------------------

bool UtScriptClass::Call(UtScriptExecutor*  aExecutorPtr,
                         UtScriptContext&   aContext,
                         const UtScriptRef& aReference,
                         size_t             aMethodIndex,
                         UtScriptMethodArgs aVarArgs,
                         bool&              aHasReturnVal,
                         UtScriptData&      aReturnVal)
{
   bool success = false;
   if (aMethodIndex < mMethodList.size())
   {
      auto& methodPtr = mMethodList[aMethodIndex];
      aHasReturnVal   = methodPtr->GetHasReturnVal();
      try
      {
         (*methodPtr)(aExecutorPtr, aContext, aReference, aVarArgs, aReturnVal);
         success = true;
      }
      catch (const UtScriptException& error)
      {
         methodPtr->ReportCallErrors(aExecutorPtr, nullptr, nullptr, aReturnVal, error.what());
         success = false;
      }
   }
   return success;
}

//--------------------------------------------------------------
//! Indicates if the class inherits from the provided class type.
//!
//! @param aClassNameId The base class type.
//! @return Returns true if the class inherits from the provided
//!         class type.
//--------------------------------------------------------------

bool UtScriptClass::IsOfType(UtStringId aClassNameId) const
{
   bool success = false;

   auto iter = find(mClassNameHierachy.begin(), mClassNameHierachy.end(), aClassNameId);
   if (iter != mClassNameHierachy.end())
   {
      success = true;
   }
   return success;
}

//--------------------------------------------------------------
//! Indicates if the class can be implicitly cast to the
//! provided class type.
//!
//! @param aClassNameId The type to implicitly cast to.
//! @return Returns true if the class can be implicitly cast
//!         to the provided type.
//--------------------------------------------------------------

bool UtScriptClass::CanImplicitCast(UtStringId aClassNameId) const
{
   bool success = false;

   // Non-basic types are implicitly castable to/from null
   if (aClassNameId == UtStringIdLiteral("null"))
   {
      success = !mBasicType;
   }
   else if (mClassName == UtStringIdLiteral("null"))
   {
      UtScriptClass* typePtr = mScriptTypesPtr->GetClass(aClassNameId);
      if (typePtr != nullptr)
      {
         success = (!typePtr->IsBasicType());
      }
   }
   else
   {
      auto iter = find(mImplicitCastTypes.begin(), mImplicitCastTypes.end(), aClassNameId);
      if (iter != mImplicitCastTypes.end())
      {
         success = true;
      }
   }
   return success;
}

//--------------------------------------------------------------
//! Indicates if the class can be explicitly cast to the
//! provided class type.
//!
//! @param aClassNameId The type to explicitly cast to.
//! @return Returns true if the class can be explicitly cast
//!         to the provided type.
//--------------------------------------------------------------

bool UtScriptClass::CanExplicitCast(UtStringId aClassNameId) const
{
   bool success = false;

   auto iter = find(mExplicitCastTypes.begin(), mExplicitCastTypes.end(), aClassNameId);
   if (iter != mExplicitCastTypes.end())
   {
      success = true;
   }
   return success;
}

void UtScriptClass::FindMethods(UtStringId aName, std::vector<UtScriptClass::InterfaceMethod*>& aMethods)
{
   auto i = std::lower_bound(mSortedMethods.begin(), mSortedMethods.end(), aName, MethodPtrCompare());
   while (i != mSortedMethods.end() && (*i)->mName == aName)
   {
      aMethods.push_back(*i);
      ++i;
   }
}

void UtScriptClass::FindMethods(UtStringId aName, size_t aArgCount, std::vector<InterfaceMethod*>& aMethods)
{
   auto i = std::lower_bound(mSortedMethods.begin(), mSortedMethods.end(), aName, MethodPtrCompare());
   while (i != mSortedMethods.end() && (*i)->mName == aName)
   {
      if ((*i)->GetArgTypes().size() == aArgCount)
      {
         aMethods.push_back(*i);
      }
      ++i;
   }
}

//! return a InterfaceMethod if there only exists one method matching the name and argument count.
//! otherwise, return nullptr
UtScriptClass::InterfaceMethod* UtScriptClass::FindUniqueMethod(UtStringId aName, size_t aArgCount)
{
   auto i = std::lower_bound(mSortedMethods.begin(), mSortedMethods.end(), aName, MethodPtrCompare());

   // count the number of methods with the same name and argcount
   int         sameArgCount = 0;
   decltype(i) found;
   while (i != mSortedMethods.end() && (*i)->mName == aName)
   {
      if ((*i)->GetArgTypes().size() == aArgCount)
      {
         ++sameArgCount;
         found = i;
      }
      ++i;
   };

   // either multiple or no methods were found with the same name and arg count.
   if (sameArgCount != 1)
   {
      return nullptr;
   }

   return *found;
}

//! Returns the template argument type of the container key.
//! Returns if none exists.
//! Note: this is only valid for container types (Call IsContainer to verify)
UtStringId UtScriptClass::GetContainerKeyTypeId() const
{
   UtStringId typeId;

   if (!mTemplateArgTypes.empty())
   {
      typeId = mTemplateArgTypes[0].mTypeId;
   }
   return typeId;
}

//! Returns the template argument class type of the container key.
//! Returns if none exists.
//! Note: this is only valid for container types (Call IsContainer to verify)
UtScriptClass* UtScriptClass::GetContainerKeyType() const
{
   assert(!mTemplateArgTypes.empty());
   return mTemplateArgTypes[0].mClassType;
}

//! Sets the template argument type of the container key.
//! Note: this is only valid for container types (Call IsContainer to verify)
void UtScriptClass::SetContainerKeyTypeId(UtStringId aKeyTypeId)
{
   if (mTemplateArgTypes.size() < 2)
   {
      mTemplateArgTypes.resize(2);
   }
   mTemplateArgTypes[0].mTypeId = aKeyTypeId;
   // disallow changing the KeyType after the UtScriptClass has been initialized.
   assert(mTemplateArgTypes[0].mClassType == nullptr);
}

//! Returns the template argument type of the container data.
//! Note: this is only valid for container types (Call IsContainer to verify)
UtStringId UtScriptClass::GetContainerDataTypeId() const
{
   UtStringId typeId;

   if (mTemplateArgTypes.size() > 1)
   {
      typeId = mTemplateArgTypes[1].mTypeId;
   }
   return typeId;
}

//! Returns the template argument class type of the container data.
//! Note: this is only valid for container types (Call IsContainer to verify)
UtScriptClass* UtScriptClass::GetContainerDataType() const
{
   assert(mTemplateArgTypes.size() > 1);
   return mTemplateArgTypes[1].mClassType;
}


//! Sets the template argument type of the container data.
//! Note: this is only valid for container types (Call IsContainer to verify)
void UtScriptClass::SetContainerDataTypeId(UtStringId aDataTypeId)
{
   if (mTemplateArgTypes.size() < 2)
   {
      mTemplateArgTypes.resize(2);
   }
   mTemplateArgTypes[1].mTypeId = aDataTypeId;
   // disallow changing the DataType after the UtScriptClass has been initialized.
   assert(mTemplateArgTypes[1].mClassType == nullptr);
}

//--------------------------------------------------------------
//! Returns the method id if the method is valid, otherwise -1.
//--------------------------------------------------------------

int UtScriptClass::GetMethodIndex(UtStringId aMethodName)
{
   int methodIndex = -1;

   for (unsigned int ii = 0; ii < mMethodList.size(); ++ii)
   {
      if (aMethodName == mMethodList[ii]->GetName())
      {
         methodIndex = ii;
         break;
      }
   }
   return methodIndex;
}

//--------------------------------------------------------------
//! Returns the method by index, or null if out of range.
//--------------------------------------------------------------
UtScriptClass::InterfaceMethod* UtScriptClass::GetMethodEntry(size_t aMethodIndex)
{
   if (aMethodIndex >= mMethodList.size())
   {
      return nullptr;
   }
   return mMethodList[aMethodIndex].get();
}

bool UtScriptClass::HasStaticMethod(UtStringId aMethodName)
{
   for (const auto& i : mMethodList)
   {
      if (i->GetName() == aMethodName)
      {
         return true;
      }
   }
   return false;
}

//--------------------------------------------------------------
//! Registers the method with this UtScriptClass, making it
//! available to be invoked in script.
//! Note: the InterfaceMethod's memory belongs to the base class
//--------------------------------------------------------------
// protected
bool UtScriptClass::AddMethod(std::unique_ptr<InterfaceMethod> aInterfaceMethodPtr)
{
   bool success                    = false;
   aInterfaceMethodPtr->mParentPtr = this;

   bool found = false;
   // Derived classes override the base classes method.
   MethodList methods;
   FindMethods(aInterfaceMethodPtr->GetName(), methods);
   for (auto& method : methods)
   {
      // Make sure the signature matches.
      size_t numArgs = aInterfaceMethodPtr->GetArgTypes().size();
      if (numArgs == method->GetArgTypes().size())
      {
         bool signatureMatches = true;
         for (size_t ii = 0; ii < numArgs; ++ii)
         {
            if (aInterfaceMethodPtr->GetArgTypes()[ii] != method->GetArgTypes()[ii])
            {
               signatureMatches = false;
               break;
            }
         }

         if (signatureMatches)
         {
            aInterfaceMethodPtr->mIndex = method->mIndex;
            auto replaceIter            = std::find_if(mMethodList.begin(),
                                            mMethodList.end(),
                                            [&](const std::unique_ptr<InterfaceMethod>& m) { return m.get() == method; });
            auto replaceSortedIter      = std::find(mSortedMethods.begin(), mSortedMethods.end(), method);
            *replaceSortedIter          = aInterfaceMethodPtr.get();
            *replaceIter                = std::move(aInterfaceMethodPtr);
            found                       = true;
            success                     = true;
            break;
         }
      }
   }

   // If the method doesn't already exist, then add it.
   if (!found)
   {
      // false-positive warning
      // NOLINTNEXTLINE(bugprone-use-after-move)
      aInterfaceMethodPtr->mIndex = mMethodList.size();
      mSortedMethods.insert(
         std::lower_bound(mSortedMethods.begin(), mSortedMethods.end(), aInterfaceMethodPtr.get(), MethodPtrCompare()),
         aInterfaceMethodPtr.get());
      mMethodList.push_back(std::move(aInterfaceMethodPtr));
      success = true;
   }

   return success;
}

//--------------------------------------------------------------
//! Registers the method with this UtScriptClass, making it
//! available to be invoked in script. In this case it is
//! registered as static methods, which allows the method to be
//! invoked from either a reference or type (i.e. MyType.StaticFoo()
//! or MyType m; m.StaticFoo()).
//! Note: the InterfaceMethod's memory belongs to the base class
//--------------------------------------------------------------
// protected
bool UtScriptClass::AddStaticMethod(std::unique_ptr<InterfaceMethod> aInterfaceMethodPtr)
{
   aInterfaceMethodPtr->SetIsStatic(true);
   return AddMethod(std::move(aInterfaceMethodPtr));
}

//--------------------------------------------------------------
//! All base classes should call this method from their constructor
//! to add themselves to the inheritance hierarchy, which is used in
//! casting decisions.
//--------------------------------------------------------------
// protected
void UtScriptClass::SetClassName(UtStringId aClassName)
{
   mClassNameHierachy.push_back(aClassName);
}

//--------------------------------------------------------------
//! Register a class type as a type that this class can be
//! implicitly cast to.  This is used by the compiler to make
//! casting decisions.
//--------------------------------------------------------------
// protected
void UtScriptClass::AddImplicitCastType(UtStringId aClassName)
{
   if (!CanImplicitCast(aClassName))
   {
      mImplicitCastTypes.push_back(aClassName);
   }
}

//--------------------------------------------------------------
//! Register a class type as a type that this class can be
//! explicitly cast to.  This is used by the compiler to make
//! casting decisions.
//--------------------------------------------------------------
// protected
void UtScriptClass::AddExplicitCastType(UtStringId aClassName)
{
   if (!CanExplicitCast(aClassName))
   {
      mExplicitCastTypes.push_back(aClassName);
   }
}

//--------------------------------------------------------------
//! @note ---- DEPRECATED USE SetClassName -----
//! All base classes should call this method from their constructor
//! to add themselves to the inheritance hierarchy, which is used in
//! casting decisions.
//--------------------------------------------------------------
// protected
void UtScriptClass::AddClassName(UtStringId aClassName)
{
   mClassNameHierachy.push_back(aClassName);
}

UtScriptClass::InterfaceMethod* UtScriptClass::FindMethod(UtStringId                         aMethodName,
                                                          const std::vector<UtScriptClass*>& aArgTypes)
{
   UtScriptClass::InterfaceMethod* varArgMethodPtr = nullptr;
   std::vector<InterfaceMethod*>   methods;
   FindMethods(aMethodName, methods);
   for (InterfaceMethod* mPtr : methods)
   {
      // variadic methods allow for an arbitrary number of arguments
      // to be specified but still require the initial arguements
      // to match the signature of the method except the last argument
      // in its signature which dictates what type the argument must be.
      const auto& methodArgs = mPtr->GetArgList();
      if (mPtr->IsVarArg())
      {
         bool isMatch = (methodArgs.size() <= aArgTypes.size() + 1);
         for (size_t a = 0; a < methodArgs.size() - 1; ++a)
         {
            UtScriptClass* argType      = aArgTypes[a];
            UtScriptClass* expectedType = a < methodArgs.size() ? methodArgs[a] : methodArgs.back();
            if (!expectedType->IsCompatibleType(argType))
            {
               isMatch = false;
               break;
            }
         }
         if (isMatch)
         {
            // don't return variadic methods immediately
            // in case a better non-variadic match is found.
            varArgMethodPtr = mPtr;
         }
      }
      if (methodArgs.size() == aArgTypes.size())
      {
         bool isMatch = true;
         for (size_t a = 0; a < methodArgs.size(); ++a)
         {
            UtScriptClass* argType = aArgTypes[a];
            if (!methodArgs[a]->IsCompatibleType(argType))
            {
               isMatch = false;
               break;
            }
         }

         if (isMatch)
         {
            return mPtr;
         }
      }
   }
   return varArgMethodPtr;
}

bool UtScriptClass::IsCompatibleType(UtScriptClass* aFromTypePtr)
{
   bool compatible = false;

   if (aFromTypePtr != nullptr)
   {
      compatible = aFromTypePtr->IsOfType(mClassName);
      if (!compatible)
      {
         compatible = aFromTypePtr->CanImplicitCast(mClassName);
      }
   }
   return compatible;
}

bool UtScriptClass::Initialize()
{
   bool ok = true;

   // Check parent classes for methods added after script class construction
   // and add them if the script class is derived from another scripting class.
   if (!mContainer && (mClassNameHierachy.size() > 2))
   {
      for (auto& classIndex : mClassNameHierachy)
      {
         auto&                               baseName = classIndex.GetString();
         UtScriptTypes::ScriptExtensionList& extList  = mScriptTypesPtr->GetScriptExtensions();
         for (auto& extindex : extList)
         {
            extindex->AddExtClassMethods(mClassName, baseName, mScriptTypesPtr);
         }
      }

      // Extensions may have added methods. Method indexing needs to match parents and children.
      for (size_t classIndex = 1; classIndex < mClassNameHierachy.size() - 1; ++classIndex)
      {
         UtScriptClass* parentClassPtr = mScriptTypesPtr->GetClass(mClassNameHierachy[classIndex]);
         if (parentClassPtr != nullptr)
         {
            // Loop through all the methods in the parent class
            for (size_t methodIndex = 0; methodIndex < parentClassPtr->GetMethodCount(); ++methodIndex)
            {
               // Get a pointer to the method
               InterfaceMethod* parentMethodPtr = parentClassPtr->GetMethodEntry(methodIndex);

               // See if there is a match in the child class
               bool       found = false;
               MethodList methods;
               FindMethods(parentMethodPtr->GetName(), parentMethodPtr->GetArgTypes().size(), methods);
               for (auto& method : methods)
               {
                  // Make sure the signature matches.
                  size_t numArgs          = parentMethodPtr->GetArgTypes().size();
                  bool   signatureMatches = true;
                  for (size_t ii = 0; ii < numArgs; ++ii)
                  {
                     if (parentMethodPtr->GetArgTypes()[ii] != method->GetArgTypes()[ii])
                     {
                        signatureMatches = false;
                        break;
                     }
                  }

                  // Found a match so check that the index of the
                  // parent and the child class are the same
                  if (signatureMatches)
                  {
                     // If child and parent indices don't match, move child
                     // method to parent index and update mIndex
                     if (method->mIndex != parentMethodPtr->mIndex)
                     {
                        size_t temp1 = method->mIndex;
                        size_t temp2 = parentMethodPtr->mIndex;
                        std::swap(mMethodList[temp1], mMethodList[temp2]);
                        mMethodList[temp1]->mIndex = temp1;
                        mMethodList[temp2]->mIndex = temp2;
                     }

                     found = true;
                     break;
                  }
               }

               // If the method doesn't already exist, then issue an error.
               if (!found)
               {
                  auto out = ut::log::error() << "Could not find script class method for derived class.";
                  out.AddNote() << "Parent Class: " << parentMethodPtr->GetParent()->GetClassName();
                  out.AddNote() << "Derived Class: " << mClassName;
                  out.AddNote() << "Method: " << parentMethodPtr->GetName();
                  ok = false;
               }
               assert(found);
            }
         }
      }
   }
   for (TemplateArg& targ : mTemplateArgTypes)
   {
      targ.mClassType = GetClass(targ.mTypeId);
      if (targ.mClassType == nullptr)
      {
         targ.mClassType = GetOrCreateContainer(targ.mTypeId);
      }
      if (targ.mClassType == nullptr)
      {
         { // RAII block
            auto out = ut::log::error() << "Could not find class for template parameter of script class.";
            out.AddNote() << "Script Class: " << mClassName;
            out.AddNote() << "Missing Template Parameter: " << targ.mTypeId;
         }
         assert(false);
      }
   }

   for (size_t i = 0; i < mMethodList.size(); ++i)
   {
      // Throw an assert if list index doesn't match the method index.
      // This difference will cause issues later.
      // Generally caused by a base classes not being initialized first.
      if (i != mMethodList[i]->GetIndex())
      {
         auto out = ut::log::error() << "Indexing is incorrect for script class method.";
         out.AddNote() << "Class: " << mClassName;
         out.AddNote() << "Method: " << mMethodList[i]->GetName();
         out.AddNote() << "List Index: " << i;
         out.AddNote() << "Method Index: " << mMethodList[i]->GetIndex();
         assert(false);
         ok = false;
      }

      ok &= mMethodList[i]->Initialize();
   }
   return ok;
}

//--------------------------------------------------------------
//! Gets an attribute from a script object
//! For script objects which contain a script instance,
//! this returns the variable with the specified name.
//--------------------------------------------------------------

UtScriptData* UtScriptClass::GetAttribute(void* aObjectPtr, UtStringId aName)
{
   UtScriptContext* instancePtr = GetContext(aObjectPtr);
   if (instancePtr != nullptr)
   {
      return instancePtr->FindVar(aName);
   }
   return nullptr;
}

int UtScriptClass::GetAttributeCount(void* aObjectPtr)
{
   UtScriptContext* instancePtr = GetContext(aObjectPtr);
   if (instancePtr != nullptr)
   {
      return static_cast<int>(instancePtr->GetVariables().size());
   }
   return 0;
}

//--------------------------------------------------------------
//! Gets the list of attribute names on this object.
//--------------------------------------------------------------
void UtScriptClass::GetAttributeList(void* aObjectPtr, std::vector<std::string>& aAttributeNameList)
{
   UtScriptContext* instancePtr = GetContext(aObjectPtr);
   if (instancePtr != nullptr)
   {
      for (auto& i : instancePtr->GetVariables())
      {
         aAttributeNameList.push_back(i.first);
      }
   }
}

//--------------------------------------------------------------
//! Gets the list of attribute names on this object.
//--------------------------------------------------------------
UtStringId UtScriptClass::GetAttributeNameAt(void* aObjectPtr, int aAttributeIndex)
{
   UtScriptContext* instancePtr = GetContext(aObjectPtr);
   if (instancePtr != nullptr)
   {
      int idx = aAttributeIndex;
      for (auto i = instancePtr->GetVariables().begin(); i != instancePtr->GetVariables().end(); ++i, --idx)
      {
         if (idx == 0)
         {
            return i->first;
         }
      }
   }
   return {};
}

//--------------------------------------------------------------
//! Sets an attribute on a script object
//! Returns 'true' if successful
//--------------------------------------------------------------

bool UtScriptClass::SetAttribute(void* aObjectPtr, UtStringId aName, const UtScriptData& aValue)
{
   bool             ok          = false;
   UtScriptContext* instancePtr = GetContext(aObjectPtr);
   if (instancePtr != nullptr)
   {
      UtScriptData& var = instancePtr->Var(aName);
      var               = aValue;
      ok                = true;
   }
   return ok;
}

//--------------------------------------------------------------
//! Calls a script on this script object
//! Returns 'true' if successful
//--------------------------------------------------------------

bool UtScriptClass::CallDynamicInternal(void*              aObjectPtr,
                                        UtScriptExecutor*  aExecutorPtr,
                                        UtStringId         aScriptName,
                                        UtScriptMethodArgs aVarArgs,
                                        UtScriptData&      aReturnVal)
{
   // Attempt to call a user's script
   UtScriptContext* instancePtr = GetContext(aObjectPtr);
   if (instancePtr != nullptr)
   {
      if (instancePtr->ExecuteInternal(aExecutorPtr, aScriptName, aReturnVal, aVarArgs.size()))
      {
         return true;
      }
   }

   // Otherwise, try to call an application method
   // No type checking is done here, just pick a method that has the right number of arguments
   // if more than one method exists, don't try to guess
   InterfaceMethod* method = FindUniqueMethod(aScriptName, aVarArgs.size());
   if (method != nullptr)
   {
      UtScriptContext* contextPtr = aExecutorPtr->GetTopContext();
      if (contextPtr == nullptr)
      {
         contextPtr = GetContext(aObjectPtr);
      }
      if (contextPtr != nullptr)
      {
         UtScriptRef ref(aObjectPtr, this, UtScriptRef::cDONT_MANAGE);
         try
         {
            (*method)(aExecutorPtr, *contextPtr, ref, aVarArgs, aReturnVal);
         }
         catch (const UtScriptException& error)
         {
            method->ReportCallErrors(aExecutorPtr, nullptr, nullptr, aReturnVal, error.what());
            return false;
         }
         return true;
      }
   }
   return false;
}

//--------------------------------------------------------------
//! Returns 'true' if this script object has a script with the specified name.
//--------------------------------------------------------------
bool UtScriptClass::HasScript(void* aObjectPtr, UtStringId aScriptName)
{
   UtScriptContext* instancePtr = GetContext(aObjectPtr);
   if (instancePtr != nullptr)
   {
      return instancePtr->FindScript(aScriptName) != nullptr;
   }
   return false;
}

bool UtScriptClass::UpcastFromType(UtScriptRef* aRefPtr)
{
   void* appObjectPtr = aRefPtr->GetAppObject();
   if (appObjectPtr != nullptr)
   {
      UtScriptClass* classPtr = aRefPtr->GetScriptClass();
      if (classPtr != nullptr)
      {
         if (classPtr->mIsScriptAccessible)
         {
            UtScriptClass* actualClassPtr = classPtr->GetTypes()->GetClass((UtScriptAccessible*)appObjectPtr);
            assert(actualClassPtr != nullptr); // null indicates GetScriptClassName() returned a bad value -- update code
            if (actualClassPtr != nullptr)
            {
               if (actualClassPtr->IsOfType(mClassName))
               {
                  aRefPtr->SetScriptClass(actualClassPtr);
                  return true;
               }
            }
         }
         // cast failure, set to null reference
         *aRefPtr = UtScriptRef(nullptr, this);
      }
   }
   return false;
}

UtScriptContext* UtScriptClass::GetContext(void* aObjectPtr)
{
   if (mIsScriptAccessible && (aObjectPtr != nullptr))
   {
      auto* scriptAccessiblePtr = static_cast<UtScriptAccessible*>(aObjectPtr);
      return scriptAccessiblePtr->GetScriptAccessibleContext();
   }
   return nullptr;
}

//! Assign the default value to 'aData'.
//! For non-basic types, this is usually a null reference.
//! For basic types, this is either an empty string, 0 or 0.0
void UtScriptClass::SetDefaultValue(UtScriptData& aData)
{
   aData.SetPointer(new UtScriptRef(nullptr, this, UtScriptRef::cMANAGE));
}

// virtual
int UtScriptClass::EnumerateSize(void* aObjectPtr)
{
   return 0;
}

// virtual
UtScriptData UtScriptClass::Enumerate(void* aObjectPtr, int aIndex)
{
   return UtScriptData();
}

//! Report that an exception has occurred.
//! This is called from within an interface method when it determines something is amiss.
//! @param aPrintFullStack true if the entire stack is to be printed, or just the current level.
//! @param aExceptionDescription A string describing the failure.
void UtScriptClass::ScriptException(bool aPrintFullStack, const std::string& aExceptionDescription)
{
   ScriptException(nullptr, aPrintFullStack, aExceptionDescription);
}

//! Report that an exception has occurred.
//! This is called from within an interface method when it determines something is amiss.
//! @param aExecutorPtr Pointer to the executor. If null then UtScriptExecutor::GetCurrentExecutor will be used.
//! @param aPrintFullStack true if the entire stack is to be printed, or just the current level.
//! @param aExceptionDescription A string describing the failure.
void UtScriptClass::ScriptException(UtScriptExecutor*  aExecutorPtr,
                                    bool               aPrintFullStack,
                                    const std::string& aExceptionDescription)
{
   UtScriptExecutor* executorPtr = aExecutorPtr;
   if (executorPtr == nullptr)
   {
      executorPtr = UtScriptExecutor::GetCurrentExecutor();
   }
   if (executorPtr != nullptr)
   {
      executorPtr->ScriptException(aPrintFullStack, aExceptionDescription);
   }
   else
   {
      ut::log::error("script") << aExceptionDescription;
   }
}

//! Locate a class given a class ID.
//! Invoke GetClass in either UtScriptTypes or UtScriptEnvironment, depending on who owns
//! the class. The former owns built-in types while the later owns types that are defined in input
//! files (script_struct, or containers where a script_struct type is a template argument).
// protected
UtScriptClass* UtScriptClass::GetClass(UtStringId aClassNameId) const
{
   UtScriptClass* classPtr = nullptr;
   if (mEnvironmentPtr == nullptr)
   {
      classPtr = mScriptTypesPtr->GetClass(aClassNameId); // Doesn't involve a script_struct
   }
   else
   {
      classPtr = mEnvironmentPtr->GetClass(aClassNameId);
   }
   return classPtr;
}

//! Locate or create a container.
//! Invoke GetOrCreateContainer in either UtScriptTypes or UtScriptEnvironment, depending on who owns
//! the class. The former owns built-in types while the later owns types that are defined in input
//! files (script_struct, or containers where a script_struct type is a template argument).
// protected
UtScriptClass* UtScriptClass::GetOrCreateContainer(const std::string& aContainerType)
{
   UtScriptClass* classPtr = nullptr;
   if (mEnvironmentPtr == nullptr)
   {
      classPtr = mScriptTypesPtr->GetOrCreateContainer(aContainerType); // Doesn't involve a script_struct
   }
   else
   {
      classPtr = mEnvironmentPtr->GetOrCreateContainer(aContainerType);
   }
   return classPtr;
}

//--------------------------------------------------------------
//! Nested class InterfaceMethod constructor.
//! aName is the method name.
//! aReturnType is a string that represents a valid type (void
//! is the default).
//! aArgTypes is a list of types separated by commas that define
//! each of the method's argument types.
//--------------------------------------------------------------

UtScriptClass::InterfaceMethod::InterfaceMethod(UtStringId         aName,
                                                UtStringId         aReturnType,
                                                const std::string& aArgTypes,
                                                int                aArgCount)
   : mParentPtr(nullptr)
   , mReturnClassPtr(nullptr)
   , mIndex(ut::script::npos)
   , mName(aName)
   , mIsStatic(false)
   , mHasReturnVal(true)
   , mIsVarArg(aArgCount == -1)
   , mReturnTypeId(aReturnType)
{
   if (aReturnType == UtStringIdLiteral("void"))
   {
      mHasReturnVal = false;
   }

   std::string argTypes;
   for (const char aArgType : aArgTypes)
   {
      if (aArgType != ' ')
      {
         argTypes += aArgType;
      }
   }

   size_t offset          = 0;
   size_t i               = 0;
   int    templateNesting = 0;
   while (i < argTypes.size())
   {
      char c = argTypes[i];
      if (c == '<')
      {
         ++templateNesting;
      }
      else if (c == '>')
      {
         --templateNesting;
      }
      else if (c == ',' && templateNesting == 0)
      {
         mArgTypes.emplace_back(argTypes.substr(offset, i - offset));
         offset = i + 1;
      }
      ++i;
   }
   if (i != offset)
   {
      mArgTypes.emplace_back(argTypes.substr(offset, i - offset));
   }

   if (aArgCount >= 0 && aArgCount != static_cast<int>(mArgTypes.size()))
   {
      { // RAII block
         auto out = ut::log::error() << "Script method has the wrong number of arguments.";
         out.AddNote() << "Method: " << aName;
         out.AddNote() << "Args Expected: " << mArgTypes.size();
         out.AddNote() << "Args Received: " << aArgCount;
      }
      assert(false);
   }
}

bool UtScriptClass::InterfaceMethod::Initialize()
{
   mArgTypeList.resize(mArgTypes.size());
   for (size_t i = 0; i < mArgTypes.size(); ++i)
   {
      mArgTypeList[i] = mParentPtr->GetClass(mArgTypes[i]);
      if (mArgTypeList[i] == nullptr)
      {
         mArgTypeList[i] = mParentPtr->GetOrCreateContainer(mArgTypes[i]);
      }
      if (mArgTypeList[i] == nullptr)
      {
         { // RAII block
            auto out = ut::log::error() << "Script class does not exist for type.";
            out.AddNote() << "Parent Class: " << mParentPtr->GetClassName();
            out.AddNote() << "Method: " << GetName();
            out.AddNote() << "Type: " << mArgTypes[i];
            out.AddNote() << "Arg: " << i;
         }
         assert(false);
         return false;
      }
   }

   UpdateReturnClass();
   if (mHasReturnVal && mReturnClassPtr == nullptr)
   {
      { // RAII block
         auto out = ut::log::error() << "Script class does not exist for return type.";
         out.AddNote() << "Parent Class: " << mParentPtr->GetClassName();
         out.AddNote() << "Method: " << GetName();
         out.AddNote() << "Type: " << mReturnTypeId;
      }
      assert(false);
      return false;
   }
   return true;
}

void UtScriptClass::InterfaceMethod::ReportCallErrors(UtScriptExecutor*         aExecutorPtr,
                                                      const UtScriptRef*        aReferencePtr,
                                                      const UtScriptMethodArgs* aVarArgs,
                                                      UtScriptData&             aReturnVal,
                                                      const std::string&        aErrorMessage)
{
   UtScriptClass*     returnClassPtr = GetReturnClass();
   const UtStringId   className      = mParentPtr->GetClassName();
   const std::string& scriptName     = mName;
   std::stringstream  ss;
   if (aErrorMessage.empty())
   {
      if (!mIsStatic && aReferencePtr != nullptr && !aReferencePtr->IsValid())
      {
         ss << "Null object reference ";
      }
      if (aVarArgs != nullptr)
      {
         for (size_t i = 0; i < aVarArgs->size(); ++i)
         {
            const UtScriptData& arg = (*aVarArgs)[i];
            if ((arg.GetType() == ut::script::Data::Type::cPOINTER) && (arg.GetPointer()->GetAppObject() == nullptr))
            {
               if (i < mArgTypes.size())
               {
                  const std::string& argType = mArgTypes[i];
                  ss << "Null " << argType << " reference for parameter " << i << ' ';
               }
               else
               {
                  ss << "Null reference for parameter " << i << ' ';
               }
            }
         }
      }
   }
   else
   {
      ss << aErrorMessage << ' ';
   }

   bool isAssert = UtStringUtil::StartsWith(aErrorMessage, "Assert Failed");
   if (!isAssert)
   {
      ss << "- Calling method '";
      if (className != UtStringIdLiteral("__BUILTIN__"))
      {
         ss << className << '.';
      }
      ss << scriptName << '\'';
   }
   ScriptException(aExecutorPtr, false, ss.str());

   // support legacy behavior on which flag causes abortions from a script exception.
   // Ideally we should always abort and not support disabling abortion, as it only causes more errors.
   if ((!isAssert && sAbortOnScriptCallError) || (isAssert && UtScriptCore::sAbortOnAssert))
   {
      throw UtException(ss.str());
   }

   std::string returnType = mReturnTypeId;
   if (!returnType.empty() && returnType != "void")
   {
      if (returnType == "double")
      {
         aReturnVal.SetDouble(0.0);
      }
      else if (returnType == "bool")
      {
         aReturnVal.SetBool(false);
      }
      else if (returnType == "int")
      {
         aReturnVal.SetInt(0);
      }
      else if (returnType == "string")
      {
         aReturnVal.SetNullString();
      }
      else if (returnType == "char")
      {
         aReturnVal.SetNullString();
      }
      else if (returnClassPtr != nullptr)
      {
         aReturnVal.SetPointer(new UtScriptRef(nullptr, returnClassPtr));
      }
   }
}

void UtScriptClass::InterfaceMethod::UpdateReturnClass()
{
   if (mReturnClassPtr != nullptr || !mHasReturnVal)
   {
      return;
   }

   mReturnClassPtr = mParentPtr->GetClass(mReturnTypeId);
   // if the return type isn't in the class dictionary, we may need to create the container
   if (mReturnClassPtr == nullptr)
   {
      std::string typeName = mReturnTypeId;
      if (typeName.find('<') != std::string::npos)
      {
         mReturnClassPtr = mParentPtr->GetOrCreateContainer(typeName);
      }
   }
}

void UtScriptClass::InterfaceMethod::SetArgTypes(const ArgTypes& aArgTypes)
{
   mArgTypes = aArgTypes;
}

//! Returns a script function object that describes this method.
//! (Used by the script parser to display available method signatures)
UtScriptFunction UtScriptClass::InterfaceMethod::GetScriptFunction() const
{
   UtScriptFunction fn;
   fn.mFlags = UtScriptFunction::cAPP_METHOD;
   if (mIsStatic)
   {
      fn.mFlags |= UtScriptFunction::cSTATIC;
   }
   fn.mArgs           = mArgTypes;
   fn.mReturnClassPtr = mReturnClassPtr;
   fn.mReturnType     = mReturnTypeId;
   return fn;
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(UtScriptClass, UtScriptClass, IsValid, 0, "bool", "")
{
   aReturnVal.SetBool(aReference.IsValid());
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(UtScriptClass, UtScriptClass, IsNull1, 0, "bool", "")
{
   aReturnVal.SetBool(!aReference.IsValid());
}

UT_DEFINE_SCRIPT_METHOD(UtScriptClass, UtScriptClass, ConvertToString, 0, "string", "")
{
   aReturnVal.SetString(aObjectClassPtr->ToString(aObjectPtr));
}
