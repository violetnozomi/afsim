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

#ifndef WSFPOINTMASSSIXDOFMANUALPILOT_HPP
#define WSFPOINTMASSSIXDOFMANUALPILOT_HPP

#include "WsfPointMassSixDOF_PilotObject.hpp"

namespace wsf
{
namespace six_dof
{
class PointMassPilotManager;

class WSF_SIX_DOF_EXPORT PointMassManualPilot : public PointMassPilotObject
{
   friend class PointMassPilotManager;

public:
   PointMassManualPilot()        = default;
   ~PointMassManualPilot()       = default;
   PointMassManualPilot& operator=(const PointMassManualPilot& other) = delete;

   // This returns the "type" of pilot object -- each child of this class
   // should override this function.
   std::string GetPilotType() const override { return "ManualPilot"; }

protected:
   explicit PointMassManualPilot(PointMassPilotObject* aPilotObject);

   PointMassManualPilot(const PointMassManualPilot& aSrc) = default;
};
} // namespace six_dof
} // namespace wsf


#endif // MRMMANUALPILOT_H
