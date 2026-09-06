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
// UtoModelPartImp.cpp : implementation file

#include "UtoModelPartImp.hpp"

#include <osgSim/DOFTransform>

#include "UtoPriv.hpp"

UtoModelPartImp::UtoModelPartImp(osgSim::DOFTransform* tnode)
   : m_Transform(tnode)
{
}

UtoModelPartImp::~UtoModelPartImp() {}

UtoPosition UtoModelPartImp::Position() const
{
   osg::Vec3 t = m_Transform->getCurrentTranslate();
   return UtoPosition(t.x(), t.y(), t.z());
}

void UtoModelPartImp::SetPosition(const UtoPosition& pos)
{
   m_Transform->setCurrentTranslate(pos);
}

UtoOrientation UtoModelPartImp::Orientation() const
{
   osg::Vec3 o = m_Transform->getCurrentHPR() * 180 / M_PI;
   return UtoOrientation(o.x(), o.y(), o.z());
}

void UtoModelPartImp::SetOrientation(const UtoOrientation& orient)
{
   osg::Vec3 o(orient.yaw, orient.pitch, orient.roll);
   m_Transform->setCurrentHPR(o * M_PI / 180);
}

UtoPosOrient UtoModelPartImp::PosOrient() const
{
   UtoPosOrient posorient;
   posorient.pos    = Position();
   posorient.orient = Orientation();
   return posorient;
}

void UtoModelPartImp::SetPosOrient(const UtoPosOrient& posorient)
{
   SetPosition(posorient.pos);
   SetOrientation(posorient.orient);
}

osg::Node* UtoModelPartImp::Node()
{
   return m_Transform.get();
}
