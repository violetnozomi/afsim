//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISAREALOBJECTSTATE_HPP
#define DISAREALOBJECTSTATE_HPP

#include "dis_export.h"

#include "DisAppearance.hpp"
#include "DisEntityType.hpp"
#include "DisGeneralAppearanceRecord.hpp"
#include "DisObjectType.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"
#include "DisSimulationAddressRecord.hpp"
#include "DisWorldCoordinateRecord.hpp"

/**!
 * The Areal Object State PDU shall communicate the addition/modification of a
 * synthetic environment object that is geometrically anchored to the terrain
 * with a set of points(at least three points that come to a closure).
 */
class DIS_EXPORT DisArealObjectState : public DisPdu
{
public:
   DisArealObjectState();
   DisArealObjectState(const DisArealObjectState& aSrc);
   DisArealObjectState(const DisPdu& aPdu, GenI& aGenI);
   ~DisArealObjectState() override;

   DisArealObjectState* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Getters
   const DisEntityId                          GetObjectId() const;
   const DisEntityId                          GetReferencedObjectId() const;
   DisUint16                                  GetUpdateNumber() const;
   DisEnum8                                   GetForceId() const;
   DisEnum8                                   GetModifications() const;
   DisUint16                                  GetNumberOfPoints() const;
   const DisObjectType                        GetObjectType() const;
   const DisEntityAppearance                  GetSpecificObjectAppearance() const;
   const DisGeneralAppearanceRecord           GetGeneralObjectAppearance() const;
   const DisSimulationAddressRecord           GetRequesterSimulationId() const;
   const DisSimulationAddressRecord           GetReceivingSimulationId() const;
   DisPtrContainer<DisWorldCoordinateRecord>* GetPoints();

   // Setters
   void SetObjectId(const DisEntityId& aObjectId);
   void SetReferenceObjectId(const DisEntityId& aObjectReferenceId);
   void SetUpdateNumber(DisUint16 aUpdateNumber);
   void SetForceId(DisEnum8 aForceId);
   void SetModifications(DisEnum8 aModifications);
   void SetNumberOfPoints(DisUint16 aNumberOfPoints);
   void SetObjectType(const DisObjectType& aObjectType);
   void SetSpecificObjectAppearance(const DisEntityAppearance& aSpecificObjectAppearance);
   void SetGeneralObjectAppearance(const DisGeneralAppearanceRecord& aGeneralObjectAppearance);
   void SetRequesterSimulationId(const DisSimulationAddressRecord& aRequesterSimulationId);
   void SetReceivingSimulationId(const DisSimulationAddressRecord& aRecievingSimulationId);

   // Data Validation
   bool                IsValid() const override;
   std::string         GetStringId() const override;
   void                Stream(std::ostream& aStream) const override;
   virtual std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;

   //! This field shall specify the unique identification of the object in the synthetic environment.
   DisEntityId mObjectId = DisEntityId::ENTITY_ID_UNKNOWN;

   //! This field shall identify the synthetic environment object with which this point object is associated.
   DisEntityId mReferencedObjectId;

   /**!
    * This field shall represent the unique update number, starting with 1,
    * of each state transition of an individual object.
    */
   DisUint16 mUpdateNumber = 0;

   //! This field shall identify the force that created or modified the object.
   DisEnum8 mForceId = 0;

   /**!
    * This field shall identify whether a modification has been made to the
    * point object’s location or orientation.
    */
   DisEnum8 mModifications = 0;

   /**!
    * This field shall identify the type of synthetic environment object.
    */
   DisObjectType mObjectType;

   /**!
    * This field shall specify specific dynamic changes to an object’s
    * appearance attributes.
    */
   DisEntityAppearance mSpecificObjectAppearance;

   /**!
    * This field shall specify general dynamic changes to an object’s
    * appearance attributes.
    */
   DisGeneralAppearanceRecord mGeneralObjectAppearance;

   //! This field shall specify the total number of points making up the areal object
   DisUint16 mNumberOfPoints = 0;

   /**!
    * This field shall specify the simulation application that is sending or has sent
    * the Point Object State PDU to the Environment Manager
    */
   DisSimulationAddressRecord mRequesterSimulationId;

   /**!
    * This field shall specify the simulation application that is to receive the
    * Point Object State PDU
    */
   DisSimulationAddressRecord mReceivingSimulationId;

   //! This field shall specify the object’s physical location in the simulated world.
   DisPtrContainer<DisWorldCoordinateRecord> mPoints;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 36;
};
#endif
