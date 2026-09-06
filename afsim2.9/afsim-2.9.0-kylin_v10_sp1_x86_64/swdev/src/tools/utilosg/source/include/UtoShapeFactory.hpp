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

/////////////////////////////////////////////////////////////////////////////
// UtoShapeFactory.hpp : header file

#if !defined(_UTOSHAPEFACTORY_HPP_)
#define _UTOSHAPEFACTORY_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include "utilosg_export.h"

#include <istream>
#include <map>

#include "UtoTypes.hpp"

class UtoShape;

class UTILOSG_EXPORT UtoShapeFactory
{
public:
   typedef UtoShape* (*CreateShapeCB)();

   static UtoShapeFactory* Instance();
   virtual ~UtoShapeFactory();

   bool      RegisterShape(const std::string& type, CreateShapeCB func);
   bool      UnregisterShape(const std::string& type);
   UtoShape* CreateShape(const std::string& type);
   bool      Supports(const std::string& type) const;

protected:
private:
   typedef std::map<std::string, CreateShapeCB> CallbackMap;

   UtoShapeFactory();
   UtoShapeFactory(const UtoShapeFactory&);
   UtoShapeFactory& operator=(const UtoShapeFactory&);

   static UtoShapeFactory* m_pMe;

   CallbackMap m_Callbacks;
};

#endif // !defined(_UTOSHAPEFACTORY_HPP_)
