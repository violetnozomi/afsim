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
#ifndef UTQTXML_HPP
#define UTQTXML_HPP

#include <string>
#include <vector>

namespace UtQtXml
{
class UtQtXmlAttribute
{
public:
   UtQtXmlAttribute() {}
   UtQtXmlAttribute(const std::string& aName, const std::string& aValue)
      : mName(aName)
      , mValue(aValue)
   {
   }
   std::string mName;
   std::string mValue;
};
typedef std::vector<UtQtXmlAttribute> UtQtXmlAttributeList;
} // namespace UtQtXml
#endif
