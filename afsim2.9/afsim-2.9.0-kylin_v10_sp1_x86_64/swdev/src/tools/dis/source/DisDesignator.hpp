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

#ifndef DISDESIGNATOR_HPP
#define DISDESIGNATOR_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDesignator : public DisPdu
{
public:
   DisDesignator();
   DisDesignator(DisPdu& aPdu, GenI& aGenI);
   DisDesignator(GenI& aGenI);
   DisDesignator(const DisDesignator& aSrc);

   ~DisDesignator() override;

   DisPdu* Clone() const override;

   int                GetClass() const override;
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   const DisEntityId& GetDesignatingEntity() const;
   const DisEntityId& GetDesignatedEntity() const;
   DisEnum16          GetCodeName() const;
   DisEnum16          GetCode() const;
   DisFloat32         GetPower() const;
   DisFloat32         GetWavelength() const;
   void               GetOffset(DisFloat32& aOffsetX, DisFloat32& aOffsetY, DisFloat32& aOffsetZ) const;
   void               GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const;
   DisEnum8           GetDeadReckoningAlgorithm() const;
   void               GetAcceleration(DisFloat32& aAccX, DisFloat32& aAccY, DisFloat32& aAccZ) const;

   // Mutators

   void SetDesignatingEntity(const DisEntityId& aDesignatingEntity);
   void SetDesignatedEntity(const DisEntityId& aDesignatedEntity);
   void SetCode(DisEnum16 aCode);
   void SetCodeName(DisEnum16 aCode);
   void SetPower(DisFloat32 aPower);
   void SetWavelength(DisFloat32 aWavelength);
   void SetOffset(DisFloat32 aOffsetX, DisFloat32 aOffsetY, DisFloat32 aOffsetZ);
   void SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ);
   void SetDeadReckoningAlgorithm(DisEnum8 aDeadReckoningAlgorithm);
   void SetAcceleration(DisFloat32 aAccX, DisFloat32 aAccY, DisFloat32 aAccZ);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

private:
   void GetMemberData(GenI& aGenI);

   DisEntityId mDesignatingEntity;
   DisEnum16   mCodeName;
   DisEntityId mDesignatedEntity;
   DisEnum16   mCode;
   DisFloat32  mPower;
   DisFloat32  mWavelength;
   DisFloat32  mOffset[3];
   DisFloat64  mLocation[3];
   DisEnum8    mDeadReckoningAlgorithm;
   DisFloat32  mAcc[3];
};

// Accessors

inline const DisEntityId& DisDesignator::GetDesignatingEntity() const
{
   return mDesignatingEntity;
}
inline const DisEntityId& DisDesignator::GetDesignatedEntity() const
{
   return mDesignatedEntity;
}
inline DisEnum16 DisDesignator::GetCodeName() const
{
   return mCodeName;
}
inline DisEnum16 DisDesignator::GetCode() const
{
   return mCode;
}
inline DisFloat32 DisDesignator::GetPower() const
{
   return mPower;
}
inline DisFloat32 DisDesignator::GetWavelength() const
{
   return mWavelength;
}
inline void DisDesignator::GetOffset(DisFloat32& aOffsetX, DisFloat32& aOffsetY, DisFloat32& aOffsetZ) const
{
   aOffsetX = mOffset[0];
   aOffsetY = mOffset[1];
   aOffsetZ = mOffset[2];
}
inline void DisDesignator::GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}
inline DisEnum8 DisDesignator::GetDeadReckoningAlgorithm() const
{
   return mDeadReckoningAlgorithm;
}
inline void DisDesignator::GetAcceleration(DisFloat32& aAccX, DisFloat32& aAccY, DisFloat32& aAccZ) const
{
   aAccX = mAcc[0];
   aAccY = mAcc[1];
   aAccZ = mAcc[2];
}

// Mutators

inline void DisDesignator::SetDesignatingEntity(const DisEntityId& aDesignatingEntity)
{
   mDesignatingEntity = aDesignatingEntity;
}
inline void DisDesignator::SetDesignatedEntity(const DisEntityId& aDesignatedEntity)
{
   mDesignatedEntity = aDesignatedEntity;
}
inline void DisDesignator::SetCode(DisEnum16 aCode)
{
   mCode = aCode;
}
inline void DisDesignator::SetCodeName(DisEnum16 aCodeName)
{
   mCodeName = aCodeName;
}
inline void DisDesignator::SetPower(DisFloat32 aPower)
{
   mPower = aPower;
}
inline void DisDesignator::SetWavelength(DisFloat32 aWavelength)
{
   mWavelength = aWavelength;
}
inline void DisDesignator::SetOffset(DisFloat32 aOffsetX, DisFloat32 aOffsetY, DisFloat32 aOffsetZ)
{
   mOffset[0] = aOffsetX;
   mOffset[1] = aOffsetY;
   mOffset[2] = aOffsetZ;
}
inline void DisDesignator::SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}
inline void DisDesignator::SetDeadReckoningAlgorithm(DisEnum8 aDeadReckoningAlgorithm)
{
   mDeadReckoningAlgorithm = aDeadReckoningAlgorithm;
}
inline void DisDesignator::SetAcceleration(DisFloat32 aAccX, DisFloat32 aAccY, DisFloat32 aAccZ)
{
   mAcc[0] = aAccX;
   mAcc[1] = aAccY;
   mAcc[2] = aAccZ;
}

#endif
