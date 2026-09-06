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

#include "UtoShaderUtilities.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "UtLog.hpp"
#include "UtPath.hpp"

bool UtoShaderUtilities::mShadersEnabled         = false;
bool UtoShaderUtilities::mGeometryShadersEnabled = false;

std::string UtoShaderUtilities::PreprocessFile(const std::string& aFilename)
{
   std::set<std::string>    defs;
   std::string              retval;
   std::vector<std::string> searchPath;
   UtPath                   dir(aFilename);
   dir.Up();
   searchPath.push_back(dir.GetSystemPath());
   PreprocessShaderFile(aFilename, retval, defs, searchPath);
   return retval;
}

std::string UtoShaderUtilities::PreprocessFile(const std::string& aFilename, const std::set<std::string>& aDefs)
{
   std::string              retval;
   std::vector<std::string> searchPath;
   UtPath                   dir(aFilename);
   dir.Up();
   searchPath.push_back(dir.GetSystemPath());
   PreprocessShaderFile(aFilename, retval, aDefs, searchPath);
   return retval;
}

std::string UtoShaderUtilities::PreprocessFile(const std::string&           aFilename,
                                               const std::set<std::string>& aDefs,
                                               std::vector<std::string>&    aSearchPath)
{
   std::string retval;
   PreprocessShaderFile(aFilename, retval, aDefs, aSearchPath);
   return retval;
}

void UtoShaderUtilities::PreprocessShaderFile(const std::string&           aFilename,
                                              std::string&                 aOutput,
                                              const std::set<std::string>& aDefs,
                                              std::vector<std::string>&    aIncludeSearchPath)
{
   std::string lSourceDirectoryString;
   std::size_t lastBackSlash = aFilename.find_last_of('\\');
   std::size_t lastSlash     = aFilename.find_last_of('/');
   std::size_t lastEither =
      (((lastBackSlash > lastSlash) && (lastBackSlash != std::string::npos)) || (lastSlash == std::string::npos)) ?
         lastBackSlash :
         lastSlash;
   if (lastEither != std::string::npos)
   {
      lSourceDirectoryString += aFilename.substr(0, lastEither + 1);
   }

   // build up the source directory string
   std::ifstream lfShaderFile(aFilename.c_str());
   if (!lfShaderFile)
   {
      auto out = ut::log::info() << "Could not open shader.";
      out.AddNote() << "File: " << aFilename;
      return;
   }

   // make them into preprocessor defines
   std::string defines;
   for (std::set<std::string>::const_iterator it = aDefs.begin(); it != aDefs.end(); ++it)
   {
      defines += "#define " + *it + '\n';
   }

   int lineNum = 0;
   if (!aOutput.empty())
   {
      // this is an included file or defines were added, reset the line number
      aOutput += LineCount(lineNum);
   }

   std::string  line;
   unsigned int startOfLine;
   while (std::getline(lfShaderFile, line))
   {
      ++lineNum;
      if ('#' == FirstCharacterOfSignificance(line, startOfLine) &&
          HandleDirective(aFilename, line, lSourceDirectoryString, defines, aOutput, aIncludeSearchPath))
      {
         // don't add to output, reset line numbers, needed for #includes
         aOutput += LineCount(lineNum);
      }
      else
      {
         aOutput += line + '\n';
      }
   }

   if (!defines.empty())
   {
      // no #version, put defines at the start
      aOutput = defines + LineCount(0) + aOutput;
   }
}

char UtoShaderUtilities::FirstCharacterOfSignificance(const std::string& aString, unsigned int& aStart)
{
   unsigned int place  = 0;
   const char*  strPtr = aString.c_str();
   while ((strPtr[place] == ' ') || (strPtr[place] == '\t'))
   {
      ++place;
   }
   aStart = place;
   if (aStart < aString.length())
   {
      return strPtr[aStart];
   }
   else
   {
      return ' ';
   }
}

bool UtoShaderUtilities::HandleDirective(const std::string&        aFilename,
                                         const std::string&        aStrPtr,
                                         const std::string&        aSourceDirectoryString,
                                         std::string&              aDefines,
                                         std::string&              aOutput,
                                         std::vector<std::string>& aIncludeSearchPath)
{
   std::stringstream ss;
   ss.str(aStrPtr);
   std::string commandWord;
   ss >> commandWord;
   std::string secondWord;
   ss >> secondWord;
   // deal with whitespace after # shift down
   if (commandWord == "#")
   {
      commandWord = secondWord;
      ss >> secondWord;
   }
   bool processDirective = false;
   if (!aDefines.empty() && (commandWord == "#version" || commandWord == "version"))
   {
      // #version first, #defines after
      aOutput += aStrPtr + '\n' + aDefines;
      // clear now that they've been inserted into the output
      aDefines.clear();
      processDirective = true;
   }
   else if ((commandWord == "#include" || commandWord == "include") && !secondWord.empty() &&
            (*secondWord.begin() == '"' || *secondWord.begin() == '<') &&
            (*secondWord.rbegin() == '"' || *secondWord.rbegin() == '>'))
   {
      std::string filename  = secondWord.substr(1, secondWord.size() - 2);
      std::string foundFile = SearchPath(filename, aIncludeSearchPath);
      if (foundFile.empty())
      {
         auto out = ut::log::error() << "Could not find include file.";
         out.AddNote() << "File: " << filename;
      }
      else
      {
         // better not be a recursive #include because at this point
         // conditionals aren't checked
         PreprocessShaderFile(foundFile, aOutput, std::set<std::string>(), aIncludeSearchPath);
      }
      processDirective = true;
   }
   return processDirective;
}

std::string UtoShaderUtilities::SearchPath(const std::string& aFilename, std::vector<std::string>& aIncludeSearchPath)
{
   if (UtPath(aFilename).Stat() == UtPath::cFILE)
      return aFilename;
   for (size_t i = 0; i < aIncludeSearchPath.size(); ++i)
   {
      UtPath path = UtPath(aIncludeSearchPath[i]) + aFilename;
      if (path.Stat() == UtPath::cFILE)
         return path.GetSystemPath();
   }
   static std::set<std::string> filesNotFound;
   if (filesNotFound.find(aFilename) == filesNotFound.end())
   {
      auto out = ut::log::warning() << "Shader not found.";
      out.AddNote() << "File: " << aFilename;
   }
   return std::string(); // not found
}

std::string UtoShaderUtilities::LineCount(int aLineNum)
{
   // GLSL directive, the next line will be treated as aLineNum+1
   std::stringstream ss;
   ss << "#line " << aLineNum << '\n';
   return ss.str();
}

// this code assumes a scaled result [0,1)
// SHADER CODE EQUIVALENT:
// float Unpackage(vec4 aRGBA)
// {
//    const vec4 bitshift = vec4(1.0 / (16581375.0), 1.0 / (65025.0), 1.0 / 255.0, 1.0);
//    return (dot(aRGBA, bitshift));
// }
// that's 255^1 = 255, 255^2 = 65025, 255^3 = 16581375
// There are rounding errors in using these functions, and a times they are off
// by 254.
double UtoShaderUtilities::RGBA_ToDouble(unsigned char aR, unsigned char aG, unsigned char aB, unsigned char aA)
{
   return aR / (255.0 * 16581375.0) + aG / (255.0 * 65025.0) + aB / (255.0 * 255.0) + aA / 255.0;
}

// this code assumes a scaled aValue [0,1)
// SHADER CODE EQUIVALENT:
// vec4 Package(float aValue)
// {
//    const vec4 bitshift = vec4(16581375.0, 65025.0, 255.0, 1.0);
//    const vec4 bitmask = vec4(0.0, 1.0 / 255.0, 1.0 / 255.0, 1.0 / 255.0);
//    vec4 res = fract(aValue * bitshift);
//    res -= res.xxyz * bitmask;
//    return res;
// }
void UtoShaderUtilities::DoubleToRGBA(double aInput, unsigned char& aR, unsigned char& aG, unsigned char& aB, unsigned char& aA)
{
   if (aInput == 1.0)
   {
      aR = 255;
      aG = 255;
      aB = 255;
      aA = 255;
      return;
   }
   double dr = aInput * 16581375.0 - (int)(aInput * 16581375.0);
   double dg = aInput * 65025.0 - (int)(aInput * 65025.0);
   double db = aInput * 255.0 - (int)(aInput * 255.0);
   double da = aInput - (int)aInput;
   dg -= dr / 255.0;
   db -= dg / 255.0;
   da -= db / 255.0;
   aR = (unsigned char)(dr * 255.0);
   aG = (unsigned char)(dg * 255.0);
   aB = (unsigned char)(db * 255.0);
   aA = (unsigned char)(da * 255.0);
}
