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
#ifndef UTQTTEXTINPUT_HPP
#define UTQTTEXTINPUT_HPP

#include "utilqt_export.h"

#include <cstdio>
#include <sstream>
#include <string>

#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QtCore/QString>

class UtAngleR;
class UtIP_Addr;
class UtLatPos;
class UtLength;
class UtLonPos;
class UtReal;
class UtSpeed;
class UtTime;

class UtQtText
{
protected:
   UTILQT_EXPORT static bool InputIsValid(std::stringstream& aIn);

public:
   static QString ToQt(const std::string& aStr) { return QString(aStr.c_str()); }
   template<typename T>
   static QString ToQt(const T& aVal)
   {
      std::stringstream ss;
      ss << aVal;
      return QString::fromStdString(ss.str());
   }

   static bool FromQt(const QString& aIn, int& aOut)
   {
      bool ok;
      aOut = aIn.toInt(&ok);
      return ok;
   }
   static bool FromQt(const QString& aIn, double& aOut)
   {
      bool ok;
      aOut = aIn.toDouble(&ok);
      return ok;
   }
   template<typename T>
   static bool FromQt(const QString& aIn, T& aOut)
   {
      if (aIn.isEmpty())
         return false;
      std::stringstream ss;
      ss << (const char*)aIn.toLatin1();
      ss >> aOut;
      return InputIsValid(ss);
   }

   // Change the widget to indicate it contains a valid or invalid value.
   UTILQT_EXPORT static void SetValid(bool aIsValid, QWidget& aWidget);

   template<typename T>
   static bool GetValue(QLineEdit& aWidget, T& aValue)
   {
      bool ok = FromQt(aWidget.text().toLatin1(), aValue);
      SetValid(ok, aWidget);
      return ok;
   }
   template<typename T>
   static bool GetValue(QPushButton& aWidget, T& aValue)
   {
      aValue = (aWidget.isChecked() ? 1 : 0);
      return true;
   }
   template<typename T>
   static void SetValue(QLineEdit& aWidget, const T& aValue, bool aIsValid = true)
   {
      QString newText = ToQt(aValue);
      if (aWidget.text() != newText)
      {
         aWidget.setText(newText);
      }
      SetValid(aIsValid, aWidget);
   }
   template<typename T>
   static void SetValue(QPushButton& aWidget, const T& aValue, bool aIsValid = true)
   {
      aWidget.setChecked((bool)aValue);
   }

private:
};

#endif
