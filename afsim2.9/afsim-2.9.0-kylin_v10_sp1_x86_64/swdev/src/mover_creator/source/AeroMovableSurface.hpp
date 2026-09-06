// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef AERO_MOVABLE_SURFACE_HPP
#define AERO_MOVABLE_SURFACE_HPP

#include "AeroSurface.hpp"

namespace Designer
{
   struct Controls
   {
      bool   mUseExponentialAngleMapping = false;
      double mControlSurfaceMinAngle_deg = 0.0;
      double mControlSurfaceMaxAngle_deg = 0.0;
      double mActuatorMinRate_dps = -300.00;
      double mActuatorMaxRate_dps = 300.00;
      double mActuatorMinAngle_deg = 0.0;
      double mActuatorMaxAngle_deg = 0.0;
      double mCurrentControlSurfaceAngle_deg = 0.0;
   };

}

#endif