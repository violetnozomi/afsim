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

#ifndef WSFSCRIPTPOINTMASSSIXDOFMOVERTESTOBJECT_HPP
#define WSFSCRIPTPOINTMASSSIXDOFMOVERTESTOBJECT_HPP

#include "wsf_six_dof_export.h"

#include "WsfScriptSixDOF_MoverTestObject.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT PointMassMoverTestObject : public MoverTestObject
{
public:
   PointMassMoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   // The PointMassMoverTestObject does not yet add functionality over the base class.
   // These methods will be test functions, typically intentionally omitted from user documentation.
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif
