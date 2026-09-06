// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTOSCREENOVERLAYMANAGER_HPP
#define UTOSCREENOVERLAYMANAGER_HPP

#include <osg/MatrixTransform>

#include "UtoOverlayManager.hpp"

class UtoViewer;
struct UtoScreenOverlayManagerIter_i;

// {secret}
struct UTILOSG_EXPORT UtoScreenOverlayManagerIteratorPolicies
{
   typedef UtoOverlay                     value_type;
   typedef UtoScreenOverlayManagerIter_i* impl_ptr;

public:
   impl_ptr GetImpl();

protected:
   UtoScreenOverlayManagerIteratorPolicies();
   UtoScreenOverlayManagerIteratorPolicies(impl_ptr imp);
   UtoScreenOverlayManagerIteratorPolicies(const UtoScreenOverlayManagerIteratorPolicies& rhs);
   ~UtoScreenOverlayManagerIteratorPolicies();

   value_type* ptr();
   void        forward();
   bool        is_equal(const UtoScreenOverlayManagerIteratorPolicies&) const;
   void        swap(UtoScreenOverlayManagerIteratorPolicies&);

private:
   UtoScreenOverlayManagerIteratorPolicies& operator=(const UtoScreenOverlayManagerIteratorPolicies& rhs);

   impl_ptr m_pImpl;
};

class UTILOSG_EXPORT UtoScreenOverlayManager
{
public:
   // Provides access to an UtoOverlay.
   typedef Utok::Iterator<UtoScreenOverlayManagerIteratorPolicies> iterator;
   // Provides access to the begin and end iterator.
   typedef Utok::IteratorPair<iterator> iterator_pair;

   UtoScreenOverlayManager(const UtoSize& aSize, UtoViewer* aViewerPtr);

   ~UtoScreenOverlayManager() = default;

   iterator AddOverlay(const std::string& name);
   iterator InsertOverlay(const std::string& name, iterator it);
   void     RemoveOverlay(iterator it);
   void     RemoveAllOverlays();

   iterator FindOverlay(const std::string& name);

   iterator_pair Overlays();
   std::string   FindName(iterator it);

   void BringOverlayToFront(iterator it);

   osg::Node* GetTree() { return mTransform.get(); }

   void SetSize(const UtoSize& size);

private:
   UtoViewer*                         mViewerPtr;
   osg::ref_ptr<osg::MatrixTransform> mTransform;
   osg::ref_ptr<osg::Projection>      mProjection;
   osg::ref_ptr<osg::Switch>          mSwitch;
   UtoOverlayManager                  mOverlays;
};

#endif
