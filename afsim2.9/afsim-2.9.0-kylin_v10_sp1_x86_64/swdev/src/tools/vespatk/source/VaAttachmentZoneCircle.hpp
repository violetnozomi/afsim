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

#ifndef VaAttachmentZoneCircle_hpp
#define VaAttachmentZoneCircle_hpp

#include "vespatk_export.h"

#include <string>

#include "UtAngleR.hpp"
#include "UtColor.hpp"
#include "UtLength.hpp"
#include "UtReal.hpp"
#include "VaAttachmentZone.hpp"

class UtoCircleShape;

namespace vespa
{
class VESPATK_DEPRECATED_EXPORT VaAttachmentZoneCircle : public VaAttachmentZone
{
public:
   VaAttachmentZoneCircle(VaEntity& aParent, VaViewer* aViewerPtr);
   ~VaAttachmentZoneCircle() override;

   VaAttachmentZoneCircle*       Ref() override { return this; }
   const VaAttachmentZoneCircle* Ref() const override { return this; }

   void SetRadius(const UtLength& aRadius);
   void SetMinRadius(const UtLength& aMinRadius);
   void SetOrientation(double aHeading, double aPitch, double aRoll) override;
   void SetBegAngle(const UtAngleR& aBegAngle);
   void SetEndAngle(const UtAngleR& aEndAngle);
   void SetX(const UtLength& aX);
   void SetY(const UtLength& aY);
   void SetUseShader(bool aUseShader);
   void SetShowTopLines(bool aShowTopLines);
   void SetHeight(const UtLength& aHeight) override;
   void SetBaseHeight(const UtLength& aBaseHeight) override;
   void SetColor(const UtColor& aColor) override;
   void SetFillColor(const UtColor& aFillColor) override;
   void SetBlendFactor(float aFactor) override;
   void SetLineWidth(double aLineWidth) override;
   void SetLineStyle(int aLineStyle) override;
   void SetEnableFill(bool aEnableFill) override;

   double          GetArea() const;
   double          GetCircumference() const;
   const UtLength& GetRadius() const { return mRadius; }
   const UtLength& GetMinRadius() const { return mMinRadius; }
   const UtAngleR& GetBegAngle() const { return mBegAngle; }
   const UtAngleR& GetEndAngle() const { return mEndAngle; }
   const UtLength& GetX() const { return mX; }
   const UtLength& GetY() const { return mY; }

   const std::string& GetShapeName() const override { return mOuterCircleShapeNameL; }

   unsigned int GetPickMode() const override { return cPICK_NOTHING; }
   bool         Pick(VaViewer*                aViewerPtr,
                     int                      aMouseX,
                     int                      aMouseY,
                     bool                     aToggle,
                     bool                     aDoubleClick         = false,
                     const VaAttachmentSubId& aAttachmentSubPartId = 0) override;

private:
   void ShadersToggled();

   void PrivateLoad() override;

   void LoadState();

   void NormalizeAngles();
   void SetAngleBounds();

   UtoCircleShape* mOuterCirclePtrL;
   std::string     mOuterCircleShapeNameL; // low
   UtoCircleShape* mOuterCirclePtrH;
   std::string     mOuterCircleShapeNameH; // high
   UtoCircleShape* mInnerCirclePtrL;
   std::string     mInnerCircleShapeNameL; // low
   UtoCircleShape* mInnerCirclePtrH;
   std::string     mInnerCircleShapeNameH; // high
   UtoCircleShape* mCircleFillPtr;
   std::string     mCircleShapeNameF; // fill
   UtLength        mRadius;
   UtLength        mMinRadius;
   UtAngleR        mBegAngle;
   UtAngleR        mEndAngle;
   UtLength        mX;
   UtLength        mY;
   bool            mUseShader;
   bool            mShowTopLines;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentZoneCircle)
#endif
