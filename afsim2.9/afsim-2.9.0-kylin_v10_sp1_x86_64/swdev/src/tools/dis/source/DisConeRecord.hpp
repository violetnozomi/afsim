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

#ifndef DISCONERECORD_HPP
#define DISCONERECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisPointRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a cone 1 geometry record as specified in the SISO-REF-010-2019.pdf (pg. 592)
 */
class DIS_EXPORT DisConeRecord : public DisPointRecord
{
public:
   DisConeRecord();
   DisConeRecord(const DisConeRecord& aSrc);
   DisConeRecord(GenI& aGenI);
   ~DisConeRecord() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool IsValid() const override;

   virtual DisConeRecord* Clone() const override;

   // Getters
   void       GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const;
   DisFloat32 GetHeight() const;
   DisFloat32 GetPeakAngle() const;
   // Setters
   void SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi);
   void SetHeight(DisFloat32 aHeight);
   void SetPeakAngle(DisFloat32 aPeakAngle);

   // Logging
   void                Stream(std::ostream& aStream) const;
   virtual std::string ToString() const override;

private:
   void       GetMemberData(GenI& aGenI) override;
   DisFloat32 mOrientation[3];
   DisFloat32 mHeight    = 0;
   DisFloat32 mPeakAngle = 0;
   DisUint32  mPadding   = 0;

   //! 20 Octets, 2.5 groups of 64, 32 bits of padding are needed
   static const DisUint16 cMIN_LENGTH_OCTETS = 24;
};

#endif
