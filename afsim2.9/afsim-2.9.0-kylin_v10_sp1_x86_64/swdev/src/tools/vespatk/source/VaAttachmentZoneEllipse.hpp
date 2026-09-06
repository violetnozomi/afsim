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

#ifndef VaAttachmentZoneEllipse_hpp
#define VaAttachmentZoneEllipse_hpp

#include "vespatk_export.h"

#include <string>

#include "UtAngleR.hpp"
#include "UtColor.hpp"
#include "UtLength.hpp"
#include "UtReal.hpp"
#include "VaAttachmentZone.hpp"

class UtoEllipseShape;

namespace vespa
{
class VESPATK_DEPRECATED_EXPORT VaAttachmentZoneEllipse : public VaAttachmentZone
{
public:
   VaAttachmentZoneEllipse(VaEntity& aParent, VaViewer* aViewerPtr);
   ~VaAttachmentZoneEllipse() override;

   VaAttachmentZoneEllipse*       Ref() override { return this; }
   const VaAttachmentZoneEllipse* Ref() const override { return this; }

   void SetRadius(const UtLength& aRadius);
   void SetMinRadius(const UtLength& aMinRadius);
   // Set the "radius" of the Longitudinal axis (horizontal / X axis);
   void SetLonAxisRadius(const UtLength& aLonAxis);
   // Set the "radius" of the Latitudinal axis (vertical / Y axis);
   void SetLatAxisRadius(const UtLength& aLatAxis);
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
   const UtLength& GetLonAxisRadius() const { return mLonAxis; }
   const UtLength& GetLatAxisRadius() const { return mLatAxis; }
   const UtAngleR& GetBegAngle() const { return mBegAngle; }
   const UtAngleR& GetEndAngle() const { return mEndAngle; }
   const UtLength& GetX() const { return mX; }
   const UtLength& GetY() const { return mY; }

   const std::string& GetShapeName() const override { return mOuterEllipseShapeNameL; }

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
   void UpdateInnerCircle();

   UtoEllipseShape* mOuterEllipsePtrL;
   std::string      mOuterEllipseShapeNameL; // low
   UtoEllipseShape* mOuterEllipsePtrH;
   std::string      mOuterEllipseShapeNameH; // high
   UtoCircleShape*  mInnerCirclePtrL;
   std::string      mInnerCircleShapeNameL; // low
   UtoCircleShape*  mInnerCirclePtrH;
   std::string      mInnerCircleShapeNameH; // high
   UtoEllipseShape* mEllipsePtrF;
   std::string      mEllipseShapeNameF; // fill
   UtLength         mLonAxis;
   UtLength         mLatAxis;
   UtAngleR         mBegAngle;
   UtAngleR         mEndAngle;
   UtLength         mX;
   UtLength         mY;
   UtLength         mRadius;
   UtLength         mMinRadius;
   bool             mUseShader;
   bool             mShowTopLines;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentZoneEllipse)
#endif
