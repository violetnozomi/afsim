// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__RelationshipType_h
#define Uci__Type__RelationshipType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ExternalCommandExecutionStateEnum_h)
# include "uci/type/ExternalCommandExecutionStateEnum.h"
#endif

#if !defined(Uci__Type__PairingRelationshipEnum_h)
# include "uci/type/PairingRelationshipEnum.h"
#endif

#if !defined(Uci__Type__RelationshipThreatType_h)
# include "uci/type/RelationshipThreatType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Describes the relationship between two identified objects. */
      class RelationshipType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RelationshipType()
         { }

         /** Returns this accessor's type constant, i.e. RelationshipType
           *
           * @return This accessor's type constant, i.e. RelationshipType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::relationshipType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RelationshipType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum RelationshipTypeChoice {
            RELATIONSHIPTYPE_CHOICE_NONE,
            RELATIONSHIPTYPE_CHOICE_ENGAGEMENTSTATUS,
            RELATIONSHIPTYPE_CHOICE_PAIRING,
            RELATIONSHIPTYPE_CHOICE_THREAT
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual RelationshipTypeChoice getRelationshipTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getRelationshipTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
           * access methods associated with the chosen element will be enabled and will provide access to the chosen element.
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
         virtual void setRelationshipTypeChoiceOrdinal(RelationshipTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EngagementStatus.
           *
           * @return The value of the enumeration identified by EngagementStatus.
           */
         virtual const uci::type::ExternalCommandExecutionStateEnum& getEngagementStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EngagementStatus.
           *
           * @return The value of the enumeration identified by EngagementStatus.
           */
         virtual uci::type::ExternalCommandExecutionStateEnum& getEngagementStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EngagementStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEngagementStatus(const uci::type::ExternalCommandExecutionStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EngagementStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEngagementStatus(uci::type::ExternalCommandExecutionStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Pairing.
           *
           * @return The value of the enumeration identified by Pairing.
           */
         virtual const uci::type::PairingRelationshipEnum& getPairing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Pairing.
           *
           * @return The value of the enumeration identified by Pairing.
           */
         virtual uci::type::PairingRelationshipEnum& getPairing()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Pairing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPairing(const uci::type::PairingRelationshipEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Pairing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPairing(uci::type::PairingRelationshipEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Threat.
           *
           * @return The acecssor that provides access to the complex content that is identified by Threat.
           */
         virtual const uci::type::RelationshipThreatType& getThreat() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Threat.
           *
           * @return The acecssor that provides access to the complex content that is identified by Threat.
           */
         virtual uci::type::RelationshipThreatType& getThreat()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Threat to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Threat
           */
         virtual void setThreat(const uci::type::RelationshipThreatType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RelationshipType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RelationshipType to copy from
           */
         RelationshipType(const RelationshipType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RelationshipType to the contents of the RelationshipType on the
           * right hand side (rhs) of the assignment operator.RelationshipType [only available to derived classes]
           *
           * @param rhs The RelationshipType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RelationshipType
           * @return A constant reference to this RelationshipType.
           */
         const RelationshipType& operator=(const RelationshipType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RelationshipType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RelationshipType_h

