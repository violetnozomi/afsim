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

#ifndef DISCREATEENTITY_HPP
#define DISCREATEENTITY_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisCreateEntity : public DisPdu
{
public:
   DisCreateEntity();
   DisCreateEntity(const DisCreateEntity& aSrc);
   DisCreateEntity(DisPdu& aPdu, GenI& aGenI);
   DisCreateEntity(GenI& aGenI);
   ~DisCreateEntity() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   DisUint32          GetRequestId() const override;

   // Mutators

   void SetOriginatingEntity(const DisEntityId& aEntityId);
   void SetReceivingEntity(const DisEntityId& aEntityId);
   void SetRequestId(DisUint32 aRequestId);

   // Input/Output

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
};

inline void DisCreateEntity::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

inline void DisCreateEntity::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

inline void DisCreateEntity::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
inline const DisEntityId& DisCreateEntity::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
inline const DisEntityId& DisCreateEntity::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
inline DisUint32 DisCreateEntity::GetRequestId() const
{
   return mRequestId;
}

#endif
