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
#ifndef Uci__Type__ImpactConditionsType_h
#define Uci__Type__ImpactConditionsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FuzeType_h)
# include "uci/type/FuzeType.h"
#endif

#if !defined(Uci__Type__SpeedType_h)
# include "uci/type/SpeedType.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__TargetHardnessEnum_h)
# include "uci/type/TargetHardnessEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The desired weapon impact conditions. Provided together with store/store variant type and quantity as part of a
        * weaponeered DMPI.
        */
      class ImpactConditionsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ImpactConditionsType()
         { }

         /** Returns this accessor's type constant, i.e. ImpactConditionsType
           *
           * @return This accessor's type constant, i.e. ImpactConditionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::impactConditionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ImpactConditionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates mode and settings for a fuze of the weapon. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FuzeType, uci::type::accessorType::fuzeType> Fuze;

         /** Returns the bounded list that is identified by the Fuze.
           *
           * @return The bounded list identified by Fuze.
           */
         virtual const uci::type::ImpactConditionsType::Fuze& getFuze() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Fuze.
           *
           * @return The bounded list identified by Fuze.
           */
         virtual uci::type::ImpactConditionsType::Fuze& getFuze()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Fuze.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFuze(const uci::type::ImpactConditionsType::Fuze& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ImpactSpeed.
           *
           * @return The value of the simple primitive data type identified by ImpactSpeed.
           */
         virtual uci::type::SpeedTypeValue getImpactSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ImpactSpeed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setImpactSpeed(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ImpactSpeed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ImpactSpeed is emabled or not
           */
         virtual bool hasImpactSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ImpactSpeed
           *
           * @param type = uci::type::accessorType::speedType This Accessor's accessor type
           */
         virtual void enableImpactSpeed(uci::base::accessorType::AccessorType type = uci::type::accessorType::speedType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ImpactSpeed */
         virtual void clearImpactSpeed()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ImpactAzimuthAngle.
           *
           * @return The value of the simple primitive data type identified by ImpactAzimuthAngle.
           */
         virtual uci::type::AngleTypeValue getImpactAzimuthAngle() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ImpactAzimuthAngle.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setImpactAzimuthAngle(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ImpactAzimuthAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ImpactAzimuthAngle is emabled or not
           */
         virtual bool hasImpactAzimuthAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ImpactAzimuthAngle
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableImpactAzimuthAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ImpactAzimuthAngle */
         virtual void clearImpactAzimuthAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ImpactElevationAngle.
           *
           * @return The value of the simple primitive data type identified by ImpactElevationAngle.
           */
         virtual uci::type::AngleTypeValue getImpactElevationAngle() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ImpactElevationAngle.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setImpactElevationAngle(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ImpactElevationAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ImpactElevationAngle is emabled or not
           */
         virtual bool hasImpactElevationAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ImpactElevationAngle
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableImpactElevationAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ImpactElevationAngle */
         virtual void clearImpactElevationAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TargetHardness.
           *
           * @return The value of the enumeration identified by TargetHardness.
           */
         virtual const uci::type::TargetHardnessEnum& getTargetHardness() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TargetHardness.
           *
           * @return The value of the enumeration identified by TargetHardness.
           */
         virtual uci::type::TargetHardnessEnum& getTargetHardness()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TargetHardness.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTargetHardness(const uci::type::TargetHardnessEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TargetHardness.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTargetHardness(uci::type::TargetHardnessEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TargetHardnessis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTargetHardness() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTargetHardness will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::targetHardnessEnum This Accessor's accessor type
           */
         virtual void enableTargetHardness(uci::base::accessorType::AccessorType type = uci::type::accessorType::targetHardnessEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTargetHardness()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ImpactConditionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ImpactConditionsType to copy from
           */
         ImpactConditionsType(const ImpactConditionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ImpactConditionsType to the contents of the ImpactConditionsType
           * on the right hand side (rhs) of the assignment operator.ImpactConditionsType [only available to derived classes]
           *
           * @param rhs The ImpactConditionsType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ImpactConditionsType
           * @return A constant reference to this ImpactConditionsType.
           */
         const ImpactConditionsType& operator=(const ImpactConditionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ImpactConditionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ImpactConditionsType_h

