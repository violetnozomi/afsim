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

#ifndef DISSTARTRESUME_HPP
#define DISSTARTRESUME_HPP

#include "dis_export.h"

#include "DisClockTime.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisStartResume : public DisPdu
{
public:
   DisStartResume();
   DisStartResume(DisPdu& aPdu, GenI& aGenI);
   DisStartResume(GenI& aGenI);
   ~DisStartResume() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;

   const DisClockTime& GetRealWorldTime() const;
   const DisClockTime& GetSimulationTime() const;
   DisUint32           GetRequestId() const override;

   // Mutators

   void SetOriginatingEntity(const DisEntityId& aOriginatingEntity);
   void SetReceivingEntity(const DisEntityId& aReceivingEntity);
   void SetRealWorldTime(const DisClockTime& aRealWorldTime);
   void SetSimulationTime(const DisClockTime& aSimulationTime);
   void SetRequestId(DisUint32 aRequestId);

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;
   bool IsValid() const override;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisClockTime mRealWorldTime;
   DisClockTime mSimulationTime;
   DisUint32    mRequestId;
};

// Accessors

inline const DisEntityId& DisStartResume::GetReceivingEntity() const
{
   return mReceivingEntity;
}

inline const DisClockTime& DisStartResume::GetRealWorldTime() const
{
   return mRealWorldTime;
}

inline const DisClockTime& DisStartResume::GetSimulationTime() const
{
   return mSimulationTime;
}

inline DisUint32 DisStartResume::GetRequestId() const
{
   return mRequestId;
}

// Mutators

// const DisEntityId& DisStartResume::GetOriginatingEntity() const
//{
//    return mOriginatingEntity ;
// }
inline void DisStartResume::SetOriginatingEntity(const DisEntityId& aOriginatingEntity)
{
   mOriginatingEntity = aOriginatingEntity;
}

inline void DisStartResume::SetReceivingEntity(const DisEntityId& aReceivingEntity)
{
   mReceivingEntity = aReceivingEntity;
}

inline void DisStartResume::SetRealWorldTime(const DisClockTime& aRealWorldTime)
{
   mRealWorldTime = aRealWorldTime;
}

inline void DisStartResume::SetSimulationTime(const DisClockTime& aSimulationTime)
{
   mSimulationTime = aSimulationTime;
}

inline void DisStartResume::SetRequestId(DisUint32 aRequest)
{
   mRequestId = aRequest;
}

#endif
