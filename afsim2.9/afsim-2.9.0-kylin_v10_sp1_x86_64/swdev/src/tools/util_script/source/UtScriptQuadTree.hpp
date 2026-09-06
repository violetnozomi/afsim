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

#ifndef UTSCRIPTQUADTREE_HPP
#define UTSCRIPTQUADTREE_HPP

#include "util_script_export.h"

#include "UtScriptClass.hpp"

//--------------------------------------------------------------
//! UtScriptQuadTree provides a script interface to a QuadTree.
//--------------------------------------------------------------

class UTIL_SCRIPT_EXPORT UtScriptQuadTree : public UtScriptClass
{
public:
   UtScriptQuadTree(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext&) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;
};

#endif
