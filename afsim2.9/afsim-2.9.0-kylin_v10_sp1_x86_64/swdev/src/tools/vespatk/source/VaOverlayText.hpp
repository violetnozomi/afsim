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

#ifndef VaOverlayText_hpp
#define VaOverlayText_hpp

#include "vespatk_export.h"

#include "UtColor.hpp"
#include "VaCallbackHolder.hpp"
#include "VaOverlay.hpp"

class UtoAttrPolyLineShape;
class UtoTextShape;

namespace vespa
{
class VESPATK_DEPRECATED_EXPORT VaOverlayText : public VaOverlay
{
public:
   VaOverlayText();
   VaOverlayText(const std::string& aName);
   ~VaOverlayText() override;

   VaOverlayText* Clone() const override;

   bool         Initialize() override;
   void         Update() override;
   virtual void SetPosition(const UtoPosition& aPosition);
   virtual void SetText(const std::string& aText);
   virtual void SetText(const std::wstring& aText);
   void         SetColor(const UtColor& aColor);
   virtual void SetColor(const UtoColor& aColor);
   virtual void SetSize(const int& aSize);

private:
   VaOverlayText(const VaOverlayText& aSrc);

   UtoTextShape* mTextPtr;

   VaCallbackHolder mCallbacks;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayText)
#endif
