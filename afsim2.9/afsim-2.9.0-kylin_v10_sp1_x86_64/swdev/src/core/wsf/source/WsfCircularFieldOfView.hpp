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

#ifndef WSFCIRCULARFIELDOFVIEW_HPP
#define WSFCIRCULARFIELDOFVIEW_HPP

#include "wsf_export.h"

class UtInput;
#include "UtMath.hpp"
#include "WsfFieldOfView.hpp"

//! A field of view with circular extent defined by a conical half-angle.
class WSF_EXPORT WsfCircularFieldOfView : public WsfFieldOfView
{
public:
   WsfCircularFieldOfView();

   bool operator==(const WsfCircularFieldOfView& aRhs) const { return (mHalfAngle == aRhs.mHalfAngle); }
   bool operator!=(const WsfCircularFieldOfView& aRhs) const { return (mHalfAngle != aRhs.mHalfAngle); }

   bool        ProcessInput(UtInput& aInput) override;
   const char* GetScriptClassName() const override { return "WsfCircularFieldOfView"; }

   WsfCircularFieldOfView* Clone() const override { return new WsfCircularFieldOfView(*this); }

   bool WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl) override;

   //! Provide the azimuth field of view.
   bool GetAzimuthFieldOfView(double& aMinAzFOV, double& aMaxAzFOV) const override
   {
      aMinAzFOV = -mHalfAngle;
      aMaxAzFOV = mHalfAngle;
      return true;
   }

   //! Provide the elevation field of view.
   bool GetElevationFieldOfView(double& aMinElFOV, double& aMaxElFOV) const override
   {
      aMinElFOV = -mHalfAngle;
      aMaxElFOV = mHalfAngle;
      return true;
   }

   //! Return the conical half-angle defining the field-of-view.
   double GetHalfAngle() const { return mHalfAngle; }

   //! Set the conical half-angle defining the field-of-view.
   void SetHalfAngle(double aHalfAngle) { mHalfAngle = aHalfAngle; }

private:
   //! @name Field of view with respect current cue.
   //! This should encompass the scan limits PLUS whatever the beam pattern might pick up
   //! when at the physical limits of the scan pattern.
   //@{
   double mHalfAngle{UtMath::cPI_OVER_2};
   //@}
};

#endif
