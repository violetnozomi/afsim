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

// DIS DE Record

#ifndef DIS_DISDERECORD_HPP
#define DIS_DISDERECORD_HPP

#include "dis_export.h"

#include "DisRecordHeader.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDERecord : public DisRecordHeader
{
public:
   enum
   {
      AREA_AIMPOINT_DE_RECORD_TYPE      = 4001,
      PRECISION_AIMPOINT_DE_RECORD_TYPE = 4000
   };

   DisDERecord(DisEnum32 aRecordType);
   DisDERecord(GenI& aGenI);
   ~DisDERecord() override;
};

#endif
