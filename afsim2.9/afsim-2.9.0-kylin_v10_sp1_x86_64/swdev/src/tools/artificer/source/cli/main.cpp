// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <exception>
#include <iostream>
#include <vector>

#include "UtPath.hpp"
#include "transformFile.hpp"

namespace
{
struct CliArgs
{
   std::string outputFormat = "v1prototype";
   UtPath      inputFilePath;
};

void printUsage(const std::string& exe)
{
   std::cerr << "Usage: " << exe << " [-h|--help] | [{-f|--output-format} <output-format>] <input-file>" << std::endl
             << " -h - Show this help" << std::endl
             << " -f - Choose output format, default \"v1prototype\". Must be one of:" << std::endl
             << "      v1prototype - Feature preview output format." << std::endl;
   exit(1);
}

CliArgs parseArgs(const std::vector<std::string>& stringArgs)
{
   CliArgs args;
   for (auto curArg = stringArgs.begin() + 1; curArg != stringArgs.end(); ++curArg)
   {
      if (*curArg == "-h" || *curArg == "--help")
      {
         printUsage(stringArgs[0]);
      }
      else if (std::next(curArg) == stringArgs.end())
      {
         args.inputFilePath = *curArg;
      }
      else if (*curArg == "-f" || *curArg == "--output-format")
      {
         ++curArg;
         args.outputFormat = *curArg;
      }
      else
      {
         std::cerr << "Invalid argument: " << *curArg << std::endl;
         printUsage(stringArgs[0]);
      }
   }
   if (args.inputFilePath.IsNull() || !args.inputFilePath.Exists())
   {
      std::cerr << "Argument error: input-file is missing or does not exist" << std::endl;
      printUsage(stringArgs[0]);
   }
   return args;
}

CliArgs parseArgs(int argc, char** argv)
{
   std::vector<std::string> stringArgs;
   for (int x = 0; x < argc; ++x)
   {
      stringArgs.push_back(argv[x]);
   }
   return parseArgs(stringArgs);
}
} // namespace


int main(int argc, char** argv)
{
   const CliArgs args = parseArgs(argc, argv);
   try
   {
      artificer::TransformFile(args.outputFormat, args.inputFilePath);
   }
   catch (const std::exception& e)
   {
      std::cerr << "Error encountered while processing " << args.inputFilePath.GetSystemPath() << ":\n    " << e.what()
                << '\n';
      return 1;
   }
   return 0;
}
