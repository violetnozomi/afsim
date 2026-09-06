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

#ifndef NEW_TYPE_DIALOG_HPP
#define NEW_TYPE_DIALOG_HPP

#include <QDialog>

namespace Ui
{
   class NewTypeDialog;
}

namespace Designer
{
   class NewTypeDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit NewTypeDialog(QWidget*        aParent,
                                Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~NewTypeDialog();

         QString GetName();

      private slots:
         void OkClicked(bool aClicked);
         void CancelClicked(bool aClicked);

      private:
         Ui::NewTypeDialog* mUIPtr;
   };
}
#endif // !NEW_TYPE_DIALOG_HPP
