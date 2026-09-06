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

#include "UtQtTextEdit.hpp"

UtQtTextEdit::UtQtTextEdit(QWidget* aParent /*=0*/)
   : QTextEdit(aParent)
{
   mResizeOnTextChange = false;
}

void UtQtTextEdit::SetResizeOnTextChange(bool aResize)
{
   if (mResizeOnTextChange != aResize)
   {
      mResizeOnTextChange = aResize;
      if (mResizeOnTextChange)
      {
         connect(this, SIGNAL(textChanged()), SLOT(onContentsChanged()));
      }
      else
      {
         disconnect(this, SIGNAL(textChanged()), this, SLOT(onContentsChanged()));
      }
   }
}

QSize UtQtTextEdit::minimumSizeHint() const
{
   return QSize(100, 20);
}

void UtQtTextEdit::onContentsChanged()
{
   updateGeometry();
}

int UtQtTextEdit::heightForWidth(int w) const
{
   return (int)document()->size().height();
}

QSize UtQtTextEdit::sizeHint() const
{
   return QSize((int)document()->size().height(), 100);
}
