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

#ifndef WSFRECTANGULARFIELDOFVIEW_HPP
#define WSFRECTANGULARFIELDOFVIEW_HPP

#include "wsf_export.h"

class UtInput;
#include "UtMath.hpp"
#include "WsfFieldOfView.hpp"

//! A field of view with rectangular az-el extents.
class WSF_EXPORT WsfRectangularFieldOfView : public WsfFieldOfView
{
public:
   WsfRectangularFieldOfView();

   bool operator==(const WsfRectangularFieldOfView& aRhs) const;
   bool operator!=(const WsfRectangularFieldOfView& aRhs) const;

   bool        ProcessInput(UtInput& aInput) override;
   const char* GetScriptClassName() const override { return "WsfRectangularFieldOfView"; }

   WsfFieldOfView* Clone() const override { return new WsfRectangularFieldOfView(*this); }

   bool WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl) override;

   bool GetAzimuthFieldOfView(double& aMinAzFOV, double& aMaxAzFOV) const override
   {
      aMinAzFOV = mMinAzFOV;
      aMaxAzFOV = mMaxAzFOV;
      return true;
   }

   bool SetAzimuthFieldOfView(double aMinAzFOV, double aMaxAzFOV) override;

   bool GetElevationFieldOfView(double& aMinElFOV, double& aMaxElFOV) const override
   {
      aMinElFOV = mMinElFOV;
      aMaxElFOV = mMaxElFOV;
      return true;
   }

   bool SetElevationFieldOfView(double aMinElFOV, double aMaxElFOV) override;

private:
   //! @name Field of view with respect current cue.
   //! This should encompass the scan limits PLUS whatever the beam pattern might pick up
   //! when at the physical limits of the scan pattern.
   //@{
   double mMinAzFOV{-UtMath::cPI};        // radians
   double mMaxAzFOV{UtMath::cPI};         // radians
   double mMinElFOV{-UtMath::cPI_OVER_2}; // radians
   double mMaxElFOV{UtMath::cPI_OVER_2};  // radians
                                          //@}
};

#endif
