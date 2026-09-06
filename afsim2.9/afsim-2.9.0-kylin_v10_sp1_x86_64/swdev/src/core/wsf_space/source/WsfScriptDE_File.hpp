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

#ifndef WSFSCRIPTDE_FILE_HPP
#define WSFSCRIPTDE_FILE_HPP

#include "wsf_space_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WSF_SPACE_EXPORT WsfScriptDE_File : public UtScriptClass
{
public:
   WsfScriptDE_File(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptDE_File() override = default;

   void Destroy(void* aObjectPtr) override;

   // File methods
   UT_DECLARE_SCRIPT_METHOD(Construct); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(DE_Number); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(JD_Start);  // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(JD_End);    // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(FileName);  // NO_DOC | FOR_TEST_ONLY

   // Static planet identifier methods
   UT_DECLARE_SCRIPT_METHOD(MERCURY);                 // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(VENUS);                   // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(EARTH);                   // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(MARS);                    // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(JUPITER);                 // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SATURN);                  // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(URANUS);                  // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(NEPTUNE);                 // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(PLUTO);                   // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(MOON);                    // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SUN);                     // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(SOLAR_SYSTEM_BARYCENTER); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(EARTH_MOON_BARYCENTER);   // NO_DOC | FOR_TEST_ONLY

   // The main method
   UT_DECLARE_SCRIPT_METHOD(PlanetPosition); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(PlanetVelocity); // NO_DOC | FOR_TEST_ONLY
};

#endif // WSFSCRIPTDE_FILE_HPP
