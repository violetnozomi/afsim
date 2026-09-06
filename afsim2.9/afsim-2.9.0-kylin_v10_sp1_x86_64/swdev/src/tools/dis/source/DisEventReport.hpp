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

#ifndef DISEVENTREPORT_HPP
#define DISEVENTREPORT_HPP

#include "dis_export.h"

#include <ostream>

#include "DisDatumSpec.hpp"
#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class DIS_EXPORT DisEventReport : public DisPdu
{
public:
   DisEventReport();
   DisEventReport(const DisEventReport& aSrc);
   DisEventReport(DisPdu& aPdu, GenI& aGenI);
   DisEventReport(GenI& aGenI);
   ~DisEventReport() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   const DisDatumSpec& GetData() const { return mData; }
   DisDatumSpec&       GetData() { return mData; }

   // Mutators

   virtual void SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void SetReceivingEntity(const DisEntityId& aEntityId);
   void         SetEventType(DisUint32 aEventType) { mEventType = aEventType; }
   void         SetData(const DisDatumSpec& aData);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Operators

   DisEventReport& operator=(const DisEventReport& aRhs);

   void Stream(std::ostream& aStream) const override;

private:
   void Copy(const DisEventReport& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId  mOriginatingEntity;
   DisEntityId  mReceivingEntity;
   DisUint32    mEventType;
   DisDatumSpec mData;
};

#endif
