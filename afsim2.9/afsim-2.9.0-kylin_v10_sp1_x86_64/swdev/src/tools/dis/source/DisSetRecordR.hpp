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

#ifndef DISSETRECORDR_HPP
#define DISSETRECORDR_HPP

#include "dis_export.h"

#include <ostream>
#include <vector>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisSpecRecord.hpp"

class DIS_EXPORT DisSetRecordR : public DisPdu
{
public:
   DisSetRecordR();
   DisSetRecordR(const DisSetRecordR& aSrc);
   DisSetRecordR(DisPdu& aPdu, GenI& aGenI);
   DisSetRecordR(GenI& aGenI);
   ~DisSetRecordR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   const DisEntityId& GetOriginatingEntity() const override;
   DisEnum8           GetReliabilityService() const override;
   DisUint32          GetRequestId() const override;

   const DisSpecRecord&              GetSpecRecord(unsigned int aIndex) const;
   const std::vector<DisSpecRecord>& GetSpecRecords() const;

   DisUint32 GetNumRecords() const;

   // Mutators

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);

   virtual void SetRequestId(DisUint32 aRequestId);
   virtual void SetReliabilityService(DisEnum8 aReliabilityService);

   void AddSpecRecord(const DisSpecRecord& aSpecRecord);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Operators

   DisSetRecordR& operator=(const DisSetRecordR& aRhs);

   void Stream(std::ostream& aStream) const override;

private:
   void Copy(const DisSetRecordR& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisUint32   mRequestId;
   DisEnum8    mReliabilityService;

   DisUint32                  mNumSpecRecords;
   std::vector<DisSpecRecord> mRecords;
};

// Accessors

inline DisUint32 DisSetRecordR::GetNumRecords() const
{
   return mNumSpecRecords;
}

#endif
