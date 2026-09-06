// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SceneGenInputFileReader.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "SceneGenInputData.hpp"

namespace
{
void Failure(std::string aMessage)
{
   ut::log::fatal() << aMessage;
   throw std::logic_error("Bad input data.");
}
} // namespace

namespace sceneGen
{
SceneGenInputFileReader::SceneGenInputFileReader(const std::string& aFilepath)
{
   mFile.open(aFilepath);
}

SceneGenInputFileReader::~SceneGenInputFileReader()
{
   Close();
}

bool SceneGenInputFileReader::IsOpen() const
{
   return mFile.is_open();
}

void SceneGenInputFileReader::Close()
{
   if (IsOpen())
   {
      mFile.close();
      mCurrentLineNumber = 0;
   }
}

void SceneGenInputFileReader::GetGroupCount()
{
   while (GetLine())
   {
      if (mCurrentLine.size())
      {
         mFileData.str(mCurrentLine);

         std::string description;

         mFileData >> description;

         if (description == "groups")
         {
            if (mFileData >> mGroups)
            {
               break;
            }
            else
            {
               Failure("Invalid group count.");
            }
         }
         else
         {
            Failure("Second line must specify number of groups to generate.");
         }
      }
   }
}

unsigned int SceneGenInputFileReader::GroupsLeft() const
{
   return mGroups;
}

void SceneGenInputFileReader::FetchData(SceneGenInputData& aData, std::set<std::string>& aPlatformType)
{
   while (GetLine())
   {
      if (mCurrentLine.size())
      {
         mFileData.str(mCurrentLine);

         std::string description;

         mFileData >> description;

         if (description == "end_group")
         {
            --mGroups;
            break;
         }
         else if (description == "name")
         {
            AssignString(aData.mName);
         }
         else if (description == "side")
         {
            AssignString(aData.mSide);
         }
         else if (description == "type")
         {
            AssignString(aData.mType);

            std::string type;
            type.resize(aData.mType.size());

            std::transform(aData.mType.begin(), aData.mType.end(), type.begin(), ::tolower);
            aPlatformType.insert(type);
         }
         else if (description == "count")
         {
            AssignCount(aData.mCount);
         }
         else if (description == "start")
         {
            AssignLatLon(aData.mStartlat, aData.mStartlon);
         }
         else if (description == "end")
         {
            AssignLatLon(aData.mEndlat, aData.mEndlon);
         }
         else if (description == "distribution")
         {
            AssignString(aData.mDistribution);
         }
         else if (description == "radius")
         {
            AssignFloat(aData.mRadius);
         }
         else if (description == "speed")
         {
            AssignFloat(aData.mSpeed);
         }
         else if (description == "altitude")
         {
            AssignFloat(aData.mAltitude);
         }
         else if (description == "tracking_sides")
         {
            AssignList(aData.mTrackSide);
         }
         else if (description == "tracking_types")
         {
            AssignList(aData.mTrackTypes);
         }
         else if (description == "move_to_track")
         {
            aData.mMoveToTrack = true;
         }
         else
         {
            Failure("Unhandled description: " + description);
         }
      }
   }
}

void SceneGenInputFileReader::AssignString(std::string& aVariable)
{
   bool valid = (mFileData >> aVariable) && (!aVariable.empty());

   if (!valid)
   {
      Failure("Explicit value after description must exist.");
   }
}

void SceneGenInputFileReader::AssignFloat(float& aVariable)
{
   bool valid = (mFileData >> aVariable) && true;

   if (!valid)
   {
      Failure("Bad float value.");
   }
}

void SceneGenInputFileReader::AssignList(std::vector<std::string>& aList)
{
   std::string item;

   while (mFileData >> item)
   {
      aList.push_back(item);
   }

   if (aList.empty())
   {
      Failure("No items for list specified.");
   }
}

void SceneGenInputFileReader::AssignCount(int& aCount)
{
   bool valid = (mFileData >> aCount) && (aCount > 0);

   if (!valid)
   {
      Failure("Count must be positive non-zero value.");
   }
}

void SceneGenInputFileReader::AssignLatLon(float& aLat, float& aLon)
{
   bool valid = (mFileData >> aLat) && (std::abs(aLat) <= cMAX_LAT);

   if (!valid)
   {
      Failure("Bad latitude value; must satisfy |latitude| <= 90.0.");
   }

   valid = (mFileData >> aLon) && (std::abs(aLon) <= cMAX_LON);

   if (!valid)
   {
      Failure("Bad longitude value; must satisfy |longitude| <= 180.0.");
   }
}

bool SceneGenInputFileReader::GetLine()
{
   bool suceeded = false;

   if (mFile.peek() == EOF)
   {
      mEOF = true;
   }
   else if (std::getline(mFile, mCurrentLine))
   {
      ++mCurrentLineNumber;
      mFileData.clear();
      suceeded = true;
   }

   return suceeded;
}

bool SceneGenInputFileReader::IsEOF() const
{
   return mEOF;
}

unsigned int SceneGenInputFileReader::GetCurrentLine() const
{
   return mCurrentLineNumber;
}

} // namespace sceneGen
