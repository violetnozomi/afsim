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

#ifndef DISENTITYTYPE_HPP
#define DISENTITYTYPE_HPP

#include "dis_export.h"

// An overloaded operator<<(ostream& aOut, const DisEntityType) function to write
// the contents of this class to an ostream can be found in DisIO.hpp.  This
// function is not defined here due to portability issues dealing with the
// old and new I/O stream libraries (i.e.: one typically can't use <iostream.h>
// and <iostream> in the same application).  See DisIO.hpp for more details.

#include <cstddef>
#include <iostream>

#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"

class GenI;
class GenO;

/*!
 * Specifies the kind of entity, the country of design, the domain, the specific identification of the entity, and any
 * extra information necessary for describing the entity.
 *
 * Additional information:
 * - A comprehensive list of entity type enumerations can be found in the "SISO-REF-010" (SISO) document.
 * - The values stored by this class are used to describe and entity's type both in the simulation and in a
 *   manner that can be used to look up the entity type in the SISO document.
 */
class DIS_EXPORT DisEntityType
{
public:
   DisEntityType() = default;
   DisEntityType(DisEnum8  aEntityKind,
                 DisEnum8  aDomain,
                 DisEnum16 aCountry,
                 DisEnum8  aCategory,
                 DisEnum8  aSubcategory,
                 DisEnum8  aSpecific,
                 DisEnum8  aExtra);
   DisEntityType(const DisEntityType& aSrc);
   ~DisEntityType() = default;

   enum EntityKindType
   {
      EKT_OTHER_KIND,
      EKT_PLATFORM_KIND,
      EKT_MUNITION_KIND,
      EKT_LIFEFORM_KIND,
      EKT_ENVIRONMENTAL_KIND,
      EKT_CULTURAL_KIND,
      EKT_SUPPLY_KIND,
      EKT_RADIO_KIND,
      EKT_EXPENDABLE_KIND,
      EKT_SENSOR_KIND
   };

   enum EntityDomainType
   {
      EDT_OTHER_DOMAIN,
      EDT_LAND_DOMAIN,
      EDT_AIR_DOMAIN,
      EDT_SURFACE_DOMAIN,
      EDT_SUBSURFACE_DOMAIN,
      EDT_SPACE_DOMAIN
   };

   enum MunitionKindType
   {
      MKT_OTHER,
      MKT_ANTIAIR,
      MKT_ANTIARMOR,
      MKT_ANTIGUIDED_WEAPON,
      MKT_ANTIRADAR,
      MKT_ANTISATELLITE,
      MKT_ANTISHIP,
      MKT_ANTISUBMARINE,
      MKT_ANTIPERSONNEL,
      MKT_BATTLEFIELD_SUPPORT,
      MKT_STRATEGIC,
      MKT_TACTICAL,
      MKT_DIRECTED_ENERGY_WEAPON
   };

   enum MunitionCategoryType
   {
      MCT_OTHER,
      MCT_MULTI_SPECTRAL,
      MCT_RF_ACTIVE,
      MCT_RF_PASSIVE,
      MCT_OPTICAL,
      MCT_ELECTRO_OPTICAL,
      MCT_SEISMIC,
      MCT_CHEMICAL_POINT_DETECTOR,
      MCT_CHEMICAL_STANDOFF,
      MCT_THERMAL,
      MCT_ACOUSTIC_ACTIVE,
      MCT_ACOUSTIC_PASSIVE,
      MCT_CONTACT_PRESSURE,
      MCT_ELECTRO_MAGNETIC_RADIATION,
      MCT_PARTICLE_RADIATION,
      MCT_MAGNETIC,
      MCT_GRAVITATIONAL
   };

   // Sort options
   static void SetEnumDocSortOption();
   static void SetLeftToRightSortOption();

   // Accessors

   DisEnum8  GetEntityKind() const;
   DisEnum8  GetDomain() const;
   DisEnum16 GetCountry() const;
   DisEnum8  GetCategory() const;
   DisEnum8  GetSubcategory() const;
   DisEnum8  GetSpecific() const;
   DisEnum8  GetExtra() const;

   // Mutators

   void SetEntityKind(DisEnum8 aEntityKind);
   void SetDomain(DisEnum8 aDomain);
   void SetCountry(DisEnum16 aCountry);
   void SetCategory(DisEnum8 aCategory);
   void SetSubcategory(DisEnum8 aSubcategory);
   void SetSpecific(DisEnum8 aSpecific);
   void SetExtra(DisEnum8 aExtra);

   // Input/output

   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   void        Stream(std::ostream& aStream) const;
   std::string ToString() const;

   // Data Validation
   bool IsValid() const;

   // Operators
   DisEntityType& operator=(const DisEntityType& aRhs);
   bool           operator==(const DisEntityType& aRhs) const;
   bool           operator<(const DisEntityType& aRhs) const;

   // This operator is provided so this type can be used as
   // a key in the STL and TC hash table containers
   size_t operator()(const DisEntityType& aType) const;

private:
   static unsigned int sEnumDocSort;

   /*!
    * Specifies the kind of entity described by this record.
    */
   DisEnum8 mEntityKind = 0;

   /*!
    * Specifies the operation domain for the entity (subsurface, surface, and land) excluding munition entities.
    * For munition entities: This field specifies the domain of the target (surface to air --> anti air etc).
    */
   DisEnum8 mDomain = 0;

   /*!
    * Specifies the country of the entity
    */
   DisEnum16 mCountry = 0;

   /*!
    * Specifies the main category of the entity
    */
   DisEnum8 mCategory = 0;

   /*!
    * Specifies the sub category of the entity
    */
   DisEnum8 mSubcategory = 0;

   /*!
    * Specifies the specfic information about the entity based off of the subcategory field.
    */
   DisEnum8 mSpecific = 0;

   /*!
    * Specifies any extra information required to describe the particular entity.
    */
   DisEnum8 mExtra = 0;
};

// Accessors

inline DisEnum8 DisEntityType::GetEntityKind() const
{
   return mEntityKind;
}

inline DisEnum8 DisEntityType::GetDomain() const
{
   return mDomain;
}

inline DisEnum16 DisEntityType::GetCountry() const
{
   return mCountry;
}

inline DisEnum8 DisEntityType::GetCategory() const
{
   return mCategory;
}

inline DisEnum8 DisEntityType::GetSubcategory() const
{
   return mSubcategory;
}

inline DisEnum8 DisEntityType::GetSpecific() const
{
   return mSpecific;
}

inline DisEnum8 DisEntityType::GetExtra() const
{
   return mExtra;
}

// Mutators
inline void DisEntityType::SetEntityKind(DisEnum8 aEntityKind)
{
   mEntityKind = aEntityKind;
}

inline void DisEntityType::SetDomain(DisEnum8 aDomain)
{
   mDomain = aDomain;
}

inline void DisEntityType::SetCountry(DisEnum16 aCountry)
{
   mCountry = aCountry;
}

inline void DisEntityType::SetCategory(DisEnum8 aCategory)
{
   mCategory = aCategory;
}

inline void DisEntityType::SetSubcategory(DisEnum8 aSubcategory)
{
   mSubcategory = aSubcategory;
}

inline void DisEntityType::SetSpecific(DisEnum8 aSpecific)
{
   mSpecific = aSpecific;
}

inline void DisEntityType::SetExtra(DisEnum8 aExtra)
{
   mExtra = aExtra;
}

// Inline Operators
inline bool DisEntityType::operator==(const DisEntityType& aRhs) const
{
   // The goofy order is attempt to optimize the code.
   // There should be the most variation in "Category".
   return (mCategory == aRhs.mCategory) && (mSubcategory == aRhs.mSubcategory) && (mCountry == aRhs.mCountry) &&
          (mSpecific == aRhs.mSpecific) && (mEntityKind == aRhs.mEntityKind) && (mDomain == aRhs.mDomain) &&
          (mExtra == aRhs.mExtra);
}

inline size_t DisEntityType::operator()(const DisEntityType& aType) const
{
   return static_cast<size_t>(aType.GetEntityKind() << 28 & 0xF0000000) |
          static_cast<size_t>(aType.GetDomain() << 24 & 0x0F000000) |
          static_cast<size_t>(aType.GetSpecific() << 20 & 0x00F00000) |
          static_cast<size_t>(aType.GetCountry() << 12 & 0x000FF000) |
          static_cast<size_t>(aType.GetSubcategory() << 8 & 0x00000F00) | static_cast<size_t>(aType.GetCategory());
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisEntityType& aEntityType)
{
   aEntityType.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisEntityType& aEntityType)
{
   aEntityType.Get(aGenI);
   return aGenI;
}

#endif
