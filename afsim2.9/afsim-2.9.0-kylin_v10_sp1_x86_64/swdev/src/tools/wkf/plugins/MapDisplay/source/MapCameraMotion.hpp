// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef MAPDISPLAYCAMERAMOTION_HPP
#define MAPDISPLAYCAMERAMOTION_HPP

#include "VaCallbackHolder.hpp"
#include "VaCameraMotionGeocentric.hpp"

namespace vespa
{
class VaEntity;
}

namespace Map
{
class CameraMotion : public vespa::VaCameraMotionGeocentric
{
public:
   CameraMotion(vespa::VaCameraBase* aCameraPtr);

   bool MouseScroll(int aDirection) override;

   bool MouseMotion(int aX, int aY, unsigned int aState) override;

   bool MouseButtonDoublePress(int aX, int aY, unsigned int aButton, unsigned int aState) override;

   bool GesturePinch(double aScale, double aAngle, int aCenterX, int aCenterY) override;

   void Follow(vespa::VaEntity* aEntityPtr);

   void AnimateToPosition(const vespa::VaPosition& aPosition, double aYaw, double aPitch, double aTime) override;

   bool RecommendNearFar(double& aNear, double& aFar) override;

   void SetZoomOnCursor(bool aState) { mZoomAtCursor = aState; }

   double GetDist() const override;

private:
   void BuildViewMatrix() override;

   void EntityMovedHandler(vespa::VaEntity* aEntityPtr, bool aMoving);
   void InteruptFollow();

   int mFollowCallback{0};

   vespa::VaCallbackHolder mPCallbacks;
   float                   mEntityAlt{0};
   bool                    mZoomAtCursor{true};

   static constexpr double cMinFollowRange = 300.0;
};
} // namespace Map
#endif
