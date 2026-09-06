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

#ifndef DISSETDATAR_HPP
#define DISSETDATAR_HPP

#include "dis_export.h"

#include "DisDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

// DisSetDataR - Ref: IEEE 1278.1-2012 (DIS)
class DIS_EXPORT DisSetDataR : public DisPdu
{
public:
   DisSetDataR();
   DisSetDataR(const DisSetDataR& aSrc);
   DisSetDataR(DisPdu& aPdu, GenI& aGenI);
   DisSetDataR(GenI& aGenI);
   ~DisSetDataR() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override { return mReceivingEntity; }
   DisEnum8           GetReliabilityService() const override { return mReliabilityService; }
   DisUint32          GetRequestId() const override { return mRequestId; }

   const DisDatumSpec& GetData() const { return mData; }
   DisDatumSpec&       GetData() { return mData; }

   // Mutators

   void SetOriginatingEntity(const DisEntityId& aEntityId);
   void SetReceivingEntity(const DisEntityId& aEntityId);
   void SetReliabilityService(DisEnum8 aReliabilityService);
   void SetRequestId(DisUint32 aRequestId);
   void SetNumFixedRecords(DisEnum32 aNumRecords);
   void SetNumVariableRecords(DisUint32 aNumRecords);
   void SetData(const DisDatumSpec& aDatumSpec);

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;

   //! Data Validation
   bool IsValid() const override;

   // Operators

   DisSetDataR& operator=(const DisSetDataR& aRhs);

   DisUint16 GetLength() override;

private:
   void Copy(const DisSetDataR& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisEnum8     mReliabilityService;
   DisUint32    mRequestId;
   DisDatumSpec mData;
};

// const DisEntityId& DisSetDataR::GetOriginatingEntity() const
//{
//    return mOriginatingEntity ;
// }
inline void DisSetDataR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}
inline void DisSetDataR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}
inline void DisSetDataR::SetReliabilityService(DisEnum8 aReqRelService)
{
   mReliabilityService = aReqRelService;
}
inline void DisSetDataR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}
inline void DisSetDataR::SetData(const DisDatumSpec& aDatumSpec)
{
   mData = aDatumSpec;
}
#endif
