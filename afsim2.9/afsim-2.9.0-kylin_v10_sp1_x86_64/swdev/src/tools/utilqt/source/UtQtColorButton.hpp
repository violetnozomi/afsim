// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef UTQTCOLORBUTTON_HPP
#define UTQTCOLORBUTTON_HPP

#include "utilqt_export.h"

#include <QColor>
#include <QPushButton>

#include "UtCallback.hpp"

class UTILQT_EXPORT UtQtColorButton : public QPushButton
{
   Q_OBJECT
public:
   UtQtColorButton(QWidget* aParent);

   void   SetColor(QColor aColor);
   QColor GetColor() const { return mCurrentColor; };

   UtCallbackListN<void(const QColor&)> ColorSelected;
   void                                 ShowAlpha(bool aShow) { mShowAlpha = aShow; }
   void                                 SetColorWindowTitle(const QString& aNewName) { mWindowName = aNewName; }
   void                                 DisableWithWarning(const QString& aWarningStr) { mWarningStr = aWarningStr; }
signals:
   void Clicked();
   void ColorChanged(const QColor&);
   void DialogClosed();

protected:
   void   HandleClick();
   QColor mCurrentColor;
   bool   mShowAlpha;

private:
   QString mWindowName;
   QString mWarningStr;
};
#endif
