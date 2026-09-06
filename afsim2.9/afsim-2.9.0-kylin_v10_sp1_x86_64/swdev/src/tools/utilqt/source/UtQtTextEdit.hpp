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
#ifndef UTQTTEXTEDIT_HPP
#define UTQTTEXTEDIT_HPP

#include "utilqt_export.h"

#include <QTextEdit>

class UTILQT_EXPORT UtQtTextEdit : public QTextEdit
{
   Q_OBJECT
private:
public:
   UtQtTextEdit(QWidget* aParent = nullptr);
   // resize the text edit to fit the contents when the contents changes
   void  SetResizeOnTextChange(bool aResize);
   int   heightForWidth(int w) const override;
   bool  hasHeightForWidth() const override { return true; }
   QSize minimumSizeHint() const override;

private slots:
   virtual void onContentsChanged();

   QSize sizeHint() const override;

private:
   bool mResizeOnTextChange;
};

#endif
