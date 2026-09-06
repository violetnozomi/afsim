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

#ifndef DISSETDATA_HPP
#define DISSETDATA_HPP

#include "dis_export.h"

#include <ostream>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;
#include "DisDatumSpec.hpp"

// DisSetData - Ref: IEEE 1278.1-2012 (DIS)
class DIS_EXPORT DisSetData : public DisPdu
{
public:
   DisSetData();
   DisSetData(const DisSetData& aSrc);
   DisSetData(DisPdu& aPdu, GenI& aGenI);
   DisSetData(GenI& aGenI);
   ~DisSetData() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetReceivingEntity() const override;
   DisUint32          GetRequestId() const override;
   DisUint32          GetNumFixedDatums() const;
   DisUint32          GetNumVariableDatums() const;

   const DisDatumSpec& GetData() const { return mData; }

   // Mutators

   void SetOriginatingEntity(const DisEntityId& aEntityId);
   void SetReceivingEntity(const DisEntityId& aEntityId);
   void SetRequestId(DisUint32 aRequestId);

   DisDatumSpec& GetData() { return mData; }

   void SetData(const DisDatumSpec& aDatumSpec);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;
   void      Stream(std::ostream& aStream) const override;
   bool      IsValid() const override;

private:
   // Disable use of assignment operator.
   DisSetData& operator=(const DisSetData& aRhs);

   void Copy(const DisSetData& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisUint32   mRequestId;
   DisUint32   mDataLength; // in BITS!

   DisDatumSpec mData;
};

inline const DisEntityId& DisSetData::GetReceivingEntity() const
{
   return mReceivingEntity;
}
inline DisUint32 DisSetData::GetRequestId() const
{
   return mRequestId;
}

// const DisEntityId& DisSetData::GetOriginatingEntity() const
//{
//    return mOriginatingEntity ;
// }

inline void DisSetData::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}
inline void DisSetData::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}
inline void DisSetData::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}
inline void DisSetData::SetData(const DisDatumSpec& aDatumSpec)
{
   mData = aDatumSpec;
}
#endif
