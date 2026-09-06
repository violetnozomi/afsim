// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef UTQTFONTBUTTON_HPP
#define UTQTFONTBUTTON_HPP

#include "utilqt_export.h"

#include <QFont>
#include <QPushButton>

#include "UtCallback.hpp"

class UTILQT_EXPORT UtQtFontButton : public QPushButton
{
public:
   explicit UtQtFontButton(QWidget* aParent);

   void         SetFont(const QFont& aFont);
   const QFont& GetFont() const { return mFont; }

   void SetWindowTitle(const QString& aNewName) { mWindowTitle = aNewName; }

private:
   QString mWindowTitle{"Choose Font"};
   QFont   mFont;
};

#endif
