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

#ifndef DISACTIONREQUESTR_HPP
#define DISACTIONREQUESTR_HPP

#include "dis_export.h"

#include "DisDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisActionRequestR : public DisPdu
{
public:
   DisActionRequestR();
   DisActionRequestR(DisPdu& aPdu, GenI& aGenI);
   DisActionRequestR(GenI& aGenI);
   ~DisActionRequestR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetReceivingEntity() const override;

   DisEnum8  GetReliabilityService() const override;
   DisUint32 GetRequestId() const override;

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);

   virtual void SetReliabilityService(DisEnum8 aReliabilityService);
   virtual void SetRequestId(DisUint32 aRequestId);

   DisUint32 GetActionId() const;

   void SetActionId(DisEnum32 aActionId);
   void SetDatumSpec(const DisDatumSpec& aDatumSpec);

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;

   const DisDatumSpec& GetData() const { return mData; }
   DisDatumSpec&       GetData() { return mData; }

   DisUint16 GetLength() override;

   void Stream(std::ostream& aStream) const override;
   // Data Validation
   bool IsValid() const override;

private:
   // Disallow copy and assignment
   DisActionRequestR(const DisActionRequestR& aSrc);
   DisActionRequestR& operator=(const DisActionRequestR& aRhs);

   // void Copy(const DisActionRequestR& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisEnum8     mReliabilityService;
   DisUint32    mRequestId;
   DisUint32    mActionId;
   DisDatumSpec mData;
};

// Accessors

inline DisUint32 DisActionRequestR::GetActionId() const
{
   return mActionId;
}

// Mutators

inline void DisActionRequestR::SetActionId(DisEnum32 aActionId)
{
   mActionId = aActionId;
}

inline void DisActionRequestR::SetDatumSpec(const DisDatumSpec& aDatumSpec)
{
   mData = aDatumSpec;
}
#endif
