// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfTipContainer.hpp"

#include <QFile>
#include <QTextStream>

#include "WkfEnvironment.hpp"
#include "WkfPlugin.hpp"
#include "WkfPluginManager.hpp"
#include "WkfTip.hpp"

wkf::TipContainer::TipContainer(QObject* aParentPtr)
   : QObject(aParentPtr)
{
   connect(&wkfEnv,
           &Environment::Initialize,
           this,
           [this]()
           {
              ReadTipsFromFile();
              QueryPluginsForTips();
           });
}

void wkf::TipContainer::ReadTipsFromFile()
{
   // Grab tips from qrc
   // Note: to add a new tip, go to tips.txt in the ui directory and write a new tip
   QFile file(":/tips.txt");
   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QTextStream in(&file);
      QString     allText = in.readAll();
      // tips are delimited with "==="
      mTips = allText.split("===");

      // Remove whitespace at the beginning and end of the tip
      for (auto& tip : mTips)
      {
         tip = tip.trimmed();
      }

      UpdateDocLinks();
   }
}

void wkf::TipContainer::QueryPluginsForTips()
{
   QFileInfo fi(wkfEnv.GetDocumentationDir());
   if (fi.isDir())
   {
      QString dir(fi.absoluteFilePath().append("/html/docs/"));
      // Get the list of all the shortcuts from plugins
      auto plugins = wkfEnv.GetPluginManager()->GetLoadedPlugins();
      for (const auto& p : plugins)
      {
         auto tips = p.second->GetTips();
         for (auto& tip : tips)
         {
            QString text = "<b>" + tip.title + "</b> \n\n" + tip.text + "\n\nFor more information visit the \n<a href=\"" +
                           dir + tip.docLocation + "\">" + tip.docDescription + "</a> documentation.";
            mTips.append(text);
         }
      }
   }
}

void wkf::TipContainer::UpdateDocLinks()
{
   // Update a href links with the documentation directory
   QFileInfo fi(wkfEnv.GetDocumentationDir());
   if (fi.isDir())
   {
      QString dir(fi.absoluteFilePath().append("/html/docs/"));
      for (auto& tip : mTips)
      {
         tip.replace("<a doclink=\"", "<a href=\"" + dir);
      }
   }
}
