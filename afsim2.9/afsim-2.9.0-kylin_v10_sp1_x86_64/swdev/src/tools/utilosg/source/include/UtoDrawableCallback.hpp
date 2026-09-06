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

#ifndef _UTODRAWABLE_CALLBACK_HPP_
#define _UTODRAWABLE_CALLBACK_HPP_

#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <osg/Drawable>

/* Used for getting called in the update pass when this class can't
 * easily be derived from the CallbackNode and set as the callback
 * object.  The DrawableCallback can only be attached to an osg::Drawable
 * or derived object.
 */
template<class CBObject>
class UtoDrawableCallback : public osg::Drawable::UpdateCallback
{
public:
   UtoDrawableCallback(CBObject* obj)
      : m_obj(obj)
      , lastTraversal(-1)
   {
   }
   void update(osg::NodeVisitor* nv, osg::Drawable* drawable) override
   {
      const int tnum = nv->getTraversalNumber();
      if (tnum == lastTraversal)
         return;
      lastTraversal = tnum;

      m_obj->Update(nv, drawable);
   }

private:
   CBObject* m_obj;
   int       lastTraversal;
};

#endif // _UTODRAWABLE_CALLBACK_HPP_
