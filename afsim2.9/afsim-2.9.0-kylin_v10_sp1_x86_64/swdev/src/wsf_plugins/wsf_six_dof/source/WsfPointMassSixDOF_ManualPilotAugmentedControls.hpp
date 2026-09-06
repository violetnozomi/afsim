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

#ifndef WSFPOINTMASSSIXDOFMANUALPILOTAUGMENTEDCONTROLS_HPP
#define WSFPOINTMASSSIXDOFMANUALPILOTAUGMENTEDCONTROLS_HPP

#include "WsfPointMassSixDOF_ManualPilot.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PointMassManualPilotAugmentedControls : public PointMassManualPilot
{
   friend class PointMassPilotManager;

public:
   PointMassManualPilotAugmentedControls()        = default;
   ~PointMassManualPilotAugmentedControls()       = default;
   PointMassManualPilotAugmentedControls& operator=(const PointMassManualPilotAugmentedControls& other) = delete;

   PointMassManualPilotAugmentedControls* Clone() const;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "Manual-CAS"; }

protected:
   explicit PointMassManualPilotAugmentedControls(PointMassPilotObject* aPilotObject);

   PointMassManualPilotAugmentedControls(const PointMassManualPilotAugmentedControls& aSrc) = default;
};
} // namespace six_dof
} // namespace wsf

#endif // MRMMANUALPILOTAUGMENTEDCONTROLS_H
