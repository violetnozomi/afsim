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

#ifndef DISPOINTOBJECTSTATE_HPP
#define DISPOINTOBJECTSTATE_HPP

#include "dis_export.h"

#include "DisAppearance.hpp"
#include "DisEntityType.hpp"
#include "DisGeneralAppearanceRecord.hpp"
#include "DisObjectType.hpp"
#include "DisPdu.hpp"
#include "DisSimulationAddressRecord.hpp"

/**!
 * The Point Object State PDU shall communicate the addition/modification of a synthetic
 * environment object that is geometrically anchored to the terrain with a single point.
 */
class DIS_EXPORT DisPointObjectState : public DisPdu
{
public:
   DisPointObjectState();
   DisPointObjectState(const DisPointObjectState& aSrc);
   DisPointObjectState(const DisPdu& aPdu, GenI& aGenI);
   ~DisPointObjectState() = default;

   DisPointObjectState* Clone() const override;

   int GetClass() const override;
   int GetFamily() const override;

   // Input/Output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) override;

   // Getters
   const DisEntityId   GetObjectId() const;
   const DisEntityId   GetReferencedObjectId() const;
   DisUint16           GetUpdateNumber() const;
   DisEnum8            GetForceId() const;
   DisEnum8            GetModifications() const;
   const DisObjectType GetObjectType() const;
   void GetObjectLocation(DisFloat64& aObjectLocationX, DisFloat64& aObjectLocationY, DisFloat64& aObjectLocationZ) const;
   void                             GetObjectOrientation(DisFloat32& aObjectOrientationX,
                                                         DisFloat32& aObjectOrientationY,
                                                         DisFloat32& aObjectOrientationZ) const;
   const DisEntityAppearance        GetSpecificObjectAppearance() const;
   const DisGeneralAppearanceRecord GetGeneralObjectAppearance() const;
   const DisSimulationAddressRecord GetRequesterSimulationId() const;
   const DisSimulationAddressRecord GetReceivingSimulationId() const;

   // Setters
   void SetObjectId(const DisEntityId& aObjectId);
   void SetReferenceObjectId(const DisEntityId& aObjectReferenceId);
   void SetUpdateNumber(DisUint16 aUpdateNumber);
   void SetForceId(DisEnum8 aForceId);
   void SetModifications(DisEnum8 aModifications);
   void SetObjectType(const DisObjectType& aObjectType);
   void SetObjectLocation(DisFloat64 aObjectLocationX, DisFloat64 aObjectLocationY, DisFloat64 aObjectLocationZ);
   void SetObjectOrientation(DisFloat32 aObjectOrientationX, DisFloat32 aObjectOrientationY, DisFloat32 aObjectOrientationZ);
   void SetSpecificObjectAppearance(const DisEntityAppearance& aSpecificObjectAppearance);
   void SetGeneralObjectAppearance(const DisGeneralAppearanceRecord& aGeneralObjectAppearance);
   void SetRequesterSimulationId(const DisSimulationAddressRecord& aRequesterSimulationId);
   void SetReceivingSimulationId(const DisSimulationAddressRecord& aRecievingSimulationId);

   // Data Validation
   bool IsValid() const override;

   std::string         GetStringId() const override;
   void                Stream(std::ostream& aStream) const override;
   virtual std::string ToString() const override;

private:
   void GetMemberData(GenI& aGenI) override;

   //! This field shall specify the unique identification of the object in the synthetic environment.
   DisEntityId mObjectId = DisEntityId::ENTITY_ID_UNKNOWN;

   //! This field shall identify the synthetic environment object with which this point object is associated.
   DisEntityId mReferencedObjectId = DisEntityId::ENTITY_ID_UNKNOWN;

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

   //! This field shall specify the object’s physical location in the simulated world.
   DisFloat64 mObjectLocation[3];

   //! This field shall specify an object’s orientation.
   DisFloat32 mOrientation[3];

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

   DisUint16 mPadding = 0;

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

   DisUint32 mPadding2 = 0;

   /**!
    * Specifies the smallest length (in octets) of this PDU (without header or any additional records).
    */
   static const DisUint16 cPDU_MIN_LENGTH_OCTETS = 76;
};
#endif
