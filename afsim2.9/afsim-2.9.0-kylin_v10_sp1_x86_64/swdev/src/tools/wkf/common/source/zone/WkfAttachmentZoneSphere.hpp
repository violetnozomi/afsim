// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WkfAttachmentZoneSphere_hpp
#define WkfAttachmentZoneSphere_hpp

#include "wkf_common_export.h"

#include <string>

#include "UtAngleR.hpp"
#include "UtColor.hpp"
#include "UtLength.hpp"
#include "UtReal.hpp"
#include "WkfAttachmentZone.hpp"

class UtoSphereShape;

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentZoneSphere : public AttachmentZone
{
public:
   AttachmentZoneSphere(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr);

   AttachmentZoneSphere*       Ref() override { return this; }
   const AttachmentZoneSphere* Ref() const override { return this; }

   void SetRadius(const UtLength& aRadius);
   void SetMinRadius(const UtLength& aRadius);
   void SetOrientation(double aHeading, double aPitch, double aRoll) override;
   void SetBegAngle(const UtAngleR& aBegAngle);
   void SetEndAngle(const UtAngleR& aEndAngle);
   void SetX(const UtLength& aX);
   void SetY(const UtLength& aY);
   void SetUseShader(bool aUseShader);
   void SetMaxAltitude(const UtLength& aMaxAlt) override;
   void SetMinAltitude(const UtLength& aMinAlt) override;

   virtual void SetMaxAltitudeSet(bool aIsSet) override { mIsMaxAltSet = aIsSet; }
   bool         IsMaxAltitudeSet() const { return mIsMaxAltSet; }
   virtual void SetMinAltitudeSet(bool aIsSet) override { mIsMinAltSet = aIsSet; }
   bool         IsMinAltitudeSet() const { return mIsMinAltSet; }

   void SetColor(const UtColor& aColor) override;
   void SetFillColor(const UtColor& aFillColor) override;
   void SetBlendFactor(float aFactor) override;
   void SetLineWidth(double aLineWidth) override;
   void SetLineStyle(int aLineStyle) override;

   const UtLength& GetRadius() const { return mRadius; }
   const UtAngleR& GetBegAngle() const { return mBegAngle; }
   const UtAngleR& GetEndAngle() const { return mEndAngle; }
   const UtLength& GetX() const { return mX; }
   const UtLength& GetY() const { return mY; }

   const std::string& GetShapeName() const override { return mSphereShapeName; }

   unsigned int GetPickMode() const override { return cPICK_NOTHING; }
   bool         Pick(vespa::VaViewer*                aViewerPtr,
                     int                             aMouseX,
                     int                             aMouseY,
                     bool                            aToggle,
                     bool                            aDoubleClick         = false,
                     const vespa::VaAttachmentSubId& aAttachmentSubPartId = 0) override;

private:
   void ShadersToggled();
   void PrivateLoad() override;

   void LoadState();

   void NormalizeAngles();
   void SetAngleBounds();

   UtoSphereShape* mSpherePtr;
   std::string     mSphereShapeName;
   UtAngleR        mBegAngle;
   UtAngleR        mEndAngle;
   UtLength        mX;
   UtLength        mY;
   UtLength        mRadius;
   UtLength        mMinRadius;
   bool            mUseShader;
   bool            mIsMaxAltSet;
   bool            mIsMinAltSet;
};
} // end namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentZoneSphere)
#endif
