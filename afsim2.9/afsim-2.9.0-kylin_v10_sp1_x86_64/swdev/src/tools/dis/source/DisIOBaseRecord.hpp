//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef DISIOBASERECORD_HPP
#define DISIOBASERECORD_HPP

#include "dis_export.h"

#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
//#include "DisEnvironmentalSpaceDataRecord.hpp"
//#include "DisGeometryRecord.hpp"
//#include "DisEnvironmentalStateRecord.hpp"
#include "DisEnvironmentalAndGeometricRecordTypeEnum.hpp"
#include "DisIORecordTypeEnum.hpp"
#include "DisRecordHeader.hpp"

//! Base class for all IO Records (see section 6.2.48 of the IEEE 1278.1-2012(DIS) pdf)
class DIS_EXPORT DisIOBaseRecord : public DisRecordHeader
{
public:
   DisIOBaseRecord() = default;
   explicit DisIOBaseRecord(GenI& aGenI);
   explicit DisIOBaseRecord(DisIORecordTypeEnum aRecordType);
   virtual ~DisIOBaseRecord() = default;

   // Input/output
   void        Get(GenI& aGenI) override;
   DisUint16   GetLength();
   void        Put(GenO& aGenO) const override;
   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;

   // Data Validation
   bool                        IsValid() const override;
   virtual DisIORecordTypeEnum GetIORecordType() const;
   DisIOBaseRecord*            Clone() const override;

   //----------------------- Script accessibility methods -----------------------
   const char* GetScriptClassName() const override;
   //--------------------- End script accessibility methods ---------------------

protected:
   DisIORecordTypeEnum GetIORecordType(DisEnum32 aRecordValue);

private:
   virtual void GetMemberData(GenI& aGenI){};

protected:
   // This has to be a variable to save the value state if the object is created in script.
   DisIORecordTypeEnum    mIORecordType                 = DisIORecordTypeEnum::IO_NONE;
   static const DisUint16 cRECORD_BASE_LENGTH_IN_OCTETS = 0;
};

#endif
