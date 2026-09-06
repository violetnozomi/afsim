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

#include "DisAttributeRecord.hpp"


DisAttributeRecord::DisAttributeRecord(DisEnum32 aRecordType)
   : DisRecordHeader(aRecordType)
{
}

DisAttributeRecord::DisAttributeRecord(GenI& aGenI)
   : DisRecordHeader(aGenI)
{
}

// virtual
DisAttributeRecord::~DisAttributeRecord() {}
