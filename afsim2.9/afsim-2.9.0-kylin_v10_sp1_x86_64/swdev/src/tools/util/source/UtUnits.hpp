// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTUNITS_HPP
#define UTUNITS_HPP

#include "ut_export.h"

#include <map>
#include <string>
#include <vector>

#include "UtMemory.hpp"

class UtUnitType;

class UT_EXPORT UtUnits
{
public:
   static void CreateInterfaces();

   //! finds the unit type by name
   //! @param aUnitType is a string representing the name of the unit type
   //! @returns a pointer to the unit type
   //! @note the valid names of the unit types are in ALL CAPS
   static UtUnitType* FindUnitType(const std::string& aUnitType);

   enum UnitType
   {
      // UtUnitTypeEnums{
      cLENGTH               = 0,
      cTIME                 = 1,
      cSPEED                = 2,
      cDATA_SIZE            = 3,
      cPOWER                = 4,
      cPOWER_DB             = 5,
      cSOLID_ANGLE          = 6,
      cMASS                 = 7,
      cFORCE                = 8,
      cTORQUE               = 9,
      cAREA                 = 10,
      cAREA_DB              = 11,
      cVOLUME               = 12,
      cTIME2                = 13,
      cACCELERATION         = 14,
      cFREQUENCY            = 15,
      cANGLE                = 16,
      cANGULAR_RATE         = 17,
      cANGULAR_ACCELERATION = 18,
      cDATA_RATE            = 19,
      cMASS_DENSITY         = 20,
      cMASS_TRANSFER        = 21,
      cENERGY               = 22,
      cFLUENCE              = 23,
      cIRRADIANCE           = 24,
      cSPECTRAL_IRRADIANCE  = 25,
      cRADIANCE             = 26,
      cSPECTRAL_RADIANCE    = 27,
      cRATIO                = 28,
      cNOISE_PRESSURE       = 29,
      cPRESSURE             = 30,
      cTEMPERATURE          = 31,
      cSPECIFIC_RANGE       = 32,
      cANGULAR_INERTIA      = 33,
      cVOLTAGE              = 34,
      cCURRENT              = 35,
      cCURRENT_DENSITY      = 36,
      cRESISTANCE           = 37,
      cCAPACITANCE          = 38,
      cIMPULSE              = 39,
      cRESPONSIVITY         = 40
      // UtUnitTypeEnums}
      ,
      _cUNIT_TYPE_COUNT
   };

   //! gets the unit type by identifier
   //! @param aTypeId is an enumeration representing the ID of the unit type
   //! @return a pointer to the unit type
   static UtUnitType* GetUnitType(UnitType aTypeId) { return mUnitTypes[aTypeId].get(); }

   //! the list of unit types
   static std::vector<std::unique_ptr<UtUnitType>> mUnitTypes;
};

//! A common interface all unit types support.
//! @note Each unit type is given a class with static functions for manipulating that unit type.
class UtUnitType
{
public:
   virtual ~UtUnitType() noexcept = default;

   virtual std::string                       GetName() const                                     = 0;
   virtual int                               GetUnitTypeId() const                               = 0;
   virtual int                               GetStandardUnitId() const                           = 0;
   virtual bool                              IsCompoundUnit() const                              = 0;
   virtual bool                              IsMultidimensional() const                          = 0;
   virtual double                            ConvertToStandard(double aValue, int aUnit) const   = 0;
   virtual double                            ConvertFromStandard(double aValue, int aUnit) const = 0;
   virtual std::string                       FindUnitName(int aUnitId) const                     = 0;
   virtual int                               ReadUnit(const std::string& aUnitName) const        = 0;
   virtual bool                              IsUnitValid(int aUnitId) const                      = 0;
   virtual bool                              IsUnitValid(const std::string& aUnitName) const     = 0;
   virtual const std::map<int, std::string>& GetUnitToString() const                             = 0;
   virtual const std::map<std::string, int>& GetStringToUnit() const                             = 0;
};
#endif
