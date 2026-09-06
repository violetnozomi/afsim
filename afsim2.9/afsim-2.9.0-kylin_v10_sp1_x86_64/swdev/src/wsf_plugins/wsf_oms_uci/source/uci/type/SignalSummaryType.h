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
#ifndef Uci__Type__SignalSummaryType_h
#define Uci__Type__SignalSummaryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

#if !defined(Uci__Type__VisibleString64Type_h)
# include "uci/type/VisibleString64Type.h"
#endif

#if !defined(Uci__Type__FrequencyModulationVariationEnum_h)
# include "uci/type/FrequencyModulationVariationEnum.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__PRI_TypeEnum_h)
# include "uci/type/PRI_TypeEnum.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioType_h)
# include "uci/type/MilliwattPowerRatioType.h"
#endif

#if !defined(Uci__Type__TrackModeEnum_h)
# include "uci/type/TrackModeEnum.h"
#endif

#if !defined(Uci__Type__EmitterUrgencyEnum_h)
# include "uci/type/EmitterUrgencyEnum.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__SignalLocationEnum_h)
# include "uci/type/SignalLocationEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalSummaryType sequence accessor class */
      class SignalSummaryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalSummaryType()
         { }

         /** Returns this accessor's type constant, i.e. SignalSummaryType
           *
           * @return This accessor's type constant, i.e. SignalSummaryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalSummaryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalSummaryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Modulation.
           *
           * @return The value of the enumeration identified by Modulation.
           */
         virtual const uci::type::ModulationEnum& getModulation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Modulation.
           *
           * @return The value of the enumeration identified by Modulation.
           */
         virtual uci::type::ModulationEnum& getModulation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Modulation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setModulation(const uci::type::ModulationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Modulation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setModulation(uci::type::ModulationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Modulationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasModulation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getModulation will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::modulationEnum This Accessor's accessor type
           */
         virtual void enableModulation(uci::base::accessorType::AccessorType type = uci::type::accessorType::modulationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearModulation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the VendorSpecificModulation.
           *
           * @return The value of the string data type identified by VendorSpecificModulation.
           */
         virtual const uci::type::VisibleString64Type& getVendorSpecificModulation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the VendorSpecificModulation.
           *
           * @return The value of the string data type identified by VendorSpecificModulation.
           */
         virtual uci::type::VisibleString64Type& getVendorSpecificModulation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VendorSpecificModulation to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setVendorSpecificModulation(const uci::type::VisibleString64Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VendorSpecificModulation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setVendorSpecificModulation(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VendorSpecificModulation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setVendorSpecificModulation(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VendorSpecificModulation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VendorSpecificModulation is emabled or not
           */
         virtual bool hasVendorSpecificModulation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VendorSpecificModulation
           *
           * @param type = uci::type::accessorType::visibleString64Type This Accessor's accessor type
           */
         virtual void enableVendorSpecificModulation(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString64Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VendorSpecificModulation */
         virtual void clearVendorSpecificModulation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FrequencyModulationVariation.
           *
           * @return The value of the enumeration identified by FrequencyModulationVariation.
           */
         virtual const uci::type::FrequencyModulationVariationEnum& getFrequencyModulationVariation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FrequencyModulationVariation.
           *
           * @return The value of the enumeration identified by FrequencyModulationVariation.
           */
         virtual uci::type::FrequencyModulationVariationEnum& getFrequencyModulationVariation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FrequencyModulationVariation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFrequencyModulationVariation(const uci::type::FrequencyModulationVariationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FrequencyModulationVariation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFrequencyModulationVariation(uci::type::FrequencyModulationVariationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FrequencyModulationVariationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFrequencyModulationVariation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFrequencyModulationVariation will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::frequencyModulationVariationEnum This Accessor's accessor type
           */
         virtual void enableFrequencyModulationVariation(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyModulationVariationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFrequencyModulationVariation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyAverage.
           *
           * @return The value of the simple primitive data type identified by FrequencyAverage.
           */
         virtual uci::type::FrequencyTypeValue getFrequencyAverage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyAverage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyAverage(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyAverage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyAverage is emabled or not
           */
         virtual bool hasFrequencyAverage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyAverage
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequencyAverage(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyAverage */
         virtual void clearFrequencyAverage()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyMin.
           *
           * @return The value of the simple primitive data type identified by FrequencyMin.
           */
         virtual uci::type::FrequencyTypeValue getFrequencyMin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyMin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyMin(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyMin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyMin is emabled or not
           */
         virtual bool hasFrequencyMin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyMin
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequencyMin(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyMin */
         virtual void clearFrequencyMin()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyMax.
           *
           * @return The value of the simple primitive data type identified by FrequencyMax.
           */
         virtual uci::type::FrequencyTypeValue getFrequencyMax() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyMax.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyMax(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyMax exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyMax is emabled or not
           */
         virtual bool hasFrequencyMax() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyMax
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequencyMax(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyMax */
         virtual void clearFrequencyMax()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_Type.
           *
           * @return The value of the enumeration identified by PRI_Type.
           */
         virtual const uci::type::PRI_TypeEnum& getPRI_Type() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_Type.
           *
           * @return The value of the enumeration identified by PRI_Type.
           */
         virtual uci::type::PRI_TypeEnum& getPRI_Type()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_Type.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_Type(const uci::type::PRI_TypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_Type.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_Type(uci::type::PRI_TypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PRI_Typeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPRI_Type() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPRI_Type will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::pRI_TypeEnum This Accessor's accessor type
           */
         virtual void enablePRI_Type(uci::base::accessorType::AccessorType type = uci::type::accessorType::pRI_TypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPRI_Type()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Average.
           *
           * @return The value of the simple primitive data type identified by PRI_Average.
           */
         virtual uci::type::DurationTypeValue getPRI_Average() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Average.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Average(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Average exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Average is emabled or not
           */
         virtual bool hasPRI_Average() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Average
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_Average(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Average */
         virtual void clearPRI_Average()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Min.
           *
           * @return The value of the simple primitive data type identified by PRI_Min.
           */
         virtual uci::type::DurationTypeValue getPRI_Min() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Min.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Min(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Min exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Min is emabled or not
           */
         virtual bool hasPRI_Min() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Min
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_Min(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Min */
         virtual void clearPRI_Min()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Max.
           *
           * @return The value of the simple primitive data type identified by PRI_Max.
           */
         virtual uci::type::DurationTypeValue getPRI_Max() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Max.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Max(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Max exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Max is emabled or not
           */
         virtual bool hasPRI_Max() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Max
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_Max(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Max */
         virtual void clearPRI_Max()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseWidthAverage.
           *
           * @return The value of the simple primitive data type identified by PulseWidthAverage.
           */
         virtual uci::type::DurationTypeValue getPulseWidthAverage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseWidthAverage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseWidthAverage(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidthAverage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidthAverage is emabled or not
           */
         virtual bool hasPulseWidthAverage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidthAverage
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePulseWidthAverage(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidthAverage */
         virtual void clearPulseWidthAverage()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseWidthMin.
           *
           * @return The value of the simple primitive data type identified by PulseWidthMin.
           */
         virtual uci::type::DurationTypeValue getPulseWidthMin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseWidthMin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseWidthMin(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidthMin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidthMin is emabled or not
           */
         virtual bool hasPulseWidthMin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidthMin
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePulseWidthMin(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidthMin */
         virtual void clearPulseWidthMin()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseWidthMax.
           *
           * @return The value of the simple primitive data type identified by PulseWidthMax.
           */
         virtual uci::type::DurationTypeValue getPulseWidthMax() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseWidthMax.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseWidthMax(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidthMax exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidthMax is emabled or not
           */
         virtual bool hasPulseWidthMax() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidthMax
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePulseWidthMax(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidthMax */
         virtual void clearPulseWidthMax()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AmplitudeAverage.
           *
           * @return The value of the simple primitive data type identified by AmplitudeAverage.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getAmplitudeAverage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AmplitudeAverage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAmplitudeAverage(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AmplitudeAverage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AmplitudeAverage is emabled or not
           */
         virtual bool hasAmplitudeAverage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AmplitudeAverage
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableAmplitudeAverage(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AmplitudeAverage */
         virtual void clearAmplitudeAverage()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TrackMode.
           *
           * @return The value of the enumeration identified by TrackMode.
           */
         virtual const uci::type::TrackModeEnum& getTrackMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TrackMode.
           *
           * @return The value of the enumeration identified by TrackMode.
           */
         virtual uci::type::TrackModeEnum& getTrackMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TrackMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTrackMode(const uci::type::TrackModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TrackMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTrackMode(uci::type::TrackModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TrackModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTrackMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTrackMode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::trackModeEnum This Accessor's accessor type
           */
         virtual void enableTrackMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::trackModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTrackMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Urgency.
           *
           * @return The value of the enumeration identified by Urgency.
           */
         virtual const uci::type::EmitterUrgencyEnum& getUrgency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Urgency.
           *
           * @return The value of the enumeration identified by Urgency.
           */
         virtual uci::type::EmitterUrgencyEnum& getUrgency()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Urgency.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setUrgency(const uci::type::EmitterUrgencyEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Urgency.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setUrgency(uci::type::EmitterUrgencyEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Urgencyis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasUrgency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getUrgency will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::emitterUrgencyEnum This Accessor's accessor type
           */
         virtual void enableUrgency(uci::base::accessorType::AccessorType type = uci::type::accessorType::emitterUrgencyEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearUrgency()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the WartimeReserve.
           *
           * @return The value of the string data type identified by WartimeReserve.
           */
         virtual const uci::type::EmptyType& getWartimeReserve() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the WartimeReserve.
           *
           * @return The value of the string data type identified by WartimeReserve.
           */
         virtual uci::type::EmptyType& getWartimeReserve()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WartimeReserve to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setWartimeReserve(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WartimeReserve to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setWartimeReserve(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WartimeReserve to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setWartimeReserve(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WartimeReserve exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WartimeReserve is emabled or not
           */
         virtual bool hasWartimeReserve() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WartimeReserve
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableWartimeReserve(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WartimeReserve */
         virtual void clearWartimeReserve()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LocationCategory.
           *
           * @return The value of the enumeration identified by LocationCategory.
           */
         virtual const uci::type::SignalLocationEnum& getLocationCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LocationCategory.
           *
           * @return The value of the enumeration identified by LocationCategory.
           */
         virtual uci::type::SignalLocationEnum& getLocationCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LocationCategory.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLocationCategory(const uci::type::SignalLocationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LocationCategory.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLocationCategory(uci::type::SignalLocationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield LocationCategoryis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLocationCategory() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLocationCategory will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::signalLocationEnum This Accessor's accessor type
           */
         virtual void enableLocationCategory(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalLocationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLocationCategory()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalSummaryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalSummaryType to copy from
           */
         SignalSummaryType(const SignalSummaryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalSummaryType to the contents of the SignalSummaryType on the
           * right hand side (rhs) of the assignment operator.SignalSummaryType [only available to derived classes]
           *
           * @param rhs The SignalSummaryType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SignalSummaryType
           * @return A constant reference to this SignalSummaryType.
           */
         const SignalSummaryType& operator=(const SignalSummaryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalSummaryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalSummaryType_h

