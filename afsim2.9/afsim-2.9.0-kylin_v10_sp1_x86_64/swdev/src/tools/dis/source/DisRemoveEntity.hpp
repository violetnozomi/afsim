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

#ifndef DISREMOVEENTITY_HPP
#define DISREMOVEENTITY_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisRemoveEntity : public DisPdu
{
public:
   DisRemoveEntity();
   DisRemoveEntity(DisPdu& aPdu, GenI& aGenI);
   DisRemoveEntity(GenI& aGenI);
   ~DisRemoveEntity() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetReceivingEntity() const override;
   DisUint32          GetRequestId() const override;

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);
   virtual void SetRequestId(DisUint32 aRequestId);

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;

   void Stream(std::ostream& aStream) const override;

private:
   // Disallow copy and assignment
   DisRemoveEntity(const DisRemoveEntity& aSrc);
   DisRemoveEntity& operator=(const DisRemoveEntity& aRhs);

   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisUint32   mRequestId;
};

#endif
