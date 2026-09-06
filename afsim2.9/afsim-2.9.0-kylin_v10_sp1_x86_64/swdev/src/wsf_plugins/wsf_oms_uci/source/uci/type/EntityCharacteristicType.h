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
#ifndef Uci__Type__EntityCharacteristicType_h
#define Uci__Type__EntityCharacteristicType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__IdentityComparisonType_h)
# include "uci/type/IdentityComparisonType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityCharacteristicType choice accessor class */
      class EntityCharacteristicType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityCharacteristicType()
         { }

         /** Returns this accessor's type constant, i.e. EntityCharacteristicType
           *
           * @return This accessor's type constant, i.e. EntityCharacteristicType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityCharacteristicType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityCharacteristicType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum EntityCharacteristicTypeChoice {
            ENTITYCHARACTERISTICTYPE_CHOICE_NONE,
            ENTITYCHARACTERISTICTYPE_CHOICE_IDENTITY,
            ENTITYCHARACTERISTICTYPE_CHOICE_IDENTITYSTALENESS,
            ENTITYCHARACTERISTICTYPE_CHOICE_POSITIONUNCERTAINTY,
            ENTITYCHARACTERISTICTYPE_CHOICE_POSITIONSTALENESS,
            ENTITYCHARACTERISTICTYPE_CHOICE_TTPL_THRESHOLD
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual EntityCharacteristicTypeChoice getEntityCharacteristicTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getEntityCharacteristicTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition,
           * the access methods associated with the chosen element will be enabled and will provide access to the chosen element.
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
         virtual void setEntityCharacteristicTypeChoiceOrdinal(EntityCharacteristicTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::IdentityComparisonType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::IdentityComparisonType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::IdentityComparisonType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IdentityStaleness.
           *
           * @return The value of the simple primitive data type identified by IdentityStaleness.
           */
         virtual uci::type::DurationTypeValue getIdentityStaleness() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IdentityStaleness.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIdentityStaleness(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PositionUncertainty.
           *
           * @return The value of the simple primitive data type identified by PositionUncertainty.
           */
         virtual xs::Float getPositionUncertainty() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PositionUncertainty.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPositionUncertainty(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PositionStaleness.
           *
           * @return The value of the simple primitive data type identified by PositionStaleness.
           */
         virtual uci::type::DurationTypeValue getPositionStaleness() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PositionStaleness.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPositionStaleness(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TTPL_Threshold.
           *
           * @return The acecssor that provides access to the complex content that is identified by TTPL_Threshold.
           */
         virtual const uci::type::RankingType& getTTPL_Threshold() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TTPL_Threshold.
           *
           * @return The acecssor that provides access to the complex content that is identified by TTPL_Threshold.
           */
         virtual uci::type::RankingType& getTTPL_Threshold()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TTPL_Threshold to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TTPL_Threshold
           */
         virtual void setTTPL_Threshold(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityCharacteristicType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityCharacteristicType to copy from
           */
         EntityCharacteristicType(const EntityCharacteristicType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityCharacteristicType to the contents of the
           * EntityCharacteristicType on the right hand side (rhs) of the assignment operator.EntityCharacteristicType [only
           * available to derived classes]
           *
           * @param rhs The EntityCharacteristicType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntityCharacteristicType
           * @return A constant reference to this EntityCharacteristicType.
           */
         const EntityCharacteristicType& operator=(const EntityCharacteristicType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityCharacteristicType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityCharacteristicType_h

