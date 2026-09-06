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

#ifndef DISSTARTRESUMER_HPP
#define DISSTARTRESUMER_HPP

#include "dis_export.h"

#include <ostream>

#include "DisClockTime.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisStartResumeR : public DisPdu
{
public:
   DisStartResumeR();
   DisStartResumeR(DisPdu& aPdu, GenI& aGenI);
   DisStartResumeR(GenI& aGenI);
   ~DisStartResumeR() override;

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
   const DisClockTime& GetSimulationTime() const;

   void SetRealWorldTime(const DisClockTime& aRealWorldTime);
   void SetSimulationTime(const DisClockTime& aSimulationTime);

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;
   bool IsValid() const override;

   DisUint16 GetLength() override;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisClockTime mRealWorldTime;
   DisClockTime mSimulationTime;
   DisEnum8     mReliabilityService;
   DisUint32    mRequestId;
};

// Accessors

inline const DisEntityId& DisStartResumeR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

inline const DisClockTime& DisStartResumeR::GetRealWorldTime() const
{
   return mRealWorldTime;
}

inline const DisClockTime& DisStartResumeR::GetSimulationTime() const
{
   return mSimulationTime;
}
inline DisEnum8 DisStartResumeR::GetReliabilityService() const
{
   return mReliabilityService;
}

inline DisUint32 DisStartResumeR::GetRequestId() const
{
   return mRequestId;
}

// Mutators

// const DisEntityId& DisStartResumeR::GetOriginatingEntity() const
//{
//    return mOriginatingEntity ;
// }
inline void DisStartResumeR::SetOriginatingEntity(const DisEntityId& aOriginatingEntity)
{
   mOriginatingEntity = aOriginatingEntity;
}
inline void DisStartResumeR::SetReceivingEntity(const DisEntityId& aReceivingEntity)
{
   mReceivingEntity = aReceivingEntity;
}

inline void DisStartResumeR::SetRealWorldTime(const DisClockTime& aRealWorldTime)
{
   mRealWorldTime = aRealWorldTime;
}

inline void DisStartResumeR::SetSimulationTime(const DisClockTime& aSimulationTime)
{
   mSimulationTime = aSimulationTime;
}

inline void DisStartResumeR::SetReliabilityService(const DisEnum8 aRequiredReliabilityService)
{
   mReliabilityService = aRequiredReliabilityService;
}

inline void DisStartResumeR::SetRequestId(DisUint32 aRequest)
{
   mRequestId = aRequest;
}

#endif
