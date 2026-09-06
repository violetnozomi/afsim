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

// DIS DD Record

#ifndef DIS_DISDDRECORD_HPP
#define DIS_DISDDRECORD_HPP

#include "dis_export.h"

#include "DisRecordHeader.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDDRecord : public DisRecordHeader
{
public:
   enum
   {
      DAMAGE_DESCRIPTION_DD_RECORD_TYPE = 4500
   };

   DisDDRecord(DisEnum32 aRecordType);
   DisDDRecord(GenI& aGenI);
   ~DisDDRecord() override;
};

#endif
