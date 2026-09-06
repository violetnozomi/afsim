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
#ifndef Uci__Type__AMTI_TaskBaseType_h
#define Uci__Type__AMTI_TaskBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AMTI_CapabilityEnum_h)
# include "uci/type/AMTI_CapabilityEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AMTI_SubCapabilityEnum_h)
# include "uci/type/AMTI_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__CollectionPolicyEnum_h)
# include "uci/type/CollectionPolicyEnum.h"
#endif

#if !defined(Uci__Type__AMTI_CollectionConstraintsType_h)
# include "uci/type/AMTI_CollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Type__ProductOutputCommandBasicType_h)
# include "uci/type/ProductOutputCommandBasicType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AMTI_TaskBaseType sequence accessor class */
      class AMTI_TaskBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AMTI_TaskBaseType()
         { }

         /** Returns this accessor's type constant, i.e. AMTI_TaskBaseType
           *
           * @return This accessor's type constant, i.e. AMTI_TaskBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aMTI_TaskBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AMTI_TaskBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a SubCapability of the AMTI Capability; the second tier in the taxonomy of AMTI. For AMTI, the second tier
           * is waveform category. See enumeration annotations for further details. [Minimum occurrences: 0] [Maximum occurrences:
           * 3]
           */
         typedef uci::base::BoundedList<uci::type::AMTI_SubCapabilityEnum, uci::type::accessorType::aMTI_SubCapabilityEnum> SubCapabilityType;

         /** Indicates a desired output Product of the Task. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandBasicType, uci::type::accessorType::productOutputCommandBasicType> Output;

         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::AMTI_CapabilityEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::AMTI_CapabilityEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::AMTI_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::AMTI_CapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityType.
           *
           * @return The bounded list identified by SubCapabilityType.
           */
         virtual const uci::type::AMTI_TaskBaseType::SubCapabilityType& getSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityType.
           *
           * @return The bounded list identified by SubCapabilityType.
           */
         virtual uci::type::AMTI_TaskBaseType::SubCapabilityType& getSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapabilityType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapabilityType(const uci::type::AMTI_TaskBaseType::SubCapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CollectionPolicy.
           *
           * @return The value of the enumeration identified by CollectionPolicy.
           */
         virtual const uci::type::CollectionPolicyEnum& getCollectionPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CollectionPolicy.
           *
           * @return The value of the enumeration identified by CollectionPolicy.
           */
         virtual uci::type::CollectionPolicyEnum& getCollectionPolicy()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CollectionPolicy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCollectionPolicy(const uci::type::CollectionPolicyEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CollectionPolicy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCollectionPolicy(uci::type::CollectionPolicyEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield CollectionPolicyis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasCollectionPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getCollectionPolicy will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::collectionPolicyEnum This Accessor's accessor type
           */
         virtual void enableCollectionPolicy(uci::base::accessorType::AccessorType type = uci::type::accessorType::collectionPolicyEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCollectionPolicy()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual const uci::type::AMTI_CollectionConstraintsType& getCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual uci::type::AMTI_CollectionConstraintsType& getCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraints
           */
         virtual void setCollectionConstraints(const uci::type::AMTI_CollectionConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraints is emabled or not
           */
         virtual bool hasCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraints
           *
           * @param type = uci::type::accessorType::aMTI_CollectionConstraintsType This Accessor's accessor type
           */
         virtual void enableCollectionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::aMTI_CollectionConstraintsType)
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


         /** Returns the bounded list that is identified by the Output.
           *
           * @return The bounded list identified by Output.
           */
         virtual const uci::type::AMTI_TaskBaseType::Output& getOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Output.
           *
           * @return The bounded list identified by Output.
           */
         virtual uci::type::AMTI_TaskBaseType::Output& getOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Output.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOutput(const uci::type::AMTI_TaskBaseType::Output& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AMTI_TaskBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AMTI_TaskBaseType to copy from
           */
         AMTI_TaskBaseType(const AMTI_TaskBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AMTI_TaskBaseType to the contents of the AMTI_TaskBaseType on the
           * right hand side (rhs) of the assignment operator.AMTI_TaskBaseType [only available to derived classes]
           *
           * @param rhs The AMTI_TaskBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AMTI_TaskBaseType
           * @return A constant reference to this AMTI_TaskBaseType.
           */
         const AMTI_TaskBaseType& operator=(const AMTI_TaskBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AMTI_TaskBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AMTI_TaskBaseType_h

