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

#ifndef DISDATAQUERY_HPP
#define DISDATAQUERY_HPP

#include "dis_export.h"

#include "DisDataQueryDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisTypes.hpp"

// DisDataQuery - Ref: IEEE 1278.1-2012 (DIS)
class DIS_EXPORT DisDataQuery : public DisPdu
{
public:
   DisDataQuery();
   DisDataQuery(DisPdu& aPdu, GenI& aGenI);
   DisDataQuery(GenI& aGenI);
   ~DisDataQuery() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors
   const DisEntityId&           GetReceivingEntityId() const;
   DisUint32                    GetRequestId() const override;
   DisUint32                    GetTimeInterval() const;
   const DisDataQueryDatumSpec& GetDatumSpec() const;
   DisDataQueryDatumSpec&       GetDatumSpec();

   // Mutators
   void SetOriginatingEntityId(const DisEntityId& aEntityId);
   void SetReceivingEntityId(const DisEntityId& aEntityId);
   void SetRequestId(DisEnum32 aRequestId);
   void SetTimeInterval(DisUint32 aTimeInterval);
   void SetDatumSpec(const DisDataQueryDatumSpec& aDatumSpec);


   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId           mOriginatingEntityId;
   DisEntityId           mReceivingEntityId;
   DisUint32             mRequestId;
   DisUint32             mTimeInterval;
   DisDataQueryDatumSpec mDatumSpec;
};

// Accessors

inline const DisEntityId& DisDataQuery::GetReceivingEntityId() const
{
   return mReceivingEntityId;
}

inline DisUint32 DisDataQuery::GetRequestId() const
{
   return mRequestId;
}

inline DisUint32 DisDataQuery::GetTimeInterval() const
{
   return mTimeInterval;
}

inline const DisDataQueryDatumSpec& DisDataQuery::GetDatumSpec() const
{
   return mDatumSpec;
}

inline DisDataQueryDatumSpec& DisDataQuery::GetDatumSpec()
{
   return mDatumSpec;
}


// Mutators

inline void DisDataQuery::SetOriginatingEntityId(const DisEntityId& aEntityId)
{
   mOriginatingEntityId = aEntityId;
}

inline void DisDataQuery::SetReceivingEntityId(const DisEntityId& aEntityId)
{
   mReceivingEntityId = aEntityId;
}

inline void DisDataQuery::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

inline void DisDataQuery::SetTimeInterval(DisUint32 aTimeInterval)
{
   mTimeInterval = aTimeInterval;
}

inline void DisDataQuery::SetDatumSpec(const DisDataQueryDatumSpec& aDatumSpec)
{
   mDatumSpec = aDatumSpec;
}

#endif
