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
// UtoHittestStrategy.cpp : implementation file

#include "UtoHittestStrategy.hpp"

#include "UtoEntity.hpp"
#include "UtoGeometryUtil.hpp"
#include "UtoViewer.hpp"
//#include "UtoSymbol.hpp"
#include <osg/LineSegment>
#include <osgUtil/SceneView>
#include <osgViewer/Viewer>

#include "UtoCmeIntersectVisitor.hpp"
#include "UtoEntity.hpp"
#include "UtoHittestUtils.hpp"
#include "UtoShapeImp.hpp"
//#include <osgUtil/IntersectVisitor>

#include <osg/Drawable>
#include <osg/Image>
#include <osg/LineSegment>
#include <osg/Texture2D>
#include <osg/TriangleFunctor>

#include "UtoIsect.hpp"

using osg::LineSegment;
using osg::TriangleFunctor;
using osg::Vec2;
using osg::Vec3;
using osgUtil2::TriangleHit;

UtoHittestStrategy::UtoHittestStrategy(UtoViewer* vimp, bool OrthoSelection)
   : m_Viewer(vimp)
   , m_OrthoSelection(OrthoSelection)
{
}

UtoHittestStrategy::~UtoHittestStrategy() {}

/* UTO Function Block **********************************************************

Function: PopulateSymbolName

Description:

*******************************************************************************/

void UtoHittestStrategy::PopulateSymbolName(SymbolNameSet& SymbolName, const std::string& symbols)
{
   std::string tokens(";,");
   size_t      start = 0, pos = (unsigned int)-1;
   while ((pos = symbols.find_first_of(tokens, start)) != symbols.npos)
   {
      SymbolName.insert(symbols.substr(start, pos - start));
      start = pos + 1;
   }
   if (start < symbols.size())
   {
      // Add the last name.
      SymbolName.insert(symbols.substr(start, symbols.npos));
   }
}

UtoHittestStrategy::SymbolHittestList* UtoHittestStrategy::HitTestSymbol(UtoPoint point,
                                                                         const std::string& /*symbols*/,
                                                                         unsigned int max,
                                                                         bool         aScreenScaled,
                                                                         float        aOrthoScale,
                                                                         float        aIconScale)
{
   typedef osgUtil::UtoCmeIntersectVisitor ISectType;

   SymbolHittestList* symbolList = new SymbolHittestList();
   if (!m_Viewer->GetOSGViewer())
   {
      return symbolList;
   }

   const osg::Viewport* viewport = m_Viewer->GetViewport();
   // the above check for the validity of OSGViewer should confirm the validity of these matrices
   osg::Matrix projectionMatrix = *m_Viewer->GetProjectionMatrix();
   osg::Matrix viewMatrix       = *m_Viewer->GetViewMatrix();
   osgUtil::UtoCmePickVisitor
      pickVisitor(viewport, projectionMatrix, viewMatrix, point.x, point.y, aScreenScaled, aOrthoScale, aIconScale);

   m_Viewer->GetScene()->accept(pickVisitor);
   if (pickVisitor.hits())
   {
      ISectType::CmeLineSegmentHitListMap::iterator segIter = pickVisitor.getSegHitList().begin();
      if (segIter != pickVisitor.getSegHitList().end())
      {
         ISectType::UtoCmeHitList&          hlist = segIter->second;
         ISectType::UtoCmeHitList::iterator cur   = hlist.begin();
         ISectType::UtoCmeHitList::iterator end   = hlist.end();

         for (unsigned int i = 0; cur != end && i < max; ++cur, ++i)
         {
            const UtoHitInfo& hitInfo = *cur;
            osg::NodePath&    path    = cur->_nodePath;
            UtoShapeImp*      shape   = UtoGeometryUtil::GetShape(path, false);

            // Find the symbol that was hit.
            if (shape && shape->GetPublicPtr())
            {
               UtoHitInfoList& shapeHits = (*symbolList)[shape->GetPublicPtr()];
               shapeHits.mShapePtr       = shape->GetPublicPtr();
               shapeHits._hits.push_back(hitInfo);
            }
         }
      }
   }
   return symbolList;
}
