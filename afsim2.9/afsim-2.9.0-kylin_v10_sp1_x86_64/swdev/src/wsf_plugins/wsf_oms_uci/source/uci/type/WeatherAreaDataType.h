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
#ifndef Uci__Type__WeatherAreaDataType_h
#define Uci__Type__WeatherAreaDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TemperatureType_h)
# include "uci/type/TemperatureType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__WeatherPrecipitationEnum_h)
# include "uci/type/WeatherPrecipitationEnum.h"
#endif

#if !defined(Uci__Type__WeatherPrecipitationAmplificationEnum_h)
# include "uci/type/WeatherPrecipitationAmplificationEnum.h"
#endif

#if !defined(Uci__Type__Velocity2D_Type_h)
# include "uci/type/Velocity2D_Type.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Uci__Type__CloudsType_h)
# include "uci/type/CloudsType.h"
#endif

#if !defined(Uci__Type__WeatherEffectsType_h)
# include "uci/type/WeatherEffectsType.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Type__WeatherSeverityEnum_h)
# include "uci/type/WeatherSeverityEnum.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherAreaDataType sequence accessor class */
      class WeatherAreaDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~WeatherAreaDataType()
         { }

         /** Returns this accessor's type constant, i.e. WeatherAreaDataType
           *
           * @return This accessor's type constant, i.e. WeatherAreaDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherAreaDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherAreaDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Temperature.
           *
           * @return The value of the simple primitive data type identified by Temperature.
           */
         virtual uci::type::TemperatureTypeValue getTemperature() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Temperature.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTemperature(uci::type::TemperatureTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Temperature exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Temperature is emabled or not
           */
         virtual bool hasTemperature() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Temperature
           *
           * @param type = uci::type::accessorType::temperatureType This Accessor's accessor type
           */
         virtual void enableTemperature(uci::base::accessorType::AccessorType type = uci::type::accessorType::temperatureType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Temperature */
         virtual void clearTemperature()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PrecipitationPotential.
           *
           * @return The value of the simple primitive data type identified by PrecipitationPotential.
           */
         virtual uci::type::PercentTypeValue getPrecipitationPotential() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PrecipitationPotential.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPrecipitationPotential(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PrecipitationPotential exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PrecipitationPotential is emabled or not
           */
         virtual bool hasPrecipitationPotential() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PrecipitationPotential
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePrecipitationPotential(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PrecipitationPotential */
         virtual void clearPrecipitationPotential()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Precipitation.
           *
           * @return The value of the enumeration identified by Precipitation.
           */
         virtual const uci::type::WeatherPrecipitationEnum& getPrecipitation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Precipitation.
           *
           * @return The value of the enumeration identified by Precipitation.
           */
         virtual uci::type::WeatherPrecipitationEnum& getPrecipitation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Precipitation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPrecipitation(const uci::type::WeatherPrecipitationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Precipitation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPrecipitation(uci::type::WeatherPrecipitationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Precipitationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPrecipitation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPrecipitation will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::weatherPrecipitationEnum This Accessor's accessor type
           */
         virtual void enablePrecipitation(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherPrecipitationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPrecipitation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PrecipitationAmplification.
           *
           * @return The value of the enumeration identified by PrecipitationAmplification.
           */
         virtual const uci::type::WeatherPrecipitationAmplificationEnum& getPrecipitationAmplification() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PrecipitationAmplification.
           *
           * @return The value of the enumeration identified by PrecipitationAmplification.
           */
         virtual uci::type::WeatherPrecipitationAmplificationEnum& getPrecipitationAmplification()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PrecipitationAmplification.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPrecipitationAmplification(const uci::type::WeatherPrecipitationAmplificationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PrecipitationAmplification.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPrecipitationAmplification(uci::type::WeatherPrecipitationAmplificationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PrecipitationAmplificationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPrecipitationAmplification() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPrecipitationAmplification will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::weatherPrecipitationAmplificationEnum This Accessor's accessor type
           */
         virtual void enablePrecipitationAmplification(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherPrecipitationAmplificationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPrecipitationAmplification()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WindVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by WindVelocity.
           */
         virtual const uci::type::Velocity2D_Type& getWindVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WindVelocity.
           *
           * @return The acecssor that provides access to the complex content that is identified by WindVelocity.
           */
         virtual uci::type::Velocity2D_Type& getWindVelocity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WindVelocity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WindVelocity
           */
         virtual void setWindVelocity(const uci::type::Velocity2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WindVelocity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WindVelocity is emabled or not
           */
         virtual bool hasWindVelocity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WindVelocity
           *
           * @param type = uci::type::accessorType::velocity2D_Type This Accessor's accessor type
           */
         virtual void enableWindVelocity(uci::base::accessorType::AccessorType type = uci::type::accessorType::velocity2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WindVelocity */
         virtual void clearWindVelocity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Visibility.
           *
           * @return The value of the simple primitive data type identified by Visibility.
           */
         virtual uci::type::DistanceTypeValue getVisibility() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Visibility.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVisibility(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Visibility exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Visibility is emabled or not
           */
         virtual bool hasVisibility() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Visibility
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableVisibility(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Visibility */
         virtual void clearVisibility()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DewPoint.
           *
           * @return The value of the simple primitive data type identified by DewPoint.
           */
         virtual uci::type::TemperatureTypeValue getDewPoint() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DewPoint.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDewPoint(uci::type::TemperatureTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DewPoint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DewPoint is emabled or not
           */
         virtual bool hasDewPoint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DewPoint
           *
           * @param type = uci::type::accessorType::temperatureType This Accessor's accessor type
           */
         virtual void enableDewPoint(uci::base::accessorType::AccessorType type = uci::type::accessorType::temperatureType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DewPoint */
         virtual void clearDewPoint()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Clouds.
           *
           * @return The acecssor that provides access to the complex content that is identified by Clouds.
           */
         virtual const uci::type::CloudsType& getClouds() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Clouds.
           *
           * @return The acecssor that provides access to the complex content that is identified by Clouds.
           */
         virtual uci::type::CloudsType& getClouds()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Clouds to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Clouds
           */
         virtual void setClouds(const uci::type::CloudsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Clouds exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Clouds is emabled or not
           */
         virtual bool hasClouds() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Clouds
           *
           * @param type = uci::type::accessorType::cloudsType This Accessor's accessor type
           */
         virtual void enableClouds(uci::base::accessorType::AccessorType type = uci::type::accessorType::cloudsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Clouds */
         virtual void clearClouds()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherEffects.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherEffects.
           */
         virtual const uci::type::WeatherEffectsType& getWeatherEffects() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherEffects.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherEffects.
           */
         virtual uci::type::WeatherEffectsType& getWeatherEffects()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherEffects to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherEffects
           */
         virtual void setWeatherEffects(const uci::type::WeatherEffectsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WeatherEffects exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WeatherEffects is emabled or not
           */
         virtual bool hasWeatherEffects() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WeatherEffects
           *
           * @param type = uci::type::accessorType::weatherEffectsType This Accessor's accessor type
           */
         virtual void enableWeatherEffects(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherEffectsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WeatherEffects */
         virtual void clearWeatherEffects()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BarometricPressure.
           *
           * @return The value of the simple primitive data type identified by BarometricPressure.
           */
         virtual xs::Float getBarometricPressure() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BarometricPressure.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBarometricPressure(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BarometricPressure exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BarometricPressure is emabled or not
           */
         virtual bool hasBarometricPressure() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BarometricPressure
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableBarometricPressure(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BarometricPressure */
         virtual void clearBarometricPressure()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Icing.
           *
           * @return The value of the enumeration identified by Icing.
           */
         virtual const uci::type::WeatherSeverityEnum& getIcing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Icing.
           *
           * @return The value of the enumeration identified by Icing.
           */
         virtual uci::type::WeatherSeverityEnum& getIcing()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Icing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setIcing(const uci::type::WeatherSeverityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Icing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setIcing(uci::type::WeatherSeverityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Icingis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasIcing() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getIcing will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::weatherSeverityEnum This Accessor's accessor type
           */
         virtual void enableIcing(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherSeverityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearIcing()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Turbulence.
           *
           * @return The value of the enumeration identified by Turbulence.
           */
         virtual const uci::type::WeatherSeverityEnum& getTurbulence() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Turbulence.
           *
           * @return The value of the enumeration identified by Turbulence.
           */
         virtual uci::type::WeatherSeverityEnum& getTurbulence()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Turbulence.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTurbulence(const uci::type::WeatherSeverityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Turbulence.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTurbulence(uci::type::WeatherSeverityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Turbulenceis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTurbulence() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTurbulence will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::weatherSeverityEnum This Accessor's accessor type
           */
         virtual void enableTurbulence(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherSeverityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTurbulence()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ThunderstormPotential.
           *
           * @return The value of the simple primitive data type identified by ThunderstormPotential.
           */
         virtual uci::type::PercentTypeValue getThunderstormPotential() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ThunderstormPotential.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setThunderstormPotential(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ThunderstormPotential exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ThunderstormPotential is emabled or not
           */
         virtual bool hasThunderstormPotential() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ThunderstormPotential
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableThunderstormPotential(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ThunderstormPotential */
         virtual void clearThunderstormPotential()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Remarks.
           *
           * @return The value of the string data type identified by Remarks.
           */
         virtual const uci::type::VisibleString1024Type& getRemarks() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Remarks.
           *
           * @return The value of the string data type identified by Remarks.
           */
         virtual uci::type::VisibleString1024Type& getRemarks()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRemarks(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarks(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarks(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Remarks exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Remarks is emabled or not
           */
         virtual bool hasRemarks() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Remarks
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableRemarks(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Remarks */
         virtual void clearRemarks()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherAreaDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherAreaDataType to copy from
           */
         WeatherAreaDataType(const WeatherAreaDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherAreaDataType to the contents of the WeatherAreaDataType on
           * the right hand side (rhs) of the assignment operator.WeatherAreaDataType [only available to derived classes]
           *
           * @param rhs The WeatherAreaDataType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::WeatherAreaDataType
           * @return A constant reference to this WeatherAreaDataType.
           */
         const WeatherAreaDataType& operator=(const WeatherAreaDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherAreaDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherAreaDataType_h

