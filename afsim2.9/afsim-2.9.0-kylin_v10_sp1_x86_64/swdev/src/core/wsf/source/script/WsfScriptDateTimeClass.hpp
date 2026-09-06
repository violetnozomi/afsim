// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTDATETIMECLASS_HPP
#define WSFSCRIPTDATETIMECLASS_HPP

#include "wsf_export.h"

#include <string>

#include "UtScriptBasicTypes.hpp"

class WSF_EXPORT WsfScriptDateTimeClass : public UtScriptClass
{
public:
   WsfScriptDateTimeClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr);
   ~WsfScriptDateTimeClass() override;

   UT_DECLARE_SCRIPT_METHOD(CurrentTime);
   UT_DECLARE_SCRIPT_METHOD(StartTime);
   UT_DECLARE_SCRIPT_METHOD(SimulationTime);

protected:
private:
};

#endif
