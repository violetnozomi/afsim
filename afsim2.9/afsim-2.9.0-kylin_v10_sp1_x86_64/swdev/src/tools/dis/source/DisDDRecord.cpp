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

#include "DisDDRecord.hpp"

#include "DisRecordHeader.hpp"
#include "GenI.hpp"
#include "GenO.hpp"


DisDDRecord::DisDDRecord(DisEnum32 aRecordType)
   : DisRecordHeader(aRecordType)
{
}

DisDDRecord::DisDDRecord(GenI& aGenI)
   : DisRecordHeader(aGenI)
{
}

DisDDRecord::~DisDDRecord() {}
