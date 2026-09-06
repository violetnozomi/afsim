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

#ifndef DISEVENTREPORTR_HPP
#define DISEVENTREPORTR_HPP

#include "dis_export.h"

#include <ostream>

#include "DisDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisEventReportR : public DisPdu
{
public:
   DisEventReportR();
   DisEventReportR(const DisEventReportR& aSrc);
   DisEventReportR(DisPdu& aPdu, GenI& aGenI);
   DisEventReportR(GenI& aGenI);
   ~DisEventReportR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   const DisDatumSpec& GetData() const { return mData; }

   const char* GetData(const unsigned int aIndex, unsigned int& aDataLength) const;

   // Mutators

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Operators

   DisEventReportR& operator=(const DisEventReportR& aRhs);

   void Stream(std::ostream& aStream) const override;

private:
   void Copy(const DisEventReportR& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisUint32    mEventType;
   DisDatumSpec mData;
};

#endif
