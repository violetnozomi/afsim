// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTSCRIPTMATRIX_HPP
#define UTSCRIPTMATRIX_HPP

#include "util_script_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace ut
{
namespace script
{

class UTIL_SCRIPT_EXPORT Matrix : public UtScriptClass
{
public:
   Matrix(UtScriptTypes* aScriptTypesPtr);

   void        Destroy(void* aObjectPtr) override;
   void*       Clone(void* aObjectPtr) override;
   bool        EqualTo(void* aLHS_Ptr, void* aRHS_Ptr) override;
   std::string ToString(void* aObjectPtr) const override;

   UT_DECLARE_SCRIPT_METHOD(ConstructDefault);
   UT_DECLARE_SCRIPT_METHOD(ConstructValue);
   UT_DECLARE_SCRIPT_METHOD(Identity);

   UT_DECLARE_SCRIPT_METHOD(Rows);    // size
   UT_DECLARE_SCRIPT_METHOD(Columns); // size

   UT_DECLARE_SCRIPT_METHOD(Get);
   UT_DECLARE_SCRIPT_METHOD(Row);
   UT_DECLARE_SCRIPT_METHOD(Column);
   UT_DECLARE_SCRIPT_METHOD(Diagonal);
   UT_DECLARE_SCRIPT_METHOD(Submatrix); // get a submatrix
   UT_DECLARE_SCRIPT_METHOD(Trace);

   UT_DECLARE_SCRIPT_METHOD(IsRow);
   UT_DECLARE_SCRIPT_METHOD(IsColumn);
   UT_DECLARE_SCRIPT_METHOD(IsPositiveDefinite);

   UT_DECLARE_SCRIPT_METHOD(Set);
   UT_DECLARE_SCRIPT_METHOD(SetRow);
   UT_DECLARE_SCRIPT_METHOD(SetColumn);
   UT_DECLARE_SCRIPT_METHOD(SetDiagonal);
   UT_DECLARE_SCRIPT_METHOD(SetSubmatrix); // set a sub part

   UT_DECLARE_SCRIPT_METHOD(Multiply);
   UT_DECLARE_SCRIPT_METHOD(Inverse);
   UT_DECLARE_SCRIPT_METHOD(PseudoInverse);
   UT_DECLARE_SCRIPT_METHOD(Transpose);
   UT_DECLARE_SCRIPT_METHOD(Add);
   UT_DECLARE_SCRIPT_METHOD(Subtract);
   UT_DECLARE_SCRIPT_METHOD(Scale);

   UT_DECLARE_SCRIPT_METHOD(CholeskyDecomposition);
   UT_DECLARE_SCRIPT_METHOD(SingularValueDecomposition);
   UT_DECLARE_SCRIPT_METHOD(Eigensystem);
};

} // namespace script
} // namespace ut

#endif // UTSCRIPTMATRIX_HPP
