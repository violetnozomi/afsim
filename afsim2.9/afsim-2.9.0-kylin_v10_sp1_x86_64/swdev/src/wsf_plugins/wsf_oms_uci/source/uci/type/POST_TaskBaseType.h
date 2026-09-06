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
#ifndef Uci__Type__POST_TaskBaseType_h
#define Uci__Type__POST_TaskBaseType_h 1

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

#if !defined(Uci__Type__LocatedEllipseType_h)
# include "uci/type/LocatedEllipseType.h"
#endif

#if !defined(Uci__Type__NIIRS_Type_h)
# include "uci/type/NIIRS_Type.h"
#endif

#if !defined(Uci__Type__OpticalCollectionConstraintsType_h)
# include "uci/type/OpticalCollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__PairIdentifierEnum_h)
# include "uci/type/PairIdentifierEnum.h"
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

      /** This is the POST_TaskBaseType sequence accessor class */
      class POST_TaskBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_TaskBaseType()
         { }

         /** Returns this accessor's type constant, i.e. POST_TaskBaseType
           *
           * @return This accessor's type constant, i.e. POST_TaskBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_TaskBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_TaskBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the spectrum that will be used for the POST Collection. [Minimum occurrences: 0] [Maximum occurrences: 14] */
         typedef uci::base::BoundedList<uci::type::POST_SubCapabilityEnum, uci::type::accessorType::pOST_SubCapabilityEnum> SubCapability;

         /** Indicates a desired output Product of the Task. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::POST_ProductOutputCommandImageryType, uci::type::accessorType::pOST_ProductOutputCommandImageryType> Output;

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
         virtual const uci::type::POST_TaskBaseType::SubCapability& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual uci::type::POST_TaskBaseType::SubCapability& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapability(const uci::type::POST_TaskBaseType::SubCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LookAtCoords.
           *
           * @return The acecssor that provides access to the complex content that is identified by LookAtCoords.
           */
         virtual const uci::type::LocatedEllipseType& getLookAtCoords() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LookAtCoords.
           *
           * @return The acecssor that provides access to the complex content that is identified by LookAtCoords.
           */
         virtual uci::type::LocatedEllipseType& getLookAtCoords()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LookAtCoords to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LookAtCoords
           */
         virtual void setLookAtCoords(const uci::type::LocatedEllipseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LookAtCoords exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LookAtCoords is emabled or not
           */
         virtual bool hasLookAtCoords() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LookAtCoords
           *
           * @param type = uci::type::accessorType::locatedEllipseType This Accessor's accessor type
           */
         virtual void enableLookAtCoords(uci::base::accessorType::AccessorType type = uci::type::accessorType::locatedEllipseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LookAtCoords */
         virtual void clearLookAtCoords()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumNIIRS.
           *
           * @return The value of the simple primitive data type identified by MinimumNIIRS.
           */
         virtual uci::type::NIIRS_TypeValue getMinimumNIIRS() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumNIIRS.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumNIIRS(uci::type::NIIRS_TypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumNIIRS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumNIIRS is emabled or not
           */
         virtual bool hasMinimumNIIRS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumNIIRS
           *
           * @param type = uci::type::accessorType::nIIRS_Type This Accessor's accessor type
           */
         virtual void enableMinimumNIIRS(uci::base::accessorType::AccessorType type = uci::type::accessorType::nIIRS_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumNIIRS */
         virtual void clearMinimumNIIRS()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DesiredNIIRS.
           *
           * @return The value of the simple primitive data type identified by DesiredNIIRS.
           */
         virtual uci::type::NIIRS_TypeValue getDesiredNIIRS() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DesiredNIIRS.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDesiredNIIRS(uci::type::NIIRS_TypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DesiredNIIRS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DesiredNIIRS is emabled or not
           */
         virtual bool hasDesiredNIIRS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DesiredNIIRS
           *
           * @param type = uci::type::accessorType::nIIRS_Type This Accessor's accessor type
           */
         virtual void enableDesiredNIIRS(uci::base::accessorType::AccessorType type = uci::type::accessorType::nIIRS_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DesiredNIIRS */
         virtual void clearDesiredNIIRS()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual const uci::type::OpticalCollectionConstraintsType& getCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraints.
           */
         virtual uci::type::OpticalCollectionConstraintsType& getCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraints
           */
         virtual void setCollectionConstraints(const uci::type::OpticalCollectionConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraints is emabled or not
           */
         virtual bool hasCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraints
           *
           * @param type = uci::type::accessorType::opticalCollectionConstraintsType This Accessor's accessor type
           */
         virtual void enableCollectionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::opticalCollectionConstraintsType)
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


         /** Returns the bounded list that is identified by the Output.
           *
           * @return The bounded list identified by Output.
           */
         virtual const uci::type::POST_TaskBaseType::Output& getOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Output.
           *
           * @return The bounded list identified by Output.
           */
         virtual uci::type::POST_TaskBaseType::Output& getOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Output.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOutput(const uci::type::POST_TaskBaseType::Output& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_TaskBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_TaskBaseType to copy from
           */
         POST_TaskBaseType(const POST_TaskBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_TaskBaseType to the contents of the POST_TaskBaseType on the
           * right hand side (rhs) of the assignment operator.POST_TaskBaseType [only available to derived classes]
           *
           * @param rhs The POST_TaskBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::POST_TaskBaseType
           * @return A constant reference to this POST_TaskBaseType.
           */
         const POST_TaskBaseType& operator=(const POST_TaskBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_TaskBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_TaskBaseType_h

