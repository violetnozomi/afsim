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

#include "WkfMilStd2525d.hpp"

#include <unordered_set>

#include <QMessageBox>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtRunEnvManager.hpp"
#include "VaEnvironment.hpp"
#include "VaModelDatabase.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMilStdIconPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
const std::string cMIL_STD_PREFIX = "milStdModel_";

std::string GetStdIdentityFromSide(WkfMilStd2525d::Side aSide)
{
   switch (aSide)
   {
   case WkfMilStd2525d::Side::FRIENDLY:
      return "03";
   case WkfMilStd2525d::Side::NEUTRAL:
      return "04";
   case WkfMilStd2525d::Side::HOSTILE:
      return "06";
   default:
      return "01";
   }
}

std::string GetSymbolSetFromDomain(const wkf::SpatialDomain& aDomain)
{
   switch (aDomain)
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
      return "01";
   case wkf::SpatialDomain::SPATIAL_DOMAIN_LAND:
      return "10";
   case wkf::SpatialDomain::SPATIAL_DOMAIN_SPACE:
      return "05";
   case wkf::SpatialDomain::SPATIAL_DOMAIN_SUBSURFACE:
      return "35";
   case wkf::SpatialDomain::SPATIAL_DOMAIN_SURFACE:
      return "30";
   default:
      return "00";
   }
}

WkfMilStd2525d::SymbolIdentificationCode SIDC_FromString(const std::string& aString, bool* okPtr = nullptr)
{
   bool                                     valid = false;
   WkfMilStd2525d::SymbolIdentificationCode retVal;
   // Set A is 10 characters, and Set B is 10 characters,
   // We do not handle the condition set C characters
   if (aString.length() == 20 || (aString.length() > 20 && aString.find_first_not_of("0123456789") == 20))
   {
      retVal.mVersion       = aString.substr(0, 2);
      retVal.mStdIdentity   = aString.substr(2, 2);
      retVal.mSymbolSet     = aString.substr(4, 2);
      retVal.mStatus        = aString.substr(6, 1);
      retVal.mHQDummy       = aString.substr(7, 1);
      retVal.mAmplifier     = aString.substr(8, 2);
      retVal.mEntity        = aString.substr(10, 2);
      retVal.mEntityType    = aString.substr(12, 2);
      retVal.mEntitySubtype = aString.substr(14, 2);
      retVal.mSector1Mod    = aString.substr(16, 2);
      retVal.mSector2Mod    = aString.substr(18, 2);
      if (aString.length() > 20)
      {
         retVal.mExtra = aString.substr(20);
      }
      valid = true;
   }
   else
   {
      auto log = ut::log::warning("Invalid SIDC in WkfMilStd2525d::SIDC_FromString()");
      log.AddNote("string = " + aString);
   }

   // If the optional okay pointer was set, set the value to the validity of SIDC code provided.
   if (okPtr)
   {
      *okPtr = valid;
   }

   return retVal;
}
} // namespace

WkfMilStd2525d::WkfMilStd2525d(const QStringList& aFileList)
{
   // Exception to match UtInput behavior when loading in models (this is simple enough to be a csv instead, but should
   // still be consistent)
   try
   {
      int failCount = 0;
      for (int i = 0; i < aFileList.size(); i++)
      {
         const QString& fileName = aFileList.at(i);
         unsigned int   line     = 1;
         QFile          file(fileName);
         if (file.open(QIODevice::ReadOnly))
         {
            while (!file.atEnd())
            {
               auto splitLine = file.readLine().split(',');
               // Do all comparisons in lowercase (due to occasional mismatches in icon case)
               std::string code = splitLine[1].simplified().toLower().toStdString();
               if (splitLine.count() == 2 && code.size() == 20 && code.find_first_not_of("0123456789") == std::string::npos)
               {
                  mIconMappings[splitLine[0]] = code;
               }
               else
               {
                  std::string errorString =
                     "Mil-Std Mapping input file incorrectly formatted on line " + std::to_string(line) + ".";
                  throw UtException(errorString);
               }
               line++;
            }
            file.close();
         }
         else
         {
            failCount++;
         }
      }
      if (failCount == aFileList.size())
      {
         std::string errorStr;
         for (int i = 0; i < aFileList.size(); i++)
         {
            errorStr.append("\n  " + aFileList.at(i).toStdString());
         }
         throw UtException("Unable to open Mil-Std mapping input files.\n Checked the following locations:" + errorStr);
      }
   }
   catch (UtException& e)
   {
      QMessageBox::warning(nullptr, "Invalid Mil-Std Mappings", QString::fromStdString(e.what()));
      auto out = ut::log::error() << e.what();
   }

   SetResourcePath();
   LoadMilStdIcons();
}

std::string WkfMilStd2525d::GetFilePathForClosestMatch(const std::string& aMilStdString) const
{
   return GetFilePathForClosestMatch(SIDC_FromString(aMilStdString));
}

// Main search function.
// First checks if there is already a file, if not grabs closest match
std::string WkfMilStd2525d::GetFilePathForClosestMatch(const SymbolIdentificationCode& aMilStdData) const
{
   std::string filePath = GetFilePath(aMilStdData);
   if (filePath.empty())
   {
      filePath = GetFilePath(GetClosestMatch(aMilStdData));
      if (filePath.empty())
      {
         ut::log::warning() << "Failed to find a mil-std2525d icon: " + aMilStdData.ToString();
      }
   }
   return filePath;
}

std::string WkfMilStd2525d::GetModelName(const PartialInput& aInput) const
{
   return GetModelName(GetSIDC_FromInput(aInput));
}

// Adds a custom model to the model database, then returns the model name
std::string WkfMilStd2525d::GetModelName(const SymbolIdentificationCode& aSIDC) const
{
   std::string modelName = cMIL_STD_PREFIX + aSIDC.ToString();
   auto        modelList = vaEnv.GetModelDatabase()->GetNameList();
   if (modelList.count(modelName) < 1)
   {
      vespa::VaModelDefinition newModel(GetFilePathForClosestMatch(aSIDC), 1.0f, osg::Vec3(0, 0, 0), true, false);
      vaEnv.GetModelDatabase()->AddCustomModelDefinition(modelName, newModel);
      // It is the caller's responsibility to reload models after all requests are fulfilled:
      // vaEnv.GetModelDatabase()->ReloadModels();
   }
   return modelName;
}

// Grab the source root to tell if models is located there
// If not, use resources folder. Site path always uses resources folder
void WkfMilStd2525d::SetResourcePath()
{
   std::string sourceRootStr;
   const char* sourceRoot = ::getenv("SOURCE_ROOT");
   if (sourceRoot)
   {
      sourceRootStr = sourceRoot;
   }

   if (!sourceRootStr.empty())
   {
      mMilDir = sourceRootStr + "/tools/vespatk/models/mil-std2525d/";
   }
   else
   {
      mMilDir = wkfEnv.GetResourcesDir().toStdString() + "/models/mil-std2525d/";
   }
   mSiteDir = wkfEnv.GetResourcesDir().toStdString() + "/site/mil-std2525d/";
}

// Load image files into vector to search for closest match
void WkfMilStd2525d::LoadMilStdIcons()
{
   auto                     milPathPtr  = ut::make_unique<UtPath>(mMilDir);
   auto                     sitePathPtr = ut::make_unique<UtPath>(mSiteDir);
   std::vector<std::string> files, dirs;

   try
   {
      // Use a set to enforce uniqueness. An icon may exist in both Models and the Site directories,
      // we do not want two entries if the same model is in both locations.
      std::unordered_set<std::string> setOfFiles;
      // Grab files from mil-std2525d path
      if (milPathPtr->ListDir(files, dirs))
      {
         for (const std::string& file : files)
         {
            setOfFiles.insert(file);
         }
      }
      files.clear();
      dirs.clear();
      // Grab files from resource/site path
      if (sitePathPtr->ListDir(files, dirs))
      {
         for (const std::string& file : files)
         {
            setOfFiles.insert(file);
         }
      }

      // save some copies of the vector since hundreds of icons may be in these directories.
      mCompleteMilStdFileList.resize(setOfFiles.size());
      // Move elements from set into the vector.
      bool valid = false;
      for (const auto& item : setOfFiles)
      {
         SymbolIdentificationCode sidc = SIDC_FromString(item, &valid);
         // Make sure the name of the file was a valid SIDC before inserting in the File List.
         if (valid)
         {
            mCompleteMilStdFileList.push_back(sidc);
         }
      }
   }
   catch (const std::exception& e)
   {
      ut::log::error(e.what());
   }
}

// Creates a path string to the image file
std::string WkfMilStd2525d::GetFilePath(const SymbolIdentificationCode& aSIDC) const
{
   // Site directory takes precedence over the Models directory, so check it first.
   UtPath sitePath(mSiteDir + "/" + aSIDC.ToString());
   if (sitePath.IsFile())
   {
      return sitePath.GetNormalizedPath();
   }

   UtPath milPath(mMilDir + "/" + aSIDC.ToString());
   if (milPath.IsFile())
   {
      return milPath.GetNormalizedPath();
   }
   return "";
}

WkfMilStd2525d::SymbolIdentificationCode WkfMilStd2525d::GetSIDC_FromInput(const PartialInput& aInput) const
{
   SymbolIdentificationCode sidc;
   // If the Entity Type is reported, use the mil-std string mapped to that entity type
   if (!aInput.mEntityType.empty())
   {
      QString icon = QString::fromStdString(aInput.mEntityType).toLower();
      if (mIconMappings.count(icon) > 0)
      {
         sidc = SIDC_FromString(mIconMappings.value(icon));
      }
   }
   else
   {
      sidc.mSymbolSet = GetSymbolSetFromDomain(aInput.mSpatialDomain);
   }
   // Overwrite the side specified in the file, with the actual side the entity is on.
   sidc.mStdIdentity = GetStdIdentityFromSide(aInput.mSide);
   return sidc;
}

// static
WkfMilStd2525d::Side WkfMilStd2525d::GetSideFromTeam(const std::string& aTeam)
{
   auto milStdPrefs = wkfEnv.GetPreferenceObject<wkf::MilStdIconPrefObject>();
   if (milStdPrefs->GetFriendTeams().count(QString::fromStdString(aTeam)) == 1)
   {
      return WkfMilStd2525d::Side::FRIENDLY;
   }
   if (milStdPrefs->GetNeutralTeams().count(QString::fromStdString(aTeam)) == 1)
   {
      return WkfMilStd2525d::Side::NEUTRAL;
   }
   if (milStdPrefs->GetHostileTeams().count(QString::fromStdString(aTeam)) == 1)
   {
      return WkfMilStd2525d::Side::HOSTILE;
   }
   return WkfMilStd2525d::Side::UNKNOWN;
}

// Loop through each file and check the difference of each file name.
// This counts the number of differences from the input object.
// Returns the object with the fewest differences between input string
WkfMilStd2525d::SymbolIdentificationCode WkfMilStd2525d::GetClosestMatch(const SymbolIdentificationCode& aSIDC) const
{
   std::vector<SymbolIdentificationCode> matchingIdentityList;
   // First loop over all loaded SIDC icons and find ones that match Team
   for (const auto& sidc : mCompleteMilStdFileList)
   {
      if (sidc.mStdIdentity == aSIDC.mStdIdentity)
      {
         matchingIdentityList.push_back(sidc);
      }
   }

   if (matchingIdentityList.size() == 1)
   {
      return matchingIdentityList.at(0);
   }
   else if (matchingIdentityList.empty())
   {
      return GetClosestMatchFromList(aSIDC, mCompleteMilStdFileList);
   }

   std::vector<SymbolIdentificationCode> matchingSymbolSetList;
   // Second loop over all remaining icons and find ones that match Domain
   for (const auto& sidc : matchingIdentityList)
   {
      if (sidc.mSymbolSet == aSIDC.mSymbolSet)
      {
         matchingSymbolSetList.push_back(sidc);
      }
   }

   if (matchingSymbolSetList.size() == 1)
   {
      return matchingSymbolSetList.at(0);
   }
   else if (matchingSymbolSetList.empty())
   {
      return GetClosestMatchFromList(aSIDC, matchingIdentityList);
   }

   return GetClosestMatchFromList(aSIDC, matchingSymbolSetList);
}

WkfMilStd2525d::SymbolIdentificationCode
WkfMilStd2525d::GetClosestMatchFromList(const SymbolIdentificationCode&              aSIDC,
                                        const std::vector<SymbolIdentificationCode>& aSIDC_list) const
{
   int         diff       = 0;
   int         strLength  = 0;
   std::string aSIDC_str  = aSIDC.ToString();
   std::string sidcStr    = "";
   int         lowestDiff = 1000; // a big number

   SymbolIdentificationCode closestMatch;
   //
   // Find the one that matches the closest
   for (const auto& sidc : aSIDC_list)
   {
      diff    = 0;
      sidcStr = sidc.ToString();

      strLength = std::min(static_cast<int>(sidcStr.size()), static_cast<int>(aSIDC_str.size()));
      for (int i = 0; i < strLength; i++)
      {
         // Ignore the 0's from the input string can throw result off
         if (sidcStr[i] != aSIDC_str[i] && aSIDC_str[i] != '0')
         {
            diff++;
         }
      }
      if (diff < lowestDiff)
      {
         closestMatch = sidc;
         lowestDiff   = diff;
      }
   }
   return closestMatch;
}

std::string WkfMilStd2525d::SymbolIdentificationCode::ToString() const
{
   return mVersion + mStdIdentity + mSymbolSet + mStatus + mHQDummy + mAmplifier + mEntity + mEntityType +
          mEntitySubtype + mSector1Mod + mSector2Mod + mExtra;
}
