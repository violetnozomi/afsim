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

#ifndef WKQUEUEABLEMESSAGEOBJECT_HPP
#define WKQUEUEABLEMESSAGEOBJECT_HPP

#include "wkf_export.h"

#include <QMessageBox>
#include <QObject>

namespace wkf
{
class WKF_EXPORT QueueableMessageObject : public QObject
{
   Q_OBJECT

public:
   // QueueableMessageObject allows the user to create a message box from any thread
   //  without having to worry about passing information between threads.
   // The connection type is passed in as an argument.
   // A Warning about using QueuedConnection: The following code does work with QueuedConnection
   //   if(!SomeErrorCondition)
   //   {
   //      QueueableMessageObject mo(Qt::QueuedConnection);
   //      mo.ShowMessageBox(QMessageBox::Warning, title, error);
   //   }
   // The QueueableMessageObject will be deleted prior to handling the signal and thus will never display a message box.
   // If using a QueuedConnection, do not create the QueueableMessageObject on the stack.
   // This is not an issue for BlockingQueuedConnection, due to the fact the calling thread will not destroy the
   //  QueueableMessageObject, until control is returned to the thread.
   // If you ever find yourself using DirectConnection, is it probably best just to use QMessageBox instead of this class.
   QueueableMessageObject(Qt::ConnectionType aConnectionType = Qt::BlockingQueuedConnection);

   // To avoid issues described above, use this static message to display a non-blocking queued message.
   //  If the message is non-critical (not a message that a user should terminate the application) this static
   //  function should be the preferred way to use this class.
   static void DisplayQueuedMessage(QMessageBox::Icon icon, const QString& title, const QString& text);

   void ShowMessageBox(QMessageBox::Icon icon, const QString& title, const QString& text, bool aForceQuit = false);
   void ShowExceptionMessageBox(const QString& title, const QString& text, bool aForceQuit = false);

signals:
#ifndef Q_MOC_RUN
private:
#endif
   void DisplayMessageBoxSignal(QMessageBox::Icon icon, const QString& title, const QString& text, bool aForceQuit);

private:
   void DisplayMessageBoxSlot(QMessageBox::Icon icon, const QString& title, const QString& text, bool aForceQuit);
};
} // namespace wkf

#endif
