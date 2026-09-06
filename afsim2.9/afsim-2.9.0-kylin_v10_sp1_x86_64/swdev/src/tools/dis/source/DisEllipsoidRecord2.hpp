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

#ifndef DISELLIPSOIDRECORD2_HPP
#define DISELLIPSOIDRECORD2_HPP

#include "dis_export.h"

#include "DisBoundingSphereRecord.hpp"
#include "DisEllipsoidRecord.hpp"
#include "DisTypes.hpp"

/**!
 * Defines a record which represents an ellipsoid that is moving, rotating, and changing
 * in size. The origin of the ellipsoid's coordinate system is the centroid of the
 * ellipsoid. The ellipsoid's x, y, and z axes pass from the origin of the ellipsoid through
 * the semi-principle axes of the ellipsoid.
 */
class DIS_EXPORT DisEllipsoidRecord2 : public DisPointRecord
{
public:
   DisEllipsoidRecord2();
   DisEllipsoidRecord2(const DisEllipsoidRecord2& aSrc);
   DisEllipsoidRecord2(GenI& aGenI);
   ~DisEllipsoidRecord2() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisEllipsoidRecord2* Clone() const override;

   // Getters
   DisFloat32 GetSigmaX() const;
   DisFloat32 GetSigmaY() const;
   DisFloat32 GetSigmaZ() const;
   void       GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   DisFloat32 GetDeltaSigmaX() const;
   DisFloat32 GetDeltaSigmaY() const;
   DisFloat32 GetDeltaSigmaZ() const;
   void       GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void GetAngularVelocity(DisFloat32& aAngularVelocityX, DisFloat32& aAngularVelocityY, DisFloat32& aAngularVelocityZ) const;

   // Setters
   void SetSigmaX(DisFloat32 aSigmaX);
   void SetSigmaY(DisFloat32 aSigmaY);
   void SetSigmaZ(DisFloat32 aSigmaZ);
   void SetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi);
   void SetDeltaSigmaX(DisFloat32 aDeltaSigmaX);
   void SetDeltaSigmaY(DisFloat32 aDeltaSigmaY);
   void SetDeltaSigmaZ(DisFloat32 aDeltaSigmaZ);
   void SetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ);
   void SetAngularVelocity(DisFloat32& aAngularVelocityX, DisFloat32& aAngularVelocityY, DisFloat32& aAngularVelocityZ);

   // Logging
   void                Stream(std::ostream& aStream) const;
   virtual std::string ToString() const override;

private:
   void       GetMemberData(GenI& aGenI) override;
   DisFloat32 mSigmaX      = 0;
   DisFloat32 mSigmaY      = 0;
   DisFloat32 mSigmaZ      = 0;
   DisFloat32 mDeltaSigmaX = 0;
   DisFloat32 mDeltaSigmaY = 0;
   DisFloat32 mDeltaSigmaZ = 0;
   DisFloat32 mOrientation[3];
   DisFloat32 mVelocity[3];
   DisFloat32 mAngularVelocity[3];
   DisUint32  mPadding = 0;
   //! 10.5 Octets in this record, 32 bits of padding will be required
   static const DisUint16 cMIN_LENGTH_OCTETS = 64;
};
#endif
