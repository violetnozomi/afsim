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
#ifndef Uci__Type__POST_CommandResponseType_h
#define Uci__Type__POST_CommandResponseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__POST_CapabilityEnum_h)
# include "uci/type/POST_CapabilityEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_SubCapabilityEnum_h)
# include "uci/type/POST_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsSettingsType_h)
# include "uci/type/PO_CollectionConstraintsSettingsType.h"
#endif

#if !defined(Uci__Type__POST_CommandRepetitionType_h)
# include "uci/type/POST_CommandRepetitionType.h"
#endif

#if !defined(Uci__Type__POST_ProductOutputCommandImageryType_h)
# include "uci/type/POST_ProductOutputCommandImageryType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_CommandResponseType sequence accessor class */
      class POST_CommandResponseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_CommandResponseType()
         { }

         /** Returns this accessor's type constant, i.e. POST_CommandResponseType
           *
           * @return This accessor's type constant, i.e. POST_CommandResponseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_CommandResponseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_CommandResponseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the frequency band or spectrum that is used to search, track, or generate products with the POST subsystem.
           * [Minimum occurrences: 0] [Maximum occurrences: 14]
           */
         typedef uci::base::BoundedList<uci::type::POST_SubCapabilityEnum, uci::type::accessorType::pOST_SubCapabilityEnum> SubCapability;

         /** Indicates a required output Product of the Command. Only Products consistent with Passive Optical Search and Track
           * and supported by the commanded Passive Optical Search and Track Capability should be requested. If omitted, no output
           * Product should be produced. See ProductTypeEnum for further details. If the associated Capability supports product
           * generators, this element should be omitted and product output should be controlled via the sibling ComponentSettings
           * element. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ProductOutputCommandImageryType, uci::type::accessorType::pOST_ProductOutputCommandImageryType> ProductOutput;

         /** Returns the value of the enumeration that is identified by the Capability.
           *
           * @return The value of the enumeration identified by Capability.
           */
         virtual const uci::type::POST_CapabilityEnum& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Capability.
           *
           * @return The value of the enumeration identified by Capability.
           */
         virtual uci::type::POST_CapabilityEnum& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Capability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapability(const uci::type::POST_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Capability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapability(uci::type::POST_CapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual const uci::type::POST_CommandResponseType::SubCapability& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual uci::type::POST_CommandResponseType::SubCapability& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapability(const uci::type::POST_CommandResponseType::SubCapability& value)
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
         virtual const uci::type::POST_CommandRepetitionType& getRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual uci::type::POST_CommandRepetitionType& getRepetition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Repetition to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Repetition
           */
         virtual void setRepetition(const uci::type::POST_CommandRepetitionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Repetition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Repetition is emabled or not
           */
         virtual bool hasRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Repetition
           *
           * @param type = uci::type::accessorType::pOST_CommandRepetitionType This Accessor's accessor type
           */
         virtual void enableRepetition(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_CommandRepetitionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Repetition */
         virtual void clearRepetition()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual const uci::type::POST_CommandResponseType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::POST_CommandResponseType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::POST_CommandResponseType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_CommandResponseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_CommandResponseType to copy from
           */
         POST_CommandResponseType(const POST_CommandResponseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_CommandResponseType to the contents of the
           * POST_CommandResponseType on the right hand side (rhs) of the assignment operator.POST_CommandResponseType [only
           * available to derived classes]
           *
           * @param rhs The POST_CommandResponseType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::POST_CommandResponseType
           * @return A constant reference to this POST_CommandResponseType.
           */
         const POST_CommandResponseType& operator=(const POST_CommandResponseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_CommandResponseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_CommandResponseType_h

