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

// DIS DE Beam Antenna Pattern Record

#ifndef DIS_DE_BEAM_ANTENNA_PATTERN_RECORD_HPP
#define DIS_DE_BEAM_ANTENNA_PATTERN_RECORD_HPP

#include "dis_export.h"

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDEBeamAntennaPatternRecord
{
public:
   DisDEBeamAntennaPatternRecord();
   DisDEBeamAntennaPatternRecord(GenI& aGenI);
   ~DisDEBeamAntennaPatternRecord();

   // Input/output

   DisUint16 GetLength() const;
   void      Get(GenI& aGenI);
   void      Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   // Accessors

   void       GetBeamDirection(DisFloat32& aDirectionX, DisFloat32& aDirectionY, DisFloat32& aDirectionZ);
   DisFloat32 GetAzimuthBeamwidth();
   DisFloat32 GetElevationBeamwidth();
   DisEnum8   GetReferenceSystem();
   DisFloat32 GetEz();
   DisFloat32 GetEx();
   DisFloat32 GetPhase();

   // Mutators

   void SetBeamDirection(DisFloat32 aDirectionX, DisFloat32 aDirectionY, DisFloat32 aDirectionZ);
   void SetAzimuthBeamwidth(const DisFloat32 aAzimuthBeamwidth);
   void SetElevationBeamwidth(const DisFloat32 aElevationBeamwidth);
   void SetReferenceSystem(const DisEnum8 aReferenceSystem);
   void SetEz(const DisFloat32 aEz);
   void SetEx(const DisFloat32 aEx);
   void SetPhase(const DisFloat32 aPhase);

private:
   DisFloat32 mDirection[3];
   DisFloat32 mAzimuthBeamwidth;
   DisFloat32 mElevationBeamwidth;
   DisEnum8   mReferenceSystem;
   DisUint8   mPadding1;
   DisUint16  mPadding2;
   DisFloat32 mEz;
   DisFloat32 mEx;
   DisFloat32 mPhase;
   DisUint32  mPadding3;
};

// Accessors

inline DisUint16 DisDEBeamAntennaPatternRecord::GetLength() const
{
   return 40;
}

inline void DisDEBeamAntennaPatternRecord::GetBeamDirection(DisFloat32& aDirectionX,
                                                            DisFloat32& aDirectionY,
                                                            DisFloat32& aDirectionZ)
{
   aDirectionX = mDirection[0];
   aDirectionY = mDirection[1];
   aDirectionZ = mDirection[2];
}

inline DisFloat32 DisDEBeamAntennaPatternRecord::GetAzimuthBeamwidth()
{
   return mAzimuthBeamwidth;
}

inline DisFloat32 DisDEBeamAntennaPatternRecord::GetElevationBeamwidth()
{
   return mElevationBeamwidth;
}

inline DisEnum8 DisDEBeamAntennaPatternRecord::GetReferenceSystem()
{
   return mReferenceSystem;
}

inline DisFloat32 DisDEBeamAntennaPatternRecord::GetEz()
{
   return mEz;
}

inline DisFloat32 DisDEBeamAntennaPatternRecord::GetEx()
{
   return mEx;
}

inline DisFloat32 DisDEBeamAntennaPatternRecord::GetPhase()
{
   return mPhase;
}

// Mutators

inline void DisDEBeamAntennaPatternRecord::SetBeamDirection(DisFloat32 aDirectionX,
                                                            DisFloat32 aDirectionY,
                                                            DisFloat32 aDirectionZ)
{
   mDirection[0] = aDirectionX;
   mDirection[1] = aDirectionY;
   mDirection[2] = aDirectionZ;
}

inline void DisDEBeamAntennaPatternRecord::SetAzimuthBeamwidth(DisFloat32 aAzimuthBeamwidth)
{
   mAzimuthBeamwidth = aAzimuthBeamwidth;
}

inline void DisDEBeamAntennaPatternRecord::SetElevationBeamwidth(DisFloat32 aElevationBeamwidth)
{
   mElevationBeamwidth = aElevationBeamwidth;
}

inline void DisDEBeamAntennaPatternRecord::SetReferenceSystem(DisEnum8 aReferenceSystem)
{
   mReferenceSystem = aReferenceSystem;
}

inline void DisDEBeamAntennaPatternRecord::SetEz(DisFloat32 aEz)
{
   mEz = aEz;
}

inline void DisDEBeamAntennaPatternRecord::SetEx(DisFloat32 aEx)
{
   mEx = aEx;
}

inline void DisDEBeamAntennaPatternRecord::SetPhase(DisFloat32 aPhase)
{
   mPhase = aPhase;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisDEBeamAntennaPatternRecord& aDEBeamAntennaPatternRecord)
{
   aDEBeamAntennaPatternRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisDEBeamAntennaPatternRecord& aDEBeamAntennaPatternRecord)
{
   aDEBeamAntennaPatternRecord.Get(aGenI);
   return aGenI;
}

#endif
