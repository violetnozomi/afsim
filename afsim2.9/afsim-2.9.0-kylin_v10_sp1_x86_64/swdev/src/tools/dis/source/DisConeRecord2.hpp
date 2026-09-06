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

#ifndef DISCONERECORD2_HPP
#define DISCONERECORD2_HPP

#include "dis_export.h"

#include <iostream>

#include "DisConeRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a cone 2 geometry record as specified in the SISO-REF-010-2019.pdf (pg. 592)
 */
class DIS_EXPORT DisConeRecord2 : public DisPointRecord
{
public:
   DisConeRecord2();
   DisConeRecord2(const DisConeRecord2& aSrc);
   DisConeRecord2(GenI& aGenI);
   ~DisConeRecord2() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool            IsValid() const override;
   virtual DisConeRecord2* Clone() const override;

   // Getters
   void       GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   DisFloat32 GetHeight() const;
   DisFloat32 GetPeakAngle() const;
   void       GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void GetAngularVelocity(DisFloat32& aAngularVelocityX, DisFloat32& aAngularVelocityY, DisFloat32& aAngularVelocityZ) const;
   DisFloat32 GetDeltaHeight() const;
   DisFloat32 GetDeltaPeakAngle() const;
   // Setters
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetAngularVelocity(DisFloat32 aAngularVelocityX, DisFloat32 aAngularVelocityY, DisFloat32 aAngularVelocityZ);
   void SetDeltaHeight(DisFloat32 aDeltaHeight);
   void SetDeltaPeakAngle(DisFloat32 aPeakAngle);
   void SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);
   void SetHeight(DisFloat32 aHeight);
   void SetPeakAngle(DisFloat32 aPeakAngle);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void       GetMemberData(GenI& aGenI) override;
   DisFloat32 mOrientation[3];
   DisFloat32 mHeight    = 0;
   DisFloat32 mPeakAngle = 0;
   DisUint32  mPadding   = 0;
   DisFloat32 mVelocity[3];
   DisFloat32 mAngularVelocity[3];
   DisFloat32 mDeltaHeight    = 0;
   DisFloat32 mDeltaPeakAngle = 0;

   //! 24 + 52 = 76 Octets = 6.5 groups of 64, 32 bits of padding are needed
   static const DisUint16 cMIN_LENGTH_OCTETS = 56;
};
#endif
