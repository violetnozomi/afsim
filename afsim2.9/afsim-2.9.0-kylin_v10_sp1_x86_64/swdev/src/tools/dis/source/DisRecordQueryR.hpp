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

#ifndef DISRECORDQUERYR_HPP
#define DISRECORDQUERYR_HPP

#include "dis_export.h"

#include <vector>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisRecordQueryR : public DisPdu
{
public:
   DisRecordQueryR();
   DisRecordQueryR(const DisRecordQueryR& aSrc);
   DisRecordQueryR(DisPdu& aPdu, GenI& aGenI);
   DisRecordQueryR(GenI& aGenI);
   ~DisRecordQueryR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   DisUint32 GetTime() const { return mTime; }
   DisUint16 GetEventType() const { return mEventType; }
   DisUint32 GetNumRecords() const;
   DisUint32 GetRecord(unsigned int aIndex) const;
   DisEnum8  GetReliabilityService() const override;
   DisUint32 GetRequestId() const override;

   // Mutators

   virtual void SetRequestId(DisUint32 aRequestId);
   virtual void SetReliabilityService(DisEnum8 aReliabilityService);
   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Operators

   DisRecordQueryR& operator=(const DisRecordQueryR& aRhs);

   void Stream(std::ostream& aStream) const override;

private:
   void Copy(const DisRecordQueryR& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;

   DisUint32 mRequestId;
   DisEnum8  mReliabilityService;
   DisEnum16 mEventType;
   DisUint32 mTime;
   DisUint32 mNumRecords;

   std::vector<DisUint32> mRecords;
};

// Accessors

inline DisUint32 DisRecordQueryR::GetNumRecords() const
{
   return mNumRecords;
}

#endif
