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
#ifndef Uci__Type__GatewayLinkStatisticsType_h
#define Uci__Type__GatewayLinkStatisticsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__DataRateType_h)
# include "uci/type/DataRateType.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Provides a view into the health of a Data Gateway providing feedback regarding Data Flow, Error Rates, and
        * Utilization Percentages.
        */
      class GatewayLinkStatisticsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GatewayLinkStatisticsType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayLinkStatisticsType
           *
           * @return This accessor's type constant, i.e. GatewayLinkStatisticsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayLinkStatisticsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayLinkStatisticsType& accessor)
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


         /** Returns the value of the SimplePrimitive data type that is identified by the GatewayRXUtilization.
           *
           * @return The value of the simple primitive data type identified by GatewayRXUtilization.
           */
         virtual uci::type::PercentTypeValue getGatewayRXUtilization() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GatewayRXUtilization.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGatewayRXUtilization(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GatewayRXUtilization exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GatewayRXUtilization is emabled or not
           */
         virtual bool hasGatewayRXUtilization() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GatewayRXUtilization
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableGatewayRXUtilization(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GatewayRXUtilization */
         virtual void clearGatewayRXUtilization()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GatewayTXUtilization.
           *
           * @return The value of the simple primitive data type identified by GatewayTXUtilization.
           */
         virtual uci::type::PercentTypeValue getGatewayTXUtilization() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GatewayTXUtilization.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGatewayTXUtilization(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GatewayTXUtilization exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GatewayTXUtilization is emabled or not
           */
         virtual bool hasGatewayTXUtilization() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GatewayTXUtilization
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableGatewayTXUtilization(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GatewayTXUtilization */
         virtual void clearGatewayTXUtilization()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RXDataErrorRate.
           *
           * @return The value of the simple primitive data type identified by RXDataErrorRate.
           */
         virtual uci::type::PercentTypeValue getRXDataErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RXDataErrorRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRXDataErrorRate(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RXDataErrorRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RXDataErrorRate is emabled or not
           */
         virtual bool hasRXDataErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RXDataErrorRate
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableRXDataErrorRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RXDataErrorRate */
         virtual void clearRXDataErrorRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RXDataRate.
           *
           * @return The value of the simple primitive data type identified by RXDataRate.
           */
         virtual uci::type::DataRateTypeValue getRXDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RXDataRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRXDataRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RXDataRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RXDataRate is emabled or not
           */
         virtual bool hasRXDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RXDataRate
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableRXDataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RXDataRate */
         virtual void clearRXDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RXMessageErrorRate.
           *
           * @return The value of the simple primitive data type identified by RXMessageErrorRate.
           */
         virtual uci::type::PercentTypeValue getRXMessageErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RXMessageErrorRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRXMessageErrorRate(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RXMessageErrorRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RXMessageErrorRate is emabled or not
           */
         virtual bool hasRXMessageErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RXMessageErrorRate
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableRXMessageErrorRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RXMessageErrorRate */
         virtual void clearRXMessageErrorRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RXMessageRate.
           *
           * @return The value of the simple primitive data type identified by RXMessageRate.
           */
         virtual xs::Double getRXMessageRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RXMessageRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRXMessageRate(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RXMessageRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RXMessageRate is emabled or not
           */
         virtual bool hasRXMessageRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RXMessageRate
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableRXMessageRate(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RXMessageRate */
         virtual void clearRXMessageRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TXDataErrorRate.
           *
           * @return The value of the simple primitive data type identified by TXDataErrorRate.
           */
         virtual uci::type::PercentTypeValue getTXDataErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TXDataErrorRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTXDataErrorRate(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TXDataErrorRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TXDataErrorRate is emabled or not
           */
         virtual bool hasTXDataErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TXDataErrorRate
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableTXDataErrorRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TXDataErrorRate */
         virtual void clearTXDataErrorRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TXDataRate.
           *
           * @return The value of the simple primitive data type identified by TXDataRate.
           */
         virtual uci::type::DataRateTypeValue getTXDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TXDataRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTXDataRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TXDataRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TXDataRate is emabled or not
           */
         virtual bool hasTXDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TXDataRate
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableTXDataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TXDataRate */
         virtual void clearTXDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TXMessageErrorRate.
           *
           * @return The value of the simple primitive data type identified by TXMessageErrorRate.
           */
         virtual uci::type::PercentTypeValue getTXMessageErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TXMessageErrorRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTXMessageErrorRate(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TXMessageErrorRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TXMessageErrorRate is emabled or not
           */
         virtual bool hasTXMessageErrorRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TXMessageErrorRate
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableTXMessageErrorRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TXMessageErrorRate */
         virtual void clearTXMessageErrorRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TXMessageRate.
           *
           * @return The value of the simple primitive data type identified by TXMessageRate.
           */
         virtual xs::Double getTXMessageRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TXMessageRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTXMessageRate(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TXMessageRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TXMessageRate is emabled or not
           */
         virtual bool hasTXMessageRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TXMessageRate
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableTXMessageRate(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TXMessageRate */
         virtual void clearTXMessageRate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayLinkStatisticsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayLinkStatisticsType to copy from
           */
         GatewayLinkStatisticsType(const GatewayLinkStatisticsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayLinkStatisticsType to the contents of the
           * GatewayLinkStatisticsType on the right hand side (rhs) of the assignment operator.GatewayLinkStatisticsType [only
           * available to derived classes]
           *
           * @param rhs The GatewayLinkStatisticsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::GatewayLinkStatisticsType
           * @return A constant reference to this GatewayLinkStatisticsType.
           */
         const GatewayLinkStatisticsType& operator=(const GatewayLinkStatisticsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayLinkStatisticsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayLinkStatisticsType_h

