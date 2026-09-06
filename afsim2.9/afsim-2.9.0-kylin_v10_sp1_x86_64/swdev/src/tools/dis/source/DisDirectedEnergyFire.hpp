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

#ifndef DIS_DIRECTED_ENERGY_FIRE_HPP
#define DIS_DIRECTED_ENERGY_FIRE_HPP

#include "dis_export.h"

#include "DisClockTime.hpp"
#include "DisDERecord.hpp"
#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisEventId.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDirectedEnergyFire : public DisPdu
{
public:
   DisDirectedEnergyFire();
   DisDirectedEnergyFire(DisPdu& aPdu, GenI& aGenI);
   DisDirectedEnergyFire(GenI& aGenI);
   ~DisDirectedEnergyFire() override;

   DisPdu* Clone() const override;

   int GetClass() const override;

   // Accessors
   const DisEntityId&   GetFiringEntity() const;
   const DisEventId&    GetEvent() const;
   const DisEntityType& GetMunitionType() const;
   const DisClockTime&  GetShotStartTime() const;
   DisFloat32           GetCumulativeShotTime() const;
   void                 GetApertureLocation(DisFloat32& aApertureLocationX,
                                            DisFloat32& aApertureLocationY,
                                            DisFloat32& aApertureLocationZ) const;
   DisFloat32           GetApertureDiameter() const;
   DisFloat32           GetWavelength() const;
   DisFloat32           GetPulseRepetitionFrequency() const;
   DisFloat32           GetPulseWidth() const;
   DisEnum16            GetFlags() const;
   DisEnum8             GetPulseShape() const;
   DisUint16            GetNumberOfDERecords();

   // Mutators
   void SetFiringEntity(const DisEntityId& aFiringEntity);
   void SetEvent(const DisEventId& aEvent);
   void SetMunitionType(const DisEntityType& aMunitionType);
   void SetShotStartTime(const DisClockTime& aShotStartTime);
   void SetCumulativeShotTime(const DisFloat32 aShotTime);
   void SetApertureLocation(DisFloat32 aApertureLocationX, DisFloat32 aApertureLocationY, DisFloat32 aApertureLocationZ);
   void SetApertureDiameter(DisFloat32 aApertureDiameter);
   void SetWavelength(DisFloat32 aWavelength);
   void SetPulseRepetitionFrequency(DisFloat32 aPulseRepetitionFrequency);
   void SetPulseWidth(DisFloat32 aPulseWidth);
   void SetFlags(DisEnum16 aFlags);
   void SetPulseShape(DisEnum8 aPulseShape);

   // Input/Output

   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Data Validation
   bool IsValid() const override;

   void AddDERecord(DisDERecord* aDERecord);

   // Return iterators to DisDERecord vector.
   std::vector<DisDERecord*>::iterator       GetDEBegin();
   std::vector<DisDERecord*>::const_iterator GetDEBegin() const;
   std::vector<DisDERecord*>::iterator       GetDEEEnd();
   std::vector<DisDERecord*>::const_iterator GetDEEEnd() const;

   unsigned int RemoveDERecord(DisDERecord* aDERecord);

   void RemoveAllDERecords();

   unsigned int RemoveDERecordWithNoDelete(DisDERecord* aDERecord);

   void RemoveAllDERecordsWithNoDelete();

private:
   void GetMemberData(GenI& aGenI);

   void UpdateDERecordCount() const;

   DisEntityId       mFiringEntity;
   DisEventId        mEvent;
   DisEntityType     mMunitionType;
   DisClockTime      mShotStartTime;
   DisFloat32        mCumulativeShotTime;
   DisFloat32        mApertureLocation[3];
   DisFloat32        mApertureDiameter;
   DisFloat32        mWavelength;
   DisUint32         mPadding4;
   DisFloat32        mPulseRepetitionFrequency;
   DisFloat32        mPulseWidth;
   DisEnum16         mFlags;
   DisEnum8          mPulseShape;
   DisUint8          mPadding1;
   DisUint32         mPadding2;
   DisUint16         mPadding3;
   mutable DisUint16 mNumberOfDERecords;

   DisPtrContainer<DisDERecord> mDERecordList;
};

// Accessors

inline void DisDirectedEnergyFire::UpdateDERecordCount() const
{
   mNumberOfDERecords = static_cast<DisUint16>(mDERecordList.GetSize());
}

inline const DisEntityId& DisDirectedEnergyFire::GetFiringEntity() const
{
   return mFiringEntity;
}

inline const DisEventId& DisDirectedEnergyFire::GetEvent() const
{
   return mEvent;
}

inline const DisEntityType& DisDirectedEnergyFire::GetMunitionType() const
{
   return mMunitionType;
}

inline const DisClockTime& DisDirectedEnergyFire::GetShotStartTime() const
{
   return mShotStartTime;
}

inline DisFloat32 DisDirectedEnergyFire::GetCumulativeShotTime() const
{
   return mCumulativeShotTime;
}

inline void DisDirectedEnergyFire::GetApertureLocation(DisFloat32& aApertureLocationX,
                                                       DisFloat32& aApertureLocationY,
                                                       DisFloat32& aApertureLocationZ) const
{
   aApertureLocationX = mApertureLocation[0];
   aApertureLocationY = mApertureLocation[1];
   aApertureLocationZ = mApertureLocation[2];
}

inline DisFloat32 DisDirectedEnergyFire::GetApertureDiameter() const
{
   return mApertureDiameter;
}

inline DisFloat32 DisDirectedEnergyFire::GetWavelength() const
{
   return mWavelength;
}

inline DisFloat32 DisDirectedEnergyFire::GetPulseRepetitionFrequency() const
{
   return mPulseRepetitionFrequency;
}

inline DisFloat32 DisDirectedEnergyFire::GetPulseWidth() const
{
   return mPulseWidth;
}

inline DisEnum16 DisDirectedEnergyFire::GetFlags() const
{
   return mFlags;
}

inline DisEnum8 DisDirectedEnergyFire::GetPulseShape() const
{
   return mPulseShape;
}

inline DisUint16 DisDirectedEnergyFire::GetNumberOfDERecords()
{
   UpdateDERecordCount();
   return mNumberOfDERecords;
}

// Mutators
inline void DisDirectedEnergyFire::SetFiringEntity(const DisEntityId& aFiringEntity)
{
   mFiringEntity = aFiringEntity;
}

inline void DisDirectedEnergyFire::SetEvent(const DisEventId& aEvent)
{
   mEvent = aEvent;
}

inline void DisDirectedEnergyFire::SetMunitionType(const DisEntityType& aMunitionType)
{
   mMunitionType = aMunitionType;
}

inline void DisDirectedEnergyFire::SetShotStartTime(const DisClockTime& aShotStartTime)
{
   mShotStartTime = aShotStartTime;
}

inline void DisDirectedEnergyFire::SetCumulativeShotTime(DisFloat32 aShotTime)
{
   mCumulativeShotTime = aShotTime;
}

inline void DisDirectedEnergyFire::SetApertureLocation(DisFloat32 aApertureLocationX,
                                                       DisFloat32 aApertureLocationY,
                                                       DisFloat32 aApertureLocationZ)
{
   mApertureLocation[0] = aApertureLocationX;
   mApertureLocation[1] = aApertureLocationY;
   mApertureLocation[2] = aApertureLocationZ;
}

inline void DisDirectedEnergyFire::SetApertureDiameter(DisFloat32 aApertureDiameter)
{
   mApertureDiameter = aApertureDiameter;
}

inline void DisDirectedEnergyFire::SetWavelength(DisFloat32 aWavelength)
{
   mWavelength = aWavelength;
}

inline void DisDirectedEnergyFire::SetPulseRepetitionFrequency(DisFloat32 aPulseRepetitionFrequency)
{
   mPulseRepetitionFrequency = aPulseRepetitionFrequency;
}

inline void DisDirectedEnergyFire::SetPulseWidth(DisFloat32 aPulseWidth)
{
   mPulseWidth = aPulseWidth;
}

inline void DisDirectedEnergyFire::SetFlags(DisEnum16 aFlags)
{
   mFlags = aFlags;
}

inline void DisDirectedEnergyFire::SetPulseShape(DisEnum8 aPulseShape)
{
   mPulseShape = aPulseShape;
}

// DE Record list methods
inline void DisDirectedEnergyFire::AddDERecord(DisDERecord* aDERecord)
{
   mDERecordList.Add(aDERecord);
}
inline std::vector<DisDERecord*>::iterator DisDirectedEnergyFire::GetDEBegin()
{
   return mDERecordList.GetBegin();
}
inline std::vector<DisDERecord*>::const_iterator DisDirectedEnergyFire::GetDEBegin() const
{
   return mDERecordList.GetBegin();
}
inline std::vector<DisDERecord*>::iterator DisDirectedEnergyFire::GetDEEEnd()
{
   return mDERecordList.GetEnd();
}
inline std::vector<DisDERecord*>::const_iterator DisDirectedEnergyFire::GetDEEEnd() const
{
   return mDERecordList.GetEnd();
}
inline unsigned int DisDirectedEnergyFire::RemoveDERecord(DisDERecord* aDERecord)
{
   return mDERecordList.Remove(aDERecord);
}
inline void DisDirectedEnergyFire::RemoveAllDERecords()
{
   mDERecordList.RemoveAll();
}
inline unsigned int DisDirectedEnergyFire::RemoveDERecordWithNoDelete(DisDERecord* aDERecord)
{
   return mDERecordList.RemoveWithNoDelete(aDERecord);
}
inline void DisDirectedEnergyFire::RemoveAllDERecordsWithNoDelete()
{
   mDERecordList.RemoveAllWithNoDelete();
}

#endif
