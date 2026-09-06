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

#ifndef DISPOINTRECORD2_HPP
#define DISPOINTRECORD2_HPP

#include "dis_export.h"

#include "DisPointRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

//! Defines a moving point in space.
class DIS_EXPORT DisPointRecord2 : public DisPointRecord
{
public:
   DisPointRecord2();
   DisPointRecord2(const DisPointRecord2& aSrc);
   DisPointRecord2(GenI& aGenI);
   ~DisPointRecord2() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisPointRecord2* Clone() const override;

   // Getters
   void GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const;
   // Setters
   void SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;

   //! The location of the point
   DisFloat32 mVelocity[3];

   /**
    * The padding for this field handles the 64 bit padding
    * requirement specified by the Environmental Record in the IEEE 1278.1-2012 (DIS).pdf document
    */
   DisUint32 mPadding = 0;

   //! Beacuse of this record only contains .5 of a 64 bit size, the get and put will have to take this into account
   static const DisUint16 cMIN_LENGTH_OCTETS = 16;
};
#endif
