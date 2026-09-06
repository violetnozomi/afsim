// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEM_SURFACECLUTTER_HPP
#define WSFEM_SURFACECLUTTER_HPP

#include "wsf_mil_export.h"

#include "WsfEM_Clutter.hpp"
class WsfEM_Rcvr;
class WsfEM_Xmtr;

//! Land and sea clutter model.
class WSF_MIL_EXPORT WsfEM_SurfaceClutter : public WsfEM_Clutter
{
public:
   WsfEM_SurfaceClutter();
   WsfEM_SurfaceClutter& operator=(const WsfEM_SurfaceClutter&) = delete;
   ~WsfEM_SurfaceClutter() override                             = default;

   static WsfEM_Clutter* ObjectFactory(const std::string& aTypeName);

   WsfEM_Clutter* Clone() const override;

   bool Initialize(WsfEM_Rcvr* aRcvrPtr) override;

   bool ProcessInput(UtInput& aInput) override;

   double ComputeClutterPower(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment, double aProcessingFactor) override;

protected:
   WsfEM_SurfaceClutter(const WsfEM_SurfaceClutter& aSrc) = default;

private:
   double ContinuousWaveClutter(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment);

   double PulseDopplerClutter(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment);

   void ComputeGeometry(WsfEM_Interaction& aInteraction,
                        WsfEnvironment&    aEnvironment,
                        double             aViewVecNED[2],
                        double&            aAdjustedAntennaHeight,
                        double&            aAdjustedEarthRadius,
                        double&            aClutterHorizon,
                        double&            aDepressionAngle);

   double ComputeRF_TwoWayPower(WsfEM_Interaction& aInteraction,
                                const double       aViewVecNED[2],
                                double             aDepressionAngle,
                                double             aSlantRange,
                                double             aCrossSection);

   double mMinimumRange;
   double mMaximumRange;
   double mContinuousWaveClutterPower;
   bool   mUseSALRAM_DataTables;
};

#endif
