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

#ifndef DISSIMULATIONADDRESSRECORD_HPP
#define DISSIMULATIONADDRESSRECORD_HPP

#include "dis_export.h"

#include <iostream>

#include "DisEntityId.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

/*!
 * Defines a simulation's designation associated with all object identifiers except those
 * contained in Live Entity PDUs.
 */
class DIS_EXPORT DisSimulationAddressRecord
{
public:
   DisSimulationAddressRecord() = default;
   DisSimulationAddressRecord(DisUint16 aSiteNumber, DisUint16 aApplicationNumber);
   DisSimulationAddressRecord(GenI& aGenI);
   DisSimulationAddressRecord(const DisSimulationAddressRecord& aSrc);
   ~DisSimulationAddressRecord() = default;

   // Input/output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   DisSimulationAddressRecord* Clone() const;

   // Logging
   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Getters
   DisUint16 GetSiteNumber() const;
   DisUint16 GetApplicationNumber() const;

   // Setters
   void SetSiteNumber(DisUint16 aSiteNumber);
   void SetApplicationNumber(DisUint16 aApplicationNumber);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisSimulationAddressRecord& aRhs) const;
   bool operator!=(const DisSimulationAddressRecord& aRhs) const;
   bool operator<(const DisSimulationAddressRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   /*!
    * A site is defined as a facility, an installation, an organizational unit, or a geographic location that has one or
    * more simulation applications capable of participating in a distributed event.A facility, an installation, an
    * organizational unit, or a geographic location may have multiple sites associated with it.
    * Each site
    */
   DisUint16 mSiteNumber = 0;

   /*!
    * An application is defined as a software program that is used to generate and process distributed simulation
    * data including live, virtual, and constructive data. An application is the second component of simulation
    * address. The application will issue and/or receive PDUs in the course of an event. Each application
    * participating in an event (e.g., training exercise) shall be assigned a unique Application Number for the site
    * with which the application is associated. See also Serial Simulations
    */
   DisUint16 mApplicationNumber = 0;

   static const int cLOCAL_RECORD_SIZE_IN_OCTETS = 4;
};
#endif
