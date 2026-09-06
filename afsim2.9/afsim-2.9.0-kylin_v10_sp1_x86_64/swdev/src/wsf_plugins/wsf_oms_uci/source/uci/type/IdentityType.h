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
#ifndef Uci__Type__IdentityType_h
#define Uci__Type__IdentityType_h 1

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

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EmitterIdentityType_h)
# include "uci/type/EmitterIdentityType.h"
#endif

#if !defined(Uci__Type__SpecificEmitterIdentityType_h)
# include "uci/type/SpecificEmitterIdentityType.h"
#endif

#if !defined(Uci__Type__FacilityIdentificationType_h)
# include "uci/type/FacilityIdentificationType.h"
#endif

#if !defined(Uci__Type__VehicleIdentificationType_h)
# include "uci/type/VehicleIdentificationType.h"
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

      /** This is the IdentityType sequence accessor class */
      class IdentityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~IdentityType()
         { }

         /** Returns this accessor's type constant, i.e. IdentityType
           *
           * @return This accessor's type constant, i.e. IdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::identityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an emitter type that has been identified in association with the asset. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EmitterIdentityType, uci::type::accessorType::emitterIdentityType> Emitter;

         /** Indicates a specific emitter type ***instance*** that has been identified in association with the asset. Specific
           * Emitter Identification (SEI) technology passively measures emitters, and characterizes unique emitter signatures that
           * can be used for friendly or enemy emitter identification and fingerprinting. A SEI and this SpecificEmitter element
           * refer to a specific physical instance of a type of emitter as given in the sibling EmitterType element. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificEmitterIdentityType, uci::type::accessorType::specificEmitterIdentityType> SpecificEmitter;

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


         /** Returns whether the Element that is identified by Standard exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Standard is emabled or not
           */
         virtual bool hasStandard() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Standard
           *
           * @param type = uci::type::accessorType::standardIdentityType This Accessor's accessor type
           */
         virtual void enableStandard(uci::base::accessorType::AccessorType type = uci::type::accessorType::standardIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Standard */
         virtual void clearStandard()
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


         /** Returns whether this optionalfield Environmentis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEnvironment will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::environmentEnum This Accessor's accessor type
           */
         virtual void enableEnvironment(uci::base::accessorType::AccessorType type = uci::type::accessorType::environmentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEnvironment()
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


         /** Returns whether the Element that is identified by Platform exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Platform is emabled or not
           */
         virtual bool hasPlatform() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Platform
           *
           * @param type = uci::type::accessorType::platformIdentityType This Accessor's accessor type
           */
         virtual void enablePlatform(uci::base::accessorType::AccessorType type = uci::type::accessorType::platformIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Platform */
         virtual void clearPlatform()
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


         /** Returns whether the Element that is identified by Specific exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Specific is emabled or not
           */
         virtual bool hasSpecific() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Specific
           *
           * @param type = uci::type::accessorType::specificIdentityType This Accessor's accessor type
           */
         virtual void enableSpecific(uci::base::accessorType::AccessorType type = uci::type::accessorType::specificIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Specific */
         virtual void clearSpecific()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual const uci::type::IdentityType::Emitter& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual uci::type::IdentityType::Emitter& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Emitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitter(const uci::type::IdentityType::Emitter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificEmitter.
           *
           * @return The bounded list identified by SpecificEmitter.
           */
         virtual const uci::type::IdentityType::SpecificEmitter& getSpecificEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificEmitter.
           *
           * @return The bounded list identified by SpecificEmitter.
           */
         virtual uci::type::IdentityType::SpecificEmitter& getSpecificEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpecificEmitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecificEmitter(const uci::type::IdentityType::SpecificEmitter& value)
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


         /** Returns whether the Element that is identified by SpecificFacility exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpecificFacility is emabled or not
           */
         virtual bool hasSpecificFacility() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpecificFacility
           *
           * @param type = uci::type::accessorType::facilityIdentificationType This Accessor's accessor type
           */
         virtual void enableSpecificFacility(uci::base::accessorType::AccessorType type = uci::type::accessorType::facilityIdentificationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpecificFacility */
         virtual void clearSpecificFacility()
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


         /** Returns whether the Element that is identified by SpecificVehicle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpecificVehicle is emabled or not
           */
         virtual bool hasSpecificVehicle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpecificVehicle
           *
           * @param type = uci::type::accessorType::vehicleIdentificationType This Accessor's accessor type
           */
         virtual void enableSpecificVehicle(uci::base::accessorType::AccessorType type = uci::type::accessorType::vehicleIdentificationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpecificVehicle */
         virtual void clearSpecificVehicle()
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


         /** Returns whether the Element that is identified by EOB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EOB is emabled or not
           */
         virtual bool hasEOB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EOB
           *
           * @param type = uci::type::accessorType::eOB_IdentityType This Accessor's accessor type
           */
         virtual void enableEOB(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_IdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EOB */
         virtual void clearEOB()
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


         /** Returns whether the Element that is identified by Weapon exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Weapon is emabled or not
           */
         virtual bool hasWeapon() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Weapon
           *
           * @param type = uci::type::accessorType::storeType This Accessor's accessor type
           */
         virtual void enableWeapon(uci::base::accessorType::AccessorType type = uci::type::accessorType::storeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Weapon */
         virtual void clearWeapon()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IdentityType to copy from
           */
         IdentityType(const IdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IdentityType to the contents of the IdentityType on the right hand
           * side (rhs) of the assignment operator.IdentityType [only available to derived classes]
           *
           * @param rhs The IdentityType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::IdentityType
           * @return A constant reference to this IdentityType.
           */
         const IdentityType& operator=(const IdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IdentityType_h

