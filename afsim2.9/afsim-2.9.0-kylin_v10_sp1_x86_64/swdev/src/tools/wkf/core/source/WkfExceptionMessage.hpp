// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFEXCEPTIONMESSAGE_HPP
#define WKFEXCEPTIONMESSAGE_HPP

#include "wkf_export.h"

#include <functional>

#include <QObject>

#include "UtBinder.hpp"
#include "UtException.hpp"
#include "WkfQueueableMessageObject.hpp"

namespace wkf
{
namespace Exception
{
WKF_EXPORT QString GetExceptionMessageBoxString(const QString& aExceptionString,
                                                const QString& aFunctionName,
                                                const QString& aPluginName = "");

// Variadic function template wraps any function in try-catch block for exception handling
template<typename RT, typename... Args>
std::function<RT(Args...)> ExceptionHandlerWrapper(const std::function<RT(Args...)>& aFunction,
                                                   const char*                       aFunctionName,
                                                   bool                              aQueueMessageBox,
                                                   const QString&                    aPluginName) noexcept
{
   static_assert(std::is_default_constructible<RT>() || std::is_void<RT>(),
                 "ExceptionHandlerWrapper can only wrap functions whose return type is void or default constructible");

   auto result = [=](Args... args) -> RT
   {
      try
      {
         return aFunction(args...);
      }
      catch (...)
      {
         auto        ex     = ut::ExceptionDetails(std::current_exception());
         std::string source = "Function " + aPluginName.toStdString() + "::" + aFunctionName;
         ut::WriteExceptionLog(source.c_str(), ex);

         QueueableMessageObject mo(aQueueMessageBox ? Qt::BlockingQueuedConnection : Qt::AutoConnection);
         QString                what = ex.mWhatMessage.empty() ? "Unknown" : QString::fromStdString(ex.mWhatMessage);

         mo.ShowExceptionMessageBox("Unhandled Exception",
                                    GetExceptionMessageBoxString(what, aFunctionName, aPluginName),
                                    ex.mFatalException);
      }
      // If a stack overflow is caught via ut::HardwareException, the stack guard frames must be reset.
      // This cannot be called within the catch{} block because the stack has not been unwound yet at that point.
      ut::ResetStackOverflow();
      return RT();
   };

   return result;
}
} // namespace Exception
} // namespace wkf

// Helper macros for ExceptionHandlerWrapper. Just pass in class name, function name, and plugin name.
#define WKF_EXCEPTION_HANDLER_BASE(CLASS, FUNCTION, QUEUEABLE, PLUGIN_NAME) \
   wkf::Exception::ExceptionHandlerWrapper(UtStd::Bind(&CLASS::FUNCTION, this), #CLASS "::" #FUNCTION, QUEUEABLE, PLUGIN_NAME)

// For functions that execute on simulation thread, and thus need to block while queuing a message box on the GUI thread.
#define WKF_EXCEPTION_HANDLER_QUEUED(CLASS, FUNCTION, PLUGIN_NAME) \
   WKF_EXCEPTION_HANDLER_BASE(CLASS, FUNCTION, true, PLUGIN_NAME)

// For functions that execute on GUI thread
#define WKF_EXCEPTION_HANDLER(CLASS, FUNCTION, PLUGIN_NAME) \
   WKF_EXCEPTION_HANDLER_BASE(CLASS, FUNCTION, false, PLUGIN_NAME)

#endif
