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

#ifndef VaViewerManager_hpp
#define VaViewerManager_hpp

#include "vespatk_export.h"

#include <vector>

#include "VaCallbackHolder.hpp"
#include "VaViewer.hpp"

namespace vespa
{
class VESPATK_EXPORT VaViewerManager
{
public:
   typedef std::vector<VaViewer*> Viewers;

   VaViewerManager();

   virtual ~VaViewerManager();

   virtual bool AddViewer(VaViewer* aViewerPtr);
   virtual bool RemoveViewer(VaViewer* aViewerPtr);

   virtual VaViewer* CurrentViewer() const;

   const Viewers&    GetViewers() const { return mViewers; }
   Viewers&          GetViewers() { return mViewers; }
   virtual VaViewer* GetViewerByIndex(unsigned int aIndex) const;

   void SetCurrentViewIndex(unsigned int aIndex) { mCurrentViewIndex = aIndex; }
   void SetCurrentViewer(VaViewer* aViewerPtr);

protected:
   Viewers          mViewers;
   unsigned int     mCurrentViewIndex;
   VaCallbackHolder mCallbacks;
};
} // end namespace vespa
#endif
