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

#ifndef DISBOUNDINGSPHERERECORD_HPP
#define DISBOUNDINGSPHERERECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisPointRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines the following two records: Bounding Sphere Record 1 and Regular sphere Sphere Record
 * as defined in the following documents:
 *    - 1278.1-2012 (DIS).pdf
 *    - SISO-REF-010-2019-Enumerations v26.pdf
 *    - PCR240_Transfer_Environment_Records_VerA.docx
 */
class DIS_EXPORT DisBoundingSphereRecord : public DisPointRecord
{
public:
   DisBoundingSphereRecord();
   DisBoundingSphereRecord(const DisBoundingSphereRecord& aSrc);
   DisBoundingSphereRecord(GenI& aGenI);
   virtual ~DisBoundingSphereRecord() override = default;

   // Input/output
   virtual void      Get(GenI& aGenI) override;
   virtual DisUint16 GetLength() override;
   virtual void      Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool                     IsValid() const override;
   virtual DisBoundingSphereRecord* Clone() const override;

   // Getters
   DisFloat32 GetRadius() const;

   // Setters
   void SetRadius(DisFloat32 aRadius);

   // Logging
   virtual std::string ToString() const override;
   void                Stream(std::ostream& aStream) const;

private:
   void GetMemberData(GenI& aGenI) override;

   //! The radius of the bounding sphere
   DisFloat32             mRadius             = 0;
   DisUint32              mPadding            = 0;
   static const DisUint16 cBASE_LENGTH_OCTETS = 8;
};
#endif
