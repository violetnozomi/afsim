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

#ifndef DISRECTANGULARRECORD2_HPP
#define DISRECTANGULARRECORD2_HPP

#include "dis_export.h"

#include "DisPointRecord.hpp"
#include "DisRectangularRecord.hpp"

/**!
 * Defines a rectangular cuboid that is moving, rotating, and changing in size. The origin
 * of the rectangular cuboid's coordinate system is the one corner of the rectangular cuboid.
 * The x, y, and z axes pass from the origin of the rectangular cuboid along
 * three edges of the rectangles connected to the origin.
 */
class DIS_EXPORT DisRectangularRecord2 : public DisPointRecord
{
public:
   DisRectangularRecord2();
   DisRectangularRecord2(const DisRectangularRecord2& aSrc);
   DisRectangularRecord2(GenI& aGenI);
   ~DisRectangularRecord2() = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisRectangularRecord2* Clone() const override;

   // Getters
   DisFloat32 GetLengthX() const;
   DisFloat32 GetLengthY() const;
   DisFloat32 GetLengthZ() const;
   DisFloat32 GetDeltaLengthX() const;
   DisFloat32 GetDeltaLengthY() const;
   DisFloat32 GetDeltaLengthZ() const;
   void       GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   void       GetOrientation(DisFloat32& aOrientationX, DisFloat32& aOrientationY, DisFloat32& aOrientationZ) const;
   void GetAngularVelocity(DisFloat32& aAngularVelocityX, DisFloat32& aAngularVelocityY, DisFloat32& aAngularVelocityZ) const;
   // Setters
   void SetLengthX(DisFloat32 aLengthX);
   void SetLengthY(DisFloat32 aLengthY);
   void SetLengthZ(DisFloat32 aLengthZ);
   void SetDeltaLengthX(DisFloat32 aDeltaLengthX);
   void SetDeltaLengthY(DisFloat32 aDeltaLengthY);
   void SetDeltaLengthZ(DisFloat32 aDeltaLengthZ);
   void SetOrientation(DisFloat32 aOrientationX, DisFloat32 aOrientationY, DisFloat32 aOrientationZ);
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);
   void SetAngularVelocity(DisFloat32 aAngularVelocityX, DisFloat32 aAngularVelocityY, DisFloat32 aAngularVelocityZ);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;

   //! The location of the point
   //! X delta length of the rectangle
   DisFloat32 mLengthX = 0;

   //! Y delta length of the rectangle
   DisFloat32 mLengthY = 0;

   //! Z delta length of the rectangle
   DisFloat32 mLengthZ = 0;

   //! The location of the point
   //! X delta length of the rectangle
   DisFloat32 mDeltaLengthX = 0;

   //! Y delta length of the rectangle
   DisFloat32 mDeltaLengthY = 0;

   //! Z delta length of the rectangle
   DisFloat32 mDeltaLengthZ = 0;

   //! Orientation of the rectangle
   DisFloat32 mOrientation[3];

   DisFloat32 mVelocity[3];

   DisFloat32 mAngularVelocity[3];

   DisUint32 mPadding = 0;

   //! 36 Octets, 4.5 groups of 64 (in this record), this record requires a padding of 32
   static const DisUint16 cMIN_LENGTH_OCTETS = 64;
};
#endif
