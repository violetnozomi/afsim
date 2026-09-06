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

#ifndef DISACKNOWLEDGE_HPP
#define DISACKNOWLEDGE_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisAcknowledge : public DisPdu
{
public:
   DisAcknowledge();
   DisAcknowledge(const DisAcknowledge& aSrc);
   DisAcknowledge(DisPdu& aPdu, GenI& aGenI);
   DisAcknowledge(GenI& aGenI);
   ~DisAcknowledge() override;

   DisPdu* Clone() const override;
   // Allow compiler to build copy constructor and operator=
   // DisAcknowledge(const DisAcknowledge& aSrc);
   // DisAcknowledge& operator=(const DisAcknowledge& aRhs);

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Deprecated.  See file DisControlEnums.hpp for replacement enum DisEnum::Control::Flag.
   enum
   {
      UpiACK_NONE                      = 0,
      UpiACK_CREATE_ENTITY             = 1,
      UpiACK_REMOVE_ENTITY             = 2,
      UpiACK_START_RESUME              = 3,
      UpiACK_START_STOP                = 4,
      UpiACKR_TRANSFER_CONTROL_REQUEST = 5
   };

   // Deprecated.  See file DisControlEnums.hpp for replacement enum DisEnum::Control::Compliance.
   enum
   {
      UpiACK_OTHER            = 0,
      UpiACK_ABLE_TO_COMPLY   = 1,
      UpiACK_UNABLE_TO_COMPLY = 2
   };

   // Accessors

   const DisEntityId& GetReceivingEntity() const override;

   // Check this accessor against DisEnum::Control::ResponseToCmdFlag
   DisEnum16 GetAcknowledgeFlag() const;

   // Check this accessor against DisEnum::Control::ResponseCompliance
   DisEnum16 GetResponseFlag() const;

   DisUint32 GetRequestId() const override;

   // Mutators

   void SetOriginatingEntity(const DisEntityId& aEntityId);
   void SetReceivingEntity(const DisEntityId& aEntityId);
   void SetAcknowledgeFlag(const DisEnum16 aAcknowledgeFlag);
   void SetResponseFlag(const DisEnum16 aResponseFlag);
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
   DisEnum16   mAcknowledgeFlag;
   DisEnum16   mResponseFlag;
   DisUint32   mRequestId;
};

// Accessors

inline DisEnum16 DisAcknowledge::GetAcknowledgeFlag() const
{
   return mAcknowledgeFlag;
}
inline DisEnum16 DisAcknowledge::GetResponseFlag() const
{
   return mResponseFlag;
}

// virtual
inline DisUint32 DisAcknowledge::GetRequestId() const
{
   return mRequestId;
}

// Mutators

inline void DisAcknowledge::SetAcknowledgeFlag(const DisEnum16 aAcknowledgeFlag)
{
   mAcknowledgeFlag = aAcknowledgeFlag;
}
inline void DisAcknowledge::SetResponseFlag(const DisEnum16 aResponseFlag)
{
   mResponseFlag = aResponseFlag;
}

#endif
