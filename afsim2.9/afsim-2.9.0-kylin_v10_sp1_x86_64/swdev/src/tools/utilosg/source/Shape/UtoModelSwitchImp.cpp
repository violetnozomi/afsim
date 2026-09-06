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
// UtoModelSwitchImp.cpp : implementation file

#include "UtoModelSwitchImp.hpp"

#include <osgSim/MultiSwitch>

UtoModelSwitchImp::UtoModelSwitchImp(osgSim::MultiSwitch* snode)
   : m_MultiSwitch(snode)
{
}

UtoModelSwitchImp::~UtoModelSwitchImp() {}

int UtoModelSwitchImp::Count() const
{
   return m_MultiSwitch->getSwitchSetList().size();
}

std::string UtoModelSwitchImp::SelectedName() const
{
   const osg::Node::DescriptionList& description = m_MultiSwitch->getDescriptions();
   unsigned int                      selected    = m_MultiSwitch->getActiveSwitchSet();
   if (selected >= description.size())
      return "";

   return description[selected];
}

int UtoModelSwitchImp::Selected() const
{
   return m_MultiSwitch->getActiveSwitchSet();
}

void UtoModelSwitchImp::SetSelected(int value)
{
   m_MultiSwitch->setActiveSwitchSet(value);
}
