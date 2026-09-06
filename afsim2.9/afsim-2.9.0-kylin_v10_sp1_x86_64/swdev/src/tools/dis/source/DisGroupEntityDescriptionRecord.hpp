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

#ifndef DISGROUPENTITYDESCRIPTIONRECORD_HPP
#define DISGROUPENTITYDESCRIPTIONRECORD_HPP

#include "dis_export.h"

#include "DisEntityAppearance.hpp"
#include "DisEntityId.hpp"
#include "DisRecordHeader.hpp"
#include "DisTypes.hpp"
#include "GenI.hpp"

class DisRecordFactory;

/**!
 * Specifies entity state information about various categories of grouped entities within an IsGroupOf PDU
 * This is a base class for all types of entity description records
 */
class DIS_EXPORT DisGroupedEntityDescriptionRecord
{
public:
   DisGroupedEntityDescriptionRecord();
   DisGroupedEntityDescriptionRecord(const DisGroupedEntityDescriptionRecord& aDisRecord);
   DisGroupedEntityDescriptionRecord(GenI& aGenI);
   virtual ~DisGroupedEntityDescriptionRecord() = default;

   // Input/output
   virtual void      Get(GenI& aGenI);
   virtual DisUint16 GetLength();
   virtual void      Put(GenO& aGenO) const;

   // Data Validation
   virtual bool IsValid() const;
   void         HandleReadError();

   virtual DisGroupedEntityDescriptionRecord* Clone() const;

   // Logging
   virtual void        Stream(std::ostream& aStream);
   virtual std::string ToString() const;

   // Getters
   DisUint16 GetEntityIdentifier() const;
   void GetEntityLocation(DisInt16& aEntityLocationX, DisInt16& aEntityLocationY, DisInt16& aEntityLocationZ) const;
   DisUint32 GetEntityAppearance() const;
   void      GetEntityOrientation(DisInt8& aDisEntityOrienationX,
                                  DisInt8& aDisEntityOrienationY,
                                  DisInt8& aDisEntityOrienationZ) const;

   // Setters
   void SetEntityIdentifier(const DisUint16 aEntityId);
   void SetEntityLocation(DisInt16 aEntityLocationX, DisInt16 aEntityLocationY, DisInt16 aEntityLocationZ);
   void SetEntityAppearance(const DisUint32 aEntityAppearance);
   void SetEntityOrientation(DisInt8 aDisEntityOrienationX, DisInt8 aDisEntityOrienationY, DisInt8 aDisEntityOrienationZ) const;

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisGroupedEntityDescriptionRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   /**!
    * A unique identifier for an entity within a group.
    * The site identifier and application identifiers have to be the same as those desginated in the
    * Group Entity ID field.
    * NOTE: Only the entity identification component of this class is transmitted.
    */
   DisUint16 mEntityId;

   /**!
    * Location of a grouped entity is specified by three offsets. The offsets are determined by the difference
    * (meters) of the grouped entity from the group reference point
    */
   mutable DisInt16 mEntityLocation[3];

   //! The appearance of the grouped entity which is required to be identical to the DisEntityAppearance record
   DisEntityAppearance mEntityAppearance;

   /**!
    * Orientation of a grouped entity specified by three Euler angles: psi, theta, and phi.
    * Used in 25 Milliradian increments.
    */
   mutable DisInt8 mEntityOrientation[3];

   //! Local record size in octets
   static const int cRECORD_MIN_LENGTH_OCTETS = 15;
};
#endif
