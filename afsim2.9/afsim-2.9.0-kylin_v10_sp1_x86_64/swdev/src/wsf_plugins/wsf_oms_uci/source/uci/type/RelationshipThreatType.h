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
#ifndef Uci__Type__RelationshipThreatType_h
#define Uci__Type__RelationshipThreatType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ThreatPostureEnum_h)
# include "uci/type/ThreatPostureEnum.h"
#endif

#if !defined(Uci__Type__ThreatTypeEnum_h)
# include "uci/type/ThreatTypeEnum.h"
#endif

#if !defined(Uci__Type__ThreatWeaponEnum_h)
# include "uci/type/ThreatWeaponEnum.h"
#endif

#if !defined(Uci__Type__EnduranceType_h)
# include "uci/type/EnduranceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Describes the nature of the threat. */
      class RelationshipThreatType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RelationshipThreatType()
         { }

         /** Returns this accessor's type constant, i.e. RelationshipThreatType
           *
           * @return This accessor's type constant, i.e. RelationshipThreatType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::relationshipThreatType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RelationshipThreatType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Posture.
           *
           * @return The value of the enumeration identified by Posture.
           */
         virtual const uci::type::ThreatPostureEnum& getPosture() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Posture.
           *
           * @return The value of the enumeration identified by Posture.
           */
         virtual uci::type::ThreatPostureEnum& getPosture()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Posture.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPosture(const uci::type::ThreatPostureEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Posture.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPosture(uci::type::ThreatPostureEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Postureis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPosture() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPosture will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::threatPostureEnum This Accessor's accessor type
           */
         virtual void enablePosture(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatPostureEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPosture()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the WeaponType.
           *
           * @return The value of the enumeration identified by WeaponType.
           */
         virtual const uci::type::ThreatTypeEnum& getWeaponType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the WeaponType.
           *
           * @return The value of the enumeration identified by WeaponType.
           */
         virtual uci::type::ThreatTypeEnum& getWeaponType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the WeaponType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setWeaponType(const uci::type::ThreatTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the WeaponType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setWeaponType(uci::type::ThreatTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield WeaponTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasWeaponType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getWeaponType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::threatTypeEnum This Accessor's accessor type
           */
         virtual void enableWeaponType(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearWeaponType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the WeaponSensor.
           *
           * @return The value of the enumeration identified by WeaponSensor.
           */
         virtual const uci::type::ThreatWeaponEnum& getWeaponSensor() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the WeaponSensor.
           *
           * @return The value of the enumeration identified by WeaponSensor.
           */
         virtual uci::type::ThreatWeaponEnum& getWeaponSensor()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the WeaponSensor.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setWeaponSensor(const uci::type::ThreatWeaponEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the WeaponSensor.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setWeaponSensor(uci::type::ThreatWeaponEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield WeaponSensoris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasWeaponSensor() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getWeaponSensor will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::threatWeaponEnum This Accessor's accessor type
           */
         virtual void enableWeaponSensor(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatWeaponEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearWeaponSensor()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endurance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endurance.
           */
         virtual const uci::type::EnduranceType& getEndurance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endurance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endurance.
           */
         virtual uci::type::EnduranceType& getEndurance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Endurance to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Endurance
           */
         virtual void setEndurance(const uci::type::EnduranceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Endurance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Endurance is emabled or not
           */
         virtual bool hasEndurance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Endurance
           *
           * @param type = uci::type::accessorType::enduranceType This Accessor's accessor type
           */
         virtual void enableEndurance(uci::base::accessorType::AccessorType type = uci::type::accessorType::enduranceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Endurance */
         virtual void clearEndurance()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RelationshipThreatType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RelationshipThreatType to copy from
           */
         RelationshipThreatType(const RelationshipThreatType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RelationshipThreatType to the contents of the
           * RelationshipThreatType on the right hand side (rhs) of the assignment operator.RelationshipThreatType [only available
           * to derived classes]
           *
           * @param rhs The RelationshipThreatType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RelationshipThreatType
           * @return A constant reference to this RelationshipThreatType.
           */
         const RelationshipThreatType& operator=(const RelationshipThreatType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RelationshipThreatType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RelationshipThreatType_h

