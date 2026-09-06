// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTORBITALCONJUNCTIONPROCESSOR_HPP
#define WSFSCRIPTORBITALCONJUNCTIONPROCESSOR_HPP

#include "wsf_space_export.h"

#include "script/WsfScriptProcessorClass.hpp"

//! A processor that will predict orbital conjunctions from tracks.
class WSF_SPACE_EXPORT WsfScriptOrbitalConjunctionProcessor : public WsfScriptProcessorClass
{
public:
   WsfScriptOrbitalConjunctionProcessor(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptOrbitalConjunctionProcessor() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;
};

#endif // WSFSCRIPTORBITALCONJUNCTIONPROCESSOR_HPP