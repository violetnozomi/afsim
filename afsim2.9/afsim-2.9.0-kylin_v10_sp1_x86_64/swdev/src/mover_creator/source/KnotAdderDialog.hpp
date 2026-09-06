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

#ifndef KNOT_ADDER_DIALOG_HPP
#define KNOT_ADDER_DIALOG_HPP

#include <QDialog>

namespace Ui
{
   class KnotAdderDialog;
}

namespace Designer
{
   class KnotAdderDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit KnotAdderDialog(QWidget*        aParent,
                                  Qt::WindowFlags aFlags = Qt::WindowFlags());
         ~KnotAdderDialog() override;

      signals:
         void CreateKnot(double aXCoord);

      private slots:
         void HandleApplyButtonPressed();

      private:
         Ui::KnotAdderDialog* mUIPtr;
   };
}
#endif // !KNOT_ADDER_DIALOG_HPP
