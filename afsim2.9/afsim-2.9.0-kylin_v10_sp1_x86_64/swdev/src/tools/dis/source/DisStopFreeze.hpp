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

#ifndef DISSTOPFREEZE_HPP
#define DISSTOPFREEZE_HPP

#include "dis_export.h"

#include <ostream>

#include "DisClockTime.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisStopFreeze : public DisPdu
{
public:
   DisStopFreeze();
   DisStopFreeze(DisPdu& aPdu, GenI& aGenI);
   DisStopFreeze(GenI& aGenI);
   DisStopFreeze(const DisStopFreeze& aSrc);

   ~DisStopFreeze() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId&  GetReceivingEntity() const override;
   const DisClockTime& GetRealWorldTime() const;
   DisEnum8            GetReason() const;
   DisEnum8            GetFrozenBehavior() const;
   DisUint32           GetRequestId() const override;

   // Mutators

   void SetOriginatingEntity(const DisEntityId& aOriginatingEntity);
   void SetReceivingEntity(const DisEntityId& aReceivingEntity);
   void SetRealWorldTime(const DisClockTime& aRealWorldTime);
   void SetReason(DisEnum8 aReason);
   void SetFrozenBehavior(DisEnum8 aFrozenBehavior);
   void SetRequestId(DisUint32 aRequestId);

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;
   bool IsValid() const override;

   // Deprecated.  See file DisControlEnums.hpp for replacement enum DisEnum::Control::Reason.
   enum
   {
      ReasonOther                = 0,
      ReasonRecess               = 1,
      ReasonTermination          = 2,
      ReasonSystemFailure        = 3,
      ReasonSecurityViolation    = 4,
      ReasonEntityReconstitution = 5,
      ReasonReset                = 6,
      ReasonRestart              = 7
   };

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisClockTime mRealWorldTime;
   DisEnum8     mReason;
   DisEnum8     mFrozenBehavior;
   DisUint32    mRequestId;
};

// Accessors
inline const DisClockTime& DisStopFreeze::GetRealWorldTime() const
{
   return mRealWorldTime;
}
inline DisEnum8 DisStopFreeze::GetReason() const
{
   return mReason;
}
inline DisEnum8 DisStopFreeze::GetFrozenBehavior() const
{
   return mFrozenBehavior;
}

// Mutators

inline void DisStopFreeze::SetRealWorldTime(const DisClockTime& aRealWorldTime)
{
   mRealWorldTime = aRealWorldTime;
}
inline void DisStopFreeze::SetReason(DisEnum8 aReason)
{
   mReason = aReason;
}
inline void DisStopFreeze::SetFrozenBehavior(DisEnum8 aFrozenBehavior)
{
   mFrozenBehavior = aFrozenBehavior;
}

#endif
