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

#ifndef KNOT_EDITOR_DIALOG_HPP
#define KNOT_EDITOR_DIALOG_HPP

#include <QDialog>
#include "CubicSpline.hpp"

namespace Ui
{
   class KnotEditorDialog;
}

namespace Designer
{
   class KnotEditorDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit KnotEditorDialog(QWidget*        aParent,
                                   Qt::WindowFlags aFlags = Qt::WindowFlags());
         ~KnotEditorDialog() override;

         void SetXCoordinate(double aVal);
         void SetYCoordinate(double aVal);
         void SetBCType(CubicSpline::BC_Type aType);
         void SetBCValue(double aVal);

      signals:
         void EditKnot(CubicSpline::Knot aKnot);

      private slots :
         void HandleApplyButtonPressed();

      private:
         Ui::KnotEditorDialog* mUIPtr;
   };
}
#endif // !KNOT_EDITOR_DIALOG_HPP
