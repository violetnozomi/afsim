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
#ifndef Uci__Type__EntityManagementSetFusionEligibilityType_h
#define Uci__Type__EntityManagementSetFusionEligibilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__EntityFusionEligibilityEnum_h)
# include "uci/type/EntityFusionEligibilityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityManagementSetFusionEligibilityType sequence accessor class */
      class EntityManagementSetFusionEligibilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityManagementSetFusionEligibilityType()
         { }

         /** Returns this accessor's type constant, i.e. EntityManagementSetFusionEligibilityType
           *
           * @return This accessor's type constant, i.e. EntityManagementSetFusionEligibilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityManagementSetFusionEligibilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityManagementSetFusionEligibilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of the Entity whose fusion eligibility is being changed. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> EntityID;

         /** Indicates the unique ID of an Entity which is excluded or included for future automated fusion operations with the
           * Entity given in the sibling EntityID element. This element is required when the sibling FusionEligibility indicates
           * SELECTIVE_EXCLUSION or SELECTIVE_INCLUSION and should be omitted otherwise. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> CorrelatedEntityID;

         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual const uci::type::EntityManagementSetFusionEligibilityType::EntityID& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityID.
           *
           * @return The bounded list identified by EntityID.
           */
         virtual uci::type::EntityManagementSetFusionEligibilityType::EntityID& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEntityID(const uci::type::EntityManagementSetFusionEligibilityType::EntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FusionEligibility.
           *
           * @return The value of the enumeration identified by FusionEligibility.
           */
         virtual const uci::type::EntityFusionEligibilityEnum& getFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FusionEligibility.
           *
           * @return The value of the enumeration identified by FusionEligibility.
           */
         virtual uci::type::EntityFusionEligibilityEnum& getFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFusionEligibility(const uci::type::EntityFusionEligibilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFusionEligibility(uci::type::EntityFusionEligibilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FusionEligibilityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFusionEligibility will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::entityFusionEligibilityEnum This Accessor's accessor type
           */
         virtual void enableFusionEligibility(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityFusionEligibilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelatedEntityID.
           *
           * @return The bounded list identified by CorrelatedEntityID.
           */
         virtual const uci::type::EntityManagementSetFusionEligibilityType::CorrelatedEntityID& getCorrelatedEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelatedEntityID.
           *
           * @return The bounded list identified by CorrelatedEntityID.
           */
         virtual uci::type::EntityManagementSetFusionEligibilityType::CorrelatedEntityID& getCorrelatedEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CorrelatedEntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCorrelatedEntityID(const uci::type::EntityManagementSetFusionEligibilityType::CorrelatedEntityID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityManagementSetFusionEligibilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityManagementSetFusionEligibilityType to copy from
           */
         EntityManagementSetFusionEligibilityType(const EntityManagementSetFusionEligibilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityManagementSetFusionEligibilityType to the contents of the
           * EntityManagementSetFusionEligibilityType on the right hand side (rhs) of the assignment
           * operator.EntityManagementSetFusionEligibilityType [only available to derived classes]
           *
           * @param rhs The EntityManagementSetFusionEligibilityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::EntityManagementSetFusionEligibilityType
           * @return A constant reference to this EntityManagementSetFusionEligibilityType.
           */
         const EntityManagementSetFusionEligibilityType& operator=(const EntityManagementSetFusionEligibilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityManagementSetFusionEligibilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityManagementSetFusionEligibilityType_h

