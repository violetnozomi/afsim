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

#ifndef DISDATAR_HPP
#define DISDATAR_HPP

#include "dis_export.h"

#include <ostream>

#include "DisDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

// DisDataR - Ref: IEEE 1278.1-2012 (DIS)
class DIS_EXPORT DisDataR : public DisPdu
{
public:
   DisDataR();
   DisDataR(const DisDataR& aSrc);
   DisDataR(DisPdu& aPdu, GenI& aGenI);
   DisDataR(GenI& aGenI);
   ~DisDataR() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId&  GetReceivingEntity() const override;
   DisUint32           GetRequestId() const override;
   DisEnum8            GetReliabilityService() const override;
   const DisDatumSpec& GetData() const { return mData; }

   // Mutators

   void SetOriginatingEntity(const DisEntityId& aEntityId);
   void SetReceivingEntity(const DisEntityId& aEntityId);
   void SetRequestId(DisUint32 aRequestId);
   void SetReliabilityService(DisEnum8 aReliabilityService);

   DisDatumSpec& GetData() { return mData; }
   void          SetDatumSpec(const DisDatumSpec& aDatumSpec);

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;

   // Operators

   DisUint16 GetLength() override;

protected:
   DisDataR& operator=(const DisDataR& aRhs);

private:
   void Copy(const DisDataR& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;
   DisUint32   mRequestId;
   DisEnum8    mReliabilityService;

   DisDatumSpec mData;
};

inline void DisDataR::SetDatumSpec(const DisDatumSpec& aDatumSpec)
{
   mData = aDatumSpec;
}
#endif
