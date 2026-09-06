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
#include "UtQtText.hpp"

void UtQtText::SetValid(bool aIsValid, QWidget& aWidget)
{
   const QColor red(240, 64, 64);

   QPalette pal = aWidget.palette();
   QColor   activeColor, inactiveColor;

   if (aIsValid)
   {
      activeColor   = QApplication::palette().color(QPalette::Active, QPalette::Text);
      inactiveColor = QApplication::palette().color(QPalette::Inactive, QPalette::Text);
   }
   else
   {
      activeColor   = red;
      inactiveColor = red;
   }

   pal.setColor(QPalette::Active, QPalette::Text, activeColor);
   pal.setColor(QPalette::Inactive, QPalette::Text, inactiveColor);
   aWidget.setPalette(pal);
}

bool UtQtText::InputIsValid(std::stringstream& aIn)
{
   bool isValid = false;
   if (aIn)
   {
      // make sure there is nothing but trailing white space
      isValid = true;
      int nextChar;
      while ((nextChar = aIn.get()) != EOF)
      {
         if (!isspace(nextChar))
         {
            isValid = false;
            break;
         }
      }
   }
   return isValid;
}
