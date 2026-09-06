// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "Configuration.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtLexicalCast.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtSpan.hpp"
#include "UtStringUtil.hpp"

#include "CommunicationReport.hpp"
#include "DetectionReport.hpp"
#include "DSV_Output.hpp"
#include "EclipseReport.hpp"
#include "EngagementReport.hpp"
#include "Report.hpp"
#include "TrajectoryReport.hpp"

Configuration::Configuration(ut::span<char*> args)
   : Configuration::Configuration()
{
   ProcessCommandLineOptions(args);
}

Configuration::Configuration(const Configuration& aSrc)
   : mOptions(aSrc.mOptions),
     mReportType(aSrc.mReportType)
{
}

Configuration::Configuration(const std::string& aConfigurationFile)
   : Configuration::Configuration()
{
   mOptions.mConfigurationFile = aConfigurationFile;
}

void Configuration::ShowUsage()
{
   std::cout << "   Options:"
             << "\n     -?   Show this message."
             << "\n     -c   Configuration filename."
             << "\n     -r   Report type ['E' | 'EC' | 'C' | 'D' | 'T' | \"DSV\"]."
             << "\n     -n   Report filename."
             << "\n     -d   Data path for output."
             << "\n     -o   Output directory."
             << "\n     -s   (optional) Starting Monte-Carlo number."
             << "\n     -e   (optional) Ending Monte-Carlo number."
             << std::endl;
}

bool Configuration::Execute()
{
   ProcessConfigurationFile();
   Report& report = CreateReport();
   InitReport();

   std::cout << " Report Type: '" << GetReportTypeStr() << "'" << std::endl;

   return report.PrintReport();
}

void Configuration::ProcessCommandLineOptions(ut::span<char*> args)
{
   if (args.size() < 2)
   {
      ShowUsage();
      exit(1);
   }

   size_t argIndex = 1;
   size_t argCount = args.size();
   while (argIndex < argCount)
   {
      std::string argValue(args[argIndex]);
      if (argValue == "-r")
      {
         std::string rtStr = args[++argIndex];
         SetReportType(rtStr);
      }
      else if (argValue == "-c")
      {
         mOptions.mConfigurationFile = args[++argIndex];
      }
      else if (argValue == "-n")
      {
         mOptions.mReportName = args[++argIndex];
      }
      else if (argValue == "-d")
      {
         mOptions.mDataFile = args[++argIndex];
      }
      else if (argValue == "-o")
      {
         mOptions.mOutputDir = args[++argIndex];
      }
      else if (argValue == "-s")
      {
         mOptions.mStartMC = ut::lexical_cast<size_t>(args[++argIndex]);
      }
      else if (argValue == "-e")
      {
         mOptions.mEndMC = ut::lexical_cast<size_t>(args[++argIndex]);
      }
      else if ((argValue == "-?") ||
               (argValue == "-help"))
      {
         ShowUsage();
         exit(0);
      }
      else
      {
         std::cerr << "Unknown argument: " << argValue << '\n';
         ShowUsage();
         exit(1);
      }
      ++argIndex;
   }
}

void Configuration::ProcessConfigurationFile()
{
   if (mOptions.mConfigurationFile.empty()) { return; }

   UtInput input;
   input.PushInput(ut::make_unique<UtInputFile>(mOptions.mConfigurationFile));
   std::string command;
   while (input.TryReadCommand(command))
   {
      if (command == "report")
      {
         std::string typeStr;
         input.ReadValue(typeStr);
         SetReportType(typeStr);
         if (mReportType == Report::Type::cUNKNOWN)
         {
            std::cerr << "" << typeStr
                      << " is not a valid report type. Please change your configuration file's"
                      << "\"report\" option to one of the following:" << std::endl
                      << "Engagement, Eclipse, Detection, Communication, Trajectory, or DSV." << std::endl
                      << "Once this is done, rerun your program." << std::endl;
            exit(1);
         }
      }
      else if (command == "report_name")
      {
         input.ReadValueQuoted(mOptions.mReportName);
      }
      else if (command == "data_file")
      {
         input.ReadValueQuoted(mOptions.mDataFile);
      }
      else if (command == "output_directory")
      {
         input.ReadValueQuoted(mOptions.mOutputDir);
      }
      else if (command == "monte_carlo_start")
      {
         std::string startMC;
         input.ReadValue(startMC);
         mOptions.mStartMC = ut::lexical_cast<size_t>(startMC);
      }
      else if (command == "monte_carlo_end")
      {
         std::string endMC;
         input.ReadValue(endMC);
         mOptions.mEndMC = ut::lexical_cast<size_t>(endMC);
      }
   }
}

Report& Configuration::CreateReport()
{
   switch (mReportType)
   {
      case Report::Type::cCOMMUNICATION:
         mReportPtr = ut::make_unique<CommunicationReport>(mOptions);
         break;
      case Report::Type::cDETECTION:
         mReportPtr = ut::make_unique<DetectionReport>(mOptions);
         break;
      case Report::Type::cENGAGEMENT:
         mReportPtr = ut::make_unique<EngagementReport>(mOptions);
         break;
      case Report::Type::cTRAJECTORY:
         mReportPtr = ut::make_unique<TrajectoryReport>(mOptions);
         break;
      case Report::Type::cDSV:
         mReportPtr = ut::make_unique<DSV_Output>(mOptions);
         break;
      case Report::Type::cECLIPSE:
         mReportPtr = ut::make_unique<EclipseReport>(mOptions);
         break;
      default:
         throw UtException("***ERROR: Report type unknown. Cannot create a report.");
   }
   return *mReportPtr;
}

void Configuration::InitReport()
{
   if (mOptions.mConfigurationFile.empty()) { return; }

   UtInput input;
   input.PushInput(ut::make_unique<UtInputFile>(mOptions.mConfigurationFile));
   std::string command;
   while (input.TryReadCommand(command))
   {
      if (mReportPtr->ProcessInputConfiguration(input))
      {
      }
      else if (command == "report")
      {
         std::string dummy;
         input.ReadValue(dummy);
      }
      else if (command == "report_name")
      {
         std::string dummy;
         input.ReadValue(dummy);
      }
      else if (command == "data_file")
      {
         std::string dummy;
         input.ReadValue(dummy);
      }
      else if (command == "output_directory")
      {
         std::string dummy;
         input.ReadValue(dummy);
      }
      else if (command == "monte_carlo_start")
      {
         unsigned int dummy;
         input.ReadValue(dummy);
      }
      else if (command == "monte_carlo_end")
      {
         unsigned int dummy;
         input.ReadValue(dummy);
      }
      else
      {
         //throw UtInput::BadValue(input);
         std::cout << "****WARNING: Input configuration value '" << command << "' not recognized." << std::endl;
      }
   }
}

void Configuration::SetReportType(const std::string& aTypeString)
{
   mReportType = Report::Type::cUNKNOWN;
   std::string typeStr(aTypeString);
   UtStringUtil::ToLower(typeStr);
   if (typeStr == "communication" || typeStr == "c")
   {
      mReportType = Report::Type::cCOMMUNICATION;
   }
   else if (typeStr == "detection" || typeStr == "d")
   {
      mReportType = Report::Type::cDETECTION;
   }
   else if (typeStr == "dsv")
   {
      mReportType = Report::Type::cDSV;
   }
   else if (typeStr == "engagement" || typeStr == "e")
   {
      mReportType = Report::Type::cENGAGEMENT;
   }
   else if (typeStr == "trajectory" || typeStr == "t")
   {
      mReportType = Report::Type::cTRAJECTORY;
   }
   else if (typeStr == "eclipse" || typeStr == "ec")
   {
      mReportType = Report::Type::cECLIPSE;
   }
}
