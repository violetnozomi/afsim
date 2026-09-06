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

#ifndef DISRECTANGULARRECORD_HPP
#define DISRECTANGULARRECORD_HPP

#include "dis_export.h"

#include "DisPointRecord.hpp"

/**!
 * Defines a rectangular cuboid with origin of the rectangular cuboid's coordiante
 * system being one corcer of the rectangular cuboid. The x, y, and z axes pass
 * from the origin of the rectangular cuboid along three edges of the rectangles
 * to the origin
 */
class DIS_EXPORT DisRectangularRecord : public DisPointRecord
{
public:
   DisRectangularRecord();
   DisRectangularRecord(const DisRectangularRecord& aSrc);
   DisRectangularRecord(GenI& aGenI);
   ~DisRectangularRecord() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisRectangularRecord* Clone() const override;

   // Getters
   DisFloat32 GetLengthX() const;
   DisFloat32 GetLengthY() const;
   DisFloat32 GetLengthZ() const;
   void       GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   // Setters
   void SetLengthX(DisFloat32 aLengthX);
   void SetLengthY(DisFloat32 aLengthY);
   void SetLengthZ(DisFloat32 aLengthZ);
   void SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);

   // Logging
   void                Stream(std::ostream& aStream) const;
   virtual std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;

   //! The location of the point
   //! X length of the rectangle
   DisFloat32 mLengthX = 0;

   //! Y length of the rectangle
   DisFloat32 mLengthY = 0;

   //! Z length of the rectangle
   DisFloat32 mLengthZ = 0;

   //! Orientation of the rectangle
   DisFloat32 mOrientation[3];

   static const DisUint16 cMIN_LENGTH_OCTETS = 24;
};
#endif
