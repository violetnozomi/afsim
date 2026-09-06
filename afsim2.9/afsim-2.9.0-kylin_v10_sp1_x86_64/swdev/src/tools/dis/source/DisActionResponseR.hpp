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

#ifndef DISACTIONRESPONSER_HPP
#define DISACTIONRESPONSER_HPP

#include "dis_export.h"

#include "DisDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisActionResponseR : public DisPdu
{
public:
   DisActionResponseR();
   DisActionResponseR(DisPdu& aPdu, GenI& aGenI);
   DisActionResponseR(GenI& aGenI);
   ~DisActionResponseR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetReceivingEntity() const override;

   DisUint32 GetRequestId() const override;
   DisUint32 GetRequestStatus() const;

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);
   virtual void SetRequestId(DisUint32 aRequestId);
   void         SetResponseStatus(DisEnum32 aResponseStatus);

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void SetDatumSpec(const DisDatumSpec& aDatumSpec);

   DisDatumSpec&       GetData() { return mData; }
   const DisDatumSpec& GetData() const { return mData; }

   DisUint16 GetLength() override;

   void Stream(std::ostream& aStream) const override;

   // Data Validation
   bool IsValid() const override;

private:
   // Disallow copy and assignment
   DisActionResponseR(const DisActionResponseR& aSrc);
   DisActionResponseR& operator=(const DisActionResponseR& aRhs);

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

inline DisEnum32 DisActionResponseR::GetRequestStatus() const
{
   return mRequestStatus;
}

// Mutators

inline void DisActionResponseR::SetResponseStatus(const DisEnum32 aResponseStatus)
{
   mRequestStatus = aResponseStatus;
}

inline void DisActionResponseR::SetDatumSpec(const DisDatumSpec& aDatumSpec)
{
   mData = aDatumSpec;
}

#endif
