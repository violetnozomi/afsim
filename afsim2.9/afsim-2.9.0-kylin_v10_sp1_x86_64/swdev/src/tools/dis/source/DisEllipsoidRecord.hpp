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

#ifndef DISELLIPSOIDRECORD_HPP
#define DISELLIPSOIDRECORD_HPP

#include "dis_export.h"

#include "DisBoundingSphereRecord.hpp"
#include "DisPointRecord.hpp"
#include "DisTypes.hpp"

/**!
 * Defines a record which represents an ellipsoid with origin of the ellipsoid's coordinate
 * system being the centroid of the ellipsoid. The ellipsoid's x, y, and z axes pass from
 * the origin of the ellipsoid through the semi-principle axes of the ellipsoid.
 */
class DIS_EXPORT DisEllipsoidRecord : public DisPointRecord
{
public:
   DisEllipsoidRecord();
   DisEllipsoidRecord(const DisEllipsoidRecord& aSrc);
   DisEllipsoidRecord(GenI& aGenI);
   virtual ~DisEllipsoidRecord() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisEllipsoidRecord* Clone() const override;

   // Getters
   DisFloat32 GetSigmaX() const;
   DisFloat32 GetSigmaY() const;
   DisFloat32 GetSigmaZ() const;
   void       GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   // Setters
   void SetSigmaX(DisFloat32 aSigmaX);
   void SetSigmaY(DisFloat32 aSigmaY);
   void SetSigmaZ(DisFloat32 aSigmaZ);
   void SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);

   // Logging
   void                Stream(std::ostream& aStream) const;
   virtual std::string ToString() const override;

private:
   void       GetMemberData(GenI& aGenI) override;
   DisFloat32 mSigmaX = 0;
   DisFloat32 mSigmaY = 0;
   DisFloat32 mSigmaZ = 0;
   DisFloat32 mOrientation[3];

   //! 12 Octets - 6 groups of 64, no padding needed
   static const DisUint16 cMIN_LENGTH_OCTETS = 24;
};
#endif
