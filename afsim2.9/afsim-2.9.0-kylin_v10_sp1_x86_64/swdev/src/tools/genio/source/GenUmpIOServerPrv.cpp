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

#include "GenUmpIOServerPrv.hpp"

// HandleList of base type
vector<GenUmpIOServerPrv*> GenUmpIOServerPrv::sHandleList;


GenUmpIOServerPrv::GenUmpIOServerPrv(const char* aCallingClassName, float aUpdatePeriodSec)
   : mCallingClassName(aCallingClassName != 0 ? aCallingClassName : "")
   , mHandleIndex(0)
   , mUpdatePeriodSec(aUpdatePeriodSec)
   , mSocketRecv(-99999)
{
}

// Virtual
GenUmpIOServerPrv::~GenUmpIOServerPrv() {}
