// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AO_CommandResponseType_h
#define Uci__Type__AO_CommandResponseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AO_CapabilityEnum_h)
# include "uci/type/AO_CapabilityEnum.h"
#endif

#if !defined(Uci__Type__OpticalCollectionConstraintsType_h)
# include "uci/type/OpticalCollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TaskAllocationConstraintType_h)
# include "uci/type/TaskAllocationConstraintType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_CommandResponseType sequence accessor class */
      class AO_CommandResponseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AO_CommandResponseType()
         { }

         /** Returns this accessor's type constant, i.e. AO_CommandResponseType
           *
           * @return This accessor's type constant, i.e. AO_CommandResponseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_CommandResponseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_CommandResponseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the System and optionally Capability that this task should be allocated to during the allocation process.
           * However, allocation to another System is acceptable. If multiple preferred Systems are given: 1) allocation to any
           * one of the Systems is the minimum expectation, 2) decomposition of the Task into multiple child Tasks allocated
           * across the multiple preferred Systems is acceptable, 3) there is no expectation that each preferred System must
           * perform the Task. This element allows the Task creator to suggest an allocation to a specific System at the risk of a
           * less than optimal overall solution. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TaskAllocationConstraintType, uci::type::accessorType::taskAllocationConstraintType> PreferredAllocation;

         /** Returns the value of the enumeration that is identified by the Capability.
           *
           * @return The value of the enumeration identified by Capability.
           */
         virtual const uci::type::AO_CapabilityEnum& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Capability.
           *
           * @return The value of the enumeration identified by Capability.
           */
         virtual uci::type::AO_CapabilityEnum& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Capability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapability(const uci::type::AO_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Capability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapability(uci::type::AO_CapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmissionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmissionConstraints.
           */
         virtual const uci::type::OpticalCollectionConstraintsType& getEmissionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmissionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmissionConstraints.
           */
         virtual uci::type::OpticalCollectionConstraintsType& getEmissionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmissionConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmissionConstraints
           */
         virtual void setEmissionConstraints(const uci::type::OpticalCollectionConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmissionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmissionConstraints is emabled or not
           */
         virtual bool hasEmissionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmissionConstraints
           *
           * @param type = uci::type::accessorType::opticalCollectionConstraintsType This Accessor's accessor type
           */
         virtual void enableEmissionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::opticalCollectionConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmissionConstraints */
         virtual void clearEmissionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual const uci::type::CommandRepetitionType& getRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual uci::type::CommandRepetitionType& getRepetition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Repetition to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Repetition
           */
         virtual void setRepetition(const uci::type::CommandRepetitionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Repetition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Repetition is emabled or not
           */
         virtual bool hasRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Repetition
           *
           * @param type = uci::type::accessorType::commandRepetitionType This Accessor's accessor type
           */
         virtual void enableRepetition(uci::base::accessorType::AccessorType type = uci::type::accessorType::commandRepetitionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Repetition */
         virtual void clearRepetition()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PreferredAllocation.
           *
           * @return The bounded list identified by PreferredAllocation.
           */
         virtual const uci::type::AO_CommandResponseType::PreferredAllocation& getPreferredAllocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PreferredAllocation.
           *
           * @return The bounded list identified by PreferredAllocation.
           */
         virtual uci::type::AO_CommandResponseType::PreferredAllocation& getPreferredAllocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PreferredAllocation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPreferredAllocation(const uci::type::AO_CommandResponseType::PreferredAllocation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_CommandResponseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_CommandResponseType to copy from
           */
         AO_CommandResponseType(const AO_CommandResponseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_CommandResponseType to the contents of the
           * AO_CommandResponseType on the right hand side (rhs) of the assignment operator.AO_CommandResponseType [only available
           * to derived classes]
           *
           * @param rhs The AO_CommandResponseType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AO_CommandResponseType
           * @return A constant reference to this AO_CommandResponseType.
           */
         const AO_CommandResponseType& operator=(const AO_CommandResponseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_CommandResponseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_CommandResponseType_h

