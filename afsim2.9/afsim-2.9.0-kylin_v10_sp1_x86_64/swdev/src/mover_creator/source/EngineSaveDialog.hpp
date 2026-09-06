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

#ifndef ENGINE_SAVE_DIALOG_HPP
#define ENGINE_SAVE_DIALOG_HPP

#include <QDialog>

namespace Ui
{
   class EngineSaveDialog;
}

namespace Designer
{
   class EngineSaveDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit EngineSaveDialog(QWidget*        aParent,
                                   Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~EngineSaveDialog();

      private slots:
         void SaveClicked(bool aClicked);
         void IgnoreClicked(bool aClicked);
         void CancelClicked(bool aClicked);

      private:
         Ui::EngineSaveDialog* mUIPtr;
   };
}
#endif // !ENGINE_SAVE_DIALOG_HPP
