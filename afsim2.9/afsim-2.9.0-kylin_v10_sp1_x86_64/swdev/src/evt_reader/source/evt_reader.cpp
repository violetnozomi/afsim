// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>

#include "EventModel.hpp"
#include "EventTableDialog.hpp"
#include "evt_reader_version_defines.hpp"
#include "UtPath.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
#ifdef _WIN32
   // To enable the application jump list to show the desired recent files, the corresponding file types are
   // registered using the following helper function.
   bool associateFileTypes()
   {
      QString displayName = QGuiApplication::applicationDisplayName();
      QString filePath = QCoreApplication::applicationFilePath();
      QString fileName = QFileInfo(filePath).fileName();

      const QString key = QStringLiteral("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
      QSettings settings(key, QSettings::NativeFormat);
      if (settings.status() != QSettings::NoError)
      {
         std::cerr << "Cannot access registry key" << key.toStdString();
         return false;
      }
      settings.setValue(QStringLiteral("FriendlyAppName"), displayName);

      settings.beginGroup(QStringLiteral("SupportedTypes"));

      settings.setValue(".evt", QString());

      settings.endGroup();

      settings.beginGroup(QStringLiteral("shell"));
      settings.beginGroup(QStringLiteral("open"));
      settings.setValue(QStringLiteral("FriendlyAppName"), displayName);
      settings.beginGroup(QStringLiteral("Command"));
      settings.setValue(QStringLiteral("."),
         QLatin1Char('"') + QDir::toNativeSeparators(filePath) + QStringLiteral("\" \"%1\""));

      return true;
   }
#endif
   void Open(EventTableDialog* aDialog, const QString& aFilename)
   {
      if (!aFilename.isEmpty())
      {
         aDialog->SetModel(new EventModel(aDialog, aFilename));
      }
   }
   void Open(EventTableDialog* aDialog)
   {
      QString filename = QFileDialog::getOpenFileName(aDialog, "Open EVT File", "", "AFSIM Event File (*.evt)");
      Open(aDialog, filename);
   }

   void UsageWarning()
   {
      QMessageBox::critical(nullptr, "Error, improper usage", "Usage: evt_reader [inputfile] [--filter filterfile] [--out outputfile]");
   }

   int evtExecute(QApplication& aApp)
   {
      int returnCode = 0;
      aApp.setOrganizationName(EVT_READER_COMPANYNAME_STR);
      aApp.setOrganizationDomain(EVT_READER_COMPANYDOMAIN_STR);
      aApp.setApplicationName(EVT_READER_PRODUCTNAME_STR);
      aApp.setApplicationDisplayName(EVT_READER_PRODUCTNAME_STR);
      aApp.setApplicationVersion(EVT_READER_PRODUCTVERSION_STR);

      QString openfile;
      QString filter;
      QString outfilename;
      if (aApp.arguments().size() > 1)
      {
         openfile = aApp.arguments()[1];
         if (openfile.startsWith("--"))
         {
            UsageWarning();
            exit(1);
         }
         int cmd = 0;
         unsigned int i = 0;
         for (auto&& arg : aApp.arguments())
         {
            if (cmd == 1)
            {
               filter = arg;
               cmd = 0;
            }
            else if (cmd == 2)
            {
               outfilename = arg;
               cmd = 0;
            }
            else if (arg == "--filter")
            {
               cmd = 1;
            }
            else if (arg == "--out")
            {
               cmd = 2;
            }
            else
            {
               if (i > 1)
               {
                  UsageWarning();
                  exit(1);
               }
            }
            ++i;
         }
      }
      if (!outfilename.isEmpty())
      {
         auto em = new EventModel(nullptr, openfile);
         QAbstractItemModel* top = em;

         if (!filter.isEmpty())
         {
            QFile file(filter);
            file.open(QIODevice::ReadOnly);
            QTextStream ts(&file);
            if (file.isOpen())
            {
               while (!ts.atEnd())
               {
                  int col;
                  ts >> col;
                  int type;
                  ts >> type;
                  if (ts.atEnd())
                  {
                     break;
                  }
                  QString str;
                  ts >> str;
                  auto fr = new EventFilterModel(top, col, type, str);
                  fr->setSourceModel(top);
                  top = fr;
               }
               file.close();
            }
         } // read and stack the filters
         QFile outfile(outfilename);
         outfile.open(QIODevice::WriteOnly);
         if (outfile.isOpen())
         {
            for (int i = 0; i < top->rowCount(); ++i)
            {
               QString data = top->data(top->index(i, 0), Qt::UserRole).toString();
               outfile.write(data.toLocal8Bit());
               // write the row
            }
            outfile.close();
         }
         // query the data
         delete em;

         //cleanup the environment
         return 0;
      }
      else
      {
         QMainWindow window;
         window.show();
         window.setMenuBar(new QMenuBar(&window));
         window.setMinimumSize(800, 600);
         auto eventTable = new EventTableDialog(&window);
         window.setCentralWidget(eventTable);
         QMenu* filemenu = window.menuBar()->addMenu("File");
         QAction* openaction = filemenu->addAction("Open");
         QObject::connect(openaction, &QAction::triggered, [eventTable]() { Open(eventTable); });
         QAction* quitaction = filemenu->addAction("Quit");
         QObject::connect(quitaction, &QAction::triggered, &aApp, &QApplication::closeAllWindows);

         if (!openfile.isEmpty())
         {
            Open(eventTable, aApp.arguments()[1]);
            if (!filter.isEmpty())
            {
               eventTable->LoadFilter(filter);
            }
         }
         // Enter the main event loop to start event handling.
         // exec() is a blocking call that does not return until quit() is called or last primary window is closed
         returnCode = aApp.exec();

         //cleanup the environment
         return returnCode;
      }
   }
}

int main(int   aArgc,
         char* aArgv[])
{
   QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
   QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
   QApplication app(aArgc, aArgv);

   return evtExecute(app);
}
