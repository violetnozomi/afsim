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
#include "WkfQueueableMessageObject.hpp"

#include <QCoreApplication>
#include <QMessageBox>

void wkf::QueueableMessageObject::DisplayQueuedMessage(QMessageBox::Icon icon, const QString& title, const QString& text)
{
   // CppCheck complains that this can leak, As long as the Qt Event Loop is running deleteLater() will clean this up -JLT
   auto* mo = new wkf::QueueableMessageObject(Qt::QueuedConnection);
   mo->ShowMessageBox(icon, title, text);
   mo->deleteLater();
   // cppcheck-suppress memleak
}

wkf::QueueableMessageObject::QueueableMessageObject(Qt::ConnectionType aConnectionType)
   : QObject(nullptr)
{
   connect(this,
           &QueueableMessageObject::DisplayMessageBoxSignal,
           this,
           &QueueableMessageObject::DisplayMessageBoxSlot,
           aConnectionType);
}

void wkf::QueueableMessageObject::ShowMessageBox(QMessageBox::Icon icon, const QString& title, const QString& text, bool aForceQuit)
{
   moveToThread(qApp->thread());
   emit DisplayMessageBoxSignal(icon, title, text, aForceQuit);
}

void wkf::QueueableMessageObject::ShowExceptionMessageBox(const QString& title, const QString& text, bool aForceQuit)
{
   ShowMessageBox(QMessageBox::Critical, title, text, aForceQuit);
}

void wkf::QueueableMessageObject::DisplayMessageBoxSlot(QMessageBox::Icon icon,
                                                        const QString&    title,
                                                        const QString&    text,
                                                        bool              aForceQuit)
{
   if (aForceQuit)
   {
      QMessageBox mb(QMessageBox::Critical, title, text, QMessageBox::Abort);
      mb.exec();
      std::abort();
   }
   else if (icon == QMessageBox::Critical)
   {
      QMessageBox mb(QMessageBox::Critical, title, text, QMessageBox::Retry | QMessageBox::Abort);
      if (mb.exec() == QMessageBox::Abort)
      {
         std::abort();
      }
   }
   else
   {
      QMessageBox* mb = new QMessageBox(icon, title, text, QMessageBox::Ok);
      mb->setAttribute(Qt::WA_DeleteOnClose, true);
      mb->show();
   }
}
