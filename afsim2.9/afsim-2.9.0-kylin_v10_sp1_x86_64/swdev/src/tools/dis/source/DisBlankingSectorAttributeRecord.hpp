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

// DIS Blanking Sector Attribute Record

#ifndef DIS_BLANKING_SECTOR_ATTRIBUTE_RECORD_HPP
#define DIS_BLANKING_SECTOR_ATTRIBUTE_RECORD_HPP

#include "dis_export.h"

#include "DisAttributeRecord.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisBlankingSectorAttributeRecord : public DisAttributeRecord
{
public:
   DisBlankingSectorAttributeRecord();
   DisBlankingSectorAttributeRecord(const DisAttributeRecord& aAttributeRecord, GenI& aGenI);
   ~DisBlankingSectorAttributeRecord() override;

   // Allow compiler to build copy constructor and operator=
   // DisBlankingSectorAttributeRecord(const DisBlankingSectorAttributeRecord& aSrc);
   // DisBlankingSectorAttributeRecord& operator=(const DisBlankingSectorAttributeRecord& aRhs);

   // Input / Output
   void      GetDerivedClass(GenI& aGenI) override;
   void      PutDerivedClass(GenO& aGenO) const override;
   DisUint16 GetDerivedClassLength() override;

   // Accessors

   DisUint8   GetEmitterNumber() const;
   DisUint8   GetBeamNumber() const;
   DisEnum8   GetStateIndicator() const;
   DisFloat32 GetLeftAzimuthRadians() const;
   DisFloat32 GetRightAzimuthRadians() const;
   DisFloat32 GetLowerElevationRadians() const;
   DisFloat32 GetUpperElevationRadians() const;
   DisFloat32 GetResidualPowerDBm() const;

   // Mutators

   void SetEmitterNumber(DisUint8 aEmitterNumber);
   void SetBeamNumber(DisUint8 aBeamNumber);
   void SetStateIndicator(DisEnum8 aStateIndicator);
   void SetLeftAzimuthRadians(DisFloat32 aLeftAzimuthRadians);
   void SetRightAzimuthRadians(DisFloat32 aRightAzimuthRadians);
   void SetLowerElevationRadians(DisFloat32 aLowerElevationRadians);
   void SetUpperElevationRadians(DisFloat32 aUpperElevationRadians);
   void SetResidualPowerDBm(DisFloat32 aResidualPowerDBm);

   // Data Validation
   bool IsValid() const override;

private:
   DisUint8   mEmitterNumber;
   DisUint8   mBeamNumber;
   DisEnum8   mStateIndicator;
   DisUint8   mPadding1;
   DisFloat32 mLeftAzimuthRadians;
   DisFloat32 mRightAzimuthRadians;
   DisFloat32 mLowerElevationRadians;
   DisFloat32 mUpperElevationRadians;
   DisFloat32 mResidualPowerDBm;
   DisUint64  mPadding2;
};

// Accessors

inline DisUint8 DisBlankingSectorAttributeRecord::GetEmitterNumber() const
{
   return mEmitterNumber;
}

inline DisUint8 DisBlankingSectorAttributeRecord::GetBeamNumber() const
{
   return mBeamNumber;
}

inline DisEnum8 DisBlankingSectorAttributeRecord::GetStateIndicator() const
{
   return mStateIndicator;
}

inline DisFloat32 DisBlankingSectorAttributeRecord::GetLeftAzimuthRadians() const
{
   return mLeftAzimuthRadians;
}

inline DisFloat32 DisBlankingSectorAttributeRecord::GetRightAzimuthRadians() const
{
   return mRightAzimuthRadians;
}

inline DisFloat32 DisBlankingSectorAttributeRecord::GetLowerElevationRadians() const
{
   return mLowerElevationRadians;
}

inline DisFloat32 DisBlankingSectorAttributeRecord::GetUpperElevationRadians() const
{
   return mUpperElevationRadians;
}

inline DisFloat32 DisBlankingSectorAttributeRecord::GetResidualPowerDBm() const
{
   return mResidualPowerDBm;
}

// Mutators

inline void DisBlankingSectorAttributeRecord::SetEmitterNumber(DisUint8 aEmitterNumber)
{
   mEmitterNumber = aEmitterNumber;
}

inline void DisBlankingSectorAttributeRecord::SetBeamNumber(DisUint8 aBeamNumber)
{
   mBeamNumber = aBeamNumber;
}

inline void DisBlankingSectorAttributeRecord::SetStateIndicator(DisEnum8 aStateIndicator)
{
   mStateIndicator = aStateIndicator;
}

inline void DisBlankingSectorAttributeRecord::SetLeftAzimuthRadians(DisFloat32 aLeftAzimuthRadians)
{
   mLeftAzimuthRadians = aLeftAzimuthRadians;
}

inline void DisBlankingSectorAttributeRecord::SetRightAzimuthRadians(DisFloat32 aRightAzimuthRadians)
{
   mRightAzimuthRadians = aRightAzimuthRadians;
}

inline void DisBlankingSectorAttributeRecord::SetLowerElevationRadians(DisFloat32 aLowerElevationRadians)
{
   mLowerElevationRadians = aLowerElevationRadians;
}

inline void DisBlankingSectorAttributeRecord::SetUpperElevationRadians(DisFloat32 aUpperElevationRadians)
{
   mUpperElevationRadians = aUpperElevationRadians;
}

inline void DisBlankingSectorAttributeRecord::SetResidualPowerDBm(DisFloat32 aResidualPowerDBm)
{
   mResidualPowerDBm = aResidualPowerDBm;
}

#endif
