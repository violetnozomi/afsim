// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__WeatherRadarActivityType_h
#define Uci__Type__WeatherRadarActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Type__WeatherRadarActivityStatusDetailType_h)
# include "uci/type/WeatherRadarActivityStatusDetailType.h"
#endif

#if !defined(Uci__Type__AirVolumeSensorReferencedType_h)
# include "uci/type/AirVolumeSensorReferencedType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherRadarActivityType sequence accessor class */
      class WeatherRadarActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~WeatherRadarActivityType()
         { }

         /** Returns this accessor's type constant, i.e. WeatherRadarActivityType
           *
           * @return This accessor's type constant, i.e. WeatherRadarActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherRadarActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherRadarActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates which electronic protection IDs are in use for this activity. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> ElectronicProtectionOptionsEmployed;

         /** Returns the accessor that provides access to the complex content that is identified by the Metrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Metrics.
           */
         virtual const uci::type::WeatherRadarActivityStatusDetailType& getMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Metrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Metrics.
           */
         virtual uci::type::WeatherRadarActivityStatusDetailType& getMetrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Metrics to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Metrics
           */
         virtual void setMetrics(const uci::type::WeatherRadarActivityStatusDetailType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Metrics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Metrics is emabled or not
           */
         virtual bool hasMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Metrics
           *
           * @param type = uci::type::accessorType::weatherRadarActivityStatusDetailType This Accessor's accessor type
           */
         virtual void enableMetrics(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherRadarActivityStatusDetailType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Metrics */
         virtual void clearMetrics()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AirVolume.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirVolume.
           */
         virtual const uci::type::AirVolumeSensorReferencedType& getAirVolume() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AirVolume.
           *
           * @return The acecssor that provides access to the complex content that is identified by AirVolume.
           */
         virtual uci::type::AirVolumeSensorReferencedType& getAirVolume()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AirVolume to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AirVolume
           */
         virtual void setAirVolume(const uci::type::AirVolumeSensorReferencedType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PolicyApplied.
           *
           * @return The value of the simple primitive data type identified by PolicyApplied.
           */
         virtual xs::Boolean getPolicyApplied() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PolicyApplied.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPolicyApplied(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PolicyApplied exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PolicyApplied is emabled or not
           */
         virtual bool hasPolicyApplied() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PolicyApplied
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePolicyApplied(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PolicyApplied */
         virtual void clearPolicyApplied()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AllProductsAndMessagesProduced.
           *
           * @return The value of the simple primitive data type identified by AllProductsAndMessagesProduced.
           */
         virtual xs::Boolean getAllProductsAndMessagesProduced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AllProductsAndMessagesProduced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAllProductsAndMessagesProduced(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsEmployed.
           */
         virtual const uci::type::WeatherRadarActivityType::ElectronicProtectionOptionsEmployed& getElectronicProtectionOptionsEmployed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsEmployed.
           */
         virtual uci::type::WeatherRadarActivityType::ElectronicProtectionOptionsEmployed& getElectronicProtectionOptionsEmployed()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ElectronicProtectionOptionsEmployed.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setElectronicProtectionOptionsEmployed(const uci::type::WeatherRadarActivityType::ElectronicProtectionOptionsEmployed& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherRadarActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherRadarActivityType to copy from
           */
         WeatherRadarActivityType(const WeatherRadarActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherRadarActivityType to the contents of the
           * WeatherRadarActivityType on the right hand side (rhs) of the assignment operator.WeatherRadarActivityType [only
           * available to derived classes]
           *
           * @param rhs The WeatherRadarActivityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::WeatherRadarActivityType
           * @return A constant reference to this WeatherRadarActivityType.
           */
         const WeatherRadarActivityType& operator=(const WeatherRadarActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherRadarActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherRadarActivityType_h

