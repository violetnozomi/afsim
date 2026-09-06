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
#ifndef Uci__Type__PO_CommandResponseType_h
#define Uci__Type__PO_CommandResponseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_CapabilityEnum_h)
# include "uci/type/PO_CapabilityEnum.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsSettingsType_h)
# include "uci/type/PO_CollectionConstraintsSettingsType.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductOutputCommandImageryType_h)
# include "uci/type/ProductOutputCommandImageryType.h"
#endif

#if !defined(Uci__Type__TaskAllocationConstraintType_h)
# include "uci/type/TaskAllocationConstraintType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_CommandResponseType sequence accessor class */
      class PO_CommandResponseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_CommandResponseType()
         { }

         /** Returns this accessor's type constant, i.e. PO_CommandResponseType
           *
           * @return This accessor's type constant, i.e. PO_CommandResponseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_CommandResponseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_CommandResponseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a required output Product of the Command. Only Products consistent with Passive Optical and supported by
           * the commanded Passive Optical Capability should be requested. If omitted, no output Product should be produced. See
           * ProductTypeEnum for further details. If the associated Capability supports product generators, this element should be
           * omitted and product output should be controlled via the sibling ComponentSettings element. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandImageryType, uci::type::accessorType::productOutputCommandImageryType> ProductOutput;

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
         virtual const uci::type::PO_CapabilityEnum& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Capability.
           *
           * @return The value of the enumeration identified by Capability.
           */
         virtual uci::type::PO_CapabilityEnum& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Capability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapability(const uci::type::PO_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Capability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapability(uci::type::PO_CapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual const uci::type::PO_CollectionConstraintsSettingsType& getCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual uci::type::PO_CollectionConstraintsSettingsType& getCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraints
           */
         virtual void setCollectionConstraints(const uci::type::PO_CollectionConstraintsSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraints is emabled or not
           */
         virtual bool hasCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraints
           *
           * @param type = uci::type::accessorType::pO_CollectionConstraintsSettingsType This Accessor's accessor type
           */
         virtual void enableCollectionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_CollectionConstraintsSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionConstraints */
         virtual void clearCollectionConstraints()
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


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual const uci::type::PO_CommandResponseType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::PO_CommandResponseType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::PO_CommandResponseType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PreferredAllocation.
           *
           * @return The bounded list identified by PreferredAllocation.
           */
         virtual const uci::type::PO_CommandResponseType::PreferredAllocation& getPreferredAllocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PreferredAllocation.
           *
           * @return The bounded list identified by PreferredAllocation.
           */
         virtual uci::type::PO_CommandResponseType::PreferredAllocation& getPreferredAllocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PreferredAllocation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPreferredAllocation(const uci::type::PO_CommandResponseType::PreferredAllocation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_CommandResponseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_CommandResponseType to copy from
           */
         PO_CommandResponseType(const PO_CommandResponseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_CommandResponseType to the contents of the
           * PO_CommandResponseType on the right hand side (rhs) of the assignment operator.PO_CommandResponseType [only available
           * to derived classes]
           *
           * @param rhs The PO_CommandResponseType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::PO_CommandResponseType
           * @return A constant reference to this PO_CommandResponseType.
           */
         const PO_CommandResponseType& operator=(const PO_CommandResponseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_CommandResponseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_CommandResponseType_h

