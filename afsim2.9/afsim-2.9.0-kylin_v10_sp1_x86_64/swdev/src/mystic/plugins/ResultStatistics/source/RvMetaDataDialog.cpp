// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvMetaDataDialog.hpp"

#include "ui_RvMetaDataDialog.h"

RvStatistics::MetaDataDialog::MetaDataDialog(QWidget* aParent)
   : QDialog(aParent)
{
   mUi.setupUi(this);
}

void RvStatistics::MetaDataDialog::SetData(const std::vector<QStringList>& aStringMap)
{
   QString text;
   for (auto&& it : aStringMap)
   {
      bool list  = (it.size() > 2);
      bool first = true;

      for (auto&& jt : it)
      {
         if (first)
         {
            text += jt + ": ";
            if (list)
            {
               text += "<ul>";
            }
            first = false;
         }
         else
         {
            if (list)
            {
               text += "<li><b>" + jt + "<b></li>";
            }
            else
            {
               text += "<b>" + jt + "</b><br>";
            }
         }
      }
      if (list)
      {
         text += "</ul>";
      }
   }
   mUi.textEdit->setText(text);
}
