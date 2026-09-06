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

#ifndef WSFSCRIPTPOINTMASSSIXDOFMOVER_HPP
#define WSFSCRIPTPOINTMASSSIXDOFMOVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfScriptSixDOF_Mover.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
//! WsfScriptPointMassSixDOF_Mover is a WsfScriptObjectClass that defines the methods in
//! WsfPointMassSixDOF_Mover.  This makes the methods available for use in script.
class WSF_SIX_DOF_EXPORT PointMassMover : public Mover
{
public:
   PointMassMover(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   // -----------------------------------------------------------------------

   // TESTING SUPPORT

   // The following functions are for Testing and should not be documented,
   //  to hide them from normal users
   UT_DECLARE_SCRIPT_METHOD(GetPointMassTestSupportObject);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif
