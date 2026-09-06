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

#include "UtoSettingsTree.hpp"

UtoSettingsTree::UtoSettingsTree(std::string aStringRep)
{
   size_t readStart = 0;
   size_t p         = aStringRep.find_first_of("([");
   while (p != std::string::npos)
   {
      if (aStringRep[p] == '[')
      {
         std::string datatag = aStringRep.substr(readStart, p - readStart);
         size_t      endData = FindEnder('[', ']', aStringRep, p + 1);
         if (endData != std::string::npos)
         {
            std::string data = aStringRep.substr(p + 1, endData - p - 1);
            AddData(datatag, data);
            readStart = endData + 1;
         }
      }
      if (aStringRep[p] == '(')
      {
         std::string childtag = aStringRep.substr(readStart, p - readStart);
         size_t      endParen = FindEnder('(', ')', aStringRep, p + 1);
         if (endParen != std::string::npos)
         {
            std::string childStr = aStringRep.substr(p + 1, endParen - p - 1);
            AddChild(childtag, childStr);
            readStart = endParen + 1;
         }
         else
         {
            readStart = std::string::npos;
         }
      }
      p = aStringRep.find_first_of("([", readStart);
   }
}

std::string UtoSettingsTree::ToString()
{
   std::string                                  output;
   std::map<std::string, std::string>::iterator it;
   for (it = mData.begin(); it != mData.end(); ++it)
   {
      output += it->first;
      output += '[';
      output += it->second;
      output += ']';
   }
   std::map<std::string, UtoSettingsTree*>::iterator jt;
   for (jt = mChildren.begin(); jt != mChildren.end(); ++jt)
   {
      output += jt->first;
      output += '(';
      output += jt->second->ToString();
      output += ')';
   }
   return output;
}

UtoSettingsTree::~UtoSettingsTree()
{
   Clear();
}

UtoSettingsTree* UtoSettingsTree::AddChild(std::string aTag)
{
   UtoSettingsTree* child = GetChild(aTag);
   if (child == nullptr)
   {
      child           = new UtoSettingsTree;
      mChildren[aTag] = child;
   }
   return child;
}

UtoSettingsTree* UtoSettingsTree::AddChild(std::string aTag, std::string aChildStr)
{
   std::map<std::string, UtoSettingsTree*>::iterator it = mChildren.find(aTag);
   if (it != mChildren.end())
   {
      mChildren.erase(it);
   }

   UtoSettingsTree* child = new UtoSettingsTree(aChildStr);
   mChildren[aTag]        = child;

   return child;
}

void UtoSettingsTree::AddData(std::string aTag, std::string aData)
{
   mData[aTag] = aData;
}


void UtoSettingsTree::Clear()
{
   mData.clear();
   std::map<std::string, UtoSettingsTree*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      delete it->second;
   }
   mChildren.clear();
}

UtoSettingsTree* UtoSettingsTree::GetChild(std::string aTag)
{
   std::map<std::string, UtoSettingsTree*>::iterator it = mChildren.find(aTag);
   if (it != mChildren.end())
   {
      return it->second;
   }
   else
   {
      return nullptr;
   }
}

UtoSettingsTree* UtoSettingsTree::TakeChild(std::string aTag)
{
   std::map<std::string, UtoSettingsTree*>::iterator it = mChildren.find(aTag);
   if (it != mChildren.end())
   {
      UtoSettingsTree* retval = it->second;
      mChildren.erase(it);
      return retval;
   }
   else
   {
      return nullptr;
   }
}

std::string UtoSettingsTree::GetData(std::string aTag)
{
   std::map<std::string, std::string>::iterator it = mData.find(aTag);
   if (it != mData.end())
   {
      return it->second;
   }
   else
   {
      return "";
   }
}

size_t UtoSettingsTree::FindEnder(const char aStarter, const char aEnder, const std::string& aString, const size_t aOffset) const
{
   int    depth = 1;
   size_t p     = aOffset - 1;
   while ((depth > 0) && (p != std::string::npos))
   {
      std::string starterEnder = "";
      starterEnder += aStarter;
      starterEnder += aEnder;
      p = aString.find_first_of(starterEnder.c_str(), p + 1);
      if (p != std::string::npos)
      {
         if (aString[p] == aStarter)
         {
            depth++;
         }
         else if (aString[p] == aEnder)
         {
            depth--;
            if (depth < 0)
            {
               return std::string::npos;
            }
         }
      }
   }
   return p;
}

std::pair<std::string, UtoSettingsTree*> UtoSettingsTree::GetChild(unsigned int i)
{
   if (i < mChildren.size())
   {
      std::map<std::string, UtoSettingsTree*>::iterator it = mChildren.begin();
      for (unsigned int j = 0; j < i; ++j)
      {
         ++it;
      }
      if (it != mChildren.end())
      {
         return std::pair<std::string, UtoSettingsTree*>(it->first, it->second);
      }
   }
   return std::pair<std::string, UtoSettingsTree*>("", (UtoSettingsTree*)nullptr);
}

std::pair<std::string, std::string> UtoSettingsTree::GetDatum(unsigned int i)
{
   if (i < mData.size())
   {
      std::map<std::string, std::string>::iterator it = mData.begin();
      for (unsigned int j = 0; j < i; ++j)
      {
         ++it;
      }
      if (it != mData.end())
      {
         return std::pair<std::string, std::string>(it->first, it->second);
      }
   }
   return std::pair<std::string, std::string>("", "");
}
