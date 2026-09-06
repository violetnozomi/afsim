// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef AUDIT_ERROR_DIALOG_HPP
#define AUDIT_ERROR_DIALOG_HPP


#include <QDialog>

namespace Ui
{
   class AuditErrorDialog;
}

namespace Designer
{
   class AuditErrorDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit AuditErrorDialog(QWidget*        aParent,
                                   Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~AuditErrorDialog();

         void showEvent(QShowEvent* aEvent) override;
         void SetErrorLabel(QString aStr);
         void ShowOverrideButton();
         void AddError(QString aErrorStr);
         void AddErrors(QStringList aErrorList);

      private slots:
         void OkClicked();
         void OverrideClicked();

      private:
         Ui::AuditErrorDialog* mUIPtr;
   };
}
#endif // !AUDIT_ERROR_DIALOG_HPP
