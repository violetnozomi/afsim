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
// UtoShapeFactory.cpp : implementation file

#include "UtoShapeFactory.hpp"

#include "UtoObjectManager.hpp"

UtoShapeFactory* UtoShapeFactory::m_pMe = nullptr;

UtoShapeFactory* UtoShapeFactory::Instance()
{
   if (m_pMe == nullptr)
   {
      m_pMe = new UtoShapeFactory;
   }
   return m_pMe;
}

UtoShapeFactory::UtoShapeFactory()
{
   UtoObjectManager::Instance()->Register("ShapeFactory", this);
}

UtoShapeFactory::~UtoShapeFactory() {}

bool UtoShapeFactory::RegisterShape(const std::string& type, CreateShapeCB func)
{
   return m_Callbacks.insert(std::make_pair(type, func)).second;
}

bool UtoShapeFactory::UnregisterShape(const std::string& type)
{
   return m_Callbacks.erase(type) == 1;
}

UtoShape* UtoShapeFactory::CreateShape(const std::string& type)
{
   CallbackMap::iterator it = m_Callbacks.find(type);
   if (it == m_Callbacks.end())
   {
      // throw std::runtime_error ("Unknown shape type");
      // DEBUG (("Unknown shape type: %s\n", type.empty () ? "<empty>" : type.c_str ()));
      return nullptr;
   }
   return it->second();
}

bool UtoShapeFactory::Supports(const std::string& type) const
{
   CallbackMap::const_iterator it = m_Callbacks.find(type);
   return (it != m_Callbacks.end());
}
