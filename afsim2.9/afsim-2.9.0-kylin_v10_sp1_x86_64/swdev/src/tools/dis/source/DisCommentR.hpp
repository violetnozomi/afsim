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

#ifndef DISCOMMENTR_HPP
#define DISCOMMENTR_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "DisVariableDatum.hpp"

class DIS_EXPORT DisCommentR : public DisPdu
{
public:
   DisCommentR();
   DisCommentR(const DisCommentR& aSrc);
   DisCommentR(DisPdu& aPdu, GenI& aGenI);
   DisCommentR(GenI& aGenI);
   ~DisCommentR() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   const DisVariableDatum& GetData() const { return mData; }

   // Mutators

   virtual void      SetOriginatingEntity(const DisEntityId& aEntityId);
   virtual void      SetReceivingEntity(const DisEntityId& aEntityId);
   DisVariableDatum& GetData() { return mData; }

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;

   DisUint16 GetLength() override;

private:
   // Operators
   DisCommentR& operator=(const DisCommentR& aRhs);

   void Copy(const DisCommentR& aRhs);
   void GetMemberData(GenI& aGenI);

   DisEntityId      mOriginatingEntity;
   DisEntityId      mReceivingEntity;
   DisVariableDatum mData;
};

#endif
