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

#include "WsfScriptPiecewiseExponentialAtmosphere.hpp"

#include "WsfPiecewiseExponentialAtmosphere.hpp"

WsfScriptPiecewiseExponentialAtmosphere::WsfScriptPiecewiseExponentialAtmosphere(const std::string& aClassName,
                                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptAtmosphere(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPiecewiseExponentialAtmosphere");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPiecewiseExponentialAtmosphere,
                        wsf::space::PiecewiseExponentialAtmosphere,
                        Construct,
                        0,
                        "WsfPiecewiseExponentialAtmosphere",
                        "")
{
   auto termPtr = ut::make_unique<wsf::space::PiecewiseExponentialAtmosphere>();
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}
