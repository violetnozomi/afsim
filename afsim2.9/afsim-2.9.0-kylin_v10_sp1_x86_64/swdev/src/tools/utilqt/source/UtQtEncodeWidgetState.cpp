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
#include "UtQtEncodeWidgetState.hpp"

#include <sstream>

#include <QAbstractButton>
#include <QComboBox>
#include <QLineEdit>
// Currently supports buttons, combo boxes(with fixed set of options), and line edits

std::string UtQtEncodeWidgetState(QWidget* aWidgetPtr)
{
   QLineEdit*       lineEditPtr = dynamic_cast<QLineEdit*>(aWidgetPtr);
   QAbstractButton* buttonPtr   = dynamic_cast<QAbstractButton*>(aWidgetPtr);
   QComboBox*       comboBoxPtr = dynamic_cast<QComboBox*>(aWidgetPtr);
   if (lineEditPtr)
   {
      return lineEditPtr->text().toStdString();
   }
   else if (comboBoxPtr)
   {
      std::stringstream ss;
      ss << comboBoxPtr->currentIndex();
      return ss.str();
   }
   else if (buttonPtr)
   {
      if (buttonPtr->isCheckable())
      {
         return buttonPtr->isChecked() ? "true" : "false";
      }
   }
   return std::string();
}

bool UtQtDecodeWidgetState(const std::string& aStateString, QWidget* aWidgetPtr)
{
   QLineEdit*       lineEditPtr = dynamic_cast<QLineEdit*>(aWidgetPtr);
   QAbstractButton* buttonPtr   = dynamic_cast<QAbstractButton*>(aWidgetPtr);
   QComboBox*       comboBoxPtr = dynamic_cast<QComboBox*>(aWidgetPtr);
   if (lineEditPtr)
   {
      lineEditPtr->setText(QString::fromStdString(aStateString));
      return true;
   }
   else if (comboBoxPtr)
   {
      std::stringstream ss(aStateString);
      int               index;
      ss >> index;
      comboBoxPtr->setCurrentIndex(index);
      return true;
   }
   else if (buttonPtr)
   {
      if (buttonPtr->isCheckable())
      {
         if (aStateString == "true")
         {
            buttonPtr->setChecked(true);
            return true;
         }
         if (aStateString == "false")
         {
            buttonPtr->setChecked(false);
            return true;
         }
      }
   }
   return false;
}
