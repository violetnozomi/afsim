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

#ifndef DISREMOVEENTITYR_HPP
#define DISREMOVEENTITYR_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisRemoveEntityR : public DisPdu
{
public:
   DisRemoveEntityR();
   DisRemoveEntityR(DisPdu& aPdu, GenI& aGenI);
   DisRemoveEntityR(GenI& aGenI);
   ~DisRemoveEntityR() override;

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

private:
   // Disallow copy and assignment
   DisRemoveEntityR(const DisRemoveEntityR& aSrc);
   DisRemoveEntityR& operator=(const DisRemoveEntityR& aRhs);

   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisEnum8    mReliabilityService;
   DisUint32   mRequestId;
};

#endif
