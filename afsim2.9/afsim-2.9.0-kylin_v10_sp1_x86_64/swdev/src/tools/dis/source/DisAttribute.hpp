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

// DIS Attribute PDU

#ifndef DIS_DIS_ATTRIBUTE_HPP
#define DIS_DIS_ATTRIBUTE_HPP

#include "dis_export.h"

#include "DisAttributeRecordSet.hpp"
#include "DisPdu.hpp"
#include "DisPtrContainer.hpp"

class GenI;
class GenO;

//! Contains information about individual attributes for a particular entity, object, or event
class DIS_EXPORT DisAttribute : public DisPdu
{
public:
   DisAttribute();
   DisAttribute(DisPdu& aPdu, GenI& aGenI);
   DisAttribute(DisPdu& aPdu);
   DisAttribute(GenI& aGenI);
   ~DisAttribute() override = default;

   // Allow compiler to build copy constructor and operator=
   // DisAttribute(const DisAttribute& aSrc);
   // DisAttribute& operator=(const DisAttribute& aRhs);

   int GetClass() const override;

   // All derived classes need to able to tell who sent this PDU
   // to the best of their ability
   const DisEntityId& GetOriginatingEntity() const override;

   // Accessors

   DisUint16 GetOriginatingSimulationSite() const;
   DisUint16 GetOriginatingSimulationApp() const;
   DisEnum8  GetAttributeRecordPduType() const;
   DisEnum8  GetAttributeRecordProtocolVersion() const;
   DisEnum32 GetMasterAttributeRecordType() const;
   DisEnum8  GetActionCode() const;
   DisEnum16 GetAttributeRecordSetCount() const;

   // Mutators

   void SetOriginatingSimulationSite(const DisUint16 aOriginatingSimulationSite);
   void SetOriginatingSimulationApp(const DisUint16 aOriginatingSimulationApp);
   void SetAttributeRecordPduType(const DisEnum8 aAttributeRecordPduType);
   void SetAttributeRecordProtocolVersion(const DisEnum8 aAttributeRecordProtocolVersion);
   void SetMasterAttributeRecordType(const DisEnum32 aMasterAttributeRecordType);
   void SetActionCode(const DisEnum8 aActionCode);

   // Input/Output

   DisUint16   GetLength() override;
   void        Get(GenI& aGenI) override;
   void        Put(GenO& aGenO) override;
   void        Stream(std::ostream& aStream) const override;
   std::string ToString() const override;

   // Data Validation
   bool IsValid() const override;

   // AttributeRecordSet list maintenance...

   // WARNING: AddAttributeRecordSet takes ownership of the passed object.
   void AddAttributeRecordSet(DisAttributeRecordSet* aAttributeRecordSet);

   // Return iterators to DisAttributeRecordSet vector.
   std::vector<DisAttributeRecordSet*>::iterator       GetAttributeRecordSetBegin();
   std::vector<DisAttributeRecordSet*>::const_iterator GetAttributeRecordSetBegin() const;
   std::vector<DisAttributeRecordSet*>::iterator       GetAttributeRecordSetEnd();
   std::vector<DisAttributeRecordSet*>::const_iterator GetAttributeRecordSetEnd() const;

   // Remove and delete a DisAttributeRecordSet object from the list.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first AttributeRecordSet on the list.
   unsigned int RemoveAttributeRecordSet(DisAttributeRecordSet* aAttributeRecordSet);

   // Remove and delete all DisAttributeRecordSet objects from the list.
   void RemoveAllAttributeRecordSets();

   // Remove DisAttributeRecordSet object from the list, but DO NOT DELETE the object.
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes the first AttributeRecordSet on the list.
   unsigned int RemoveAttributeRecordSetWithNoDelete(DisAttributeRecordSet* aAttributeRecordSet);

   // Remove all DisAttributeRecordSet objects from the list, but DO NOT DELETE them.
   void RemoveAllAttributeRecordSetsWithNoDelete();

   // Enumerations

   // Symbolic names defined in IEEE 1278.1-200X - Draft 14 Delta, Sec 6.1.7.
   enum MasterAttributeRecordType
   {
      MULTIPLES_PRESENT = 0 // Indicates that multiple different attribute record types are present in the same PDU
   };

   enum ActionCodeType
   {
      NotUsed             = 0,
      ExtendingAnotherPDU = 1,
      PartialStateUpdate  = 2
   };

   enum StateIndicatorType
   {
      StateUpdate       = 0,
      ChangedDataUpdate = 1,
      Ceased            = 2,
      NotTransmitting = 99 // Not defined in the DIS standard, but used to facilitate sending the one last "ceased" record
   };

protected:
   static const DisUint16 sSizeOfFixedPortion;
   mutable DisEntityId    mOriginatingEntity;

   virtual void GetMemberData(GenI& aGenI);
   void         UpdateAttributeRecordSetCount() const;

   /// The Originating Simulation Address is used for the Site and App only, not the Entity/Object Id
   /*!
    * Identifies the originating simulation issuing the PDU
    */
   DisUint16 mOriginatingSimulationSite;
   /*!
    * Identifies the originating simulation application issuing the PDU
    */
   DisUint16 mOriginatingSimulationApp;
   /*!
    * Padding
    */
   DisUint32 mPadding1;
   /*!
    * Padding
    */
   DisUint16 mPadding2;
   /*!
    * Represents the type of PDU that is being extended or updated (if applicable)
    */
   DisEnum8 mAttributeRecordPduType;
   /*!
    * Indicates the protocol version associated with the attribute record
    */
   DisEnum8 mAttributeRecordProtocolVersion;
   /*!
    * Contains the attribute record type of the PDU
    */
   DisEnum32 mMasterAttributeRecordType;
   /*!
    * Identifies the action code applicable to this PDU
    */
   DisEnum8 mActionCode;
   /*!
    * Padding
    */
   DisUint8 mPadding3;
   /*!
    * The number of attribute record sets
    */
   mutable DisUint16 mNumberOfAttributeRecordSets;
   /*!
    * The list of attribute records associated with this PDU
    */
   DisPtrContainer<DisAttributeRecordSet> mAttributeRecordSetList;
};

inline void DisAttribute::UpdateAttributeRecordSetCount() const
{
   mNumberOfAttributeRecordSets = static_cast<DisUint16>(mAttributeRecordSetList.GetSize());
}

// Accessors

inline DisUint16 DisAttribute::GetOriginatingSimulationSite() const
{
   return mOriginatingSimulationSite;
}

inline DisUint16 DisAttribute::GetOriginatingSimulationApp() const
{
   return mOriginatingSimulationApp;
}

inline DisEnum8 DisAttribute::GetAttributeRecordPduType() const
{
   return mAttributeRecordPduType;
}

inline DisEnum8 DisAttribute::GetAttributeRecordProtocolVersion() const
{
   return mAttributeRecordProtocolVersion;
}

inline DisEnum32 DisAttribute::GetMasterAttributeRecordType() const
{
   return mMasterAttributeRecordType;
}

inline DisEnum8 DisAttribute::GetActionCode() const
{
   return mActionCode;
}

inline DisUint16 DisAttribute::GetAttributeRecordSetCount() const
{
   UpdateAttributeRecordSetCount();
   return mNumberOfAttributeRecordSets;
}

// Mutators

inline void DisAttribute::SetOriginatingSimulationSite(const DisUint16 aOriginatingSimulationSite)
{
   mOriginatingSimulationSite = aOriginatingSimulationSite;
}

inline void DisAttribute::SetOriginatingSimulationApp(const DisUint16 aOriginatingSimulationApp)
{
   mOriginatingSimulationApp = aOriginatingSimulationApp;
}

inline void DisAttribute::SetAttributeRecordPduType(const DisEnum8 aAttributeRecordPduType)
{
   mAttributeRecordPduType = aAttributeRecordPduType;
}

inline void DisAttribute::SetAttributeRecordProtocolVersion(const DisEnum8 aAttributeRecordProtocolVersion)
{
   mAttributeRecordProtocolVersion = aAttributeRecordProtocolVersion;
}

inline void DisAttribute::SetMasterAttributeRecordType(const DisEnum32 aMasterAttributeRecordType)
{
   mMasterAttributeRecordType = aMasterAttributeRecordType;
}

inline void DisAttribute::SetActionCode(const DisEnum8 aActionCode)
{
   mActionCode = aActionCode;
}

// Attribute Record Set list methods

inline void DisAttribute::AddAttributeRecordSet(DisAttributeRecordSet* aAttributeRecordSet)
{
   mAttributeRecordSetList.Add(aAttributeRecordSet);
}

inline std::vector<DisAttributeRecordSet*>::iterator DisAttribute::GetAttributeRecordSetBegin()
{
   return mAttributeRecordSetList.GetBegin();
}

inline std::vector<DisAttributeRecordSet*>::const_iterator DisAttribute::GetAttributeRecordSetBegin() const
{
   return mAttributeRecordSetList.GetBegin();
}

inline std::vector<DisAttributeRecordSet*>::iterator DisAttribute::GetAttributeRecordSetEnd()
{
   return mAttributeRecordSetList.GetEnd();
}

inline std::vector<DisAttributeRecordSet*>::const_iterator DisAttribute::GetAttributeRecordSetEnd() const
{
   return mAttributeRecordSetList.GetEnd();
}

inline unsigned int DisAttribute::RemoveAttributeRecordSet(DisAttributeRecordSet* aAttributeRecordSet)
{
   return mAttributeRecordSetList.Remove(aAttributeRecordSet);
}

inline void DisAttribute::RemoveAllAttributeRecordSets()
{
   mAttributeRecordSetList.RemoveAll();
}

inline unsigned int DisAttribute::RemoveAttributeRecordSetWithNoDelete(DisAttributeRecordSet* aAttributeRecordSet)
{
   return mAttributeRecordSetList.RemoveWithNoDelete(aAttributeRecordSet);
}

inline void DisAttribute::RemoveAllAttributeRecordSetsWithNoDelete()
{
   mAttributeRecordSetList.RemoveAllWithNoDelete();
}


#endif
