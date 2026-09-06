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

#ifndef DISACKNOWLEDGER_HPP
#define DISACKNOWLEDGER_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisAcknowledgeR : public DisPdu
{
public:
   DisAcknowledgeR();
   DisAcknowledgeR(DisPdu& aPdu, GenI& aGenI);
   DisAcknowledgeR(GenI& aGenI);

   DisAcknowledgeR(const DisAcknowledgeR& aSrc);

   ~DisAcknowledgeR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   const DisEntityId& GetOriginatingEntity() const override;
   const DisEntityId& GetReceivingEntity() const override;

   DisUint32 GetRequestId() const override;

   // Check this accessor against DisEnum::Control::ResponseToCmdFlag
   DisEnum16 GetAcknowledgeFlag() const;

   // Check this accessor against DisEnum::Control::ResponseCompliance
   DisEnum16 GetResponseFlag() const;

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);

   virtual void SetRequestId(DisUint32 aRequestId);

   void SetAcknowledgeFlag(const DisEnum16 aAcknowledgeFlag);
   void SetResponseFlag(const DisEnum16 aResponseFlag);

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;

   void Stream(std::ostream& aStream) const override;
   bool IsValid() const override;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId mOriginatingEntity;
   DisEntityId mReceivingEntity;

   DisEnum16 mAcknowledgeFlag;
   DisEnum16 mResponseFlag;
   DisUint32 mRequestId;
};

// Accessors

inline DisEnum16 DisAcknowledgeR::GetAcknowledgeFlag() const
{
   return mAcknowledgeFlag;
}

inline DisEnum16 DisAcknowledgeR::GetResponseFlag() const
{
   return mResponseFlag;
}

inline void DisAcknowledgeR::SetAcknowledgeFlag(const DisEnum16 aAcknowledgeFlag)
{
   mAcknowledgeFlag = aAcknowledgeFlag;
}
inline void DisAcknowledgeR::SetResponseFlag(const DisEnum16 aResponseFlag)
{
   mResponseFlag = aResponseFlag;
}

#endif
