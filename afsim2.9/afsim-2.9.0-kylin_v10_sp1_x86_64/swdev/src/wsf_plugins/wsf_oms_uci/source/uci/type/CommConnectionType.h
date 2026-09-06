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
#ifndef Uci__Type__CommConnectionType_h
#define Uci__Type__CommConnectionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__CommModulationTypeEnum_h)
# include "uci/type/CommModulationTypeEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to perform the tuning of a communications capability. */
      class CommConnectionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommConnectionType()
         { }

         /** Returns this accessor's type constant, i.e. CommConnectionType
           *
           * @return This accessor's type constant, i.e. CommConnectionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commConnectionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommConnectionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element represents a potential list of transmit channels to tune this waveform to. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> TransmitChannel;

         /** This element represents a potential list of transmit frequencies to tune this waveform to. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyType, uci::type::accessorType::frequencyType> TransmitFrequency;

         /** This element represents a potential list of receive channels to tune this waveform to. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> ReceiveChannel;

         /** This element represents a potential list of receive frequencies to tune this waveform to. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyType, uci::type::accessorType::frequencyType> ReceiveFrequency;

         /** This element represents a key to a configuration containing the network ID / Name. This is specifically an ID to aid
           * a waveform to ID its own terminal, a waveform network, or IP Address. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> NetworkID;

         /** Returns the bounded list that is identified by the TransmitChannel.
           *
           * @return The bounded list identified by TransmitChannel.
           */
         virtual const uci::type::CommConnectionType::TransmitChannel& getTransmitChannel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransmitChannel.
           *
           * @return The bounded list identified by TransmitChannel.
           */
         virtual uci::type::CommConnectionType::TransmitChannel& getTransmitChannel()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TransmitChannel.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTransmitChannel(const uci::type::CommConnectionType::TransmitChannel& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransmitFrequency.
           *
           * @return The bounded list identified by TransmitFrequency.
           */
         virtual const uci::type::CommConnectionType::TransmitFrequency& getTransmitFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransmitFrequency.
           *
           * @return The bounded list identified by TransmitFrequency.
           */
         virtual uci::type::CommConnectionType::TransmitFrequency& getTransmitFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TransmitFrequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTransmitFrequency(const uci::type::CommConnectionType::TransmitFrequency& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InstantaneousTransmitBandwidth.
           *
           * @return The value of the simple primitive data type identified by InstantaneousTransmitBandwidth.
           */
         virtual uci::type::FrequencyTypeValue getInstantaneousTransmitBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InstantaneousTransmitBandwidth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInstantaneousTransmitBandwidth(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InstantaneousTransmitBandwidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InstantaneousTransmitBandwidth is emabled or not
           */
         virtual bool hasInstantaneousTransmitBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InstantaneousTransmitBandwidth
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableInstantaneousTransmitBandwidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InstantaneousTransmitBandwidth */
         virtual void clearInstantaneousTransmitBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiveChannel.
           *
           * @return The bounded list identified by ReceiveChannel.
           */
         virtual const uci::type::CommConnectionType::ReceiveChannel& getReceiveChannel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiveChannel.
           *
           * @return The bounded list identified by ReceiveChannel.
           */
         virtual uci::type::CommConnectionType::ReceiveChannel& getReceiveChannel()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ReceiveChannel.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setReceiveChannel(const uci::type::CommConnectionType::ReceiveChannel& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiveFrequency.
           *
           * @return The bounded list identified by ReceiveFrequency.
           */
         virtual const uci::type::CommConnectionType::ReceiveFrequency& getReceiveFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiveFrequency.
           *
           * @return The bounded list identified by ReceiveFrequency.
           */
         virtual uci::type::CommConnectionType::ReceiveFrequency& getReceiveFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ReceiveFrequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setReceiveFrequency(const uci::type::CommConnectionType::ReceiveFrequency& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InstantaneousReceiveBandwidth.
           *
           * @return The value of the simple primitive data type identified by InstantaneousReceiveBandwidth.
           */
         virtual uci::type::FrequencyTypeValue getInstantaneousReceiveBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InstantaneousReceiveBandwidth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInstantaneousReceiveBandwidth(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InstantaneousReceiveBandwidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InstantaneousReceiveBandwidth is emabled or not
           */
         virtual bool hasInstantaneousReceiveBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InstantaneousReceiveBandwidth
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableInstantaneousReceiveBandwidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InstantaneousReceiveBandwidth */
         virtual void clearInstantaneousReceiveBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Modulation.
           *
           * @return The value of the enumeration identified by Modulation.
           */
         virtual const uci::type::CommModulationTypeEnum& getModulation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Modulation.
           *
           * @return The value of the enumeration identified by Modulation.
           */
         virtual uci::type::CommModulationTypeEnum& getModulation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Modulation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setModulation(const uci::type::CommModulationTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Modulation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setModulation(uci::type::CommModulationTypeEnum::EnumerationItem value)
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
           * @param type = uci::type::accessorType::commModulationTypeEnum This Accessor's accessor type
           */
         virtual void enableModulation(uci::base::accessorType::AccessorType type = uci::type::accessorType::commModulationTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearModulation()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the NetworkID.
           *
           * @return The bounded list identified by NetworkID.
           */
         virtual const uci::type::CommConnectionType::NetworkID& getNetworkID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the NetworkID.
           *
           * @return The bounded list identified by NetworkID.
           */
         virtual uci::type::CommConnectionType::NetworkID& getNetworkID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the NetworkID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setNetworkID(const uci::type::CommConnectionType::NetworkID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NetworkTimeRef.
           *
           * @return The value of the simple primitive data type identified by NetworkTimeRef.
           */
         virtual xs::Boolean getNetworkTimeRef() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NetworkTimeRef.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNetworkTimeRef(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NetworkTimeRef exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NetworkTimeRef is emabled or not
           */
         virtual bool hasNetworkTimeRef() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NetworkTimeRef
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableNetworkTimeRef(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NetworkTimeRef */
         virtual void clearNetworkTimeRef()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ExternalTimeRef.
           *
           * @return The value of the simple primitive data type identified by ExternalTimeRef.
           */
         virtual xs::Boolean getExternalTimeRef() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ExternalTimeRef.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExternalTimeRef(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ExternalTimeRef exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ExternalTimeRef is emabled or not
           */
         virtual bool hasExternalTimeRef() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ExternalTimeRef
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableExternalTimeRef(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ExternalTimeRef */
         virtual void clearExternalTimeRef()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NetworkTime.
           *
           * @return The value of the simple primitive data type identified by NetworkTime.
           */
         virtual uci::type::DateTimeTypeValue getNetworkTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NetworkTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNetworkTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NetworkTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NetworkTime is emabled or not
           */
         virtual bool hasNetworkTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NetworkTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableNetworkTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NetworkTime */
         virtual void clearNetworkTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NetworkTimeUncertaintyRange.
           *
           * @return The value of the simple primitive data type identified by NetworkTimeUncertaintyRange.
           */
         virtual xs::Float getNetworkTimeUncertaintyRange() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NetworkTimeUncertaintyRange.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNetworkTimeUncertaintyRange(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NetworkTimeUncertaintyRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NetworkTimeUncertaintyRange is emabled or not
           */
         virtual bool hasNetworkTimeUncertaintyRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NetworkTimeUncertaintyRange
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableNetworkTimeUncertaintyRange(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NetworkTimeUncertaintyRange */
         virtual void clearNetworkTimeUncertaintyRange()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommConnectionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommConnectionType to copy from
           */
         CommConnectionType(const CommConnectionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommConnectionType to the contents of the CommConnectionType on
           * the right hand side (rhs) of the assignment operator.CommConnectionType [only available to derived classes]
           *
           * @param rhs The CommConnectionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CommConnectionType
           * @return A constant reference to this CommConnectionType.
           */
         const CommConnectionType& operator=(const CommConnectionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommConnectionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommConnectionType_h

