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

#include "GenBufIManaged.hpp"

GenBufIManaged::GenBufIManaged(BufType aBufType, unsigned long aSize, bool aAllowGrowth)
   : GenBuf(aSize, aAllowGrowth)
   , GenBufXferI(aBufType)
{
}


GenBufIManaged::~GenBufIManaged() {}
