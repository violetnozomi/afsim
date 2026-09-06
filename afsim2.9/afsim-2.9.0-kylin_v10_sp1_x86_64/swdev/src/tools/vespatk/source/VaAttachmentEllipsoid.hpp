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

#ifndef VaAttachmentEllipsoid_hpp
#define VaAttachmentEllipsoid_hpp

#include "vespatk_export.h"

#include "UtAngleR.hpp"
#include "UtColor.hpp"
#include "VaAttachment.hpp"

namespace vespa
{
class VESPATK_EXPORT VaAttachmentEllipsoid : public VaAttachment
{
public:
   VaAttachmentEllipsoid(VaEntity& aParent, VaViewer* aViewerPtr);
   ~VaAttachmentEllipsoid() override;

   void SetAxes(const UtLength& aAxisX, const UtLength& aAxisY, const UtLength& aAxisZ);

   void SetAxisX(const UtLength& aAxisX);
   void SetAxisY(const UtLength& aAxisY);
   void SetAxisZ(const UtLength& aAxisZ);
   void SetOrientation(const UtAngleR& aYaw, const UtAngleR& aPitch, const UtAngleR& aRoll);

   virtual void SetColor(const UtColor& aColor);
   virtual void SetLineWidth(double aLineWidth);
   virtual void SetLineStyle(int aLineStyle);

   virtual void SetEnableFill(bool aEnableFill);
   void         ShowSliceLines(bool aShow);
   void         ShowStackLines(bool aShow);

   double GetVolume() const;

   const UtLength& GetAxisX() const { return mAxisX; }
   const UtLength& GetAxisY() const { return mAxisY; }
   const UtLength& GetAxisZ() const { return mAxisZ; }

private:
   void PrivateLoad() override;

   void LoadState();

   UtoSphereShape* mEllipsoidPtr;
   std::string     mEllipsoidShapeName;
   UtLength        mAxisX;
   UtLength        mAxisY;
   UtLength        mAxisZ;
   UtAngleR        mYaw;
   UtAngleR        mPitch;
   UtAngleR        mRoll;
   UtColor         mColor;
   double          mLineWidth;
   int             mLineStyle;
   int             mLineStippleFactor;
   unsigned short  mLineStipplePattern;
   bool            mEnableFill;
   bool            mShowSlices;
   bool            mShowStacks;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentEllipsoid)
#endif
