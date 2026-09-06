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
#ifndef Uci__Type__PO_CapabilityCommandType_h
#define Uci__Type__PO_CapabilityCommandType_h 1

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__PO_TargetType_h)
# include "uci/type/PO_TargetType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsType_h)
# include "uci/type/PO_ComponentSettingsType.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsSettingsType_h)
# include "uci/type/PO_CollectionConstraintsSettingsType.h"
#endif

#if !defined(Uci__Type__PairIdentifierEnum_h)
# include "uci/type/PairIdentifierEnum.h"
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

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_CapabilityCommandType sequence accessor class */
      class PO_CapabilityCommandType : public virtual uci::type::CapabilityCommandBaseType {
      public:

         /** The destructor */
         virtual ~PO_CapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. PO_CapabilityCommandType
           *
           * @return This accessor's type constant, i.e. PO_CapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_CapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_CapabilityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a required output Product of the Command. Only Products consistent with Passive Optical and supported by
           * the commanded Passive Optical Capability should be requested. If omitted, no output Product should be produced. See
           * ProductTypeEnum for further details. If the associated Capability supports product generators, this element should be
           * omitted and product output should be controlled via the sibling ComponentSettings element. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandImageryType, uci::type::accessorType::productOutputCommandImageryType> ProductOutput;

         /** Returns the accessor that provides access to the complex content that is identified by the PO_Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by PO_Target.
           */
         virtual const uci::type::PO_TargetType& getPO_Target() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PO_Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by PO_Target.
           */
         virtual uci::type::PO_TargetType& getPO_Target()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PO_Target to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PO_Target
           */
         virtual void setPO_Target(const uci::type::PO_TargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentSettings.
           */
         virtual const uci::type::PO_ComponentSettingsType& getComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentSettings.
           */
         virtual uci::type::PO_ComponentSettingsType& getComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentSettings
           */
         virtual void setComponentSettings(const uci::type::PO_ComponentSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ComponentSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ComponentSettings is emabled or not
           */
         virtual bool hasComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ComponentSettings
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsType This Accessor's accessor type
           */
         virtual void enableComponentSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ComponentSettings */
         virtual void clearComponentSettings()
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


         /** Returns the value of the enumeration that is identified by the PairIdentifier.
           *
           * @return The value of the enumeration identified by PairIdentifier.
           */
         virtual const uci::type::PairIdentifierEnum& getPairIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PairIdentifier.
           *
           * @return The value of the enumeration identified by PairIdentifier.
           */
         virtual uci::type::PairIdentifierEnum& getPairIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PairIdentifier.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPairIdentifier(const uci::type::PairIdentifierEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PairIdentifier.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPairIdentifier(uci::type::PairIdentifierEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PairIdentifieris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPairIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPairIdentifier will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::pairIdentifierEnum This Accessor's accessor type
           */
         virtual void enablePairIdentifier(uci::base::accessorType::AccessorType type = uci::type::accessorType::pairIdentifierEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPairIdentifier()
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
         virtual const uci::type::PO_CapabilityCommandType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::PO_CapabilityCommandType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::PO_CapabilityCommandType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_CapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_CapabilityCommandType to copy from
           */
         PO_CapabilityCommandType(const PO_CapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_CapabilityCommandType to the contents of the
           * PO_CapabilityCommandType on the right hand side (rhs) of the assignment operator.PO_CapabilityCommandType [only
           * available to derived classes]
           *
           * @param rhs The PO_CapabilityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::PO_CapabilityCommandType
           * @return A constant reference to this PO_CapabilityCommandType.
           */
         const PO_CapabilityCommandType& operator=(const PO_CapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_CapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_CapabilityCommandType_h

