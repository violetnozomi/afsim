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

#ifndef WSFSCRIPTINTEGRATINGSPACEMOVER_HPP
#define WSFSCRIPTINTEGRATINGSPACEMOVER_HPP

#include "wsf_space_export.h"

#include "WsfScriptSpaceMoverClass.hpp"

class WSF_SPACE_EXPORT WsfScriptIntegratingSpaceMover : public WsfScriptSpaceMoverClass
{
public:
   WsfScriptIntegratingSpaceMover(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptIntegratingSpaceMover() override = default;

   UT_DECLARE_SCRIPT_METHOD(OrbitalDynamics);
   UT_DECLARE_SCRIPT_METHOD(SetOrbitalDynamics);
   UT_DECLARE_SCRIPT_METHOD(OrbitalIntegrator);
   UT_DECLARE_SCRIPT_METHOD(SetOrbitalIntegrator);
};

#endif // WSFSCRIPTINTEGRATINGSPACEMOVER_HPP
