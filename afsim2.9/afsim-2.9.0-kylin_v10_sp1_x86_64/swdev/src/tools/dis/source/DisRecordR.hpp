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

#ifndef DISRECORDR_HPP
#define DISRECORDR_HPP

#include "dis_export.h"

#include <vector>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisSpecRecord.hpp"

class DIS_EXPORT DisRecordR : public DisPdu
{
public:
   DisRecordR();
   DisRecordR(const DisRecordR& aSrc);
   DisRecordR(DisPdu& aPdu, GenI& aGenI);
   DisRecordR(GenI& aGenI);
   ~DisRecordR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   const DisEntityId& GetOriginatingEntity() const override;
   DisUint32          GetRequestId() const override;

   DisUint32 GetNumRecordSets() const { return mNumSpecRecords; }
   DisUint32 GetResponseSerial() const { return mResponseSerial; }
   DisEnum16 GetEventType() const { return mEventType; }

   const DisSpecRecord& GetRecordSet(unsigned int aIndex) const;
   DisSpecRecord&       GetRecordSet(unsigned int aIndex);

   // Mutators

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);
   virtual void SetReliabilityService(DisEnum8 aReliabilityService);
   virtual void SetRequestId(DisUint32 aRequestId);

   void SetResponseSerial(DisUint32 aResponseSerial) { mResponseSerial = aResponseSerial; }
   void SetEventType(DisEnum16 aEventType) { mEventType = aEventType; }

   void AddSpecRecord(const DisSpecRecord& aRecord);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;
   void      Stream(std::ostream& aStream) const override;

   // Operators

   DisRecordR& operator=(const DisRecordR& aRhs);

private:
   void Copy(const DisRecordR& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId                mOriginatingEntity;
   DisEntityId                mReceivingEntity;
   DisUint32                  mRequestId;
   DisEnum8                   mReliabilityService;
   DisEnum16                  mEventType;
   DisUint32                  mResponseSerial;
   DisUint32                  mNumSpecRecords;
   std::vector<DisSpecRecord> mSpecRecordSet;
};

#endif
