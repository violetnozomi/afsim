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

#ifndef DISCREATEENTITYR_HPP
#define DISCREATEENTITYR_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisCreateEntityR : public DisPdu
{
public:
   DisCreateEntityR();
   DisCreateEntityR(DisPdu& aPdu, GenI& aGenI);
   DisCreateEntityR(GenI& aGenI);
   DisCreateEntityR(const DisCreateEntityR& aSrc);

   ~DisCreateEntityR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetReceivingEntity() const override;
   DisEnum8           GetReliabilityService() const override;
   DisUint32          GetRequestId() const override;

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);
   virtual void SetReliabilityService(DisEnum8 aReliabilityService);
   virtual void SetRequestId(DisUint32 aRequestId);

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;

   // Data Validation
   bool IsValid() const override;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisUint32   mRequestId;
   DisEnum8    mReliabilityService;
};

#endif
