// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTBRAWLERMOVERCLASS_HPP
#define WSFSCRIPTBRAWLERMOVERCLASS_HPP

#include "wsf_brawler_export.h"

#include "script/WsfScriptMoverClass.hpp"

class WSF_BRAWLER_EXPORT WsfScriptBrawlerMoverClass : public WsfScriptMoverClass
{
public:
   WsfScriptBrawlerMoverClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   // Maneuvers
   UT_DECLARE_SCRIPT_METHOD(Slice_1);
   UT_DECLARE_SCRIPT_METHOD(Slice_2);
   UT_DECLARE_SCRIPT_METHOD(Prlvl);
   UT_DECLARE_SCRIPT_METHOD(Prlvlg);
   UT_DECLARE_SCRIPT_METHOD(SetMaxThrottle);
};

#endif
