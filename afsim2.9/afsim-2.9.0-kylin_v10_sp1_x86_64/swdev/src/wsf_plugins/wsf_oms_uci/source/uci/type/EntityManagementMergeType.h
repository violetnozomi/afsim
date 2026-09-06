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
#ifndef Uci__Type__EntityManagementMergeType_h
#define Uci__Type__EntityManagementMergeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__EntityFusionSimpleEligibilityEnum_h)
# include "uci/type/EntityFusionSimpleEligibilityEnum.h"
#endif

#if !defined(Uci__Type__MergeEnum_h)
# include "uci/type/MergeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityManagementMergeType sequence accessor class */
      class EntityManagementMergeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityManagementMergeType()
         { }

         /** Returns this accessor's type constant, i.e. EntityManagementMergeType
           *
           * @return This accessor's type constant, i.e. EntityManagementMergeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityManagementMergeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityManagementMergeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of an Entity to be merged and become a contributor to another Entity. Upon successful merging
           * the Entity will be removed. If the sibling MergeTo element is omitted, at least two instances of this element must be
           * provided. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> ContributorID;

         /** Returns the bounded list that is identified by the ContributorID.
           *
           * @return The bounded list identified by ContributorID.
           */
         virtual const uci::type::EntityManagementMergeType::ContributorID& getContributorID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ContributorID.
           *
           * @return The bounded list identified by ContributorID.
           */
         virtual uci::type::EntityManagementMergeType::ContributorID& getContributorID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ContributorID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setContributorID(const uci::type::EntityManagementMergeType::ContributorID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MergeToID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MergeToID.
           */
         virtual const uci::type::EntityID_Type& getMergeToID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MergeToID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MergeToID.
           */
         virtual uci::type::EntityID_Type& getMergeToID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MergeToID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MergeToID
           */
         virtual void setMergeToID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MergeToID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MergeToID is emabled or not
           */
         virtual bool hasMergeToID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MergeToID
           *
           * @param type = uci::type::accessorType::entityID_Type This Accessor's accessor type
           */
         virtual void enableMergeToID(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MergeToID */
         virtual void clearMergeToID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ResultingFusionEligibility.
           *
           * @return The value of the enumeration identified by ResultingFusionEligibility.
           */
         virtual const uci::type::EntityFusionSimpleEligibilityEnum& getResultingFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ResultingFusionEligibility.
           *
           * @return The value of the enumeration identified by ResultingFusionEligibility.
           */
         virtual uci::type::EntityFusionSimpleEligibilityEnum& getResultingFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ResultingFusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setResultingFusionEligibility(const uci::type::EntityFusionSimpleEligibilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ResultingFusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setResultingFusionEligibility(uci::type::EntityFusionSimpleEligibilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MergeCharacteristics.
           *
           * @return The value of the enumeration identified by MergeCharacteristics.
           */
         virtual const uci::type::MergeEnum& getMergeCharacteristics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MergeCharacteristics.
           *
           * @return The value of the enumeration identified by MergeCharacteristics.
           */
         virtual uci::type::MergeEnum& getMergeCharacteristics()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MergeCharacteristics.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMergeCharacteristics(const uci::type::MergeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MergeCharacteristics.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMergeCharacteristics(uci::type::MergeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield MergeCharacteristicsis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasMergeCharacteristics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getMergeCharacteristics will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::mergeEnum This Accessor's accessor type
           */
         virtual void enableMergeCharacteristics(uci::base::accessorType::AccessorType type = uci::type::accessorType::mergeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearMergeCharacteristics()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityManagementMergeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityManagementMergeType to copy from
           */
         EntityManagementMergeType(const EntityManagementMergeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityManagementMergeType to the contents of the
           * EntityManagementMergeType on the right hand side (rhs) of the assignment operator.EntityManagementMergeType [only
           * available to derived classes]
           *
           * @param rhs The EntityManagementMergeType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntityManagementMergeType
           * @return A constant reference to this EntityManagementMergeType.
           */
         const EntityManagementMergeType& operator=(const EntityManagementMergeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityManagementMergeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityManagementMergeType_h

