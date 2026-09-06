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
#include "UtQtCSSLoader.hpp"

#include <QtCore/QFile>
#include <QtCore/QTextStream>

QString CSSLoader(QString aFileName)
{
   QFile css_file(aFileName);
   css_file.open(QIODevice::ReadOnly);
   QTextStream stream(&css_file);
   QString     css;
   QString     streamString;
   // create a single string out of the css file
   do
   {
      streamString = stream.readLine();
      css.append(streamString);
   } while (!streamString.isNull());

   // return the css file as a string to pass to QWidget::setStyleSheet(QString)
   return css;
}
