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

#ifndef VaOverlayCompass_hpp
#define VaOverlayCompass_hpp

#include "vespatk_export.h"

#include "VaOverlay.hpp"

class UtoModelShape;

namespace vespa
{
class VESPATK_EXPORT VaOverlayCompass : public VaOverlay
{
public:
   VaOverlayCompass();
   VaOverlayCompass(const std::string& aName);
   ~VaOverlayCompass() override;

   VaOverlayCompass* Clone() const override;

   bool Initialize() override;
   void Update() override;

private:
   VaOverlayCompass(const VaOverlayCompass& aSrc);

   UtoModelShape* mCompassPtr;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayCompass)
#endif
