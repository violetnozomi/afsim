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

#ifndef DISSTOPFREEZER_HPP
#define DISSTOPFREEZER_HPP

#include "dis_export.h"

#include "DisClockTime.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisStopFreezeR : public DisPdu
{
public:
   DisStopFreezeR();
   DisStopFreezeR(DisPdu& aPdu, GenI& aGenI);
   DisStopFreezeR(GenI& aGenI);
   ~DisStopFreezeR() override;

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

   const DisClockTime& GetRealWorldTime() const;
   DisEnum8            GetReason() const;
   DisEnum8            GetFrozenBehavior() const;

   void SetRealWorldTime(const DisClockTime& aRealWorldTime);
   void SetReason(DisEnum8 aReason);
   void SetFrozenBehavior(DisEnum8 aFrozenBehavior);

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;
   bool IsValid() const override;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisClockTime mRealWorldTime;
   DisUint32    mRequestId;
   DisEnum8     mReason;
   DisEnum8     mFrozenBehavior;
   DisEnum8     mReliabilityService;
};

inline const DisClockTime& DisStopFreezeR::GetRealWorldTime() const
{
   return mRealWorldTime;
}

inline DisEnum8 DisStopFreezeR::GetReason() const
{
   return mReason;
}

inline DisEnum8 DisStopFreezeR::GetFrozenBehavior() const
{
   return mFrozenBehavior;
}

// Mutators

inline void DisStopFreezeR::SetRealWorldTime(const DisClockTime& aRealWorldTime)
{
   mRealWorldTime = aRealWorldTime;
}
inline void DisStopFreezeR::SetReason(DisEnum8 aReason)
{
   mReason = aReason;
}
inline void DisStopFreezeR::SetFrozenBehavior(DisEnum8 aFrozenBehavior)
{
   mFrozenBehavior = aFrozenBehavior;
}

#endif
