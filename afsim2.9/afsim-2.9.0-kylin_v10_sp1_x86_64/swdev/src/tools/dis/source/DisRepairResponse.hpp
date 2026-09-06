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

#ifndef DISREPAIRRESPONSE_HPP
#define DISREPAIRRESPONSE_HPP

#include "dis_export.h"

#include <ostream>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisRepairResponse : public DisPdu
{
public:
   DisRepairResponse();
   DisRepairResponse(const DisRepairResponse& aSrc);
   DisRepairResponse(const DisPdu& aPdu, GenI& aGenI);
   DisRepairResponse(GenI& aGenI);
   ~DisRepairResponse() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetRequestingEntityId() const;
   const DisEntityId& GetServicingEntityId() const;

   const DisEntityId& GetOriginatingEntity() const override;

   DisUint8 GetServiceResult() const { return mResult; }
   void     SetServiceResult(DisUint8 aResult) { mResult = aResult; }

   // Mutators

   void SetRequestingEntityId(const DisEntityId& aEntityId);
   void SetServicingEntityId(const DisEntityId& aEntityId);

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;

   void Stream(std::ostream& aStream) const override;

private:
   void GetMemberData(GenI& aGenI);

   // Disallow assignment
   DisRepairResponse& operator=(const DisRepairResponse& aRhs);

   DisEntityId mRequestingEntityId;
   DisEntityId mServicingEntityId;

   DisUint8 mResult;
};

// Accessors

inline const DisEntityId& DisRepairResponse::GetRequestingEntityId() const
{
   return mRequestingEntityId;
}
inline const DisEntityId& DisRepairResponse::GetServicingEntityId() const
{
   return mServicingEntityId;
}

// Mutators

inline void DisRepairResponse::SetRequestingEntityId(const DisEntityId& aEntityId)
{
   mRequestingEntityId = aEntityId;
}
inline void DisRepairResponse::SetServicingEntityId(const DisEntityId& aEntityId)
{
   mServicingEntityId = aEntityId;
}

#endif
