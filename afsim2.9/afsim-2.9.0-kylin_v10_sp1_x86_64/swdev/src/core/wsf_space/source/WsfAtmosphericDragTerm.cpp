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

#include "WsfAtmosphericDragTerm.hpp"

#include "UtInput.hpp"
#include "UtLLAPos.hpp"
#include "UtLog.hpp"
#include "WsfAtmosphereSimulationExtension.hpp"
#include "WsfOrbitalDynamics.hpp"

bool WsfAtmosphericDragTerm::ProcessInput(UtInput& aInput)
{
   bool        retval{true};
   std::string command = aInput.GetCommand();
   if (command == "drag_coefficient")
   {
      aInput.ReadValue(mDragCoefficient);
      aInput.ValueGreater(mDragCoefficient, 0.0);
   }
   else if (command == "cross_sectional_area")
   {
      aInput.ReadValueOfType(mCrossSectionalArea, UtInput::cAREA);
      aInput.ValueGreater(mCrossSectionalArea, 0.0);
   }
   else if (command == "atmosphere_model")
   {
      aInput.ReadCommand(mModelName);
   }
   else
   {
      retval = false;
   }
   return retval;
}

UtVec3d WsfAtmosphericDragTerm::ComputeAcceleration(double            aMass,
                                                    const UtCalendar& aTime,
                                                    const UtVec3d&    aPosition,
                                                    const UtVec3d&    aVelocity) const
{
   UtVec3d retval{0.0, 0.0, 0.0};
   auto    dynamicsPtr = GetDynamics();
   if (dynamicsPtr && mAtmospherePtr)
   {
      dynamicsPtr->UpdateConverterTime(aTime);
      UtVec3d  velWCS    = dynamicsPtr->GetWCS_VelocityFromECI(aPosition, aVelocity);
      double   vMag      = velWCS.Normalize();
      UtLLAPos llaPos    = dynamicsPtr->GetLLA_FromWCS(dynamicsPtr->GetWCS_PositionFromECI(aPosition));
      double   density   = mAtmospherePtr->GetDensity(aTime, llaPos);
      double   prefactor = -0.5 * mCrossSectionalArea * mDragCoefficient * vMag * vMag * density / aMass;
      return dynamicsPtr->GetECI_AccelerationFromWCS(velWCS.Multiply(prefactor));
   }
   return retval;
}

bool WsfAtmosphericDragTerm::Initialize(const WsfOrbitalDynamics& aDynamics)
{
   WsfOrbitalDynamicsTerm::Initialize(aDynamics);
   mAtmospherePtr = wsf::space::AtmosphereSimulationExtension::Get(*aDynamics.GetSimulation()).GetAtmosphere(mModelName);
   if (!mAtmospherePtr)
   {
      auto msg = ut::log::error() << "Unable to initialize orbital dynamics term.";
      msg.AddNote() << "Unable to find atmosphere named '" << mModelName << "'.";
   }
   return mAtmospherePtr != nullptr;
}
