// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtQtException.hpp"

#include <thread>

#include <QApplication>
#include <QMessageBox>
#include <QObject>

#include "UtMemory.hpp"
#include "UtTypeInfo.hpp"

static const std::thread::id sThread = std::this_thread::get_id();

ut::qt::detail::QueueableExceptionMessage::QueueableExceptionMessage() noexcept
{
   connect(this,
           &QueueableExceptionMessage::ErrorSignal,
           this,
           &QueueableExceptionMessage::ErrorSlot,
           Qt::BlockingQueuedConnection);
}

void ut::qt::detail::QueueableExceptionMessage::ErrorSlot(const QString& aTitle, const QString& aDescription)
{
   QMessageBox::critical(nullptr, aTitle, aDescription);
}

void ut::qt::ErrorMessageBox(const QString& aTitle, const QString& aDescription) noexcept
{
   // This can be called after the QApplication has been destroyed.
   // If that happens, the QMessageBox::exec() method will crash with a null pointer exception.
   // To prevent that, we conditionally construct a new QApplication with dummy command line arguments.
   int                           argc    = 1;
   char                          argv0[] = "";
   char*                         argv[]  = {argv0, nullptr};
   std::unique_ptr<QApplication> app     = nullptr;
   if (!qApp)
   {
      // The new application is heap allocated to preserve stack space in case this is called as a result of a stack overflow.
      app = ut::make_unique<QApplication>(argc, argv);
   }

   // Check the thread first to avoid deadlock.
   if (sThread == std::this_thread::get_id())
   {
      QMessageBox::critical(nullptr, aTitle, aDescription);
   }
   else
   {
      ut::qt::detail::QueueableExceptionMessage msg;
      msg.moveToThread(qApp->thread());
      msg.ErrorSignal(aTitle, aDescription);
   }
}

void ut::qt::TerminateHandler()
{
   ut::WriteTerminateLog();

   QString appName = ut::GetApplicationName() ? ut::GetApplicationName() : "The application";
   QString file    = QString::fromStdString(ut::GetApplicationLogPath());
   QString message = R"HTML(
      <p __NOSPACE>%1 has terminated unexpectedly.</p>
      <p __NOSPACE>See '<a href="file:///%2">%2</a>' for more information.</p>
      <p __NOSPACE>Please provide this log with a bug report to the AFSIM Development Team.</p>
      )HTML";

   message.replace("__NOSPACE", "style=\"margin-top: 0px; margin-bottom: 0px;\"");

   ErrorMessageBox("Program Terminated", message.arg(appName, file));
}

void ut::qt::ReportException(const char* aSource, std::exception_ptr aException, const char* aAdditionalInfo)
{
   const auto ex = ut::ExceptionDetails(aException);
   ut::WriteExceptionLog(aSource, ex);

   QString message;
   QString title;
   if (ex.mTypeName.empty())
   {
      title = "Unknown Exception";
      message += "<p __NOSPACE>" + QString(aSource) + " terminated unexpectedly due to an unknown exception.</p>";
   }
   else
   {
      title                  = ex.mFatalException ? "Fatal Exception" : "Unhandled Exception";
      const QString form     = R"HTML(
         <p __NOSPACE>%1 terminated unexpectedly due to %2 exception.</p>
         <p __TABBED>Type: %3</p>
         <p __TABBED>What: %4</p>
         )HTML";
      const QString category = ex.mFatalException ? "a fatal" : "an unhandled";
      const QString type     = QString::fromStdString(ex.mTypeName).toHtmlEscaped();
      const QString what     = QString::fromStdString(ex.mWhatMessage).toHtmlEscaped();

      message += form.arg(aSource, category, type, what);
   }

   if (aAdditionalInfo)
   {
      message += "<p __NOSPACE>";
      message += QString(aAdditionalInfo).toHtmlEscaped();
      message += "</p>";
   }

   const QString file = QString::fromStdString(ut::GetApplicationLogPath());
   const QString form = R"HTML(
      <br>
      <p __NOSPACE>See '<a href="file:///%1">%1</a>' for more information.</p>
      <p __NOSPACE>Please provide this log with a bug report to the AFSIM Development Team.</p>
      )HTML";

   message += form.arg(file);
   message.replace("__TABBED", "style=\"margin-left: 20%; __PATTR\"");
   message.replace("__NOSPACE", "style=\"__PATTR\"");
   message.replace("__PATTR", "margin-top: 0px; margin-bottom: 0px;");

   ErrorMessageBox(title, message);
}
