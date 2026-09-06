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
#ifndef Uci__Type__EntityManagementSplitType_h
#define Uci__Type__EntityManagementSplitType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityFusionEligibilityEnum_h)
# include "uci/type/EntityFusionEligibilityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityManagementSplitType sequence accessor class */
      class EntityManagementSplitType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityManagementSplitType()
         { }

         /** Returns this accessor's type constant, i.e. EntityManagementSplitType
           *
           * @return This accessor's type constant, i.e. EntityManagementSplitType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityManagementSplitType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityManagementSplitType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of a contributing Entity which should be split (un-merged) from the Entity given by the
           * sibling SplitFrom element. If omitted, all contributors should be split from it. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> ContributorID;

         /** Returns the accessor that provides access to the complex content that is identified by the SplitFromID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SplitFromID.
           */
         virtual const uci::type::EntityID_Type& getSplitFromID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SplitFromID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SplitFromID.
           */
         virtual uci::type::EntityID_Type& getSplitFromID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SplitFromID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SplitFromID
           */
         virtual void setSplitFromID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ContributorID.
           *
           * @return The bounded list identified by ContributorID.
           */
         virtual const uci::type::EntityManagementSplitType::ContributorID& getContributorID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ContributorID.
           *
           * @return The bounded list identified by ContributorID.
           */
         virtual uci::type::EntityManagementSplitType::ContributorID& getContributorID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ContributorID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setContributorID(const uci::type::EntityManagementSplitType::ContributorID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ResultingFusionEligibility.
           *
           * @return The value of the enumeration identified by ResultingFusionEligibility.
           */
         virtual const uci::type::EntityFusionEligibilityEnum& getResultingFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ResultingFusionEligibility.
           *
           * @return The value of the enumeration identified by ResultingFusionEligibility.
           */
         virtual uci::type::EntityFusionEligibilityEnum& getResultingFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ResultingFusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setResultingFusionEligibility(const uci::type::EntityFusionEligibilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ResultingFusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setResultingFusionEligibility(uci::type::EntityFusionEligibilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityManagementSplitType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityManagementSplitType to copy from
           */
         EntityManagementSplitType(const EntityManagementSplitType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityManagementSplitType to the contents of the
           * EntityManagementSplitType on the right hand side (rhs) of the assignment operator.EntityManagementSplitType [only
           * available to derived classes]
           *
           * @param rhs The EntityManagementSplitType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntityManagementSplitType
           * @return A constant reference to this EntityManagementSplitType.
           */
         const EntityManagementSplitType& operator=(const EntityManagementSplitType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityManagementSplitType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityManagementSplitType_h

