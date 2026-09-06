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

#include "UtScriptQuadTree.hpp"

#include "UtMemory.hpp"
#include "UtQuadTreeGeneric.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtScriptVector.hpp"

UtScriptQuadTree::UtScriptQuadTree(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(UtQuadTreeGeneric);

   mConstructible = true;
   mCloneable     = true;

   UT_SCRIPT_METHOD(int, Add, (double lat, double lon, int id))
   {
      self.Add(lat, lon, id);
      return id;
   };

   UT_SCRIPT_WRAP_MEMBER("Remove", Remove);

   (UT_SCRIPT_METHOD(std::unique_ptr<std::vector<UtScriptData>>, InRange, (double lat, double lon, double range)) {
      auto vec = ut::make_unique<std::vector<UtScriptData>>();
      self.InRange(lat, lon, range, *vec);
      return vec;
   }).SetReturnTypeId(UtStringIdLiteral("Array<int>"));
}

void UtScriptQuadTree::Destroy(void* aObjectPtr)
{
   delete static_cast<UtQuadTreeGeneric*>(aObjectPtr);
}

void* UtScriptQuadTree::Clone(void* aObjectPtr)
{
   return new UtQuadTreeGeneric(*static_cast<UtQuadTreeGeneric*>(aObjectPtr));
}

void* UtScriptQuadTree::Create(const UtScriptContext&)
{
   return static_cast<void*>(new UtQuadTreeGeneric());
}
