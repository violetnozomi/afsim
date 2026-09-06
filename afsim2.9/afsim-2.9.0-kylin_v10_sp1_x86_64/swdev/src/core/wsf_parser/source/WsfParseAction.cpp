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

#include "WsfParseAction.hpp"

#include "WsfParseRule.hpp"

WsfParseActionPart::~WsfParseActionPart()
{
   // delete mRHS_Type;
}

WsfParseActionAssign::WsfParseActionAssign()
   : WsfParseActionPart(cASSIGN)
   , mLHS_Type(nullptr)
   , mRHS(nullptr)
{
}

WsfParseActionAssign::~WsfParseActionAssign()
{
   delete mRHS;
}

WsfParseActionCopy::~WsfParseActionCopy() {}

WsfParseActionListOp::~WsfParseActionListOp()
{
   delete mPushedValuePtr;
}

WsfParseActionNegate::~WsfParseActionNegate()
{
   delete mSubPart;
}

WsfParseAction::~WsfParseAction()
{
   for (size_t i = 0; i < mSubActions.size(); ++i)
   {
      delete mSubActions[i];
   }
}

WsfParseActionObjectMapOp::WsfParseActionObjectMapOp()
   : WsfParseActionPart(cOBJECT_MAP_OP)
   , mOperation(cGET)
   , mAttributeName()
   , mApplyOrd(-1)
   , mAttributeAddr()
   , mObjectMapTypePtr(nullptr)
   , mObjectName(nullptr)
{
}

WsfParseActionObjectMapOp::~WsfParseActionObjectMapOp()
{
   delete mObjectName;
}

WsfParseActionCall::~WsfParseActionCall()
{
   for (size_t i = 0; i < mArgs.size(); ++i)
   {
      delete mArgs[i];
   }
}
