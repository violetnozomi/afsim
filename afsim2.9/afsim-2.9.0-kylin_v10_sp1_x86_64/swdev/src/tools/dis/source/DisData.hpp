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

#ifndef DISDATA_HPP
#define DISDATA_HPP

#include "dis_export.h"

#include <ostream>

#include "DisDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

// DisData - Ref: IEEE 1278.1-2012 (DIS)
class DIS_EXPORT DisData : public DisPdu
{
public:
   DisData();
   DisData(const DisData& aSrc);
   DisData(DisPdu& aPdu, GenI& aGenI);
   DisData(GenI& aGenI);
   ~DisData() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   DisUint32          GetRequestId() const override;
   DisUint32          GetNumFixedDatums() const;
   DisUint32          GetNumVariableDatums() const;

   const DisDatumSpec& GetData() const { return mData; }

   // Mutators
   void SetDatumSpec(const DisDatumSpec& aDatumSpec);

   void          SetOriginatingEntity(const DisEntityId& aEntityId);
   void          SetReceivingEntity(const DisEntityId& aEntityId);
   void          SetRequestId(DisUint32 aRequestId);
   DisDatumSpec& GetData() { return mData; }

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;
   void      Stream(std::ostream& aStream) const override;
   //! Data Validation
   bool IsValid() const override;

private:
   DisData& operator=(const DisData& aRhs);

   void Copy(const DisData& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisUint32   mRequestId;
   DisUint32   mDataLength; // in BITS!

   DisDatumSpec mData;
};

inline void DisData::SetDatumSpec(const DisDatumSpec& aDatumSpec)
{
   mData = aDatumSpec;
}
#endif
