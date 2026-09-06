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

#ifndef UTOSETTINGSTREE_HPP
#define UTOSETTINGSTREE_HPP

#include "utilosg_export.h"

#include <map>
#include <string>

class UTILOSG_EXPORT UtoSettingsTree
{
public:
   UtoSettingsTree() {}
   UtoSettingsTree(std::string aStringRep);

   virtual ~UtoSettingsTree();

   UtoSettingsTree* AddChild(std::string aTag);
   UtoSettingsTree* AddChild(std::string aTag, std::string aChildData);
   void             AddData(std::string aTag, std::string aData);

   std::string      ToString();
   void             Clear();
   UtoSettingsTree* GetChild(std::string aTag);
   UtoSettingsTree* TakeChild(std::string aTag);
   std::string      GetData(std::string aTag);

   size_t                                   GetChildCount() { return mChildren.size(); }
   std::pair<std::string, UtoSettingsTree*> GetChild(unsigned int i);
   size_t                                   GetDatumCount() { return mData.size(); }
   std::pair<std::string, std::string>      GetDatum(unsigned int i);

private:
   size_t FindEnder(const char aStarter, const char aEnder, const std::string& aString, const size_t aOffset) const;

   std::map<std::string, UtoSettingsTree*> mChildren;
   std::map<std::string, std::string>      mData;
};

#endif
