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

#ifndef CALCULATE_AERO_DIALOG_HPP
#define CALCULATE_AERO_DIALOG_HPP

#include <QDialog>

class QTimer;
namespace Ui
{
   class CalculateAeroDialog;
}

namespace Designer
{
   class CalculateAeroDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit CalculateAeroDialog(QWidget*        aParent,
                                      Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~CalculateAeroDialog();
         void showEvent(QShowEvent* aEvent);

         void SetLabelText(QString aStr);
         void SetWindowTitle(QString aStr);
         void EndDialog();

      private slots:
         void UpdateProgressBar();

      private:
         Ui::CalculateAeroDialog* mUIPtr;
         QTimer*                    mProgressTimer;
   };
}
#endif // !CALCULATE_AERO_DIALOG_HPP

