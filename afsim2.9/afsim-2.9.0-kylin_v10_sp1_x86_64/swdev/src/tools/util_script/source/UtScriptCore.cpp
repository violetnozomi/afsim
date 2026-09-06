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

#include "UtScriptCore.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>

#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtLogPublisher.hpp"
#include "UtMemory.hpp"
#include "UtScriptException.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptMethodDefine.hpp"

std::mutex  UtScriptCore::sWriteBufferMutex;
std::string UtScriptCore::sWriteBuffer;
bool        UtScriptCore::sAbortOnAssert = true;

using MutexLock = std::unique_lock<std::mutex>;

//! Retrieves, clears, and locks the write buffer.
static std::string LockWriteBuffer(MutexLock& aLock) noexcept
{
   std::string retval;
   aLock = MutexLock(UtScriptCore::sWriteBufferMutex);
   retval.swap(UtScriptCore::sWriteBuffer);
   return retval;
}

//! Retrieves and clears the write buffer, but does not lock it.
static std::string GetWriteBuffer() noexcept
{
   MutexLock lock;
   return LockWriteBuffer(lock);
}

//! Sets the write buffer.
//! If any newlines were written to it, publishes the complete lines.
//! The UtScriptCore::sWriteBufferMutex must be locked before calling.
static void SetWriteBuffer(std::string&& aString, bool aDebug) noexcept
{
   using ut::log::Message;
   const std::string& primaryType = aDebug ? Message::Debug() : Message::Info();
   auto&              buf         = UtScriptCore::sWriteBuffer;
   buf                            = std::move(aString);
   ut::log::Publisher::PublishLines(buf, {primaryType, "script"});
}

UtScriptCore::UtScriptCore(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(UtStringIdLiteral("__BUILTIN__"), aScriptTypesPtr)
{
   UT_SCRIPT_NONWRAPPING_CLASS(__BUILTIN__);

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, writeln, (), UtScriptData)
   {
      auto out = ut::log::info("script") << ::GetWriteBuffer();
      for (const auto& arg : aVarArgs)
      {
         out << arg;
      }

      out.ForceSend();
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, write, (), UtScriptData)
   {
      MutexLock          lock;
      std::ostringstream oss{::LockWriteBuffer(lock)};
      for (const auto& arg : aVarArgs)
      {
         oss << arg;
      }
      ::SetWriteBuffer(oss.str(), false);
   };

   UT_SCRIPT_DEPRECATED("Use 'writeln' instead")
   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, print, (), UtScriptData) // NO_DOC | DEPRECATED
   {
      auto out = ut::log::info("script") << ::GetWriteBuffer();
      for (const auto& arg : aVarArgs)
      {
         out << arg;
      }
      out.ForceSend();
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, write_d, (), UtScriptData)
   {
      // calls to write_d will be skipped if debug writes weren't enabled.
      MutexLock          lock;
      std::ostringstream oss{::LockWriteBuffer(lock)};
      for (const auto& arg : aVarArgs)
      {
         oss << arg;
      }
      ::SetWriteBuffer(oss.str(), true);
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, writeln_debug, (), UtScriptData)
   {
      // calls to writeln_debug will be skipped if debug writes weren't enabled.
      auto out = ut::log::debug("script") << ::GetWriteBuffer();
      for (const auto& arg : aVarArgs)
      {
         out << arg;
      }
      out.ForceSend();
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, writeln_fatal, (), UtScriptData)
   {
      auto out = ut::log::fatal("script") << ::GetWriteBuffer();
      for (const auto& arg : aVarArgs)
      {
         out << arg;
      }
      out.ForceSend();
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, writeln_error, (), UtScriptData)
   {
      auto out = ut::log::error("script") << ::GetWriteBuffer();
      for (const auto& arg : aVarArgs)
      {
         out << arg;
      }
      out.ForceSend();
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, writeln_warning, (), UtScriptData)
   {
      auto out = ut::log::warning("script") << ::GetWriteBuffer();
      for (const auto& arg : aVarArgs)
      {
         out << arg;
      }
      out.ForceSend();
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, writeln_developer, (), UtScriptData)
   {
      auto out = ut::log::developer("script") << ::GetWriteBuffer();
      for (const auto& arg : aVarArgs)
      {
         out << arg;
      }
      out.ForceSend();
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(void, writeln_d, (), UtScriptData)
   {
      // calls to writeln_d will be skipped if debug writes weren't enabled.
      auto out = ut::log::debug("script") << ::GetWriteBuffer();
      for (const auto& arg : aVarArgs)
      {
         out << arg;
      }
      out.ForceSend();
   };

   UT_SCRIPT_STATIC_VARIADIC_METHOD(std::string, write_str, (), UtScriptData)
   {
      std::stringstream ss;
      for (const auto& arg : aVarArgs)
      {
         ss << arg;
      }
      return ss.str();
   };

   UT_SCRIPT_STATIC_METHOD(bool, has_attr, (const UtScriptData* aObject, UtStringId aAttribName))
   {
      UtScriptRef* refPtr = aObject->GetPointer();
      if (refPtr && refPtr->GetScriptClass())
      {
         return nullptr != refPtr->GetScriptClass()->GetAttribute(refPtr->GetAppObject(), aAttribName);
      }
      return false;
   };

   UT_SCRIPT_STATIC_METHOD(std::vector<std::string>, list_attr, (const UtScriptData* aObject))
   {
      UtScriptRef* refPtr = aObject->GetPointer();
      if (refPtr && refPtr->GetScriptClass())
      {
         std::vector<std::string> attrList;
         refPtr->GetScriptClass()->GetAttributeList(refPtr->GetAppObject(), attrList);
         return attrList;
      }
      return {};
   };

   UT_SCRIPT_STATIC_METHOD(int, attr_count, (const UtScriptData* aObject))
   {
      UtScriptRef* refPtr = aObject->GetPointer();
      if (refPtr && refPtr->GetScriptClass())
      {
         return refPtr->GetScriptClass()->GetAttributeCount(refPtr->GetAppObject());
      }
      return 0;
   };

   UT_SCRIPT_STATIC_METHOD(UtStringId, attr_name_at, (const UtScriptData* aObject, int aAttribIndex))
   {
      UtScriptRef* refPtr = aObject->GetPointer();
      if (refPtr && refPtr->GetScriptClass())
      {
         return refPtr->GetScriptClass()->GetAttributeNameAt(refPtr->GetAppObject(), aAttribIndex);
      }
      return {};
   };

   UT_SCRIPT_STATIC_METHOD(UtScriptData*, get_attr, (const UtScriptData* aObject, UtStringId aAttribName))
   {
      UtScriptRef* refPtr = aObject->GetPointer();
      if (refPtr && refPtr->GetScriptClass())
      {
         return refPtr->GetScriptClass()->GetAttribute(refPtr->GetAppObject(), aAttribName);
      }
      return nullptr;
   };

   UT_SCRIPT_STATIC_METHOD(bool, has_script, (const UtScriptData* aObject, UtStringId aAttribName))
   {
      UtScriptRef* refPtr = aObject->GetPointer();
      if (refPtr && refPtr->GetScriptClass())
      {
         return refPtr->GetScriptClass()->HasScript(refPtr->GetAppObject(), aAttribName);
      }
      return false;
   };

   UT_SCRIPT_STATIC_METHOD(void, __print_callstack, ())
   {
      auto out = ut::log::debug() << "Callstack:";
      UtScriptExecutor::PrintAllCallStacks(out, true);
   };

#ifdef assert
#undef assert
#endif
   UT_SCRIPT_STATIC_METHOD(void, assert, (bool expectation, const std::string& message))
   {
      if (!expectation)
      {
         UT_SCRIPT_ABORT("Assert Failed: " + message);
      }
   };

   UT_SCRIPT_STATIC_METHOD(void, assert, (bool expectation))
   {
      if (!expectation)
      {
         UT_SCRIPT_ABORT("Assert Failed");
      }
   };
}

// virtual
UtScriptCore::~UtScriptCore() = default;
