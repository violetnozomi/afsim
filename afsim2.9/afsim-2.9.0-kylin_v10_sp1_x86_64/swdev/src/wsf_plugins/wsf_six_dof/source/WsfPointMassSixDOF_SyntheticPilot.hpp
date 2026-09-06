// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPOINTMASSSIXDOFSYNTHETICPILOT_HPP
#define WSFPOINTMASSSIXDOFSYNTHETICPILOT_HPP

#include "WsfPointMassSixDOF_PilotObject.hpp"

namespace wsf
{
namespace six_dof
{
class PointMassPilotManager;

class WSF_SIX_DOF_EXPORT PointMassSyntheticPilot : public PointMassPilotObject
{
   friend class PointMassPilotManager;

public:
   PointMassSyntheticPilot()        = default;
   ~PointMassSyntheticPilot()       = default;
   PointMassSyntheticPilot& operator=(const PointMassSyntheticPilot& other) = delete;

   PointMassSyntheticPilot* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "SyntheticPilot"; }

   void HoldAltitude() override;
   void HoldVerticalSpeed() override;
   void HoldPitchAngle() override;
   void HoldBankAngle() override;
   void HoldHeading() override;
   void HoldSpeedKCAS() override;
   void HoldSpeedKTAS() override;
   void HoldSpeedMach() override;

protected:
   explicit PointMassSyntheticPilot(PointMassPilotObject* aPilotObject);

   PointMassSyntheticPilot(const PointMassSyntheticPilot& aSrc) = default;
};
} // namespace six_dof
} // namespace wsf

#endif // MRMSYNTHETICPILOT_H
