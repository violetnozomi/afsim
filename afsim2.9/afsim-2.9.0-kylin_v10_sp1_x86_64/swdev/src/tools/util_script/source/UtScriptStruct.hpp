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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTSCRIPTSTRUCT_HPP
#define UTSCRIPTSTRUCT_HPP

#include "util_script_export.h"

#include <memory>

class UtInput;
#include "UtScriptClassDefine.hpp"

class UTIL_SCRIPT_EXPORT UtScriptStruct : public UtScriptClass
{
public:
   UtScriptStruct(UtScriptTypes* aScriptTypePtr);

   UtScriptStruct(const std::string&               aName,
                  UtScriptTypes*                   aScriptTypesPtr,
                  std::unique_ptr<UtScriptContext> aScriptContextPtr);

   static bool ProcessInput(UtInput& aInput, UtScriptEnvironment* aTypesPtr);

   //! Return the pointer to the script context for the 'script_struct' (pseudo-class).
   UtScriptContext* GetStructContext() const { return mScriptContextPtr.get(); }

   void* Create(const UtScriptContext& /*aContext*/) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UtScriptContext* GetContext(void* aObjectPtr) override;
   std::string      ToString(void* /*aObjectPtr*/) const override;
   bool             EqualTo(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(New);

private:
   void ProcessInput(UtInput& aInput);

   std::unique_ptr<UtScriptContext> mScriptContextPtr{nullptr};
};

#endif
