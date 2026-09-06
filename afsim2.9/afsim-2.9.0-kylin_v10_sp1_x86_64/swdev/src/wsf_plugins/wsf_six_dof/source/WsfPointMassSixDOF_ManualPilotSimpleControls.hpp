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

#ifndef WSFPOINTMASSSIXDOFMANUALPILOTSIMPLECONTROLS_HPP
#define WSFPOINTMASSSIXDOFMANUALPILOTSIMPLECONTROLS_HPP

#include "WsfPointMassSixDOF_ManualPilot.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PointMassManualPilotSimpleControls : public PointMassManualPilot
{
   friend class PointMassPilotManager;

public:
   PointMassManualPilotSimpleControls()        = default;
   ~PointMassManualPilotSimpleControls()       = default;
   PointMassManualPilotSimpleControls& operator=(const PointMassManualPilotSimpleControls& other) = delete;

   PointMassManualPilotSimpleControls* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Manual-Simple"; }

   bool UseSimpleYawDamper() { return mSimpleYawDamper; }

protected:
   explicit PointMassManualPilotSimpleControls(PointMassPilotObject* aPilotObject);

   PointMassManualPilotSimpleControls(const PointMassManualPilotSimpleControls& aSrc) = default;

   bool mSimpleYawDamper = false;
};
} // namespace six_dof
} // namespace wsf


#endif // MRMMANUALPILOTSIMPLECONTROLS_H
