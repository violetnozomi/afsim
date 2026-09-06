// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTSOLARTERMINATOR_HPP
#define WSFSCRIPTSOLARTERMINATOR_HPP

#include "wsf_space_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WSF_SPACE_EXPORT WsfScriptSolarTerminator : public UtScriptClass
{
public:
   WsfScriptSolarTerminator(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptSolarTerminator() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetPhaseOfDay);
   UT_DECLARE_SCRIPT_METHOD(TWILIGHT_CIVIL);
   UT_DECLARE_SCRIPT_METHOD(TWILIGHT_NAUTICAL);
   UT_DECLARE_SCRIPT_METHOD(TWILIGHT_ASTRONOMICAL);
   UT_DECLARE_SCRIPT_METHOD(PHASE_DAY);
   UT_DECLARE_SCRIPT_METHOD(PHASE_TWILIGHT);
   UT_DECLARE_SCRIPT_METHOD(PHASE_NIGHT);

   UT_DECLARE_SCRIPT_METHOD(PlatformSolarIllumination);
   UT_DECLARE_SCRIPT_METHOD(ILLUMINATED);
   UT_DECLARE_SCRIPT_METHOD(EARTH_PENUMBRA);
   UT_DECLARE_SCRIPT_METHOD(EARTH_UMBRA);
};

#endif // WSFSCRIPTSOLARTERMINATOR_HPP
