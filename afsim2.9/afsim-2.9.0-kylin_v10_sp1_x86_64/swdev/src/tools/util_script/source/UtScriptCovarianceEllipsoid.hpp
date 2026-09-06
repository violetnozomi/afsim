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

#ifndef UTSCRIPTCOVARIANCEELLIPSOID_HPP
#define UTSCRIPTCOVARIANCEELLIPSOID_HPP

#include "util_script_export.h"

#include "UtScriptClass.hpp"
class UtCovarianceEllipsoid;

class UTIL_SCRIPT_EXPORT UtScriptEllipsoid : public UtScriptClass
{
public:
   UtScriptEllipsoid(UtScriptTypes* aScriptTypesPtr);

   //! Creates a new script reference of a new UtVec object
   static UtScriptEllipsoid* sClassPtr;

   static UtScriptRef* CreateRef(const UtCovarianceEllipsoid& aVec);

   void  Destroy(void* aEllipsoidPtr) override;
   void* Clone(void* aEllipsoidPtr) override;
};

#endif
