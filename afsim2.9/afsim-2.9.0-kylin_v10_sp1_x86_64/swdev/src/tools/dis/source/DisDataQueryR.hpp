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

#ifndef DISDATAQUERYR_HPP
#define DISDATAQUERYR_HPP

#include "dis_export.h"

#include <vector>

#include "DisDataQueryDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisTypes.hpp"

// DisDataQueryR - Ref: IEEE 1278.1-2012 (DIS)
class DIS_EXPORT DisDataQueryR : public DisPdu
{
public:
   DisDataQueryR();
   DisDataQueryR(DisPdu& aPdu, GenI& aGenI);
   DisDataQueryR(GenI& aGenI);
   ~DisDataQueryR() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors
   const DisEntityId&           GetReceivingEntityId() const;
   DisEnum8                     GetRequiredReliabilityService() const;
   DisUint32                    GetRequestId() const override;
   DisUint32                    GetTimeInterval() const;
   const DisDataQueryDatumSpec& GetDatumSpec() const;
   DisDataQueryDatumSpec&       GetDatumSpec();

   // Mutators
   void SetOriginatingEntityId(const DisEntityId& aEntityId);
   void SetReceivingEntityId(const DisEntityId& aEntityId);
   void SetRequiredReliabilityService(DisEnum8 aRequiredReliabilityService);
   void SetRequestId(DisEnum32 aRequestId);
   void SetTimeInterval(DisUint32 aTimeInterval);
   void SetDatumSpec(const DisDataQueryDatumSpec& aDatumSpec);


   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId           mOriginatingEntityId;
   DisEntityId           mReceivingEntityId;
   DisEnum8              mRequiredReliabilityService;
   DisUint32             mRequestId;
   DisUint32             mTimeInterval;
   DisDataQueryDatumSpec mDatumSpec;
};

// Accessors

inline const DisEntityId& DisDataQueryR::GetReceivingEntityId() const
{
   return mReceivingEntityId;
}

inline DisEnum8 DisDataQueryR::GetRequiredReliabilityService() const
{
   return mRequiredReliabilityService;
}

inline DisUint32 DisDataQueryR::GetRequestId() const
{
   return mRequestId;
}

inline DisUint32 DisDataQueryR::GetTimeInterval() const
{
   return mTimeInterval;
}

inline const DisDataQueryDatumSpec& DisDataQueryR::GetDatumSpec() const
{
   return mDatumSpec;
}

inline DisDataQueryDatumSpec& DisDataQueryR::GetDatumSpec()
{
   return mDatumSpec;
}


// Mutators

inline void DisDataQueryR::SetOriginatingEntityId(const DisEntityId& aEntityId)
{
   mOriginatingEntityId = aEntityId;
}

inline void DisDataQueryR::SetReceivingEntityId(const DisEntityId& aEntityId)
{
   mReceivingEntityId = aEntityId;
}

inline void DisDataQueryR::SetRequiredReliabilityService(DisEnum8 aRequiredReliabilityService)
{
   mRequiredReliabilityService = aRequiredReliabilityService;
}

inline void DisDataQueryR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

inline void DisDataQueryR::SetTimeInterval(DisUint32 aTimeInterval)
{
   mTimeInterval = aTimeInterval;
}

inline void DisDataQueryR::SetDatumSpec(const DisDataQueryDatumSpec& aDatumSpec)
{
   mDatumSpec = aDatumSpec;
}


#endif
