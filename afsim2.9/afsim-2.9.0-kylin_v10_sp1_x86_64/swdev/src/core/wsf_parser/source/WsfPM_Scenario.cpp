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

#include "WsfPM_Scenario.hpp"

#include <sstream>

#include "WsfPM_ObjectList.hpp"

namespace
{
void ExtractTokens(const std::string& aString, std::set<std::string>& aSet)
{
   std::string        token;
   std::istringstream tokenStream(aString);
   while (std::getline(tokenStream, token, ' '))
   {
      aSet.insert(token);
   }
}
} // namespace

WsfProxy::String WsfPM_Scenario::GetClassificationLabel()
{
   WsfPM_List     classificationPMList  = *this + "classificationList";
   WsfPProxyList* classificationListPtr = classificationPMList.list();
   if (classificationListPtr)
   {
      while (classificationListPtr->Size() > 0)
      {
         WsfPProxyValue node = classificationListPtr->Back();
         WsfPProxyValue lv   = node.GetAttr("classificationLevel");

         DetermineLevel(lv.ValueToString());

         WsfPProxyValue t = node.GetAttr("trigraphsString");
         WsfPProxyValue c = node.GetAttr("caveatsString");

         ExtractTokens(t.ValueToString(), mTrigraphsSet);
         ExtractTokens(c.ValueToString(), mCaveatsSet);

         classificationListPtr->PopBack();
      }
   }

   WsfPM_List     classLevels        = *this + "classLevels";
   WsfPProxyList* classLevelsListPtr = classLevels.list();

   if (classLevelsListPtr)
   {
      for (size_t i = 0; i < classLevelsListPtr->Size(); ++i)
      {
         std::string lv = classLevelsListPtr->Get(i).GetAttr("levelString").ValueToString();
         UtStringUtil::Trim(lv, "\"");
         if (mClassificationString.find(lv) != std::string::npos)
         {
            auto color = classLevelsListPtr->Get(i).GetAttr("color");
            int  red   = 0;
            color.GetAttr("red").GetIntegerValue(red);

            int green = 0;
            color.GetAttr("green").GetIntegerValue(green);

            int blue = 0;
            color.GetAttr("blue").GetIntegerValue(blue);

            mColor.Set(static_cast<unsigned char>(red), static_cast<unsigned char>(green), static_cast<unsigned char>(blue));
         }
      }
   }

   std::string formattedTrigraphs = FormatTrigraphsString();
   std::string formattedCaveats   = FormatCaveatsString();

   if (!formattedTrigraphs.empty())
   {
      mClassificationString += formattedTrigraphs;
   }

   if (!formattedCaveats.empty())
   {
      mClassificationString += " - " + formattedCaveats;
   }

   return WsfProxy::String(mClassificationString);
}

std::string WsfPM_Scenario::FormatTrigraphsString() const
{
   std::string formattedTrigraphs;
   for (const auto& trigraph : mTrigraphsSet)
   {
      formattedTrigraphs += ("//" + trigraph);
   }
   return formattedTrigraphs;
}

std::string WsfPM_Scenario::FormatCaveatsString() const
{
   std::string formattedCaveats;
   for (const auto& caveat : mCaveatsSet)
   {
      if (!caveat.empty())
      {
         formattedCaveats += caveat;
         // If it is not the last caveat, add the delimiter
         if (caveat != *mCaveatsSet.rbegin())
         {
            formattedCaveats += "//";
         }
      }
   }
   return formattedCaveats;
}

void WsfPM_Scenario::DetermineLevel(const std::string& aLevelString)
{
   WsfPM_List     classLevels        = *this + "classLevels";
   WsfPProxyList* classLevelsListPtr = classLevels.list();
   size_t         level              = 0;
   bool           levelFound         = false;

   if (classLevelsListPtr)
   {
      for (size_t i = 0; i < classLevelsListPtr->Size(); ++i)
      {
         std::string lv = classLevelsListPtr->Get(i).GetAttr("levelString").ValueToString();
         if (aLevelString.find(lv) != std::string::npos)
         {
            level      = i;
            levelFound = true;
         }
      }
   }

   if (levelFound && (level >= mClassificationLevel))
   {
      mClassificationLevel  = level;
      mClassificationString = aLevelString;
      UtStringUtil::Trim(mClassificationString, "\"");
      mLevelSet = true;
   }

   if (!mLevelSet)
   {
      mClassificationString.clear();
   }
}
