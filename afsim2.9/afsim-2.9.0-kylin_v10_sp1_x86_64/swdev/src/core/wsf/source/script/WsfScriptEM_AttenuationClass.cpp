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

#include "WsfScriptEM_AttenuationClass.hpp"

// =================================================================================================
WsfScriptEM_AttenuationClass::WsfScriptEM_AttenuationClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptObjectClass(aClassName, aTypesPtr)
{
   SetClassName("WsfEM_Attenuation");

   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;
}
