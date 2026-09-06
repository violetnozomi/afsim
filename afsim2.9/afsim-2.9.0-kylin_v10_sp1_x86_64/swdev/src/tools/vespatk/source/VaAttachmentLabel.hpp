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

#ifndef VaAttachmentLabel_hpp
#define VaAttachmentLabel_hpp

#include "vespatk_export.h"

#include <string>
#include <vector>

#include "UtColor.hpp"
#include "UtLength.hpp"
#include "UtoTextShape.hpp"
#include "VaAttachment.hpp"

namespace vespa
{
class Viewer;
class VaEntity;
class VaOverlayLabels;

//! Class for text that can be attached to entities
class VESPATK_DEPRECATED_EXPORT VaAttachmentLabel : public VaAttachment
{
public:
   friend class VaOverlayLabels;

   enum AlignFlags
   {
      BOTTOM_LEFT  = UtoTextShape::BOTTOM | UtoTextShape::LEFT,
      BOTTOM_RIGHT = UtoTextShape::BOTTOM | UtoTextShape::RIGHT,
      TOP_LEFT     = UtoTextShape::TOP | UtoTextShape::LEFT,
      TOP_RIGHT    = UtoTextShape::TOP | UtoTextShape::RIGHT
   };

   VaAttachmentLabel(VaEntity& aParent, VaViewer* aViewerPtr);
   ~VaAttachmentLabel() override;

   void UpdateFrame(double aTime) override;

   void UpdateVisibility() override;

   void           SetColor(const UtColor& aColor);
   const UtColor& GetColor() const;

   void        SetString(const std::string& aString);
   std::string GetString() { return mString; }

   void SetAlignment(const int aAlignment);
   int  GetAlignment() { return mAlignment; }

   void       SetPosition(VaViewer* aViewerPtr, const int aPosition[]);
   const int* GetPosition(VaViewer* aViewerPtr);

   void        SetFont(const std::string& style);
   std::string Face() const; // font face

   void       SetOffset(const int aX, const int aY);
   const int* GetOffset() { return mLabelOffset; }

   void SetSize(int aSize);
   int  GetSize() const { return mSize; }

private:
   void         PrivateLoad() override;
   virtual void PrivateLoadForViewer(VaViewer* aViewerPtr);

   void PrivateInitializeCallbacks() override;

   void LoadState();

   void CameraUpdatedCB(VaViewer* aViewerPtr);

   std::map<int, VaOverlayLabels*> mOverlayMap;
   std::map<int, int>              mLabelIndexMap;
   bool                            mVisible;
   UtColor                         mColor;
   std::string                     mString;
   std::string                     mFace;
   int                             mAlignment;
   struct intPos
   {
      int xy[2];
   };

   std::map<int, intPos> mLabelPosition;
   int                   mLabelOffset[2];
   int                   mSize;
};
} // end namespace vespa.

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentLabel)
#endif
