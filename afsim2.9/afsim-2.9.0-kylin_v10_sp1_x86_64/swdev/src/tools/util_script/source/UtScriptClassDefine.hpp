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

#include "UtScriptClass.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptException.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"

//! UT_DECLARE_SCRIPT_METHOD streamlines the class declaration syntax for InterfaceMethods.
//! It creates an Interface method with the name METHOD_NAME.

#define UT_DECLARE_SCRIPT_METHOD(METHOD_NAME)                  \
   class METHOD_NAME : public UtScriptClass::InterfaceMethod   \
   {                                                           \
   public:                                                     \
      METHOD_NAME(const std::string& aName = #METHOD_NAME);    \
      virtual void operator()(UtScriptExecutor*  aExecutorPtr, \
                              UtScriptContext&   aContext,     \
                              const UtScriptRef& aReference,   \
                              UtScriptMethodArgs aVarArgs,     \
                              UtScriptData&      aReturnVal);       \
   }

#define UT_SCRIPT_METHOD_DECL(CLASS, METHOD)                                     \
   template<class T>                                                             \
   void CLASS##METHOD##_Execute(UtScriptExecutor*               aExecutorPtr,    \
                                UtScriptContext&                aContext,        \
                                const UtScriptRef&              aReference,      \
                                T*                              aObjectPtr,      \
                                UtScriptClass*                  aObjectClassPtr, \
                                UtScriptData&                   aReturnVal,      \
                                UtScriptClass*                  aReturnClassPtr, \
                                UtScriptMethodArgs              aVarArgs,        \
                                UtScriptClass::InterfaceMethod* aInterfaceMethodPtr)

#define UT_DEFINE_SCRIPT_METHOD_IMP(CLASS, SCOPE_OP, OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES, CHECK_LINE)               \
   UT_SCRIPT_METHOD_DECL(CLASS, METHOD);                                                                                        \
   SCOPE_OP METHOD::METHOD(const std::string& aName)                                                                            \
      : UtScriptClass::InterfaceMethod(aName, RET_TYPE, ARG_TYPES, NUM_ARGS)                                                    \
   {                                                                                                                            \
   }                                                                                                                            \
   void SCOPE_OP METHOD::operator()(UtScriptExecutor*  aExecutorPtr,                                                            \
                                    UtScriptContext&   aContext,                                                                \
                                    const UtScriptRef& aReference,                                                              \
                                    UtScriptMethodArgs aVarArgs,                                                                \
                                    UtScriptData&      aReturnVal)                                                              \
   {                                                                                                                            \
      if (NUM_ARGS >= 0)                                                                                                        \
         assert(aVarArgs.size() == static_cast<size_t>(NUM_ARGS));                                                              \
      auto           objPtr      = aReference.GetAppObject<OBJ_TYPE>();                                                         \
      UtScriptClass* retClassPtr = GetReturnClass();                                                                            \
      CHECK_LINE                                                                                                                \
      CLASS##METHOD##_Execute(aExecutorPtr, aContext, aReference, objPtr, mParentPtr, aReturnVal, retClassPtr, aVarArgs, this); \
   }                                                                                                                            \
   UT_SCRIPT_METHOD_DECL(CLASS, METHOD)

// Checks both aReference and aVarArgs
#define UT_SCRIPT_CHECK_IMP                                                  \
   if (!CheckForCallErrors(aExecutorPtr, aReference, &aVarArgs, aReturnVal)) \
      return;
// Checks only aReference, not aVarArgs
#define UT_SCRIPT_CHECK_NO_ARG                                                                               \
   if (!CheckForCallErrors(aExecutorPtr, aReference, static_cast<UtScriptMethodArgs*>(nullptr), aReturnVal)) \
      return;

//! NOTE: Make sure to include UtScriptContext.hpp, UtScriptClass.hpp, and UtScriptData.hpp
//!       when the following macro is used.
//!
//! UT_DEFINE_SCRIPT_METHOD streamlines the class definition syntax for InterfaceMethods.
//! It uses a double-dispatch technique to typecast the application object from a void*
//! to the appropriate type.  It also checks the size of the argument list and throws
//! an assert if there's an inconsistency.
//! CLASS     - The derived UtScriptClass name (e.g. WsfScriptPlatformClass).
//! OBJ_TYPE  - The type of the application object (e.g. WsfPlatform).
//! METHOD    - The derived UtScriptClass::InterfaceMethod name (e.g. GetPlatform).
//! NUM_ARGS  - The required number of arguments in the argument list.
//! RET_TYPE  - The type of the return argument as a string.
//! ARG_TYPES - A string containing a comma separated list of types. Use an empty string
//!             if there are no arguments.

//! Each script method has implicit access to the following variables.
//! template <class T>
//! bool CLASS##METHOD##_Execute(UtScriptExecutor*                aExecutorPtr,    // Script executor which is calling
//! this function
//!                              UtScriptContext&                 aContext,        // The current script context.
//!                              const UtScriptRef&               aReference,      // The current script reference.
//!                              T*                               aObjectPtr,      // A pointer to the application
//!                              object. UtScriptClass*                   aObjectClassPtr, // A pointer to the
//!                              application object's class type. UtScriptData&                    aReturnVal,      //
//!                              An object to load the return value into. UtScriptClass* aReturnClassPtr, // A pointer
//!                              to the return object's class type. const std::vector<UtScriptData>& aVarArgs);       //
//!                              A list containing the input arguments to the script method.

#define UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES) \
   UT_DEFINE_SCRIPT_METHOD_IMP(CLASS, CLASS::, OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES, UT_SCRIPT_CHECK_IMP)

//! Clone of UT_DEFINE_SCRIPT_METHOD, except it will never check aObjectPtr for null
#define UT_DEFINE_SCRIPT_METHOD_NOCHECK(CLASS, OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES) \
   UT_DEFINE_SCRIPT_METHOD_IMP(CLASS, CLASS::, OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES, )

//! Clone of UT_DEFINE_SCRIPT_METHOD, except it will never check aVarArgs for null
#define UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(CLASS, OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES) \
   UT_DEFINE_SCRIPT_METHOD_IMP(CLASS, CLASS::, OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES, UT_SCRIPT_CHECK_NO_ARG)

#define UT_DEFINE_SCRIPT_METHOD_EXT(OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES) \
   UT_DECLARE_SCRIPT_METHOD(METHOD);                                                 \
   UT_DEFINE_SCRIPT_METHOD_IMP(, , OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES, UT_SCRIPT_CHECK_IMP)

//! Clone of UT_DEFINE_SCRIPT_METHOD_EXT, except it will never check aObjectPtr for null
#define UT_DEFINE_SCRIPT_METHOD_NOCHECK_EXT(OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES) \
   UT_DECLARE_SCRIPT_METHOD(METHOD);                                                         \
   UT_DEFINE_SCRIPT_METHOD_IMP(, , OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES, )

//! Clone of UT_DEFINE_SCRIPT_METHOD_EXT, except it will never check aVarArgs for null
#define UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK_EXT(OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES) \
   UT_DECLARE_SCRIPT_METHOD(METHOD);                                                              \
   UT_DEFINE_SCRIPT_METHOD_IMP(, , OBJ_TYPE, METHOD, NUM_ARGS, RET_TYPE, ARG_TYPES, UT_SCRIPT_CHECK_NO_ARG)


//! Call while inside a script implementation to print a warning if condition is true, but continue executing the script
#define UT_SCRIPT_WARN(CONDITION_, MESSAGE_)                                                      \
   if (CONDITION_)                                                                                \
   {                                                                                              \
      aInterfaceMethodPtr->ReportCallErrors(nullptr, nullptr, &aVarArgs, aReturnVal, (MESSAGE_)); \
   }
