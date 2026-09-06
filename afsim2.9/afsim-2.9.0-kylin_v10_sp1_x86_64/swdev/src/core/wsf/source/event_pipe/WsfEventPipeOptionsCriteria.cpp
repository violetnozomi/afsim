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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfEventPipeOptionsCriteria.hpp"

#include "WsfPlatform.hpp"

bool WsfEventPipeOptionsCriteria::Matches(const WsfPlatform& aPlatform)
{
   switch (mKind)
   {
   case cPLATFORM_NAME:
      return aPlatform.GetName() == mText;
   case cPLATFORM_TYPE:
      return aPlatform.IsA_TypeOf(mText);
   case cCATEGORY_NAME:
      return aPlatform.IsCategoryMember(mText);
   case cSIDE_NAME:
      return aPlatform.GetSide() == mText;
   }
   return false;
}
