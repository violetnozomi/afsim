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
#ifndef Uci__Type__IdentityConfidenceType_h
#define Uci__Type__IdentityConfidenceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__StandardIdentityConfidenceType_h)
# include "uci/type/StandardIdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__EnvironmentIdentityConfidenceType_h)
# include "uci/type/EnvironmentIdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__PlatformIdentityConfidenceType_h)
# include "uci/type/PlatformIdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__SpecificIdentityConfidenceType_h)
# include "uci/type/SpecificIdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__EmitterMultipleType_h)
# include "uci/type/EmitterMultipleType.h"
#endif

#if !defined(Uci__Type__SpecificEmitterMultipleType_h)
# include "uci/type/SpecificEmitterMultipleType.h"
#endif

#if !defined(Uci__Type__SpecificVehicleIdentityConfidenceType_h)
# include "uci/type/SpecificVehicleIdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__SpecificFacilityIdentityConfidenceType_h)
# include "uci/type/SpecificFacilityIdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__EOB_IdentityConfidenceType_h)
# include "uci/type/EOB_IdentityConfidenceType.h"
#endif

#if !defined(Uci__Type__StoreConfidenceType_h)
# include "uci/type/StoreConfidenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the IdentityConfidenceType sequence accessor class */
      class IdentityConfidenceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~IdentityConfidenceType()
         { }

         /** Returns this accessor's type constant, i.e. IdentityConfidenceType
           *
           * @return This accessor's type constant, i.e. IdentityConfidenceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::identityConfidenceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IdentityConfidenceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a possible "standard identity" of the Entity. The value of the child Confidence element of all instances of
           * this element should sum to 100%. If multiple instances are given, each must be of a different standard identity as
           * indicated by the child element. [Minimum occurrences: 0] [Maximum occurrences: 8]
           */
         typedef uci::base::BoundedList<uci::type::StandardIdentityConfidenceType, uci::type::accessorType::standardIdentityConfidenceType> Standard;

         /** Indicates a possible "environment" of the Entity. The value of the child Confidence element of all instances of this
           * element should sum to 100%. If multiple instances are given, each must be of a different environment type as
           * indicated by the child element. [Minimum occurrences: 0] [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::EnvironmentIdentityConfidenceType, uci::type::accessorType::environmentIdentityConfidenceType> Environment;

         /** Indicates a possible "platform identity" of the Entity. The value of the child Confidence element of all instances of
           * this element should sum to 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PlatformIdentityConfidenceType, uci::type::accessorType::platformIdentityConfidenceType> Platform;

         /** Indicates a possible "specific identity" of the Entity. The value of the child Confidence element of all instances of
           * this element should sum to 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificIdentityConfidenceType, uci::type::accessorType::specificIdentityConfidenceType> Specific;

         /** Indicates an emitter that has been identified in association with the Entity. An Entity can have multiple emitters
           * each of which are represented by one instance of this element. Each emitter can in turn have an ambiguity list of
           * emitter types indicated by descendant elements. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EmitterMultipleType, uci::type::accessorType::emitterMultipleType> Emitter;

         /** Indicates a specific emitter instance that has been identified in association with the Entity. An Entity can have
           * multiple emitters and therefore multiple specific emitters. Each specific emitter can in turn have an ambiguity list
           * of specific emitter instance possibilities as indicated by descendant elements. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificEmitterMultipleType, uci::type::accessorType::specificEmitterMultipleType> SpecificEmitter;

         /** Indicates a possible specific vehicle instance, generally a blue forces vehicle, that has been identified in
           * association with the Entity. The value of the child Confidence element of all instances of this element should sum to
           * 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificVehicleIdentityConfidenceType, uci::type::accessorType::specificVehicleIdentityConfidenceType> SpecificVehicle;

         /** Indicates a possible specific facility instance, generally a blue forces facility, that has been identified in
           * association with the Entity. The value of the child Confidence element of all instances of this element should sum to
           * 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SpecificFacilityIdentityConfidenceType, uci::type::accessorType::specificFacilityIdentityConfidenceType> SpecificFacility;

         /** Indicates a possible identity of the Entity in the taxonomy/nomenclature of the Electronic Order of Battle (EOB). The
           * value of the child Confidence element of all instances of this element should sum to 100%. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EOB_IdentityConfidenceType, uci::type::accessorType::eOB_IdentityConfidenceType> EOB;

         /** Indicates a type of weapon available for designation to the specified target/Entity. This element represents the
           * MIL-STD-6016 specific type of the store. The value of the child Confidence element of all instances of this element
           * should sum to 100%. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::StoreConfidenceType, uci::type::accessorType::storeConfidenceType> Weapon;

         /** Returns the bounded list that is identified by the Standard.
           *
           * @return The bounded list identified by Standard.
           */
         virtual const uci::type::IdentityConfidenceType::Standard& getStandard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Standard.
           *
           * @return The bounded list identified by Standard.
           */
         virtual uci::type::IdentityConfidenceType::Standard& getStandard()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Standard.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStandard(const uci::type::IdentityConfidenceType::Standard& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Environment.
           *
           * @return The bounded list identified by Environment.
           */
         virtual const uci::type::IdentityConfidenceType::Environment& getEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Environment.
           *
           * @return The bounded list identified by Environment.
           */
         virtual uci::type::IdentityConfidenceType::Environment& getEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Environment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEnvironment(const uci::type::IdentityConfidenceType::Environment& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Platform.
           *
           * @return The bounded list identified by Platform.
           */
         virtual const uci::type::IdentityConfidenceType::Platform& getPlatform() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Platform.
           *
           * @return The bounded list identified by Platform.
           */
         virtual uci::type::IdentityConfidenceType::Platform& getPlatform()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Platform.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPlatform(const uci::type::IdentityConfidenceType::Platform& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Specific.
           *
           * @return The bounded list identified by Specific.
           */
         virtual const uci::type::IdentityConfidenceType::Specific& getSpecific() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Specific.
           *
           * @return The bounded list identified by Specific.
           */
         virtual uci::type::IdentityConfidenceType::Specific& getSpecific()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Specific.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecific(const uci::type::IdentityConfidenceType::Specific& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual const uci::type::IdentityConfidenceType::Emitter& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual uci::type::IdentityConfidenceType::Emitter& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Emitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitter(const uci::type::IdentityConfidenceType::Emitter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificEmitter.
           *
           * @return The bounded list identified by SpecificEmitter.
           */
         virtual const uci::type::IdentityConfidenceType::SpecificEmitter& getSpecificEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificEmitter.
           *
           * @return The bounded list identified by SpecificEmitter.
           */
         virtual uci::type::IdentityConfidenceType::SpecificEmitter& getSpecificEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpecificEmitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecificEmitter(const uci::type::IdentityConfidenceType::SpecificEmitter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificVehicle.
           *
           * @return The bounded list identified by SpecificVehicle.
           */
         virtual const uci::type::IdentityConfidenceType::SpecificVehicle& getSpecificVehicle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificVehicle.
           *
           * @return The bounded list identified by SpecificVehicle.
           */
         virtual uci::type::IdentityConfidenceType::SpecificVehicle& getSpecificVehicle()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpecificVehicle.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecificVehicle(const uci::type::IdentityConfidenceType::SpecificVehicle& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificFacility.
           *
           * @return The bounded list identified by SpecificFacility.
           */
         virtual const uci::type::IdentityConfidenceType::SpecificFacility& getSpecificFacility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SpecificFacility.
           *
           * @return The bounded list identified by SpecificFacility.
           */
         virtual uci::type::IdentityConfidenceType::SpecificFacility& getSpecificFacility()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SpecificFacility.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSpecificFacility(const uci::type::IdentityConfidenceType::SpecificFacility& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EOB.
           *
           * @return The bounded list identified by EOB.
           */
         virtual const uci::type::IdentityConfidenceType::EOB& getEOB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EOB.
           *
           * @return The bounded list identified by EOB.
           */
         virtual uci::type::IdentityConfidenceType::EOB& getEOB()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EOB.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEOB(const uci::type::IdentityConfidenceType::EOB& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Weapon.
           *
           * @return The bounded list identified by Weapon.
           */
         virtual const uci::type::IdentityConfidenceType::Weapon& getWeapon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Weapon.
           *
           * @return The bounded list identified by Weapon.
           */
         virtual uci::type::IdentityConfidenceType::Weapon& getWeapon()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Weapon.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWeapon(const uci::type::IdentityConfidenceType::Weapon& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IdentityConfidenceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IdentityConfidenceType to copy from
           */
         IdentityConfidenceType(const IdentityConfidenceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IdentityConfidenceType to the contents of the
           * IdentityConfidenceType on the right hand side (rhs) of the assignment operator.IdentityConfidenceType [only available
           * to derived classes]
           *
           * @param rhs The IdentityConfidenceType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::IdentityConfidenceType
           * @return A constant reference to this IdentityConfidenceType.
           */
         const IdentityConfidenceType& operator=(const IdentityConfidenceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IdentityConfidenceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IdentityConfidenceType_h

