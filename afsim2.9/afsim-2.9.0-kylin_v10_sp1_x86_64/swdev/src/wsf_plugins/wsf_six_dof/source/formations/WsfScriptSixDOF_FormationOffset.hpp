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

#ifndef WSF_SIXDOF_SCRIPTFORMATIONOFFSET_HPP
#define WSF_SIXDOF_SCRIPTFORMATIONOFFSET_HPP

#include "wsf_six_dof_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT FormationOffset : public UtScriptClass
{
public:
   FormationOffset(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~FormationOffset() override = default;

   void* Create(const UtScriptContext& aContext) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);

   UT_DECLARE_SCRIPT_METHOD(GetStack);
   UT_DECLARE_SCRIPT_METHOD(GetRight);
   UT_DECLARE_SCRIPT_METHOD(GetAhead);
   UT_DECLARE_SCRIPT_METHOD(GetRelativeBearing);
   UT_DECLARE_SCRIPT_METHOD(GetRange);
   UT_DECLARE_SCRIPT_METHOD(IsWelded);

   UT_DECLARE_SCRIPT_METHOD(SetStack);
   UT_DECLARE_SCRIPT_METHOD(SetRight);
   UT_DECLARE_SCRIPT_METHOD(SetAhead);
   UT_DECLARE_SCRIPT_METHOD(SetRangeAndBearing);
   UT_DECLARE_SCRIPT_METHOD(SetWelded);

   UT_DECLARE_SCRIPT_METHOD(Add);
   UT_DECLARE_SCRIPT_METHOD(Subtract);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTFORMATIONOFFSET_HPP
