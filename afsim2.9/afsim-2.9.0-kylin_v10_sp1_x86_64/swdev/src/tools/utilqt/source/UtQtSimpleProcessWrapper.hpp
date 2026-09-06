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

#ifndef UTQTSIMPLEPROCESSWRAPPER_HPP
#define UTQTSIMPLEPROCESSWRAPPER_HPP

#include "utilqt_export.h"

#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QString>
#include <QStringList>

//! This is a simple QProcess wrapper that is meant to be subclassed. The
//! purpose of this class is mainly to provide a couple facilities for error
//! reporting, since Qt does not provide anything convenient by default.
//!
//! (This class is used in AVATAR and the AFNES IDE.)
class UTILQT_EXPORT UtQtSimpleProcessWrapper : public QObject
{
   Q_OBJECT

public:
   explicit UtQtSimpleProcessWrapper(QObject* aParent = nullptr);
   ~UtQtSimpleProcessWrapper() override;

   virtual bool Execute() = 0;
   QProcess*    GetProcess();

signals:
   void FinishedWithError(QString aErrorMsg);

protected:
   virtual bool ProcessWillExecute() const;
   bool         Execute(const QString&             aExePath,
                        const QStringList&         aProcessArgs = QStringList(),
                        const QProcessEnvironment& aProcessEnv  = QProcessEnvironment::systemEnvironment());
   void         EmitErrorWithOutput();

private slots:
   void HandleProcessError(QProcess::ProcessError aProcessError);

private:
   QProcess* mProcess;
};

#endif // UTQTSIMPLEPROCESSWRAPPER_HPP
