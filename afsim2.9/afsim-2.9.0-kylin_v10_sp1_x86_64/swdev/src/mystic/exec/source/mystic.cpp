// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>

#include "RvEnvironment.hpp"
#include "RvFactory.hpp"
#include "RvRunManager.hpp"
#include "RvStartupDialog.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"
#include "UtQtException.hpp"
#include "VaPredefine.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfVtkEnvironment.hpp"
#include "mystic_version_defines.hpp"


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
   QString filePath    = QCoreApplication::applicationFilePath();
   QString fileName    = QFileInfo(filePath).fileName();

   const QString key = QStringLiteral("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\") + fileName;
   QSettings     settings(key, QSettings::NativeFormat);
   if (settings.status() != QSettings::NoError)
   {
      std::cerr << "Cannot access registry key" << key.toStdString();
      return false;
   }
   settings.setValue(QStringLiteral("FriendlyAppName"), displayName);

   settings.beginGroup(QStringLiteral("SupportedTypes"));

   settings.setValue(".aer", QString());

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

bool IsFileReadable(const QString& aFilename) noexcept
{
   return QFileInfo(aFilename).isReadable();
}

void ShowUsageDialog(const QString& aComplaint = "")
{
   QFileInfo f(qApp->arguments()[0]);
   QString   str;
   if (!aComplaint.isEmpty())
   {
      str = aComplaint + "\n\n";
   }
   str += "Usage: " + f.fileName() +
          " [input_file]\n"
          "\n"
          "-cf <filename>    Use the specified configuration file,\n"
          "                  modifications will be saved to specified file\n"
          "-icf <filename>   Imports the specified configuration file,\n"
          "                  modifications will not be saved to specified file\n"
          "-console          Enable output to the console window\n"
          "-?, -h, -help     Display command line options and quit\n";
   QDialog usage;
   usage.setLayout(new QVBoxLayout(&usage));
   QLabel* l = new QLabel(str, &usage);
   l->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
   usage.layout()->addWidget(l);
   QPushButton* ok = new QPushButton("OK", &usage);
   usage.layout()->addWidget(ok);
   QObject::connect(ok, &QPushButton::clicked, &usage, &QDialog::accept);
   usage.exec();
}

int rvExecute(QApplication& aApp)
{
   int returnCode = 0;
   aApp.setOrganizationName(MYSTIC_COMPANYNAME_STR);
   aApp.setOrganizationDomain(MYSTIC_COMPANYDOMAIN_STR);
   aApp.setApplicationName(MYSTIC_PRODUCTNAME_STR);
   aApp.setApplicationDisplayName(MYSTIC_PRODUCTNAME_STR);
   aApp.setApplicationVersion(MYSTIC_PRODUCTVERSION_STR);
   aApp.setWindowIcon(QIcon(":images/mystic_icon.ico"));

   if (qApp->arguments().contains("-?") || qApp->arguments().contains("-h") || qApp->arguments().contains("-help"))
   {
      ShowUsageDialog();
      return 0;
   }
   QString configFile;
   bool    importConfig = false;
   bool    showConsole  = false;
#ifdef _DEBUG
#ifdef _WIN32
   showConsole = true;
#endif
#endif
   QList<QString> args    = qApp->arguments();
   int            cfIndex = args.indexOf("-cf");
   if (cfIndex != -1)
   {
      if ((cfIndex + 1) < args.size())
      {
         if (!IsFileReadable(args[cfIndex + 1]))
         {
            QMessageBox::warning(nullptr,
                                 "Invalid Argument",
                                 QString("-cf file does not exist or cannot be opened.\nFile: %1").arg(args[cfIndex + 1]));
            auto out = ut::log::warning() << "-cf file does not exist or cannot be opened.";
            out.AddNote() << "File: " << args[cfIndex + 1].toStdString();
         }
         args.removeAt(cfIndex);
         configFile = args.at(cfIndex);
         args.removeAt(cfIndex);
      }
      else
      {
         ShowUsageDialog("-cf requires a configuration file");
         return 0;
      }
   }
   cfIndex = args.indexOf("-icf");
   if (cfIndex != -1)
   {
      if ((cfIndex + 1) < args.size())
      {
         if (!IsFileReadable(args[cfIndex + 1]))
         {
            QMessageBox::warning(nullptr,
                                 "Invalid Argument",
                                 QString("-icf file does not exist or cannot be opened.\nFile: %1").arg(args[cfIndex + 1]));
            auto out = ut::log::warning() << "-icf file does not exist or cannot be opened.";
            out.AddNote() << "File: " << args[cfIndex + 1].toStdString();
         }
         args.removeAt(cfIndex);
         configFile = args.at(cfIndex);
         args.removeAt(cfIndex);
         importConfig = true;
      }
      else
      {
         ShowUsageDialog("-icf requires a configuration file");
         return 0;
      }
   }
   int consoleIndex = args.indexOf("-console");
   if (consoleIndex != -1)
   {
      showConsole = true;
      args.removeAt(consoleIndex);
   }

   if (showConsole)
   {
#ifdef _WIN32
      bool has_console = ::AttachConsole(ATTACH_PARENT_PROCESS) == TRUE;
      if (!has_console)
      {
         // We weren't launched from a console, so make one.
         has_console = AllocConsole() == TRUE;
      }
      if (has_console)
      {
         for (auto& file : {stdout, stderr})
         {
            if (freopen("CONOUT$", "w", file))
            {
               setvbuf(file, nullptr, _IONBF, 0);
            }
         }
      }
#endif
   }

   // Create the Wkf and Vtk Environments
   wkf::VtkEnvironment::Create(new rv::Factory); // vaEnv

   // Create the environment which creates the main window
   wkf::Environment::Create("mystic", "rv", configFile, importConfig);
   rv::Environment::Create();
   rv::RunManager::Create();

   wkfEnv.StartUp();
   // Hide the Main Window until completion of the Start Dialog by the user.
   wkfEnv.GetMainWindow()->hide();
   vaEnv.DisableBoxSelection();
   rvEnv.UpdateMenus();

   QMenu* helpmenu = wkfEnv.GetMainWindow()->GetMenuByName("Help");
   if (helpmenu)
   {
      QList<QAction*> actions = helpmenu->actions();
      for (auto&& it : actions)
      {
         if (it->text() == "Documentation")
         {
            it->setVisible(true);
         }
         else if (it->text() == "About Mystic")
         {
            it->setText("About");
         }
      }
   }

   if (args.size() >= 2)
   {
      rvEnv.OpenEventRecording(args[1]);
   }
   else
   {
      QString file;
      // launch dialog if no scenario was specified and -ups was not given.
      rv::StartupDialog dialog(file, nullptr, Qt::MSWindowsFixedSizeDialogHint);
      if (dialog.exec() != QDialog::Rejected)
      {
         rvEnv.OpenEventRecording(file);
      }
   }

   wkfEnv.GetMainWindow()->show();
   // Enter the main event loop to start event handling.
   // exec() is a blocking call that does not return until quit() is called or last primary window is closed
   returnCode = aApp.exec();

   // cleanup the environment
   rv::RunManager::GetInstance().Shutdown();
   rvEnv.Shutdown();
   wkfEnv.Shutdown();
   return returnCode;
}
} // namespace

int main(int aArgc, char* aArgv[])
try
{
   ut::SetApplicationErrorHandling(&ut::qt::TerminateHandler);
   ut::SetupApplicationLog("Mystic", MYSTIC_VERSION, "mystic-exception.log");

   QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
   QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
   QApplication app(aArgc, aArgv);

   // Inner try-catch to create exception report popup before QApplication close
   try
   {
      return rvExecute(app);
   }
   catch (...)
   {
      ut::qt::ReportException("Mystic", std::current_exception());
      return 1;
   }
}
catch (...)
{
   ut::qt::ReportException("Mystic", std::current_exception());
   return 1;
}
