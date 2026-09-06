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
#ifndef Uci__Type__CommControlType_h
#define Uci__Type__CommControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__CommConnectionType_h)
# include "uci/type/CommConnectionType.h"
#endif

#if !defined(Uci__Type__CommReceiverStateEnum_h)
# include "uci/type/CommReceiverStateEnum.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioType_h)
# include "uci/type/MilliwattPowerRatioType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__CommTransmitterStateEnum_h)
# include "uci/type/CommTransmitterStateEnum.h"
#endif

#if !defined(Uci__Type__DataRateType_h)
# include "uci/type/DataRateType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__RemoteDestinationType_h)
# include "uci/type/RemoteDestinationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to control the waveform setup, modification, and status for a capability. */
      class CommControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommControlType()
         { }

         /** Returns this accessor's type constant, i.e. CommControlType
           *
           * @return This accessor's type constant, i.e. CommControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Represents the squelch level for a waveform to be operating at.; [Minimum inclusive: 0] [Maximum inclusive: 255] */
         typedef uci::base::UnsignedIntAccessor SquelchLevel;
         //  The type of primitive data accessed by SquelchLevel
         typedef xs::UnsignedInt SquelchLevelValue;

         /** Represents the volume level for the J-Voice digitized audio.; [Minimum inclusive: 0] [Maximum inclusive: 255] */
         typedef uci::base::UnsignedIntAccessor VolumeLevel;
         //  The type of primitive data accessed by VolumeLevel
         typedef xs::UnsignedInt VolumeLevelValue;

         /** Sets / Indicates the Antenna Component associated with this waveform capability. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> AntennaID;

         /** Returns the value of the SimplePrimitive data type that is identified by the InhibitTransmit.
           *
           * @return The value of the simple primitive data type identified by InhibitTransmit.
           */
         virtual xs::Boolean getInhibitTransmit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InhibitTransmit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInhibitTransmit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InhibitTransmit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InhibitTransmit is emabled or not
           */
         virtual bool hasInhibitTransmit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InhibitTransmit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableInhibitTransmit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InhibitTransmit */
         virtual void clearInhibitTransmit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InhibitReceive.
           *
           * @return The value of the simple primitive data type identified by InhibitReceive.
           */
         virtual xs::Boolean getInhibitReceive() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InhibitReceive.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInhibitReceive(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InhibitReceive exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InhibitReceive is emabled or not
           */
         virtual bool hasInhibitReceive() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InhibitReceive
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableInhibitReceive(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InhibitReceive */
         virtual void clearInhibitReceive()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InhibitRelay.
           *
           * @return The value of the simple primitive data type identified by InhibitRelay.
           */
         virtual xs::Boolean getInhibitRelay() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InhibitRelay.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInhibitRelay(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InhibitRelay exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InhibitRelay is emabled or not
           */
         virtual bool hasInhibitRelay() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InhibitRelay
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableInhibitRelay(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InhibitRelay */
         virtual void clearInhibitRelay()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConnectionSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConnectionSettings.
           */
         virtual const uci::type::CommConnectionType& getConnectionSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConnectionSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConnectionSettings.
           */
         virtual uci::type::CommConnectionType& getConnectionSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConnectionSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConnectionSettings
           */
         virtual void setConnectionSettings(const uci::type::CommConnectionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConnectionSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConnectionSettings is emabled or not
           */
         virtual bool hasConnectionSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConnectionSettings
           *
           * @param type = uci::type::accessorType::commConnectionType This Accessor's accessor type
           */
         virtual void enableConnectionSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::commConnectionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConnectionSettings */
         virtual void clearConnectionSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ReceiverState.
           *
           * @return The value of the enumeration identified by ReceiverState.
           */
         virtual const uci::type::CommReceiverStateEnum& getReceiverState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ReceiverState.
           *
           * @return The value of the enumeration identified by ReceiverState.
           */
         virtual uci::type::CommReceiverStateEnum& getReceiverState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ReceiverState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setReceiverState(const uci::type::CommReceiverStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ReceiverState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setReceiverState(uci::type::CommReceiverStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ReceiverStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasReceiverState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getReceiverState will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commReceiverStateEnum This Accessor's accessor type
           */
         virtual void enableReceiverState(uci::base::accessorType::AccessorType type = uci::type::accessorType::commReceiverStateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearReceiverState()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RxPowerLevel.
           *
           * @return The value of the simple primitive data type identified by RxPowerLevel.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getRxPowerLevel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RxPowerLevel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRxPowerLevel(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RxPowerLevel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RxPowerLevel is emabled or not
           */
         virtual bool hasRxPowerLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RxPowerLevel
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableRxPowerLevel(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RxPowerLevel */
         virtual void clearRxPowerLevel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SquelchLevel.
           *
           * @return The value of the simple primitive data type identified by SquelchLevel.
           */
         virtual uci::type::CommControlType::SquelchLevelValue getSquelchLevel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SquelchLevel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSquelchLevel(uci::type::CommControlType::SquelchLevelValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SquelchLevel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SquelchLevel is emabled or not
           */
         virtual bool hasSquelchLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SquelchLevel
           *
           * @param type = uci::type::commControlType::accessorType::squelchLevel This Accessor's accessor type
           */
         virtual void enableSquelchLevel(uci::base::accessorType::AccessorType type = uci::type::commControlType::accessorType::squelchLevel)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SquelchLevel */
         virtual void clearSquelchLevel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmitterState.
           *
           * @return The value of the enumeration identified by TransmitterState.
           */
         virtual const uci::type::CommTransmitterStateEnum& getTransmitterState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmitterState.
           *
           * @return The value of the enumeration identified by TransmitterState.
           */
         virtual uci::type::CommTransmitterStateEnum& getTransmitterState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmitterState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmitterState(const uci::type::CommTransmitterStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmitterState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmitterState(uci::type::CommTransmitterStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TransmitterStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTransmitterState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTransmitterState will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commTransmitterStateEnum This Accessor's accessor type
           */
         virtual void enableTransmitterState(uci::base::accessorType::AccessorType type = uci::type::accessorType::commTransmitterStateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTransmitterState()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TxPowerLevel.
           *
           * @return The value of the simple primitive data type identified by TxPowerLevel.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getTxPowerLevel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TxPowerLevel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTxPowerLevel(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TxPowerLevel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TxPowerLevel is emabled or not
           */
         virtual bool hasTxPowerLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TxPowerLevel
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableTxPowerLevel(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TxPowerLevel */
         virtual void clearTxPowerLevel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VolumeLevel.
           *
           * @return The value of the simple primitive data type identified by VolumeLevel.
           */
         virtual uci::type::CommControlType::VolumeLevelValue getVolumeLevel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VolumeLevel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVolumeLevel(uci::type::CommControlType::VolumeLevelValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VolumeLevel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VolumeLevel is emabled or not
           */
         virtual bool hasVolumeLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VolumeLevel
           *
           * @param type = uci::type::commControlType::accessorType::volumeLevel This Accessor's accessor type
           */
         virtual void enableVolumeLevel(uci::base::accessorType::AccessorType type = uci::type::commControlType::accessorType::volumeLevel)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VolumeLevel */
         virtual void clearVolumeLevel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RF_DataRate.
           *
           * @return The value of the simple primitive data type identified by RF_DataRate.
           */
         virtual uci::type::DataRateTypeValue getRF_DataRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RF_DataRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRF_DataRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RF_DataRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RF_DataRate is emabled or not
           */
         virtual bool hasRF_DataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RF_DataRate
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableRF_DataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RF_DataRate */
         virtual void clearRF_DataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaID.
           *
           * @return The bounded list identified by AntennaID.
           */
         virtual const uci::type::CommControlType::AntennaID& getAntennaID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaID.
           *
           * @return The bounded list identified by AntennaID.
           */
         virtual uci::type::CommControlType::AntennaID& getAntennaID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AntennaID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAntennaID(const uci::type::CommControlType::AntennaID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoteDestination.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoteDestination.
           */
         virtual const uci::type::RemoteDestinationType& getRemoteDestination() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoteDestination.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoteDestination.
           */
         virtual uci::type::RemoteDestinationType& getRemoteDestination()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RemoteDestination to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RemoteDestination
           */
         virtual void setRemoteDestination(const uci::type::RemoteDestinationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemoteDestination exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemoteDestination is emabled or not
           */
         virtual bool hasRemoteDestination() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemoteDestination
           *
           * @param type = uci::type::accessorType::remoteDestinationType This Accessor's accessor type
           */
         virtual void enableRemoteDestination(uci::base::accessorType::AccessorType type = uci::type::accessorType::remoteDestinationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemoteDestination */
         virtual void clearRemoteDestination()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommControlType to copy from
           */
         CommControlType(const CommControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommControlType to the contents of the CommControlType on the
           * right hand side (rhs) of the assignment operator.CommControlType [only available to derived classes]
           *
           * @param rhs The CommControlType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::CommControlType
           * @return A constant reference to this CommControlType.
           */
         const CommControlType& operator=(const CommControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommControlType_h

