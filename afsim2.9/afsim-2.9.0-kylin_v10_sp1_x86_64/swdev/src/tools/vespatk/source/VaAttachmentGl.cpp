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

#include "VaAttachmentGl.hpp"

#include <functional>
#include <sstream>

#include "UtLog.hpp"
#include "UtoGLShape.hpp"
#include "VaUtils.hpp"

namespace vespa
{
VaAttachmentGl::VaAttachmentGl(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName)
   : VaAttachment(aParent, aViewerPtr, aName)
   , mShapeName("")
   , mGlPtr(nullptr)
{
   std::ostringstream oss;
   oss << GetName() << '_' << GetUniqueId();
   SetName(oss.str());
   SetType<VaAttachmentGl>();
}

VaAttachmentGl::~VaAttachmentGl() {}

void VaAttachmentGl::PrivateLoad()
{
   static unsigned int uid = 0;

   if (mShapeName == "")
   {
      std::ostringstream oss;
      oss << "VaAttachmentGl_" << uid++;
      mShapeName = oss.str();
   }

   // Remove the previous model.
   RemoveShapeFromParent(mShapeName);
   mGlPtr = nullptr;

   UtoGLShape* glShapePtr = new UtoGLShape(this, std::mem_fn(&VaAttachmentGl::DrawFunction));
   mGlPtr                 = dynamic_cast<UtoGLShape*>(AddShapeToParent(mShapeName, *glShapePtr));
   if (mGlPtr == nullptr)
   {
      ut::log::warning() << "Could not bind shape: gl";
   }

   if (mGlPtr != nullptr)
   {
      mGlPtr->SetColor(UtoColor(0, 0, 0, 128));
      mGlPtr->SetSize(20000000.0);
   }
}

osg::RenderInfo* VaAttachmentGl::GetRenderInfo() const
{
   return mGlPtr->GetRenderInfo();
}

} // namespace vespa
