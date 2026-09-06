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

#ifndef DISREPAIRCOMPLETE_HPP
#define DISREPAIRCOMPLETE_HPP

#include "dis_export.h"

#include <ostream>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisRepairComplete : public DisPdu
{
public:
   DisRepairComplete();
   DisRepairComplete(const DisRepairComplete& aSrc);
   DisRepairComplete(const DisPdu& aPdu, GenI& aGenI);
   DisRepairComplete(GenI& aGenI);

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors

   const DisEntityId& GetRequestingEntityId() const;
   const DisEntityId& GetServicingEntityId() const;

   const DisEntityId& GetOriginatingEntity() const override;

   DisEnum16 GetRepair() const { return mRepair; }
   void      SetRepair(DisEnum16 aRepair) { mRepair = aRepair; }

   // Mutators

   void SetRequestingEntityId(const DisEntityId& aEntityId);
   void SetServicingEntityId(const DisEntityId& aEntityId);

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;
   void Stream(std::ostream& aStream) const override;

private:
   void GetMemberData(GenI& aGenI);

   // Disallow assignment
   DisRepairComplete& operator=(const DisRepairComplete& aRhs);

   DisEntityId mRequestingEntityId;
   DisEntityId mServicingEntityId;

   DisEnum16 mRepair;
};

// Accessors

inline const DisEntityId& DisRepairComplete::GetRequestingEntityId() const
{
   return mRequestingEntityId;
}

inline const DisEntityId& DisRepairComplete::GetServicingEntityId() const
{
   return mServicingEntityId;
}

// Mutators

inline void DisRepairComplete::SetRequestingEntityId(const DisEntityId& aEntityId)
{
   mRequestingEntityId = aEntityId;
}

inline void DisRepairComplete::SetServicingEntityId(const DisEntityId& aEntityId)
{
   mServicingEntityId = aEntityId;
}

#endif
