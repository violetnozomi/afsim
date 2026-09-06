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

#ifndef DISFLARESTATERECORD_HPP
#define DISFLARESTATERECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisEntityType.hpp"
#include "DisEnvironmentalStateRecord.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/**!
 * Defines a flare state record as defined by the SISO-REF-010-2019.pdf (pg. 592)
 */
class DIS_EXPORT DisFlareStateRecord : public DisEnvironmentalStateRecord
{
public:
   DisFlareStateRecord() = default;
   DisFlareStateRecord(const DisFlareStateRecord& aSrc);
   DisFlareStateRecord(GenI& aGenI);
   ~DisFlareStateRecord() override = default;

   // Input/output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;

   // Getters
   DisUint32      GetTimeSinceCreation() const;
   DisEntityType& GetMunitionSource();
   DisUint32      GetNumberIntensity() const;
   DisUint32      GetNumberOfSources() const;
   DisUint16      GetGeometryIndex() const;

   // Setters
   void SetTimeSinceCreation(DisUint32 aTimeSinceCreation);
   void SetMunitionSource(const DisEntityType& aMunitionSource);
   void SetNumberIntensity(DisUint32 aNumberIntensity);
   void SetNumberOfSources(DisUint32 aNumberOfSources);
   void SetGeometryIndex(DisUint16 aGeometryIndex);

   // Data Validation
   bool                 IsValid() const override;
   DisFlareStateRecord* Clone() const override;

   // Logging
   std::string ToString() const override;
   void        Stream(std::ostream& aStream) const;

private:
   void                   GetMemberData(GenI& aGenI) override;
   DisUint32              mTimeSinceCreation = 0;
   DisEntityType          mMunitionSource;
   DisUint32              mNumberIntensity    = 0;
   DisUint32              mNumberOfSources    = 0;
   DisUint16              mGeometryIndex      = 0;
   DisUint16              mPadding            = 0;
   static const DisUint16 cBASE_LENGTH_OCTETS = 24;
};
#endif
