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

#if !defined(_UTOUPDATE_CALLBACK_HPP_)
#define _UTOUPDATE_CALLBACK_HPP_

#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <osg/NodeCallback>
#include <osg/NodeVisitor>

#include "UtoRenderInfo.hpp"

/* Used for getting called in the update pass when this class can't
 * easily be derived from the CallbackNode and set as the callback
 * object.
 */
template<class CBObject>
class UtoUpdateCallback : public osg::NodeCallback
{
public:
   UtoUpdateCallback(CBObject* obj)
      : m_obj(obj)
      , lastTraversal(-1)
   {
   }
   void operator()(osg::Node* node, osg::NodeVisitor* nv) override
   {
      const int tnum = nv->getTraversalNumber();
      if (tnum == lastTraversal)
         return;
      lastTraversal = tnum;

      m_obj->Update(dynamic_cast<UtoRenderInfo*>(nv->getUserData()), nv);
      traverse(node, nv);
   }

private:
   CBObject* m_obj;
   int       lastTraversal;
};

#endif
