// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__COMINT_ReceiveDwellType_h
#define Uci__Type__COMINT_ReceiveDwellType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DwellID_Type_h)
# include "uci/type/DwellID_Type.h"
#endif

#if !defined(Uci__Type__COMINT_DwellStateEnum_h)
# include "uci/type/COMINT_DwellStateEnum.h"
#endif

#if !defined(Uci__Type__COMINT_DwellTypeEnum_h)
# include "uci/type/COMINT_DwellTypeEnum.h"
#endif

#if !defined(Uci__Type__SensorConeType_h)
# include "uci/type/SensorConeType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioType_h)
# include "uci/type/MilliwattPowerRatioType.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__COMINT_InteractiveType_h)
# include "uci/type/COMINT_InteractiveType.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_ReceiveDwellType sequence accessor class */
      class COMINT_ReceiveDwellType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_ReceiveDwellType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_ReceiveDwellType
           *
           * @return This accessor's type constant, i.e. COMINT_ReceiveDwellType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_ReceiveDwellType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_ReceiveDwellType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a component with the Dwell is assigned to use. Required if the Dwell is assigned/active. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> ComponentID;

         /** Indicates the frequency coverage of the Dwell, including all RF_Control, Command, MDF and other restrictions.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> Frequency;

         /** Returns the accessor that provides access to the complex content that is identified by the DwellID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DwellID.
           */
         virtual const uci::type::DwellID_Type& getDwellID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DwellID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DwellID.
           */
         virtual uci::type::DwellID_Type& getDwellID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DwellID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DwellID
           */
         virtual void setDwellID(const uci::type::DwellID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DwellState.
           *
           * @return The value of the enumeration identified by DwellState.
           */
         virtual const uci::type::COMINT_DwellStateEnum& getDwellState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DwellState.
           *
           * @return The value of the enumeration identified by DwellState.
           */
         virtual uci::type::COMINT_DwellStateEnum& getDwellState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DwellState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDwellState(const uci::type::COMINT_DwellStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DwellState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDwellState(uci::type::COMINT_DwellStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DwellType.
           *
           * @return The value of the enumeration identified by DwellType.
           */
         virtual const uci::type::COMINT_DwellTypeEnum& getDwellType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DwellType.
           *
           * @return The value of the enumeration identified by DwellType.
           */
         virtual uci::type::COMINT_DwellTypeEnum& getDwellType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DwellType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDwellType(const uci::type::COMINT_DwellTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DwellType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDwellType(uci::type::COMINT_DwellTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield DwellTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasDwellType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getDwellType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::cOMINT_DwellTypeEnum This Accessor's accessor type
           */
         virtual void enableDwellType(uci::base::accessorType::AccessorType type = uci::type::accessorType::cOMINT_DwellTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearDwellType()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DwellFOV.
           *
           * @return The acecssor that provides access to the complex content that is identified by DwellFOV.
           */
         virtual const uci::type::SensorConeType& getDwellFOV() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DwellFOV.
           *
           * @return The acecssor that provides access to the complex content that is identified by DwellFOV.
           */
         virtual uci::type::SensorConeType& getDwellFOV()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DwellFOV to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DwellFOV
           */
         virtual void setDwellFOV(const uci::type::SensorConeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DwellFOV exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DwellFOV is emabled or not
           */
         virtual bool hasDwellFOV() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DwellFOV
           *
           * @param type = uci::type::accessorType::sensorConeType This Accessor's accessor type
           */
         virtual void enableDwellFOV(uci::base::accessorType::AccessorType type = uci::type::accessorType::sensorConeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DwellFOV */
         virtual void clearDwellFOV()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StartTime.
           *
           * @return The value of the simple primitive data type identified by StartTime.
           */
         virtual uci::type::DateTimeTypeValue getStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StartTime is emabled or not
           */
         virtual bool hasStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StartTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StartTime */
         virtual void clearStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CumulativeDwellTime.
           *
           * @return The value of the simple primitive data type identified by CumulativeDwellTime.
           */
         virtual uci::type::DurationTypeValue getCumulativeDwellTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CumulativeDwellTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCumulativeDwellTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CumulativeDwellTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CumulativeDwellTime is emabled or not
           */
         virtual bool hasCumulativeDwellTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CumulativeDwellTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableCumulativeDwellTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CumulativeDwellTime */
         virtual void clearCumulativeDwellTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual const uci::type::COMINT_ReceiveDwellType::ComponentID& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentID.
           *
           * @return The bounded list identified by ComponentID.
           */
         virtual uci::type::COMINT_ReceiveDwellType::ComponentID& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentID(const uci::type::COMINT_ReceiveDwellType::ComponentID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Frequency.
           *
           * @return The bounded list identified by Frequency.
           */
         virtual const uci::type::COMINT_ReceiveDwellType::Frequency& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Frequency.
           *
           * @return The bounded list identified by Frequency.
           */
         virtual uci::type::COMINT_ReceiveDwellType::Frequency& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Frequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFrequency(const uci::type::COMINT_ReceiveDwellType::Frequency& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DwellRevisitInterval.
           *
           * @return The value of the simple primitive data type identified by DwellRevisitInterval.
           */
         virtual uci::type::DurationTypeValue getDwellRevisitInterval() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DwellRevisitInterval.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDwellRevisitInterval(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DwellRevisitInterval exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DwellRevisitInterval is emabled or not
           */
         virtual bool hasDwellRevisitInterval() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DwellRevisitInterval
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableDwellRevisitInterval(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DwellRevisitInterval */
         virtual void clearDwellRevisitInterval()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseWidthLow.
           *
           * @return The value of the simple primitive data type identified by PulseWidthLow.
           */
         virtual uci::type::DurationTypeValue getPulseWidthLow() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseWidthLow.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseWidthLow(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidthLow exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidthLow is emabled or not
           */
         virtual bool hasPulseWidthLow() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidthLow
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePulseWidthLow(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidthLow */
         virtual void clearPulseWidthLow()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseWidthHigh.
           *
           * @return The value of the simple primitive data type identified by PulseWidthHigh.
           */
         virtual uci::type::DurationTypeValue getPulseWidthHigh() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseWidthHigh.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseWidthHigh(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidthHigh exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidthHigh is emabled or not
           */
         virtual bool hasPulseWidthHigh() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidthHigh
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePulseWidthHigh(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidthHigh */
         virtual void clearPulseWidthHigh()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_High.
           *
           * @return The value of the simple primitive data type identified by PRI_High.
           */
         virtual uci::type::DurationTypeValue getPRI_High() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_High.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_High(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_High exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_High is emabled or not
           */
         virtual bool hasPRI_High() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_High
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_High(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_High */
         virtual void clearPRI_High()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NativeSensitivity.
           *
           * @return The value of the simple primitive data type identified by NativeSensitivity.
           */
         virtual uci::type::DecibelTypeValue getNativeSensitivity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NativeSensitivity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNativeSensitivity(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NativeSensitivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NativeSensitivity is emabled or not
           */
         virtual bool hasNativeSensitivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NativeSensitivity
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableNativeSensitivity(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NativeSensitivity */
         virtual void clearNativeSensitivity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Desensitization.
           *
           * @return The value of the simple primitive data type identified by Desensitization.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getDesensitization() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Desensitization.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDesensitization(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Desensitization exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Desensitization is emabled or not
           */
         virtual bool hasDesensitization() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Desensitization
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableDesensitization(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Desensitization */
         virtual void clearDesensitization()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VideoBandwidth.
           *
           * @return The value of the simple primitive data type identified by VideoBandwidth.
           */
         virtual uci::type::FrequencyTypeValue getVideoBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VideoBandwidth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVideoBandwidth(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VideoBandwidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VideoBandwidth is emabled or not
           */
         virtual bool hasVideoBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VideoBandwidth
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableVideoBandwidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VideoBandwidth */
         virtual void clearVideoBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the COMINT_Interactive.
           *
           * @return The acecssor that provides access to the complex content that is identified by COMINT_Interactive.
           */
         virtual const uci::type::COMINT_InteractiveType& getCOMINT_Interactive() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the COMINT_Interactive.
           *
           * @return The acecssor that provides access to the complex content that is identified by COMINT_Interactive.
           */
         virtual uci::type::COMINT_InteractiveType& getCOMINT_Interactive()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the COMINT_Interactive to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by COMINT_Interactive
           */
         virtual void setCOMINT_Interactive(const uci::type::COMINT_InteractiveType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by COMINT_Interactive exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by COMINT_Interactive is emabled or not
           */
         virtual bool hasCOMINT_Interactive() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by COMINT_Interactive
           *
           * @param type = uci::type::accessorType::cOMINT_InteractiveType This Accessor's accessor type
           */
         virtual void enableCOMINT_Interactive(uci::base::accessorType::AccessorType type = uci::type::accessorType::cOMINT_InteractiveType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by COMINT_Interactive */
         virtual void clearCOMINT_Interactive()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StopTime.
           *
           * @return The value of the simple primitive data type identified by StopTime.
           */
         virtual uci::type::DateTimeTypeValue getStopTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StopTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStopTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StopTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StopTime is emabled or not
           */
         virtual bool hasStopTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StopTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableStopTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StopTime */
         virtual void clearStopTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SampleRate.
           *
           * @return The value of the simple primitive data type identified by SampleRate.
           */
         virtual xs::Float getSampleRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SampleRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSampleRate(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SampleRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SampleRate is emabled or not
           */
         virtual bool hasSampleRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SampleRate
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableSampleRate(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SampleRate */
         virtual void clearSampleRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FFT_PointsPerSample.
           *
           * @return The value of the simple primitive data type identified by FFT_PointsPerSample.
           */
         virtual xs::Int getFFT_PointsPerSample() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FFT_PointsPerSample.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFFT_PointsPerSample(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FFT_PointsPerSample exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FFT_PointsPerSample is emabled or not
           */
         virtual bool hasFFT_PointsPerSample() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FFT_PointsPerSample
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableFFT_PointsPerSample(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FFT_PointsPerSample */
         virtual void clearFFT_PointsPerSample()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_ReceiveDwellType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_ReceiveDwellType to copy from
           */
         COMINT_ReceiveDwellType(const COMINT_ReceiveDwellType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_ReceiveDwellType to the contents of the
           * COMINT_ReceiveDwellType on the right hand side (rhs) of the assignment operator.COMINT_ReceiveDwellType [only
           * available to derived classes]
           *
           * @param rhs The COMINT_ReceiveDwellType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::COMINT_ReceiveDwellType
           * @return A constant reference to this COMINT_ReceiveDwellType.
           */
         const COMINT_ReceiveDwellType& operator=(const COMINT_ReceiveDwellType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_ReceiveDwellType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_ReceiveDwellType_h

