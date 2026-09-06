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

#include "WsfScriptPointMassSixDOF_MoverTestObject.hpp"

#include "UtMemory.hpp"
#include "WsfPointMassSixDOF_MoverTestObject.hpp"
#include "WsfPointMassSixDOF_Utils.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
PointMassMoverTestObject::PointMassMoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : MoverTestObject(aClassName, aTypesPtr)
{
   SetClassName("WsfPointMassSixDOF_MoverTestObject");

   // Add each of the method objects to the class.

   // The PointMassMoverTestObject does not yet add functionality over the base class.
   // These methods will be test functions, typically intentionally omitted from user documentation.
}
} // namespace script
} // namespace six_dof
} // namespace wsf
