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
// UtoGeodeShapeImp.cpp : implementation file

#include "UtoGeodeShapeImp.hpp"

#include <osg/CopyOp>
#include <osg/Geode>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/Point>

#include "UtoPriv.hpp"

UtoGeodeShapeImp::UtoGeodeShapeImp()
{
   m_Geode = new osg::Geode;
   AddShapeChildNode(m_Geode.get());
}

UtoGeodeShapeImp::UtoGeodeShapeImp(const UtoGeodeShapeImp& rhs)
   : UtoShapeImp(rhs)
   , m_Geode(new osg::Geode(*rhs.m_Geode.get(), osg::CopyOp::DEEP_COPY_STATESETS | osg::CopyOp::DEEP_COPY_STATEATTRIBUTES))
{
   // remove the old drawables and let the derived classes add copies back
   m_Geode->removeDrawables(0, m_Geode->getNumDrawables());
   AddShapeChildNode(m_Geode.get());
}

UtoGeodeShapeImp::~UtoGeodeShapeImp() {}

void UtoGeodeShapeImp::SetOwner(UtoEntity* owner)
{
   UtoShapeImp::SetOwner(owner);
}

short UtoGeodeShapeImp::Pattern() const
{
   const osg::LineStipple* linestipple = GetStippleAttributeSet();
   if (linestipple)
   {
      return linestipple->getPattern();
   }
   // -1 as in all bits set not error
   return -1;
}

int UtoGeodeShapeImp::PatternFactor() const
{
   const osg::LineStipple* linestipple = GetStippleAttributeSet();
   if (linestipple)
   {
      return linestipple->getFactor();
   }
   return 1;
}

void UtoGeodeShapeImp::SetPattern(short pattern, int factor)
{
   if (pattern == (short)0xffff)
   {
      // remove any line stipple state set if it is going to be a solid line
      osg::StateSet* stateset = m_RootNode->getStateSet();
      if (stateset)
      {
         stateset->removeAttribute(osg::StateAttribute::LINESTIPPLE);
      }
   }
   else
   {
      osg::LineStipple* linestipple = GetStippleAttributeSet();
      linestipple->setPattern(pattern);
      linestipple->setFactor(factor);
   }
}

/* UTO Function Block **********************************************************

  Function: GetStippleAttributeSet

  Description:
   Retrieves the current line stipple set attribute casting as required.
   If the line stipple set attribute doesn't exist it is created and enabled.

*******************************************************************************/

osg::LineStipple* UtoGeodeShapeImp::GetStippleAttributeSet()
{
   osg::StateSet*    stateset = m_RootNode->getOrCreateStateSet();
   osg::LineStipple* linestipple =
      dynamic_cast<osg::LineStipple*>(stateset->getAttribute(osg::StateAttribute::LINESTIPPLE));
   if (!linestipple)
   {
      linestipple = new osg::LineStipple;
      stateset->setAttribute(linestipple, osg::StateAttribute::ON);
      stateset->setMode(GL_LINE_STIPPLE, osg::StateAttribute::ON);
   }
   return linestipple;
}

/* UTO Function Block **********************************************************

  Function: GetStippleAttributeSet

  Description:
   Retrieves the current line stipple set attribute casting as required.

*******************************************************************************/

const osg::LineStipple* UtoGeodeShapeImp::GetStippleAttributeSet() const
{
   const osg::StateSet* stateset = m_RootNode->getStateSet();
   if (stateset)
   {
      return dynamic_cast<const osg::LineStipple*>(stateset->getAttribute(osg::StateAttribute::LINESTIPPLE));
   }
   return nullptr;
}

double UtoGeodeShapeImp::Width() const
{
   const osg::LineWidth* linewidth = GetLineWidthAttributeSet();
   if (linewidth)
   {
      return linewidth->getWidth();
   }
   return 1.0;
}

void UtoGeodeShapeImp::SetWidth(double width)
{
   osg::LineWidth* linewidth = GetLineWidthAttributeSet();
   linewidth->setWidth(width);
}

double UtoGeodeShapeImp::PointSize() const
{
   const osg::Point* pointsize = GetPointAttributeSet();
   if (pointsize)
   {
      return pointsize->getSize();
   }
   return 1.0;
}

void UtoGeodeShapeImp::SetPointSize(double pointSize)
{
   osg::Point* pointsize = GetPointAttributeSet();
   pointsize->setSize(pointSize);
}

bool UtoGeodeShapeImp::PointSmooth() const
{
   bool pointsmooth = false;

   const osg::StateSet* stateset = m_RootNode->getStateSet();
   if (stateset)
   {
      pointsmooth = (stateset->getMode(GL_POINT_SMOOTH) == osg::StateAttribute::ON);
   }
   return pointsmooth;
}

void UtoGeodeShapeImp::SetPointSmooth(bool pointSmooth)
{
   osg::StateSet* stateset = m_RootNode->getOrCreateStateSet();
   if (stateset)
   {
      if (pointSmooth)
      {
         stateset->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
      }
      else
      {
         stateset->setMode(GL_POINT_SMOOTH, osg::StateAttribute::OFF);
      }
   }
}

/* UTO Function Block **********************************************************

  Function: GetWidthAttributeSet

  Description:
   Retrieves the current line width set attribute casting as required.
   If the line width set attribute doesn't exist it is created and enabled.

*******************************************************************************/

osg::LineWidth* UtoGeodeShapeImp::GetLineWidthAttributeSet()
{
   osg::StateSet*  stateset  = m_RootNode->getOrCreateStateSet();
   osg::LineWidth* linewidth = dynamic_cast<osg::LineWidth*>(stateset->getAttribute(osg::StateAttribute::LINEWIDTH));
   if (!linewidth)
   {
      linewidth = new osg::LineWidth;
      stateset->setAttribute(linewidth, osg::StateAttribute::ON);
   }
   return linewidth;
}

/* UTO Function Block **********************************************************

  Function: GetWidthAttributeSet

  Description:
   Retrieves the current line width set attribute casting as required.

*******************************************************************************/

const osg::LineWidth* UtoGeodeShapeImp::GetLineWidthAttributeSet() const
{
   const osg::StateSet* stateset = m_RootNode->getStateSet();
   if (stateset)
   {
      return dynamic_cast<const osg::LineWidth*>(stateset->getAttribute(osg::StateAttribute::LINEWIDTH));
   }
   return nullptr;
}

/* UTO Function Block **********************************************************

  Function: GetPointAttributeSet

  Description:
   Retrieves the current point size set attribute casting as required.
   If the point size set attribute doesn't exist it is created and enabled.

*******************************************************************************/

osg::Point* UtoGeodeShapeImp::GetPointAttributeSet()
{
   osg::StateSet* stateset  = m_RootNode->getOrCreateStateSet();
   osg::Point*    pointsize = dynamic_cast<osg::Point*>(stateset->getAttribute(osg::StateAttribute::POINT));
   if (!pointsize)
   {
      pointsize = new osg::Point();
      stateset->setAttribute(pointsize, osg::StateAttribute::ON);
   }
   return pointsize;
}

/* UTO Function Block **********************************************************

  Function: GetPointAttributeSet

  Description:
   Retrieves the current point set attribute casting as required.

*******************************************************************************/


const osg::Point* UtoGeodeShapeImp::GetPointAttributeSet() const
{
   const osg::StateSet* stateset = m_RootNode->getStateSet();
   if (stateset)
   {
      return dynamic_cast<const osg::Point*>(stateset->getAttribute(osg::StateAttribute::POINT));
   }
   return nullptr;
}
