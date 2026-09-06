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

#ifndef UTSCRIPTCLASS_HPP
#define UTSCRIPTCLASS_HPP

#include "util_script_export.h"

#include <memory>
#include <string>
#include <vector>

class UtScriptContext;
namespace ut
{
namespace script
{
class Data;
}
} // namespace ut
using UtScriptData = ut::script::Data;
class UtScriptEnvironment;
class UtScriptExecutor;
class UtScriptFunction;
class UtScriptRef;
class UtScriptTypes;
#include "UtSpan.hpp"
#include "UtStringId.hpp"

using UtScriptMethodArgs = ut::span<const UtScriptData>;

//--------------------------------------------------------------
//! UtScriptClass provides an abstraction for classes that are
//! defined in application space.  Each method that the
//! application programmer wants to export to script for a
//! particular class is registered with a derived version of
//! this class. The compiler uses this class to make sure method
//! calls are valid.
//--------------------------------------------------------------

class UTIL_SCRIPT_EXPORT UtScriptClass
{
public:
   class InterfaceMethod; // Nested class.

   friend class InterfaceMethod;

   //! Define as set of typedefs to provide access to the methods.
   using MethodList         = std::vector<InterfaceMethod*>;
   using MethodListItr      = std::vector<InterfaceMethod*>::iterator;
   using ConstMethodListItr = std::vector<InterfaceMethod*>::const_iterator;

   struct TemplateArg
   {
      TemplateArg(UtStringId aTypeId)
         : mTypeId(aTypeId)
      {
      }
      TemplateArg() = default;

      UtStringId     mTypeId;
      UtScriptClass* mClassType = nullptr;
   };
   //! Define some typedefs to provide access to the template argument types.
   using TemplateArgTypes = std::vector<TemplateArg>;

   //! Define some typedefs to provide access to the argument types.
   using ArgTypes = std::vector<UtStringId>;

   UtScriptClass(UtStringId              aClassName,
                 UtScriptTypes*          aScriptTypesPtr,
                 const TemplateArgTypes& aTemplateArgTypes = {},
                 bool                    aBasicType        = false);

   UtScriptClass(const UtScriptClass&) = delete; // Disable Copying
   UtScriptClass(UtScriptClass&&)      = default;
   UtScriptClass& operator=(const UtScriptClass&) = delete;
   UtScriptClass& operator=(UtScriptClass&&) = default;

   virtual ~UtScriptClass() = default;

   bool AddMethod(std::unique_ptr<InterfaceMethod> aInterfaceMethodPtr);
   bool AddStaticMethod(std::unique_ptr<InterfaceMethod> aInterfaceMethodPtr);

   virtual bool Initialize();

   bool Call(UtScriptExecutor*  aExecutorPtr,
             UtScriptContext&   aContext,
             const UtScriptRef& aReference,
             size_t             aMethodIndex,
             UtScriptMethodArgs aVarArgs,
             bool&              aHasReturnVal,
             UtScriptData&      aReturnVal);

   //! Provides access to the methods of the class.
   std::vector<std::unique_ptr<InterfaceMethod>>::const_iterator Begin() const { return mMethodList.begin(); }
   std::vector<std::unique_ptr<InterfaceMethod>>::const_iterator End() const { return mMethodList.end(); }

   const TemplateArgTypes& GetTemplateArgTypes() const { return mTemplateArgTypes; }

   const std::vector<UtStringId>& GetClassNameHierachy() const { return mClassNameHierachy; }
   const std::vector<UtStringId>& GetImplicitCastTypes() const { return mImplicitCastTypes; }
   const std::vector<UtStringId>& GetExplicitCastTypes() const { return mExplicitCastTypes; }

   void FindMethods(UtStringId aName, std::vector<InterfaceMethod*>& aMethods);

   void FindMethods(UtStringId aName, size_t aArgCount, std::vector<InterfaceMethod*>& aMethods);

   //! Finds a method with the given name and argCount.
   //! if multiple methods match, or if no methods match, returns nullptr.
   InterfaceMethod* FindUniqueMethod(UtStringId aName, size_t aArgCount);

   //! Returns the template argument type of the container key.
   //! Note: this is only valid for container types (Call IsContainer to verify)
   UtStringId GetContainerKeyTypeId() const;

   //! Returns the template argument class type of the container key.
   //! Note: this is only valid for container types (Call IsContainer to verify)
   UtScriptClass* GetContainerKeyType() const;

   //! Sets the template argument type of the container key.
   //! Note: this is only valid for container types (Call IsContainer to verify)
   void SetContainerKeyTypeId(UtStringId aKeyTypeId);

   //! Returns the template argument type of the container data.
   //! Note: this is only valid for container types (Call IsContainer to verify)
   UtStringId GetContainerDataTypeId() const;

   //! Returns the template argument class type of the container data.
   //! Note: this is only valid for container types (Call IsContainer to verify)
   UtScriptClass* GetContainerDataType() const;

   //! Sets the template argument type of the container data.
   //! Note: this is only valid for container types (Call IsContainer to verify)
   void SetContainerDataTypeId(UtStringId aDataTypeId);

   //! Returns true if this class can be constructed in script.
   bool IsConstructible() const { return mConstructible; }

   //! Returns true if this class can be cloned in script.
   bool IsCloneable() const { return mCloneable; }

   //! Returns true if this class is a container.
   bool IsContainer() const { return mContainer; }

   //! Returns true if this class can be used in less than comparisons.
   bool IsLessThanComparable() const { return mLessThanComparable; }

   //! Returns true if this class can be used in equality comparisons.
   bool IsEqualityComparable() const { return mEqualityComparable; }

   bool IsOfType(UtStringId aClassNameId) const;

   virtual bool CanImplicitCast(UtStringId aClassNameId) const;

   bool CanExplicitCast(UtStringId aClassNameId) const;

   bool IsCompatibleType(UtScriptClass* aFromTypePtr);

   //! Returns true if the type is a basic type (int, double, bool, string).
   bool IsBasicType() const { return mBasicType; }

   //! The UtScriptClass derived types that represent base types
   //! (int, double, bool, string) override the appropriate
   //! method to indicate their type.
   virtual bool        IsVoid() const { return false; }
   virtual bool        IsNull() const { return false; }
   virtual bool        IsInt() const { return false; }
   virtual bool        IsDouble() const { return false; }
   virtual bool        IsBool() const { return false; }
   virtual bool        IsString() const { return false; }
   virtual std::string ToString(void* /*aObjectPtr*/) const { return ""; }
   virtual std::string ToStringN(void* aObjectPtr, int aMaxStringSize) const { return ToString(aObjectPtr); }

   //! Returns the object's script context if available.
   virtual UtScriptContext* GetContext(void* /*aObjectPtr*/);

   int GetMethodIndex(UtStringId aMethodName);

   size_t GetMethodCount() const { return mMethodList.size(); }

   InterfaceMethod* GetMethodEntry(size_t aMethodIndex);
   InterfaceMethod* FindMethod(UtStringId aMethodName, const std::vector<UtScriptClass*>& aArgTypes);
   bool             HasStaticMethod(UtStringId aMethodName);

   //! Returns the class name.
   UtStringId GetClassName() const { return mClassName; }

   //! Creates and returns a default instance of the class type.
   //! Some classes may not be able to provide this feature, which
   //! means they can't be used by value in script. The memory will
   //! be managed by the caller.
   //! @param The script context, which can be used to access application variables.
   //! @return Returns the newly created application object.
   virtual void* Create(const UtScriptContext& /*aContext*/) { return nullptr; }

   //! Destroys the memory associated with objects created via 'Create'.
   //! or Clone().
   virtual void Destroy(void* /*aObject*/) {}

   //! Clones and returns the instance of the class type.
   //! Some classes may not be able to provide this feature, which
   //! means they can't be used by value in script.  The memory will
   //! be managed by the caller.
   virtual void* Clone(void* /*aObject*/) { return nullptr; }

   //! A callback that notifies the class type that a reference (UtScriptRef)
   //! was just generated.  This allows users to perform initialization that
   //! may be specific to a reference, such as setting auxiliary data.
   virtual void OnNewScriptRef(UtScriptRef& aReference) {}

   //! Override this method if you want the less than operator to
   //! compare something other than the object's memory address.
   virtual bool LessThan(void* aLHS, void* aRHS) { return std::less<void*>()(aLHS, aRHS); }

   //! Override this method if you want the equality operator to
   //! compare something other than the object's memory address.
   virtual bool EqualTo(void* aLHS, void* aRHS) { return std::equal_to<void*>()(aLHS, aRHS); }

   UtScriptData* GetAttribute(void* aObjectPtr, UtStringId aName);

   virtual void GetAttributeList(void* aObjectPtr, std::vector<std::string>& aAttributeNameList);

   virtual UtStringId GetAttributeNameAt(void* aObjectPtr, int aAttributeIndex);

   virtual int GetAttributeCount(void* aObjectPtr);

   virtual bool SetAttribute(void* aObjectPtr, UtStringId aName, const UtScriptData& aValue);

   //! This should only be called internally by the script engine.
   bool CallDynamicInternal(void*              aObjectPtr,
                            UtScriptExecutor*  aExecutorPtr,
                            UtStringId         aScriptName,
                            UtScriptMethodArgs aVarArgs,
                            UtScriptData&      aReturnVal);

   virtual bool HasScript(void* aObjectPtr, UtStringId aScriptName);

   virtual bool UpcastFromType(UtScriptRef* aRefPtr);

   virtual void SetDefaultValue(UtScriptData& aData);

   // Implementing EnumerateSize() and Enumerate() will allow inspection in the script debugger
   virtual int          EnumerateSize(void* aObjectPtr);
   virtual UtScriptData Enumerate(void* aObjectPtr, int aIndex);

   UtScriptTypes* GetTypes() const { return mScriptTypesPtr; }

   //! Returns true if the class represents a 'pseudo-class' (i.e.: a 'script_struct')
   bool IsPseudoClass() const { return mIsPseudoClass; }

   static void ScriptException(bool aPrintFullStack, const std::string& aExceptionDescription);

   static void ScriptException(UtScriptExecutor* aExecutorPtr, bool aPrintFullStack, const std::string& aExceptionDescription);

   //! Set the pointer to the owning environment for classes owned by directly by the environment.
   //! 'script_struct' classes are registered in the environment and not UtScriptTypes. UtScriptTypes
   //! is only for classes that involve compiled-in types that can be shared application-wide.
   //! 'script_struct' are user-defined and are restricted to the environment in which they
   //! are created (in WSF world this would be a scenario).
   void SetEnvironment(UtScriptEnvironment* aEnvironmentPtr) { mEnvironmentPtr = aEnvironmentPtr; }

   //--------------------------------------------------------------
   //! Nested class.
   //! InterfaceMethod provides an abstraction for the member
   //! methods of a class.  For each method that will be exported
   //! to script, an InterfaceMethod functor is created.
   //--------------------------------------------------------------
   class UTIL_SCRIPT_EXPORT InterfaceMethod
   {
   public:
      friend class UtScriptClass;

      InterfaceMethod(UtStringId aName, UtStringId aReturnType, const std::string& aArgTypes = "", int aArgCount = 0);

      virtual ~InterfaceMethod() = default;

      UtScriptClass* GetParent() const { return mParentPtr; }
      bool           Initialize();

      size_t                             GetIndex() const { return mIndex; }
      UtStringId                         GetName() const { return mName; }
      bool                               GetIsStatic() const { return mIsStatic; }
      bool                               GetHasReturnVal() const { return mHasReturnVal; }
      UtStringId                         GetReturnTypeId() const { return mReturnTypeId; }
      const ArgTypes&                    GetArgTypes() const { return mArgTypes; }
      bool                               IsVarArg() const { return mIsVarArg; }
      const std::vector<UtScriptClass*>& GetArgList() const { return mArgTypeList; }
      UtScriptClass*                     GetArgClass(size_t aIndex) const
      {
         if (mArgTypeList.size() > aIndex)
         {
            return mArgTypeList[aIndex];
         }
         return mArgTypeList.back();
      }
      void SetIsStatic(bool aIsStatic) { mIsStatic = aIsStatic; }
      void SetReturnTypeId(UtStringId aReturnTypeId) { mReturnTypeId = aReturnTypeId; }
      void SetArgTypes(const ArgTypes& aArgTypes);

      bool operator<(const InterfaceMethod& aRhs) const { return mName < aRhs.mName; }
      bool operator==(const InterfaceMethod& aMethod) const { return (mName == aMethod.mName); }

      //! Derived classes implement operator() to call to the actual
      //! application layer object by type-casting the void* and arguments
      //! to the correct type.
      virtual void operator()(UtScriptExecutor*  aExecutorPtr,
                              UtScriptContext&   aContext,
                              const UtScriptRef& aReference,
                              UtScriptMethodArgs aVarArgs,
                              UtScriptData&      aReturnVal) = 0;

      //! Aborts the script if any null references are found in the 'this' pointer or script arguments
      // This function is declared in the header due to its (supposed) high value of being inlined.
      // templates are used to further encourage this inlining, and to allow UtScriptData
      // and UsScriptMethodArgs to be forward declared.
      template<class UsScriptRef, class UsScriptMethodArgs>
      bool CheckForCallErrors(UtScriptExecutor*         aExecutorPtr,
                              const UsScriptRef&        aReference,
                              const UsScriptMethodArgs* aVarArgs,
                              UtScriptData&             aReturnVal)
      {
         bool ok = true;
         if (aVarArgs != nullptr)
         {
            for (size_t i = 0; i < aVarArgs->size(); ++i)
            {
               const auto& arg = (*aVarArgs)[i];
               if ((arg.GetType() == UsScriptMethodArgs::value_type::Type::cPOINTER) && (!arg.GetPointer()->IsValid()))
               {
                  ok = false;
               }
            }
         }
         if (!mIsStatic && !aReference.IsValid())
         {
            ok = false;
         }
         if (!ok)
         {
            ReportCallErrors(aExecutorPtr, &aReference, aVarArgs, aReturnVal);
         }
         return ok;
      }

      //! Determines if an application will abort when a script is called with invalid data.
      //! Currently checks only null 'this' references on methods.
      static void SetAbortOnScriptCallErrors(bool aDoAbort) { sAbortOnScriptCallError = aDoAbort; }

      void ReportCallErrors(UtScriptExecutor*         aExecutorPtr,
                            const UtScriptRef*        aReferencePtr,
                            const UtScriptMethodArgs* aVarArgs,
                            UtScriptData&             aReturnVal,
                            const std::string&        aErrorMessage = "");

      //! Returns a pointer to the return value's script class
      UtScriptClass* GetReturnClass() { return mReturnClassPtr; }

      UtScriptFunction GetScriptFunction() const;

   protected:
      void UpdateReturnClass();

      UtScriptClass* mParentPtr;
      UtScriptClass* mReturnClassPtr;

      size_t                      mIndex;
      UtStringId                  mName;
      bool                        mIsStatic;
      bool                        mHasReturnVal;
      bool                        mIsVarArg;
      UtStringId                  mReturnTypeId;
      ArgTypes                    mArgTypes;
      std::vector<UtScriptClass*> mArgTypeList;

      static bool sAbortOnScriptCallError;

   private:
      InterfaceMethod();
   };

   class IsValid : public InterfaceMethod
   {
   public:
      IsValid(const std::string& aName = "IsValid");
      void operator()(UtScriptExecutor*  aExecutorPtr,
                      UtScriptContext&   aContext,
                      const UtScriptRef& aReference,
                      UtScriptMethodArgs aVarArgs,
                      UtScriptData&      aReturnVal) override;
   };

   class IsNull1 : public InterfaceMethod
   {
   public:
      IsNull1(const std::string& aName = "IsNull");
      void operator()(UtScriptExecutor*  aExecutorPtr,
                      UtScriptContext&   aContext,
                      const UtScriptRef& aReference,
                      UtScriptMethodArgs aVarArgs,
                      UtScriptData&      aReturnVal) override;
   };

   class ConvertToString : public InterfaceMethod
   {
   public:
      ConvertToString(const std::string& aName = "ToString");
      void operator()(UtScriptExecutor*  aExecutorPtr,
                      UtScriptContext&   aContext,
                      const UtScriptRef& aReference,
                      UtScriptMethodArgs aVarArgs,
                      UtScriptData&      aReturnVal) override;
   };

protected:
   void SetClassName(UtStringId aClassName);

   void AddImplicitCastType(UtStringId aClassName);

   void AddExplicitCastType(UtStringId aClassName);

   void AddClassName(UtStringId aClassName);

   UtScriptClass* GetClass(UtStringId aClassNameId) const;
   UtScriptClass* GetOrCreateContainer(const std::string& aContainerType);

   bool mBasicType : 1;          // This is set to true for all the basic types: int, double, bool, and string.
   bool mConstructible : 1;      // Indicates if this class can be constructed in script.
   bool mCloneable : 1;          // Indicates if this class can be cloned in script.
   bool mContainer : 1;          // Indicates if this class is a container type (such as and array or map).
   bool mLessThanComparable : 1; // Indicates if this class can be used in less than comparisons.
   bool mEqualityComparable : 1; // Indicates if this class can be used in equality comparisons.
   bool mIsScriptAccessible : 1; // Indicates the app object pointer can be cast to UtScriptAccessible*
   bool mIsPseudoClass : 1;      // Indicates if the class represents a 'pseudo-class' (i.e.: a 'script_struct')
   // This is required to support up-casting
private:
   std::vector<UtStringId> mClassNameHierachy; // The class hierarchy stored as name ids.
   std::vector<UtStringId> mImplicitCastTypes; // The class types that this class can be implicitly cast to.
   std::vector<UtStringId> mExplicitCastTypes; // The class types that this class can be explicitly cast to.
   std::vector<std::unique_ptr<InterfaceMethod>> mMethodList; // The list of methods available to this class.
   UtStringId                                    mClassName;
   MethodList                                    mSortedMethods;
   TemplateArgTypes mTemplateArgTypes; // The list of template argument types (only for container types).
   UtScriptTypes*   mScriptTypesPtr;
   //! If this class was registered to a UtScriptEnvironment then this is the pointer to the owning environment.
   //! This will be the case if the class is either a script_struct class OR a container where a template
   //! argument is a script_struct class.
   UtScriptEnvironment* mEnvironmentPtr{nullptr};
};

#endif
