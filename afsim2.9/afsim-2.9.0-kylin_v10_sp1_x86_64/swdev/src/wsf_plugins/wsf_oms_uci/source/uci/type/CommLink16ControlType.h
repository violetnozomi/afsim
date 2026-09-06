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
#ifndef Uci__Type__CommLink16ControlType_h
#define Uci__Type__CommLink16ControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommLink16AdvancedSlotNotificationType_h)
# include "uci/type/CommLink16AdvancedSlotNotificationType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__CommLink16DelayReductionType_h)
# include "uci/type/CommLink16DelayReductionType.h"
#endif

#if !defined(Uci__Type__CommLink16EnhancedThroughputCodeRateEnum_h)
# include "uci/type/CommLink16EnhancedThroughputCodeRateEnum.h"
#endif

#if !defined(Uci__Type__CommLink16InitialEntryGroupEnum_h)
# include "uci/type/CommLink16InitialEntryGroupEnum.h"
#endif

#if !defined(Uci__Type__CommLink16InitialEntryMessageType_h)
# include "uci/type/CommLink16InitialEntryMessageType.h"
#endif

#if !defined(Uci__Type__CommInitialEntryWordsType_h)
# include "uci/type/CommInitialEntryWordsType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__CommLink16InterferenceProtModeEnum_h)
# include "uci/type/CommLink16InterferenceProtModeEnum.h"
#endif

#if !defined(Uci__Type__CommLink16MissionInfoWordsType_h)
# include "uci/type/CommLink16MissionInfoWordsType.h"
#endif

#if !defined(Uci__Type__CommLink16NetSelectionType_h)
# include "uci/type/CommLink16NetSelectionType.h"
#endif

#if !defined(Uci__Type__CommLink16_PPLI_PoolEnum_h)
# include "uci/type/CommLink16_PPLI_PoolEnum.h"
#endif

#if !defined(Uci__Type__CommLink16RangeModeEnum_h)
# include "uci/type/CommLink16RangeModeEnum.h"
#endif

#if !defined(Uci__Type__CommLink16SecondarySourceTrackNumberType_h)
# include "uci/type/CommLink16SecondarySourceTrackNumberType.h"
#endif

#if !defined(Uci__Type__CommLink16TxPG_Type_h)
# include "uci/type/CommLink16TxPG_Type.h"
#endif

#if !defined(Uci__Type__CommLink16TransmitAntennaEnum_h)
# include "uci/type/CommLink16TransmitAntennaEnum.h"
#endif

#if !defined(Uci__Type__CommLink16ReceiptComplianceWordsType_h)
# include "uci/type/CommLink16ReceiptComplianceWordsType.h"
#endif

#if !defined(Uci__Type__CommLink16TxPowerSelectionEnum_h)
# include "uci/type/CommLink16TxPowerSelectionEnum.h"
#endif

#if !defined(Uci__Type__CommLink16TransmitModeEnum_h)
# include "uci/type/CommLink16TransmitModeEnum.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to perform Link 16 Specific tuning of a communications capability. */
      class CommLink16ControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLink16ControlType()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16ControlType
           *
           * @return This accessor's type constant, i.e. CommLink16ControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16ControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16ControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Contains the Advanced Slot Notification for both modes. Indicates Advanced Slot Notification (ASN) Mode Selection
           * (MS) of Mode A (1) or Mode B (0), ASN Enabled (1) or Disabled (0) and the ASN Slot Selection (0..31, the default is
           * 7). [Minimum occurrences: 0] [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::CommLink16AdvancedSlotNotificationType, uci::type::accessorType::commLink16AdvancedSlotNotificationType> AdvancedSlotNotificationMode;

         /** [Minimum inclusive: 0] [Maximum inclusive: 511] */
         typedef uci::base::IntAccessor DataReductionPG_Index;
         //  The type of primitive data accessed by DataReductionPG_Index
         typedef xs::Int DataReductionPG_IndexValue;

         /** The Delay Reduction Participation Group (PG) Index Number specifies which message(s) are subject to Delay Reduction.
           * [Minimum occurrences: 0] [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::CommLink16DelayReductionType, uci::type::accessorType::commLink16DelayReductionType> DelayReduction;

         /** The Initial Entry Message send 10 words in Bit Words 580-589. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommInitialEntryWordsType, uci::type::accessorType::commInitialEntryWordsType> InitialEntryWords;

         /** [Minimum inclusive: 0] [Maximum inclusive: 255] */
         typedef uci::base::IntAccessor InitializationIndexNumber;
         //  The type of primitive data accessed by InitializationIndexNumber
         typedef xs::Int InitializationIndexNumberValue;

         /** Used to set / identify the net selection per PG Index. [Minimum occurrences: 0] [Maximum occurrences: 8] */
         typedef uci::base::BoundedList<uci::type::CommLink16NetSelectionType, uci::type::accessorType::commLink16NetSelectionType> NetSelectionStatus;

         /** [Minimum inclusive: 0] [Maximum inclusive: 15] */
         typedef uci::base::IntAccessor PlatformStrength;
         //  The type of primitive data accessed by PlatformStrength
         typedef xs::Int PlatformStrengthValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 511] */
         typedef uci::base::IntAccessor RouteEstPG_Index;
         //  The type of primitive data accessed by RouteEstPG_Index
         typedef xs::Int RouteEstPG_IndexValue;

         /** Up to 15 Secondary TNs used for forwarding. One location is reserved for the Net Manager Indicator. [Minimum
           * occurrences: 0] [Maximum occurrences: 16]
           */
         typedef uci::base::BoundedList<uci::type::CommLink16SecondarySourceTrackNumberType, uci::type::accessorType::commLink16SecondarySourceTrackNumberType> SecondarySourceTrackNumber;

         /** Allows the host to set a specific power level, packing limit, antenna configuration, staleness, and storage limit for
           * a specific PG. [Minimum occurrences: 0] [Maximum occurrences: 10]
           */
         typedef uci::base::BoundedList<uci::type::CommLink16TxPG_Type, uci::type::accessorType::commLink16TxPG_Type> TransmitPG_Slots;

         /** Used to indicate whether the terminal will perform receipt compliance (R/C) processing on an original message
           * provided by the Host. [Minimum occurrences: 0] [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::CommLink16ReceiptComplianceWordsType, uci::type::accessorType::commLink16ReceiptComplianceWordsType> ReceiptComplianceProcessingWords;

         /** Returns the bounded list that is identified by the AdvancedSlotNotificationMode.
           *
           * @return The bounded list identified by AdvancedSlotNotificationMode.
           */
         virtual const uci::type::CommLink16ControlType::AdvancedSlotNotificationMode& getAdvancedSlotNotificationMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AdvancedSlotNotificationMode.
           *
           * @return The bounded list identified by AdvancedSlotNotificationMode.
           */
         virtual uci::type::CommLink16ControlType::AdvancedSlotNotificationMode& getAdvancedSlotNotificationMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AdvancedSlotNotificationMode.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAdvancedSlotNotificationMode(const uci::type::CommLink16ControlType::AdvancedSlotNotificationMode& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DataReductionPG_Index.
           *
           * @return The value of the simple primitive data type identified by DataReductionPG_Index.
           */
         virtual uci::type::CommLink16ControlType::DataReductionPG_IndexValue getDataReductionPG_Index() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DataReductionPG_Index.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDataReductionPG_Index(uci::type::CommLink16ControlType::DataReductionPG_IndexValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataReductionPG_Index exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataReductionPG_Index is emabled or not
           */
         virtual bool hasDataReductionPG_Index() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataReductionPG_Index
           *
           * @param type = uci::type::commLink16ControlType::accessorType::dataReductionPG_Index This Accessor's accessor type
           */
         virtual void enableDataReductionPG_Index(uci::base::accessorType::AccessorType type = uci::type::commLink16ControlType::accessorType::dataReductionPG_Index)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataReductionPG_Index */
         virtual void clearDataReductionPG_Index()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DelayReduction.
           *
           * @return The bounded list identified by DelayReduction.
           */
         virtual const uci::type::CommLink16ControlType::DelayReduction& getDelayReduction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DelayReduction.
           *
           * @return The bounded list identified by DelayReduction.
           */
         virtual uci::type::CommLink16ControlType::DelayReduction& getDelayReduction()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DelayReduction.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDelayReduction(const uci::type::CommLink16ControlType::DelayReduction& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EnhancedThroughputCodeRate.
           *
           * @return The value of the enumeration identified by EnhancedThroughputCodeRate.
           */
         virtual const uci::type::CommLink16EnhancedThroughputCodeRateEnum& getEnhancedThroughputCodeRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EnhancedThroughputCodeRate.
           *
           * @return The value of the enumeration identified by EnhancedThroughputCodeRate.
           */
         virtual uci::type::CommLink16EnhancedThroughputCodeRateEnum& getEnhancedThroughputCodeRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EnhancedThroughputCodeRate.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEnhancedThroughputCodeRate(const uci::type::CommLink16EnhancedThroughputCodeRateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EnhancedThroughputCodeRate.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEnhancedThroughputCodeRate(uci::type::CommLink16EnhancedThroughputCodeRateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EnhancedThroughputCodeRateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEnhancedThroughputCodeRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEnhancedThroughputCodeRate will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::commLink16EnhancedThroughputCodeRateEnum This Accessor's accessor type
           */
         virtual void enableEnhancedThroughputCodeRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16EnhancedThroughputCodeRateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEnhancedThroughputCodeRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the InitialEntryGroup.
           *
           * @return The value of the enumeration identified by InitialEntryGroup.
           */
         virtual const uci::type::CommLink16InitialEntryGroupEnum& getInitialEntryGroup() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the InitialEntryGroup.
           *
           * @return The value of the enumeration identified by InitialEntryGroup.
           */
         virtual uci::type::CommLink16InitialEntryGroupEnum& getInitialEntryGroup()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the InitialEntryGroup.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setInitialEntryGroup(const uci::type::CommLink16InitialEntryGroupEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the InitialEntryGroup.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setInitialEntryGroup(uci::type::CommLink16InitialEntryGroupEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield InitialEntryGroupis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasInitialEntryGroup() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getInitialEntryGroup will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commLink16InitialEntryGroupEnum This Accessor's accessor type
           */
         virtual void enableInitialEntryGroup(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16InitialEntryGroupEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearInitialEntryGroup()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * InitialEntryMessageAssignment.
           *
           * @return The acecssor that provides access to the complex content that is identified by InitialEntryMessageAssignment.
           */
         virtual const uci::type::CommLink16InitialEntryMessageType& getInitialEntryMessageAssignment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * InitialEntryMessageAssignment.
           *
           * @return The acecssor that provides access to the complex content that is identified by InitialEntryMessageAssignment.
           */
         virtual uci::type::CommLink16InitialEntryMessageType& getInitialEntryMessageAssignment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InitialEntryMessageAssignment to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InitialEntryMessageAssignment
           */
         virtual void setInitialEntryMessageAssignment(const uci::type::CommLink16InitialEntryMessageType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InitialEntryMessageAssignment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InitialEntryMessageAssignment is emabled or not
           */
         virtual bool hasInitialEntryMessageAssignment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InitialEntryMessageAssignment
           *
           * @param type = uci::type::accessorType::commLink16InitialEntryMessageType This Accessor's accessor type
           */
         virtual void enableInitialEntryMessageAssignment(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16InitialEntryMessageType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InitialEntryMessageAssignment */
         virtual void clearInitialEntryMessageAssignment()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InitialEntryWords.
           *
           * @return The bounded list identified by InitialEntryWords.
           */
         virtual const uci::type::CommLink16ControlType::InitialEntryWords& getInitialEntryWords() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InitialEntryWords.
           *
           * @return The bounded list identified by InitialEntryWords.
           */
         virtual uci::type::CommLink16ControlType::InitialEntryWords& getInitialEntryWords()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the InitialEntryWords.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setInitialEntryWords(const uci::type::CommLink16ControlType::InitialEntryWords& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InitializationActivityIndicator.
           *
           * @return The value of the simple primitive data type identified by InitializationActivityIndicator.
           */
         virtual xs::Boolean getInitializationActivityIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InitializationActivityIndicator.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInitializationActivityIndicator(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InitializationActivityIndicator exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InitializationActivityIndicator is emabled or not
           */
         virtual bool hasInitializationActivityIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InitializationActivityIndicator
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableInitializationActivityIndicator(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InitializationActivityIndicator */
         virtual void clearInitializationActivityIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InitializationIndexNumber.
           *
           * @return The value of the simple primitive data type identified by InitializationIndexNumber.
           */
         virtual uci::type::CommLink16ControlType::InitializationIndexNumberValue getInitializationIndexNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InitializationIndexNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInitializationIndexNumber(uci::type::CommLink16ControlType::InitializationIndexNumberValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InitializationIndexNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InitializationIndexNumber is emabled or not
           */
         virtual bool hasInitializationIndexNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InitializationIndexNumber
           *
           * @param type = uci::type::commLink16ControlType::accessorType::initializationIndexNumber This Accessor's accessor type
           */
         virtual void enableInitializationIndexNumber(uci::base::accessorType::AccessorType type = uci::type::commLink16ControlType::accessorType::initializationIndexNumber)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InitializationIndexNumber */
         virtual void clearInitializationIndexNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the InterferenceProtectionMode.
           *
           * @return The value of the enumeration identified by InterferenceProtectionMode.
           */
         virtual const uci::type::CommLink16InterferenceProtModeEnum& getInterferenceProtectionMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the InterferenceProtectionMode.
           *
           * @return The value of the enumeration identified by InterferenceProtectionMode.
           */
         virtual uci::type::CommLink16InterferenceProtModeEnum& getInterferenceProtectionMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the InterferenceProtectionMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setInterferenceProtectionMode(const uci::type::CommLink16InterferenceProtModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the InterferenceProtectionMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setInterferenceProtectionMode(uci::type::CommLink16InterferenceProtModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield InterferenceProtectionModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasInterferenceProtectionMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getInterferenceProtectionMode will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::commLink16InterferenceProtModeEnum This Accessor's accessor type
           */
         virtual void enableInterferenceProtectionMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16InterferenceProtModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearInterferenceProtectionMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionInformationWords.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionInformationWords.
           */
         virtual const uci::type::CommLink16MissionInfoWordsType& getMissionInformationWords() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionInformationWords.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionInformationWords.
           */
         virtual uci::type::CommLink16MissionInfoWordsType& getMissionInformationWords()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionInformationWords to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionInformationWords
           */
         virtual void setMissionInformationWords(const uci::type::CommLink16MissionInfoWordsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionInformationWords exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionInformationWords is emabled or not
           */
         virtual bool hasMissionInformationWords() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionInformationWords
           *
           * @param type = uci::type::accessorType::commLink16MissionInfoWordsType This Accessor's accessor type
           */
         virtual void enableMissionInformationWords(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16MissionInfoWordsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionInformationWords */
         virtual void clearMissionInformationWords()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the NetSelectionStatus.
           *
           * @return The bounded list identified by NetSelectionStatus.
           */
         virtual const uci::type::CommLink16ControlType::NetSelectionStatus& getNetSelectionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the NetSelectionStatus.
           *
           * @return The bounded list identified by NetSelectionStatus.
           */
         virtual uci::type::CommLink16ControlType::NetSelectionStatus& getNetSelectionStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the NetSelectionStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setNetSelectionStatus(const uci::type::CommLink16ControlType::NetSelectionStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NetEntryTxEnable.
           *
           * @return The value of the simple primitive data type identified by NetEntryTxEnable.
           */
         virtual xs::Boolean getNetEntryTxEnable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NetEntryTxEnable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNetEntryTxEnable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NetEntryTxEnable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NetEntryTxEnable is emabled or not
           */
         virtual bool hasNetEntryTxEnable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NetEntryTxEnable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableNetEntryTxEnable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NetEntryTxEnable */
         virtual void clearNetEntryTxEnable()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PlatformStrength.
           *
           * @return The value of the simple primitive data type identified by PlatformStrength.
           */
         virtual uci::type::CommLink16ControlType::PlatformStrengthValue getPlatformStrength() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PlatformStrength.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPlatformStrength(uci::type::CommLink16ControlType::PlatformStrengthValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlatformStrength exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlatformStrength is emabled or not
           */
         virtual bool hasPlatformStrength() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlatformStrength
           *
           * @param type = uci::type::commLink16ControlType::accessorType::platformStrength This Accessor's accessor type
           */
         virtual void enablePlatformStrength(uci::base::accessorType::AccessorType type = uci::type::commLink16ControlType::accessorType::platformStrength)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlatformStrength */
         virtual void clearPlatformStrength()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PPLIPool.
           *
           * @return The value of the enumeration identified by PPLIPool.
           */
         virtual const uci::type::CommLink16_PPLI_PoolEnum& getPPLIPool() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PPLIPool.
           *
           * @return The value of the enumeration identified by PPLIPool.
           */
         virtual uci::type::CommLink16_PPLI_PoolEnum& getPPLIPool()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PPLIPool.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPPLIPool(const uci::type::CommLink16_PPLI_PoolEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PPLIPool.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPPLIPool(uci::type::CommLink16_PPLI_PoolEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PPLIPoolis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPPLIPool() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPPLIPool will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commLink16_PPLI_PoolEnum This Accessor's accessor type
           */
         virtual void enablePPLIPool(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16_PPLI_PoolEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPPLIPool()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RangeMode.
           *
           * @return The value of the enumeration identified by RangeMode.
           */
         virtual const uci::type::CommLink16RangeModeEnum& getRangeMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RangeMode.
           *
           * @return The value of the enumeration identified by RangeMode.
           */
         virtual uci::type::CommLink16RangeModeEnum& getRangeMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RangeMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRangeMode(const uci::type::CommLink16RangeModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RangeMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRangeMode(uci::type::CommLink16RangeModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield RangeModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasRangeMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getRangeMode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commLink16RangeModeEnum This Accessor's accessor type
           */
         virtual void enableRangeMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16RangeModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearRangeMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RouteEstPG_Index.
           *
           * @return The value of the simple primitive data type identified by RouteEstPG_Index.
           */
         virtual uci::type::CommLink16ControlType::RouteEstPG_IndexValue getRouteEstPG_Index() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RouteEstPG_Index.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRouteEstPG_Index(uci::type::CommLink16ControlType::RouteEstPG_IndexValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RouteEstPG_Index exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RouteEstPG_Index is emabled or not
           */
         virtual bool hasRouteEstPG_Index() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RouteEstPG_Index
           *
           * @param type = uci::type::commLink16ControlType::accessorType::routeEstPG_Index This Accessor's accessor type
           */
         virtual void enableRouteEstPG_Index(uci::base::accessorType::AccessorType type = uci::type::commLink16ControlType::accessorType::routeEstPG_Index)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RouteEstPG_Index */
         virtual void clearRouteEstPG_Index()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecondarySourceTrackNumber.
           *
           * @return The bounded list identified by SecondarySourceTrackNumber.
           */
         virtual const uci::type::CommLink16ControlType::SecondarySourceTrackNumber& getSecondarySourceTrackNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecondarySourceTrackNumber.
           *
           * @return The bounded list identified by SecondarySourceTrackNumber.
           */
         virtual uci::type::CommLink16ControlType::SecondarySourceTrackNumber& getSecondarySourceTrackNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SecondarySourceTrackNumber.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSecondarySourceTrackNumber(const uci::type::CommLink16ControlType::SecondarySourceTrackNumber& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransmitPG_Slots.
           *
           * @return The bounded list identified by TransmitPG_Slots.
           */
         virtual const uci::type::CommLink16ControlType::TransmitPG_Slots& getTransmitPG_Slots() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransmitPG_Slots.
           *
           * @return The bounded list identified by TransmitPG_Slots.
           */
         virtual uci::type::CommLink16ControlType::TransmitPG_Slots& getTransmitPG_Slots()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TransmitPG_Slots.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTransmitPG_Slots(const uci::type::CommLink16ControlType::TransmitPG_Slots& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmitAntenna.
           *
           * @return The value of the enumeration identified by TransmitAntenna.
           */
         virtual const uci::type::CommLink16TransmitAntennaEnum& getTransmitAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmitAntenna.
           *
           * @return The value of the enumeration identified by TransmitAntenna.
           */
         virtual uci::type::CommLink16TransmitAntennaEnum& getTransmitAntenna()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmitAntenna.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmitAntenna(const uci::type::CommLink16TransmitAntennaEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmitAntenna.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmitAntenna(uci::type::CommLink16TransmitAntennaEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TransmitAntennais available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTransmitAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTransmitAntenna will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commLink16TransmitAntennaEnum This Accessor's accessor type
           */
         virtual void enableTransmitAntenna(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16TransmitAntennaEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTransmitAntenna()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiptComplianceProcessingWords.
           *
           * @return The bounded list identified by ReceiptComplianceProcessingWords.
           */
         virtual const uci::type::CommLink16ControlType::ReceiptComplianceProcessingWords& getReceiptComplianceProcessingWords() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiptComplianceProcessingWords.
           *
           * @return The bounded list identified by ReceiptComplianceProcessingWords.
           */
         virtual uci::type::CommLink16ControlType::ReceiptComplianceProcessingWords& getReceiptComplianceProcessingWords()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ReceiptComplianceProcessingWords.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setReceiptComplianceProcessingWords(const uci::type::CommLink16ControlType::ReceiptComplianceProcessingWords& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ReceiveAntenna.
           *
           * @return The value of the enumeration identified by ReceiveAntenna.
           */
         virtual const uci::type::CommLink16TransmitAntennaEnum& getReceiveAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ReceiveAntenna.
           *
           * @return The value of the enumeration identified by ReceiveAntenna.
           */
         virtual uci::type::CommLink16TransmitAntennaEnum& getReceiveAntenna()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ReceiveAntenna.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setReceiveAntenna(const uci::type::CommLink16TransmitAntennaEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ReceiveAntenna.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setReceiveAntenna(uci::type::CommLink16TransmitAntennaEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ReceiveAntennais available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasReceiveAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getReceiveAntenna will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commLink16TransmitAntennaEnum This Accessor's accessor type
           */
         virtual void enableReceiveAntenna(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16TransmitAntennaEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearReceiveAntenna()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmitPowerSelection.
           *
           * @return The value of the enumeration identified by TransmitPowerSelection.
           */
         virtual const uci::type::CommLink16TxPowerSelectionEnum& getTransmitPowerSelection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmitPowerSelection.
           *
           * @return The value of the enumeration identified by TransmitPowerSelection.
           */
         virtual uci::type::CommLink16TxPowerSelectionEnum& getTransmitPowerSelection()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmitPowerSelection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmitPowerSelection(const uci::type::CommLink16TxPowerSelectionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmitPowerSelection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmitPowerSelection(uci::type::CommLink16TxPowerSelectionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TransmitPowerSelectionis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTransmitPowerSelection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTransmitPowerSelection will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::commLink16TxPowerSelectionEnum This Accessor's accessor type
           */
         virtual void enableTransmitPowerSelection(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16TxPowerSelectionEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTransmitPowerSelection()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmitMode.
           *
           * @return The value of the enumeration identified by TransmitMode.
           */
         virtual const uci::type::CommLink16TransmitModeEnum& getTransmitMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmitMode.
           *
           * @return The value of the enumeration identified by TransmitMode.
           */
         virtual uci::type::CommLink16TransmitModeEnum& getTransmitMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmitMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmitMode(const uci::type::CommLink16TransmitModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmitMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmitMode(uci::type::CommLink16TransmitModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TransmitModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTransmitMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTransmitMode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commLink16TransmitModeEnum This Accessor's accessor type
           */
         virtual void enableTransmitMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16TransmitModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTransmitMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceGroupRate.
           *
           * @return The value of the simple primitive data type identified by VoiceGroupRate.
           */
         virtual uci::type::FrequencyTypeValue getVoiceGroupRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceGroupRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceGroupRate(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceGroupRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceGroupRate is emabled or not
           */
         virtual bool hasVoiceGroupRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceGroupRate
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableVoiceGroupRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceGroupRate */
         virtual void clearVoiceGroupRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceGroupCoding.
           *
           * @return The value of the simple primitive data type identified by VoiceGroupCoding.
           */
         virtual xs::Boolean getVoiceGroupCoding() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceGroupCoding.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceGroupCoding(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceGroupCoding exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceGroupCoding is emabled or not
           */
         virtual bool hasVoiceGroupCoding() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceGroupCoding
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableVoiceGroupCoding(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceGroupCoding */
         virtual void clearVoiceGroupCoding()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16ControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16ControlType to copy from
           */
         CommLink16ControlType(const CommLink16ControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16ControlType to the contents of the CommLink16ControlType
           * on the right hand side (rhs) of the assignment operator.CommLink16ControlType [only available to derived classes]
           *
           * @param rhs The CommLink16ControlType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CommLink16ControlType
           * @return A constant reference to this CommLink16ControlType.
           */
         const CommLink16ControlType& operator=(const CommLink16ControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16ControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16ControlType_h

