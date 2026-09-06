//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef DISSPHERERECORD2_HPP
#define DISSPHERERECORD2_HPP

#include "dis_export.h"

#include "DisPointRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a sphere that is moving, rotating, and changing in size. The origin
 * of the sphere's coordinate system is the centroid of the sphere
 */
class DIS_EXPORT DisSphereRecord2 : public DisPointRecord
{
public:
   DisSphereRecord2();
   DisSphereRecord2(const DisSphereRecord2& aSrc);
   DisSphereRecord2(GenI& aGenI);
   ~DisSphereRecord2() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool              IsValid() const override;
   virtual DisSphereRecord2* Clone() const override;

   // Getters
   DisFloat32 GetRadius() const;
   DisFloat32 GetDeltaRadius() const;
   void       GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void GetAngularVelocity(DisFloat32& aAngularVelocityX, DisFloat32& aAngularVelocityY, DisFloat32& aAngularVelocityZ) const;
   // Setters
   void SetRadius(DisFloat32 aRadius);
   void SetDeltaRadius(DisFloat32 aDeltaRadius);
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetAngularVelocity(DisFloat32 aAngularVelocityX, DisFloat32 aAngularVelocityY, DisFloat32 aAngularVelocityZ);

   // Logging
   virtual void        Stream(std::ostream& aStream);
   virtual std::string ToString();

private:
   void GetMemberData(GenI& aGenI) override;

   //! The radius of the bounding sphere
   DisFloat32 mRadius = 0;

   //! The change of the sphere's radius
   DisFloat32 mDeltaRadius = 0;

   //! Velocity of the sphere
   DisFloat32 mVelocity[3];

   //! Angular velocity of the sphere
   DisFloat32 mAngularVelocity[3];

   static const DisUint16 cBASE_LENGTH_OCTETS = 32;
};
#endif
