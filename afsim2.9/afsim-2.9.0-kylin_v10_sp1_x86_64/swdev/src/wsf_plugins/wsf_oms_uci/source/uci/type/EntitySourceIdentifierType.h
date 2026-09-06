// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Type__EntitySourceIdentifierType_h
#define Uci__Type__EntitySourceIdentifierType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EOB_RecordID_Type_h)
# include "uci/type/EOB_RecordID_Type.h"
#endif

#if !defined(Uci__Type__EntityExternalType_h)
# include "uci/type/EntityExternalType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityFusionSourceType_h)
# include "uci/type/EntityFusionSourceType.h"
#endif

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

#if !defined(Uci__Type__EntityCapabilitySourceType_h)
# include "uci/type/EntityCapabilitySourceType.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

#if !defined(Uci__Type__OperatorID_Type_h)
# include "uci/type/OperatorID_Type.h"
#endif

#if !defined(Uci__Type__SOB_SatelliteRecordID_Type_h)
# include "uci/type/SOB_SatelliteRecordID_Type.h"
#endif

#if !defined(Uci__Type__SOB_C2_RecordID_Type_h)
# include "uci/type/SOB_C2_RecordID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntitySourceIdentifierType choice accessor class */
      class EntitySourceIdentifierType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntitySourceIdentifierType()
         { }

         /** Returns this accessor's type constant, i.e. EntitySourceIdentifierType
           *
           * @return This accessor's type constant, i.e. EntitySourceIdentifierType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entitySourceIdentifierType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntitySourceIdentifierType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the Entity is an output of a fusion service and provides references to a fusion input Entity that
           * fused/contributed to it. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityFusionSourceType, uci::type::accessorType::entityFusionSourceType> Fusion;

         /** Indicates the unique ID of the Product from which the Entity was derived. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> ProductMetadataID;

         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum EntitySourceIdentifierTypeChoice {
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_NONE,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_EOB_RECORDID,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_EXTERNALID,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_FUSION,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_INTERNALLYDERIVEDID,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_CAPABILITY,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_PRODUCTMETADATAID,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_OPERATORID,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_SOB_RECORDID,
            ENTITYSOURCEIDENTIFIERTYPE_CHOICE_SOB_C2_RECORDID
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual EntitySourceIdentifierTypeChoice getEntitySourceIdentifierTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getEntitySourceIdentifierTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition,
           * the access methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setEntitySourceIdentifierTypeChoiceOrdinal(EntitySourceIdentifierTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_RecordID.
           */
         virtual const uci::type::EOB_RecordID_Type& getEOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_RecordID.
           */
         virtual uci::type::EOB_RecordID_Type& getEOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EOB_RecordID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EOB_RecordID
           */
         virtual void setEOB_RecordID(const uci::type::EOB_RecordID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ExternalID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ExternalID.
           */
         virtual const uci::type::EntityExternalType& getExternalID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ExternalID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ExternalID.
           */
         virtual uci::type::EntityExternalType& getExternalID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ExternalID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ExternalID
           */
         virtual void setExternalID(const uci::type::EntityExternalType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Fusion.
           *
           * @return The bounded list identified by Fusion.
           */
         virtual const uci::type::EntitySourceIdentifierType::Fusion& getFusion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Fusion.
           *
           * @return The bounded list identified by Fusion.
           */
         virtual uci::type::EntitySourceIdentifierType::Fusion& getFusion()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Fusion.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFusion(const uci::type::EntitySourceIdentifierType::Fusion& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InternallyDerivedID.
           *
           * @return The acecssor that provides access to the complex content that is identified by InternallyDerivedID.
           */
         virtual const uci::type::ID_Type& getInternallyDerivedID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InternallyDerivedID.
           *
           * @return The acecssor that provides access to the complex content that is identified by InternallyDerivedID.
           */
         virtual uci::type::ID_Type& getInternallyDerivedID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InternallyDerivedID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InternallyDerivedID
           */
         virtual void setInternallyDerivedID(const uci::type::ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Capability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Capability.
           */
         virtual const uci::type::EntityCapabilitySourceType& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Capability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Capability.
           */
         virtual uci::type::EntityCapabilitySourceType& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Capability to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Capability
           */
         virtual void setCapability(const uci::type::EntityCapabilitySourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual const uci::type::EntitySourceIdentifierType::ProductMetadataID& getProductMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual uci::type::EntitySourceIdentifierType::ProductMetadataID& getProductMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductMetadataID(const uci::type::EntitySourceIdentifierType::ProductMetadataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorID.
           */
         virtual const uci::type::OperatorID_Type& getOperatorID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorID.
           */
         virtual uci::type::OperatorID_Type& getOperatorID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperatorID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperatorID
           */
         virtual void setOperatorID(const uci::type::OperatorID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SOB_RecordID.
           */
         virtual const uci::type::SOB_SatelliteRecordID_Type& getSOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SOB_RecordID.
           */
         virtual uci::type::SOB_SatelliteRecordID_Type& getSOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SOB_RecordID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SOB_RecordID
           */
         virtual void setSOB_RecordID(const uci::type::SOB_SatelliteRecordID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SOB_C2_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SOB_C2_RecordID.
           */
         virtual const uci::type::SOB_C2_RecordID_Type& getSOB_C2_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SOB_C2_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SOB_C2_RecordID.
           */
         virtual uci::type::SOB_C2_RecordID_Type& getSOB_C2_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SOB_C2_RecordID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SOB_C2_RecordID
           */
         virtual void setSOB_C2_RecordID(const uci::type::SOB_C2_RecordID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntitySourceIdentifierType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntitySourceIdentifierType to copy from
           */
         EntitySourceIdentifierType(const EntitySourceIdentifierType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntitySourceIdentifierType to the contents of the
           * EntitySourceIdentifierType on the right hand side (rhs) of the assignment operator.EntitySourceIdentifierType [only
           * available to derived classes]
           *
           * @param rhs The EntitySourceIdentifierType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntitySourceIdentifierType
           * @return A constant reference to this EntitySourceIdentifierType.
           */
         const EntitySourceIdentifierType& operator=(const EntitySourceIdentifierType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntitySourceIdentifierType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntitySourceIdentifierType_h

