// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISGAUSSIANPUFFRECORD_HPP
#define DISGAUSSIANPUFFRECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisPointRecord.hpp"

/**!
 * Defines a Gaussian Puff point
 */
class DIS_EXPORT DisGaussianPuffRecord : public DisPointRecord
{
public:
   DisGaussianPuffRecord();
   DisGaussianPuffRecord(const DisGaussianPuffRecord& aSrc);
   DisGaussianPuffRecord(GenI& aGenI);
   ~DisGaussianPuffRecord() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool                   IsValid() const override;
   virtual DisGaussianPuffRecord* Clone() const override;

   // Getters
   void       GetOriginatingLocation(DisFloat64& aXLocation, DisFloat64& aYLocation, DisFloat64& aZLocation) const;
   DisFloat32 GetSigmaX() const;
   DisFloat32 GetSigmaY() const;
   DisFloat32 GetSigmaZ() const;
   DisFloat32 GetDeltaSigmaX() const;
   DisFloat32 GetDeltaSigmaY() const;
   DisFloat32 GetDeltaSigmaZ() const;
   void       GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   void       GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void GetAngularVelocity(DisFloat32& aAngularVelocityX, DisFloat32& aAngularVelocityY, DisFloat32& aAngularVelocityZ) const;
   DisFloat32 GetCentroidHeight() const;

   // Setters
   void SetOriginatingLocation(DisFloat64 aXLocation, DisFloat64 aYLocation, DisFloat64 aZLocation);
   void SetSigmaX(DisFloat32 aSigmaX);
   void SetSigmaY(DisFloat32 aSigmaY);
   void SetSigmaZ(DisFloat32 aSigmaZ);
   void SetDeltaSigmaX(DisFloat32 aDeltaSigmaX);
   void SetDeltaSigmaY(DisFloat32 aDeltaSigmaY);
   void SetDeltaSigmaZ(DisFloat32 aDeltaSigmaZ);
   void SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetAngularVelocity(DisFloat32 aAngularVelocityX, DisFloat32 aAngularVelocityY, DisFloat32 aAngularVelocityZ);
   void SetCentroidHeight(DisFloat32 aCentroidHeight);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;
   //! puff location is handled by the parent class point record
   DisFloat64 mOriginatingLocation[3];
   DisFloat32 mSigmaX      = 0;
   DisFloat32 mSigmaY      = 0;
   DisFloat32 mSigmaZ      = 0;
   DisFloat32 mDeltaSigmaX = 0;
   DisFloat32 mDeltaSigmaY = 0;
   DisFloat32 mDeltaSigmaZ = 0;
   DisFloat32 mOrientation[3];
   DisFloat32 mVelocity[3];
   DisFloat32 mAngularVelocity[3];
   DisFloat32 mCentroidHeight = 0;
   //! 112 Octets in this record, 14 groups of 64, no need for padding
   static const DisUint16 cMIN_LENGTH_OCTETS = 88;
};
#endif
