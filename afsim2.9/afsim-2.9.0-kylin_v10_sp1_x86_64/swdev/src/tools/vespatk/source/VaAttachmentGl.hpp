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

#ifndef VaAttachmentGl_hpp
#define VaAttachmentGl_hpp

#include "vespatk_export.h"

#include "UtColor.hpp"
#include "VaAttachment.hpp"

namespace osg
{
class RenderInfo;
}

namespace vespa
{
class VESPATK_EXPORT VaAttachmentGl : public VaAttachment
{
public:
   VaAttachmentGl(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName = "gl");
   ~VaAttachmentGl() override;


   // this will only be valid within the DrawFunction()
   osg::RenderInfo* GetRenderInfo() const;

private:
   void PrivateLoad() override;

   virtual void DrawFunction(){};

   std::string mShapeName;
   UtoGLShape* mGlPtr;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentGl)
#endif
