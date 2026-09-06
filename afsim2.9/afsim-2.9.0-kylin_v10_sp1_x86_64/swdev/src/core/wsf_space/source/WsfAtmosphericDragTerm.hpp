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

#ifndef WSFATMOSPHERICDRAGTERM_HPP
#define WSFATMOSPHERICDRAGTERM_HPP

#include "WsfOrbitalDynamicsTerm.hpp"

class UtCalendar;
class UtInput;
#include "wsf_space_export.h"

#include "UtVec3.hpp"
#include "WsfAtmosphere.hpp"

//! An orbital dynamics term that represents the drag experienced by the spacecraft.
class WSF_SPACE_EXPORT WsfAtmosphericDragTerm : public WsfOrbitalDynamicsTerm
{
public:
   WsfAtmosphericDragTerm()                                     = default;
   WsfAtmosphericDragTerm(const WsfAtmosphericDragTerm& aOther) = default;
   ~WsfAtmosphericDragTerm() override                           = default;

   bool ProcessInput(UtInput& aInput) override;

   WsfAtmosphericDragTerm* Clone() const override { return new WsfAtmosphericDragTerm{*this}; }

   const char* GetScriptClassName() const override { return "WsfAtmosphericDragTerm"; }

   UtVec3d ComputeAcceleration(double            aMass,
                               const UtCalendar& aTime,
                               const UtVec3d&    aPosition,
                               const UtVec3d&    aVelocity) const override;

   bool Initialize(const WsfOrbitalDynamics& aDynamics) override;

   //! Return the drag coefficient used by this term.
   double GetDragCoefficient() const { return mDragCoefficient; }

   //! Set the drag coefficient used by this term.
   void SetDragCoefficient(double aDragCoefficient) { mDragCoefficient = aDragCoefficient; }

   //! Return the cross sectional area in m^2 used by this term.
   double GetCrossSectionalArea() const { return mCrossSectionalArea; }

   // Set the cross sectional area in m^2 used by this term.
   void SetCrossSectionalArea(double aCrossSectionalArea) { mCrossSectionalArea = aCrossSectionalArea; }

   //! Get the name of the atmosphere model used by this term to compute the density.
   const std::string& GetAtmosphereModelName() const { return mModelName; }

   //! Set the name of the atmosphere model used by this term to compute the density.
   void SetAtmosphereModelName(const std::string& aModelName) { mModelName = aModelName; }

   std::string GetTermType() const override { return cTYPE; }

   constexpr static const char* cTYPE{"atmospheric_drag"};

private:
   double                        mDragCoefficient{0.0};
   double                        mCrossSectionalArea{0.0};
   std::string                   mModelName{};
   const wsf::space::Atmosphere* mAtmospherePtr{nullptr};
};

#endif // WSFATMOSPHERICDRAGTERM_HPP
