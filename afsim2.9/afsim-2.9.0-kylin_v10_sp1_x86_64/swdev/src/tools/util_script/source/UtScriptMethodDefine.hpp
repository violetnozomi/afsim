// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTSCRIPTMETHODDEFINE_HPP
#define UTSCRIPTMETHODDEFINE_HPP

#include "UtScriptMethodBuilder.hpp" // implementation details for this file
#include "UtStringIdLiteral.hpp"

//-----------------------------------------------------------
//! Sets the class that the script class is wrapping around.
//! If you wish to have the script class name be different from the c++ class's name,
//! place UT_MAP_CLASS_TO_SCRIPT_NAME(CLASS,QuotedScriptName) from UtScriptAccessible
//! in the associated class's header
#define UT_SCRIPT_WRAP_CLASS(CLASS)                       \
   SetClassName(ut::script::GetScriptName<CLASS>::value); \
   UtScriptClass& scriptClass = *this;                    \
   using MethodBuilder        = ut::script::UtScriptMethodBuilder<CLASS>;

//! special case of UT_SCRIPT_WRAP_CLASS where no class
//! is actually being wrapped around, and only static
//! script methods are going to be added.
#define UT_SCRIPT_NONWRAPPING_CLASS(SCRIPT_CLASS_NAME)  \
   SetClassName(UtStringIdLiteral(#SCRIPT_CLASS_NAME)); \
   UtScriptClass& scriptClass = *this;                  \
   using MethodBuilder        = ut::script::UtScriptMethodBuilder<void>;

//! Allows script methods to be added to pre-existing script classes outside
//! of the ScriptClass's constructor.
//! The primary purpose for this is when a plugin adds new methods to a script class
//! defined in a different library.
#define UT_SCRIPT_EXTEND_CLASS(aScriptTypes, CLASS)                                                \
   UtScriptClass* scriptClassPtr = aScriptTypes.GetClass(ut::script::GetScriptName<CLASS>::value); \
   assert(scriptClassPtr != nullptr);                                                              \
   UtScriptClass& scriptClass = *scriptClassPtr;                                                   \
   using MethodBuilder        = ut::script::UtScriptMethodBuilder<CLASS>;

//-----------------------------------------------------------
/**
Creates a script method named FUNC_NAME that gets added to the script class
with the corresponding return/param types.

Usage:
Should be called from the constructor of a script class.
Must call UT_SCRIPT_WRAP_CLASS in the constructor before defining script methods.
Special variable `self` is the instance of the class that is being wrapped around.

An error check for null dereference will be performed on non-primitive paramaters
passed by reference or by value. Prefer only using pointer paramaters when the
method itself will handle null.

To return managed ownership of a type, you can return by value,const&, or unique_ptr.
It is preferred that you return by unique_ptr when returning managed ownership so that
you can avoid an unnecessary heap alloc/copy operation. An exception where you should
prefer return by value/ref is with basic_types (arithmetic/bool/string) and container
types (vector/map)

Example:
@code
// Foo.hpp
struct Foo
{
   int GetVal();
   int AddTogether(int a, int b);
   void SetVal(int arg);
   static Stuff& GetStaticStuff();
};

Foo make_foo();

// FooScriptClass.cpp
FooScriptClass::FooScriptClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName,aTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(Foo);

   // creates a method named 'Sum' that returns an 'int' and takes an int* and an int.
   UT_SCRIPT_METHOD(int,Sum,(int* a_ptr, int b))
   {
      if(a_ptr==nullptr) { return 0; }
      // `self` is a reference to the instance of Foo being worked on.
      return self.AddTogether(*a_ptr,b);
   };

   // seperate macro for creating static methods
   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<Foo>,Create,())
   {
      return ut::make_unique<Foo>();
   };

   // handy dandy macro that allows you to transparently pass through to a member function
   // or member variable.
   UT_SCRIPT_WRAP_MEMBER("SetVal", SetVal);

   // also works for static methods.
   UT_SCRIPT_WRAP_MEMBER("StaticStuff",GetStaticStuff)
}
@endcode
*/
#define UT_SCRIPT_METHOD(RETURN_TYPE, FUNC_NAME, PAREN_ENCLOSED_ARGS)                                                \
   MethodBuilder::SignatureBuilder<RETURN_TYPE PAREN_ENCLOSED_ARGS>{scriptClass, UtStringIdLiteral(#FUNC_NAME)}&&[]( \
      MethodBuilder::Class & self,                                                                                   \
      UtScriptContext & aContext UT_UNWRAP_PARENTHESIS PAREN_ENCLOSED_ARGS) -> RETURN_TYPE


//! @sa UT_SCRIPT_METHOD
//! Same as UT_SCRIPT_METHOD but adds a static method instead of a member method.
#define UT_SCRIPT_STATIC_METHOD(RETURN_TYPE, FUNC_NAME, PAREN_ENCLOSED_ARGS)                                       \
   MethodBuilder::SignatureBuilder<RETURN_TYPE PAREN_ENCLOSED_ARGS>{scriptClass, UtStringIdLiteral(#FUNC_NAME)} || \
      [](UtScriptContext & aContext UT_UNWRAP_PARENTHESIS PAREN_ENCLOSED_ARGS) -> RETURN_TYPE

//! @sa UT_SCRIPT_METHOD
//! Similar to UT_SCRIPT_METHOD but adds a variadic method that allows for a dynamic number of arguments.
//! param VARPACK_TYPE - The class type to require all trailing arguements in the variadic pack to be a type of.
#define UT_SCRIPT_VARIADIC_METHOD(RETURN_TYPE, FUNC_NAME, PAREN_ENCLOSED_ARGS, VARPACK_TYPE)                          \
   MethodBuilder::SignatureBuilder<RETURN_TYPE PAREN_ENCLOSED_ARGS, VARPACK_TYPE>{scriptClass,                        \
                                                                                  UtStringIdLiteral(#FUNC_NAME)}&&[]( \
      MethodBuilder::Class & self,                                                                                    \
      UtScriptContext & aContext,                                                                                     \
      UtScriptMethodArgs aVarArgs UT_UNWRAP_PARENTHESIS PAREN_ENCLOSED_ARGS) -> RETURN_TYPE

//! @sa UT_SCRIPT_METHOD
//! Similar to UT_SCRIPT_METHOD but adds a static variadic method that allows for a dynamic number of arguments.
//! param VARPACK_TYPE - The class type to require all trailing arguements in the variadic pack to be a type of.
#define UT_SCRIPT_STATIC_VARIADIC_METHOD(RETURN_TYPE, FUNC_NAME, PAREN_ENCLOSED_ARGS, VARPACK_TYPE)                 \
   MethodBuilder::SignatureBuilder<RETURN_TYPE PAREN_ENCLOSED_ARGS, VARPACK_TYPE>{scriptClass,                      \
                                                                                  UtStringIdLiteral(#FUNC_NAME)} || \
      [](UtScriptContext & aContext, UtScriptMethodArgs aVarArgs UT_UNWRAP_PARENTHESIS PAREN_ENCLOSED_ARGS) -> RETURN_TYPE

//-----------------------------------------------------------
//! Adds a method thats is a simple wrapper around a member function or member variable.
//! @code
//! class Foo { int GetBar(double arg); };
//! ...
//! // in script class constructor, adds script method named Bar that calls Foo::GetBar
//! UT_SCRIPT_WRAP_CLASS(Foo);
//! UT_SCRIPT_WRAP_MEMBER("Bar", GetBar);
//! @endcode
#define UT_SCRIPT_WRAP_MEMBER(QUOTED_METHOD_NAME, MEMBER_NAME) \
   MethodBuilder::WrapFuncPtr(scriptClass, UtStringIdLiteral(QUOTED_METHOD_NAME), &MethodBuilder::Class::MEMBER_NAME);

//-----------------------------------------------------------
//! Marks the following UT_SCRIPT_METHOD as constexpr.
//! constexpr methods should have no side effects and should
//! always return the same value given the same inputs.
//! When possible, constexpr methods may be called at script
//! compile time instead of at runtime.
#define UT_SCRIPT_CONSTEXPR ut::script::MarkConstexpr{} |=

//-----------------------------------------------------------
//! Marks the following UT_SCRIPT_METHOD as deprecated.
//! Use of deprecated methods will cause the script compiler
//! to log a warning that the method is deprecated along with
//! the provided message.
//! Recommended information to include in the message are:
//! - What version the method will be removed in.
//! - How to convert to a new approach (if available)
#define UT_SCRIPT_DEPRECATED(MESSAGE) ut::script::MarkDeprecated{MESSAGE} |=

//-----------------------------------------------------------
//! Adds a method that returns a constexpr value.
#define UT_SCRIPT_CONSTEXPR_VALUE(METHOD_NAME, CONSTEXPR_VALUE)                                                                  \
   UT_SCRIPT_CONSTEXPR                                                                                                           \
   MethodBuilder::SignatureBuilder<typename std::conditional<std::is_convertible<decltype(CONSTEXPR_VALUE), const char*>::value, \
                                                             const char*,                                                        \
                                                             decltype(CONSTEXPR_VALUE)>::type()>{scriptClass,                    \
                                                                                                 UtStringIdLiteral(              \
                                                                                                    #METHOD_NAME)} ||            \
      [](UtScriptContext& aContext)                                                                                              \
   {                                                                                                                             \
      constexpr auto val = CONSTEXPR_VALUE;                                                                                      \
      return val;                                                                                                                \
   };

#endif