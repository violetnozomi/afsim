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


// DIS Emitter Beam

#ifndef DISUABEAM_HPP
#define DISUABEAM_HPP

#include "dis_export.h"

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisUABeam
{
public:
   DisUABeam();
   ~DisUABeam();

   // Allow compiler to build copy constructor and operator=
   // DisUABeam(const DisUABeam& aSrc);
   // DisUABeam& operator=(const DisUABeam& aRhs);

   // Accessors

   DisUint8   GetDataLength();
   DisUint8   GetBeamId() const;
   DisUint16  GetIndex() const;
   DisUint16  GetScanPattern() const;
   DisFloat32 GetAzimuthCenter() const;
   DisFloat32 GetAzimuthSweep() const;
   DisFloat32 GetElevationCenter() const;
   DisFloat32 GetElevationSweep() const;

   // Mutators

   void SetBeamId(DisUint8 aBeamId);
   void SetIndex(DisUint16 aIndex);
   void SetScanPattern(DisUint16 aScan);
   void SetAzimuthCenter(DisFloat32 aAzimuthCenter);
   void SetAzimuthSweep(DisFloat32 aAzimuthSweep);
   void SetElevationCenter(DisFloat32 aElevationCenter);
   void SetElevationSweep(DisFloat32 aElevationSweep);

   // Input/output

   DisUint16 GetLength();
   void      Get(GenI& aGenI);

   //! Data Validation
   bool IsValid() const;

   // A "GetLength()" call must be made prior to calling this function
   // Generally this is not a problem, because "DisEmission::Put(GenIO&)"
   // will call GetLength at the appropriate time.
   void Put(GenO& aGenO);

   // The following gets the length as read from the header, not the
   // length computed by adding the length of the base data plus the
   // length required for the specified number of Track/Jam entries.
   DisUint16 GetLengthRead() const;

private:
   DisUint8   mDataLength;
   DisUint8   mBeamId;
   DisUint16  mIndex;
   DisUint16  mScanPattern;
   DisFloat32 mAzimuthCenter;
   DisFloat32 mAzimuthSweep;
   DisFloat32 mElevationCenter;
   DisFloat32 mElevationSweep;

   DisUint16 mLengthRead;
};

// Accessors

inline DisUint8 DisUABeam::GetBeamId() const
{
   return mBeamId;
}
inline DisUint16 DisUABeam::GetIndex() const
{
   return mIndex;
}
inline DisUint16 DisUABeam::GetScanPattern() const
{
   return mScanPattern;
}
inline DisFloat32 DisUABeam::GetAzimuthCenter() const
{
   return mAzimuthCenter;
}
inline DisFloat32 DisUABeam::GetAzimuthSweep() const
{
   return mAzimuthSweep;
}
inline DisFloat32 DisUABeam::GetElevationCenter() const
{
   return mElevationCenter;
}
inline DisFloat32 DisUABeam::GetElevationSweep() const
{
   return mElevationSweep;
}
inline DisUint16 DisUABeam::GetLengthRead() const
{
   return mLengthRead;
}

// Mutators

inline void DisUABeam::SetBeamId(DisUint8 aBeamId)
{
   mBeamId = aBeamId;
}
inline void DisUABeam::SetIndex(DisUint16 aIndex)
{
   mIndex = aIndex;
}
inline void DisUABeam::SetScanPattern(DisUint16 aScan)
{
   mScanPattern = aScan;
}
inline void DisUABeam::SetAzimuthCenter(DisFloat32 aAzimuthCenter)
{
   mAzimuthCenter = aAzimuthCenter;
}
inline void DisUABeam::SetAzimuthSweep(DisFloat32 aAzimuthSweep)
{
   mAzimuthSweep = aAzimuthSweep;
}
inline void DisUABeam::SetElevationCenter(DisFloat32 aElevationCenter)
{
   mElevationCenter = aElevationCenter;
}
inline void DisUABeam::SetElevationSweep(DisFloat32 aElevationSweep)
{
   mElevationSweep = aElevationSweep;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, DisUABeam& aBeam)
{
   aBeam.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisUABeam& aBeam)
{
   aBeam.Get(aGenI);
   return aGenI;
}

#endif
