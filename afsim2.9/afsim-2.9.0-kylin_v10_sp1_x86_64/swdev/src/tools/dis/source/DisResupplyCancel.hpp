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

#ifndef DISRESUPPLYCANCEL_HPP
#define DISRESUPPLYCANCEL_HPP

#include "dis_export.h"

#include <ostream>

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisResupplyCancel : public DisPdu
{
public:
   DisResupplyCancel();
   DisResupplyCancel(const DisResupplyCancel& aSrc);
   DisResupplyCancel(const DisPdu& aPdu, GenI& aGenI);
   DisResupplyCancel(GenI& aGenI);
   ~DisResupplyCancel() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Note that this Cancel PDU can be issued by either the
   // Requesting or the Servicing entity.  There is no way
   // to know for sure who the originator is, other than
   // compare each DisEntityId to self, and figure it out.

   // virtual const DisEntityId& GetOriginatingEntity() const;

   // Accessors

   const DisEntityId& GetRequestingEntityId() const;
   const DisEntityId& GetServicingEntityId() const;

   // Mutators

   void SetRequestingEntityId(const DisEntityId& aEntityId);
   void SetServicingEntityId(const DisEntityId& aEntityId);

   // Input/Output

   void Get(GenI& aGenI) override;
   void Put(GenO& aGenO) override;

   void Stream(std::ostream& aStream) const override;
   // Data Validation
   bool IsValid() const override;

private:
   void GetMemberData(GenI& aGenI);

   // Disallow assignment
   DisResupplyCancel& operator=(const DisResupplyCancel& aRhs);

   DisEntityId mRequestingEntityId;
   DisEntityId mServicingEntityId;
};

// Accessors

inline const DisEntityId& DisResupplyCancel::GetRequestingEntityId() const
{
   return mRequestingEntityId;
}
inline const DisEntityId& DisResupplyCancel::GetServicingEntityId() const
{
   return mServicingEntityId;
}

// Mutators

inline void DisResupplyCancel::SetRequestingEntityId(const DisEntityId& aEntityId)
{
   mRequestingEntityId = aEntityId;
}
inline void DisResupplyCancel::SetServicingEntityId(const DisEntityId& aEntityId)
{
   mServicingEntityId = aEntityId;
}

#endif
