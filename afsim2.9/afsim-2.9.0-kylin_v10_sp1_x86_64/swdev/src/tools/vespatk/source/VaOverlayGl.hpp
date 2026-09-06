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

#ifndef VAOVERLAYGL_HPP
#define VAOVERLAYGL_HPP

#include "vespatk_export.h"

#include "UtoGLShape.hpp"
#include "VaOverlay.hpp"

namespace vespa
{

class VESPATK_EXPORT VaOverlayGl : public VaOverlay
{
public:
   friend class UtoGLShape;

   VaOverlayGl();
   VaOverlayGl(const std::string& aName);
   VaOverlayGl(const std::string& aName, OverlayType aType);
   ~VaOverlayGl() override;

   VaOverlayGl* Clone() const override;

   bool Initialize() override;
   void Update() override;

   template<class T, class Func>
   void SetFunc(T* p, Func f)
   {
      if (mShapePtr != nullptr)
      {
         mShapePtr->SetFn(p, f);
      }
   }

   virtual void DrawFunction();

protected:
   void EnableLineSmoothing(bool aState);
   void EnableLighting(bool aState);

   VaOverlayGl(const VaOverlayGl& aSrc);

   UtoGLShape* mShapePtr;

   bool mLighting;
   bool mLineSmoothing;
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayGl)
#endif
