// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtQtSimpleProcessWrapper.hpp"

#include <cassert>

#include <QFile>
#include <QtGlobal>

UtQtSimpleProcessWrapper::UtQtSimpleProcessWrapper(QObject* aParent)
   : QObject(aParent)
   , mProcess(new QProcess(this))
{
   connect(mProcess, SIGNAL(error(QProcess::ProcessError)), SLOT(HandleProcessError(QProcess::ProcessError)));
}

UtQtSimpleProcessWrapper::~UtQtSimpleProcessWrapper() {}

QProcess* UtQtSimpleProcessWrapper::GetProcess()
{
   return mProcess;
}

//! Subclasses can override this method to do some kind of work before the
//! processes executes (such as generating an input file that will be passed
//! to the process via arguments). If this method returns false, then the
//! process will not execute.
bool UtQtSimpleProcessWrapper::ProcessWillExecute() const
{
   return true;
}

//! Executes the given program with the given arguments and environment. The
//! process will not run if the executable path does not exist or if the virtual
//! ProcessWillExecute method returns false.
bool UtQtSimpleProcessWrapper::Execute(const QString&             aExePath,
                                       const QStringList&         aProcessArgs,
                                       const QProcessEnvironment& aProcessEnv)
{
   if (!aExePath.isEmpty() && QFile(aExePath).exists())
   {
      if (ProcessWillExecute())
      {
         mProcess->setProcessEnvironment(aProcessEnv);
         mProcess->start(aExePath, aProcessArgs);
         return true;
      }
   }

   return false;
}

//! Writes all of the processes's output to a string and emits it. This is
//! intended to be used when a process finishes successfully but returns an
//! error code.
void UtQtSimpleProcessWrapper::EmitErrorWithOutput()
{
   const QString stdOut(mProcess->readAllStandardOutput());
   const QString stdErr(mProcess->readAllStandardError());

   QString errorMsg(stdOut);

   if (!stdErr.isEmpty())
   {
      if (!errorMsg.isEmpty())
      {
         errorMsg += "\n\n";
      }

      errorMsg += stdErr;
   }

   emit FinishedWithError(errorMsg);
}

//! Returns the process error/crash as a human-readable string.
void UtQtSimpleProcessWrapper::HandleProcessError(QProcess::ProcessError aProcessError)
{
#if QT_VERSION >= 0x050000
   QString errorMsg(QString("The process %1 ").arg(mProcess->program()));
#else
   QString errorMsg("The process ");
#endif

   switch (aProcessError)
   {
   case QProcess::FailedToStart:
      errorMsg += "failed to start.";
      break;
   case QProcess::Crashed:
      errorMsg += "crashed.";
      break;
   case QProcess::Timedout:
      errorMsg += "timed out.";
      break;
   case QProcess::WriteError:
      errorMsg += "could not be written to.";
      break;
   case QProcess::ReadError:
      errorMsg = "could not be read from.";
      break;
   case QProcess::UnknownError:
   default:
      errorMsg += "experienced an unknown error.";
      break;
   }

   emit FinishedWithError(errorMsg);
}
