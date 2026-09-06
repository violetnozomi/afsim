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
#ifndef Uci__Type__EntityIdentityChoiceType_h
#define Uci__Type__EntityIdentityChoiceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__StandardIdentityType_h)
# include "uci/type/StandardIdentityType.h"
#endif

#if !defined(Uci__Type__EnvironmentEnum_h)
# include "uci/type/EnvironmentEnum.h"
#endif

#if !defined(Uci__Type__PlatformIdentityType_h)
# include "uci/type/PlatformIdentityType.h"
#endif

#if !defined(Uci__Type__SpecificIdentityType_h)
# include "uci/type/SpecificIdentityType.h"
#endif

#if !defined(Uci__Type__EmitterIdentityType_h)
# include "uci/type/EmitterIdentityType.h"
#endif

#if !defined(Uci__Type__SpecificEmitterIdentityType_h)
# include "uci/type/SpecificEmitterIdentityType.h"
#endif

#if !defined(Uci__Type__VehicleIdentificationType_h)
# include "uci/type/VehicleIdentificationType.h"
#endif

#if !defined(Uci__Type__FacilityIdentificationType_h)
# include "uci/type/FacilityIdentificationType.h"
#endif

#if !defined(Uci__Type__EOB_IdentityType_h)
# include "uci/type/EOB_IdentityType.h"
#endif

#if !defined(Uci__Type__StoreType_h)
# include "uci/type/StoreType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityIdentityChoiceType choice accessor class */
      class EntityIdentityChoiceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityIdentityChoiceType()
         { }

         /** Returns this accessor's type constant, i.e. EntityIdentityChoiceType
           *
           * @return This accessor's type constant, i.e. EntityIdentityChoiceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityIdentityChoiceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityIdentityChoiceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum EntityIdentityChoiceTypeChoice {
            ENTITYIDENTITYCHOICETYPE_CHOICE_NONE,
            ENTITYIDENTITYCHOICETYPE_CHOICE_STANDARD,
            ENTITYIDENTITYCHOICETYPE_CHOICE_ENVIRONMENT,
            ENTITYIDENTITYCHOICETYPE_CHOICE_PLATFORM,
            ENTITYIDENTITYCHOICETYPE_CHOICE_SPECIFIC,
            ENTITYIDENTITYCHOICETYPE_CHOICE_EMITTER,
            ENTITYIDENTITYCHOICETYPE_CHOICE_SPECIFICEMITTER,
            ENTITYIDENTITYCHOICETYPE_CHOICE_SPECIFICVEHICLE,
            ENTITYIDENTITYCHOICETYPE_CHOICE_SPECIFICFACILITY,
            ENTITYIDENTITYCHOICETYPE_CHOICE_EOB,
            ENTITYIDENTITYCHOICETYPE_CHOICE_WEAPON
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual EntityIdentityChoiceTypeChoice getEntityIdentityChoiceTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getEntityIdentityChoiceTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition,
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
         virtual void setEntityIdentityChoiceTypeChoiceOrdinal(EntityIdentityChoiceTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Standard.
           *
           * @return The acecssor that provides access to the complex content that is identified by Standard.
           */
         virtual const uci::type::StandardIdentityType& getStandard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Standard.
           *
           * @return The acecssor that provides access to the complex content that is identified by Standard.
           */
         virtual uci::type::StandardIdentityType& getStandard()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Standard to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Standard
           */
         virtual void setStandard(const uci::type::StandardIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Environment.
           *
           * @return The value of the enumeration identified by Environment.
           */
         virtual const uci::type::EnvironmentEnum& getEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Environment.
           *
           * @return The value of the enumeration identified by Environment.
           */
         virtual uci::type::EnvironmentEnum& getEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Environment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEnvironment(const uci::type::EnvironmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Environment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEnvironment(uci::type::EnvironmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Platform.
           *
           * @return The acecssor that provides access to the complex content that is identified by Platform.
           */
         virtual const uci::type::PlatformIdentityType& getPlatform() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Platform.
           *
           * @return The acecssor that provides access to the complex content that is identified by Platform.
           */
         virtual uci::type::PlatformIdentityType& getPlatform()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Platform to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Platform
           */
         virtual void setPlatform(const uci::type::PlatformIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Specific.
           *
           * @return The acecssor that provides access to the complex content that is identified by Specific.
           */
         virtual const uci::type::SpecificIdentityType& getSpecific() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Specific.
           *
           * @return The acecssor that provides access to the complex content that is identified by Specific.
           */
         virtual uci::type::SpecificIdentityType& getSpecific()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Specific to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Specific
           */
         virtual void setSpecific(const uci::type::SpecificIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emitter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emitter.
           */
         virtual const uci::type::EmitterIdentityType& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emitter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emitter.
           */
         virtual uci::type::EmitterIdentityType& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Emitter to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Emitter
           */
         virtual void setEmitter(const uci::type::EmitterIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SpecificEmitter.
           *
           * @return The acecssor that provides access to the complex content that is identified by SpecificEmitter.
           */
         virtual const uci::type::SpecificEmitterIdentityType& getSpecificEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SpecificEmitter.
           *
           * @return The acecssor that provides access to the complex content that is identified by SpecificEmitter.
           */
         virtual uci::type::SpecificEmitterIdentityType& getSpecificEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SpecificEmitter to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SpecificEmitter
           */
         virtual void setSpecificEmitter(const uci::type::SpecificEmitterIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SpecificVehicle.
           *
           * @return The acecssor that provides access to the complex content that is identified by SpecificVehicle.
           */
         virtual const uci::type::VehicleIdentificationType& getSpecificVehicle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SpecificVehicle.
           *
           * @return The acecssor that provides access to the complex content that is identified by SpecificVehicle.
           */
         virtual uci::type::VehicleIdentificationType& getSpecificVehicle()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SpecificVehicle to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SpecificVehicle
           */
         virtual void setSpecificVehicle(const uci::type::VehicleIdentificationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SpecificFacility.
           *
           * @return The acecssor that provides access to the complex content that is identified by SpecificFacility.
           */
         virtual const uci::type::FacilityIdentificationType& getSpecificFacility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SpecificFacility.
           *
           * @return The acecssor that provides access to the complex content that is identified by SpecificFacility.
           */
         virtual uci::type::FacilityIdentificationType& getSpecificFacility()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SpecificFacility to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SpecificFacility
           */
         virtual void setSpecificFacility(const uci::type::FacilityIdentificationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB.
           */
         virtual const uci::type::EOB_IdentityType& getEOB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB.
           */
         virtual uci::type::EOB_IdentityType& getEOB()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EOB to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EOB
           */
         virtual void setEOB(const uci::type::EOB_IdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Weapon.
           *
           * @return The acecssor that provides access to the complex content that is identified by Weapon.
           */
         virtual const uci::type::StoreType& getWeapon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Weapon.
           *
           * @return The acecssor that provides access to the complex content that is identified by Weapon.
           */
         virtual uci::type::StoreType& getWeapon()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Weapon to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Weapon
           */
         virtual void setWeapon(const uci::type::StoreType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityIdentityChoiceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityIdentityChoiceType to copy from
           */
         EntityIdentityChoiceType(const EntityIdentityChoiceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityIdentityChoiceType to the contents of the
           * EntityIdentityChoiceType on the right hand side (rhs) of the assignment operator.EntityIdentityChoiceType [only
           * available to derived classes]
           *
           * @param rhs The EntityIdentityChoiceType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EntityIdentityChoiceType
           * @return A constant reference to this EntityIdentityChoiceType.
           */
         const EntityIdentityChoiceType& operator=(const EntityIdentityChoiceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityIdentityChoiceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityIdentityChoiceType_h

