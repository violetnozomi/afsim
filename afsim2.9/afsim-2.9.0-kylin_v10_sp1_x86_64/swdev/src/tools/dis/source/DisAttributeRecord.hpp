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

// DIS Attribute Record

#ifndef DIS_DIS_ATTRIBUTE_RECORD_HPP
#define DIS_DIS_ATTRIBUTE_RECORD_HPP

#include "dis_export.h"

#include "DisRecordHeader.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisAttributeRecord : public DisRecordHeader
{
public:
   enum AttributeRecordType
   {
      BLANKING_SECTOR_ATTRIBUTE_RECORD_TYPE        = 3500,
      ANGLE_DECEPTION_ATTRIBUTE_RECORD_TYPE        = 3501,
      FALSE_TARGETS_ATTRIBUTE_RECORD_TYPE          = 3502,
      FALSE_TARGETS_ENHANCED_ATTRIBUTE_RECORD_TYPE = 3598
   };

   DisAttributeRecord(DisEnum32 aRecordType);
   DisAttributeRecord(GenI& aGenI);
   ~DisAttributeRecord() override;
};

#endif
