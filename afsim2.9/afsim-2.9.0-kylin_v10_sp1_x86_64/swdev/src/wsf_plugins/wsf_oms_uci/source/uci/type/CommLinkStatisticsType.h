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
#ifndef Uci__Type__CommLinkStatisticsType_h
#define Uci__Type__CommLinkStatisticsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__DataRateType_h)
# include "uci/type/DataRateType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to report current statistics of a link on a communications capability. */
      class CommLinkStatisticsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLinkStatisticsType()
         { }

         /** Returns this accessor's type constant, i.e. CommLinkStatisticsType
           *
           * @return This accessor's type constant, i.e. CommLinkStatisticsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLinkStatisticsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLinkStatisticsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetworkLinkID.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetworkLinkID.
           */
         virtual const uci::type::NetworkLinkID_Type& getNetworkLinkID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetworkLinkID.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetworkLinkID.
           */
         virtual uci::type::NetworkLinkID_Type& getNetworkLinkID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NetworkLinkID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NetworkLinkID
           */
         virtual void setNetworkLinkID(const uci::type::NetworkLinkID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NetworkLinkID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NetworkLinkID is emabled or not
           */
         virtual bool hasNetworkLinkID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NetworkLinkID
           *
           * @param type = uci::type::accessorType::networkLinkID_Type This Accessor's accessor type
           */
         virtual void enableNetworkLinkID(uci::base::accessorType::AccessorType type = uci::type::accessorType::networkLinkID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NetworkLinkID */
         virtual void clearNetworkLinkID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CarrierToNoiseRatio.
           *
           * @return The value of the simple primitive data type identified by CarrierToNoiseRatio.
           */
         virtual uci::type::DecibelTypeValue getCarrierToNoiseRatio() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CarrierToNoiseRatio.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCarrierToNoiseRatio(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CarrierToNoiseRatio exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CarrierToNoiseRatio is emabled or not
           */
         virtual bool hasCarrierToNoiseRatio() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CarrierToNoiseRatio
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableCarrierToNoiseRatio(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CarrierToNoiseRatio */
         virtual void clearCarrierToNoiseRatio()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SignalToNoiseRatio.
           *
           * @return The value of the simple primitive data type identified by SignalToNoiseRatio.
           */
         virtual uci::type::DecibelTypeValue getSignalToNoiseRatio() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SignalToNoiseRatio.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSignalToNoiseRatio(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalToNoiseRatio exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalToNoiseRatio is emabled or not
           */
         virtual bool hasSignalToNoiseRatio() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalToNoiseRatio
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableSignalToNoiseRatio(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalToNoiseRatio */
         virtual void clearSignalToNoiseRatio()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CurrentDataRate.
           *
           * @return The value of the simple primitive data type identified by CurrentDataRate.
           */
         virtual uci::type::DataRateTypeValue getCurrentDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CurrentDataRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCurrentDataRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CurrentDataRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CurrentDataRate is emabled or not
           */
         virtual bool hasCurrentDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CurrentDataRate
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableCurrentDataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CurrentDataRate */
         virtual void clearCurrentDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MessageErrorRate.
           *
           * @return The value of the simple primitive data type identified by MessageErrorRate.
           */
         virtual uci::type::PercentTypeValue getMessageErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MessageErrorRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMessageErrorRate(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MessageErrorRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MessageErrorRate is emabled or not
           */
         virtual bool hasMessageErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MessageErrorRate
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableMessageErrorRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MessageErrorRate */
         virtual void clearMessageErrorRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BitErrorRate.
           *
           * @return The value of the simple primitive data type identified by BitErrorRate.
           */
         virtual uci::type::PercentTypeValue getBitErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BitErrorRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBitErrorRate(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BitErrorRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BitErrorRate is emabled or not
           */
         virtual bool hasBitErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BitErrorRate
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableBitErrorRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BitErrorRate */
         virtual void clearBitErrorRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TxLatency.
           *
           * @return The value of the simple primitive data type identified by TxLatency.
           */
         virtual uci::type::DurationTypeValue getTxLatency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TxLatency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTxLatency(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TxLatency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TxLatency is emabled or not
           */
         virtual bool hasTxLatency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TxLatency
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableTxLatency(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TxLatency */
         virtual void clearTxLatency()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RxLatency.
           *
           * @return The value of the simple primitive data type identified by RxLatency.
           */
         virtual uci::type::DurationTypeValue getRxLatency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RxLatency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRxLatency(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RxLatency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RxLatency is emabled or not
           */
         virtual bool hasRxLatency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RxLatency
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableRxLatency(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RxLatency */
         virtual void clearRxLatency()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FixedDataRate.
           *
           * @return The value of the simple primitive data type identified by FixedDataRate.
           */
         virtual xs::Boolean getFixedDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FixedDataRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFixedDataRate(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FixedDataRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FixedDataRate is emabled or not
           */
         virtual bool hasFixedDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FixedDataRate
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableFixedDataRate(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FixedDataRate */
         virtual void clearFixedDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChannelID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChannelID.
           */
         virtual const uci::type::ForeignKeyType& getChannelID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChannelID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChannelID.
           */
         virtual uci::type::ForeignKeyType& getChannelID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChannelID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChannelID
           */
         virtual void setChannelID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChannelID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChannelID is emabled or not
           */
         virtual bool hasChannelID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChannelID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableChannelID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChannelID */
         virtual void clearChannelID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLinkStatisticsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLinkStatisticsType to copy from
           */
         CommLinkStatisticsType(const CommLinkStatisticsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLinkStatisticsType to the contents of the
           * CommLinkStatisticsType on the right hand side (rhs) of the assignment operator.CommLinkStatisticsType [only available
           * to derived classes]
           *
           * @param rhs The CommLinkStatisticsType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CommLinkStatisticsType
           * @return A constant reference to this CommLinkStatisticsType.
           */
         const CommLinkStatisticsType& operator=(const CommLinkStatisticsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLinkStatisticsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLinkStatisticsType_h

