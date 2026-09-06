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

#ifndef DISACTIONRESPONSE_HPP
#define DISACTIONRESPONSE_HPP

#include "dis_export.h"

#include "DisDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisActionResponse : public DisPdu
{
public:
   DisActionResponse();
   DisActionResponse(DisPdu& aPdu, GenI& aGenI);
   DisActionResponse(GenI& aGenI);
   ~DisActionResponse() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetReceivingEntity() const override;

   DisUint32 GetRequestId() const override;

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);

   virtual void SetRequestId(DisUint32 aRequestId);

   DisUint32 GetRequestStatus() const;

   void SetRequestStatus(DisEnum32 aRequestStatus);
   void SetDatumSpec(const DisDatumSpec& aDatumSpec);

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;

   DisDatumSpec& GetData() { return mData; }

   DisUint16 GetLength() override;

   void Stream(std::ostream& aStream) const override;

   // Data Validation
   bool IsValid() const override;

private:
   // Disallow copy and assignment
   DisActionResponse(const DisActionResponse& aSrc);
   DisActionResponse& operator=(const DisActionResponse& aRhs);

   void DeleteClassData();
   void GetMemberData(GenI& aGenI);
   void SetTheData(unsigned char* aDataPtr, unsigned int aDataLength);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisUint32    mRequestId;
   DisEnum32    mRequestStatus;
   DisDatumSpec mData;
};

// Accessors

inline DisEnum32 DisActionResponse::GetRequestStatus() const
{
   return mRequestStatus;
}

// Mutators

inline void DisActionResponse::SetRequestStatus(const DisEnum32 aRequestStatus)
{
   mRequestStatus = aRequestStatus;
}

inline void DisActionResponse::SetDatumSpec(const DisDatumSpec& aDatumSpec)
{
   mData = aDatumSpec;
}
#endif
