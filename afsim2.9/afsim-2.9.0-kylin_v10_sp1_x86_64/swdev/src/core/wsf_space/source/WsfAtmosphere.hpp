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

#ifndef WSFATMOSPHERE_HPP
#define WSFATMOSPHERE_HPP

#include "WsfObject.hpp"

class UtCalendar;
#include "UtCentralBody.hpp"
#include "UtCloneablePtr.hpp"
class UtLLAPos;

#include "wsf_space_export.h"

namespace wsf
{
namespace space
{

class WSF_SPACE_EXPORT Atmosphere : public WsfObject
{
public:
   Atmosphere();
   Atmosphere(const Atmosphere& aOther) = default;
   virtual ~Atmosphere()                = default;

   Atmosphere* Clone() const override = 0;

   bool ProcessInput(UtInput& aInput) override;

   const ut::CentralBody& GetCentralBody() const { return *mCentralBodyPtr; }

   const char* GetScriptClassName() const override { return "WsfAtmosphereModel"; }

   //! Return the model density for a given time and location.
   //!
   //! This will return the density of the atmosphere (in kg / m^3) at the
   //! given @p aEpoch and given body-centric latitude, longitude and altitude,
   //! @p aLLA. This is the only state variable that subclasses of this
   //! class are required to implement.
   //!
   //! \param aEpoch  The date and time at which the density is being queried.
   //! \param aLLA    The body-centric latitude, longitude and altitude at which the density is being queried.
   //! \returns       The density (in kg / m^3).
   virtual double GetDensity(const UtCalendar& aEpoch, const UtLLAPos& aLLA) const = 0;

protected:
   ut::CloneablePtr<ut::CentralBody> mCentralBodyPtr;
};

} // namespace space
} // namespace wsf

#endif // WSFATMOSPHERE_HPP
