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
#ifndef Uci__Type__CommIFDL_NetLinkStatusType_h
#define Uci__Type__CommIFDL_NetLinkStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__CommIFDL_CommModeEnum_h)
# include "uci/type/CommIFDL_CommModeEnum.h"
#endif

#if !defined(Uci__Type__CommIFDL_TxRateEnum_h)
# include "uci/type/CommIFDL_TxRateEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommIFDL_NetLinkStatusType sequence accessor class */
      class CommIFDL_NetLinkStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommIFDL_NetLinkStatusType()
         { }

         /** Returns this accessor's type constant, i.e. CommIFDL_NetLinkStatusType
           *
           * @return This accessor's type constant, i.e. CommIFDL_NetLinkStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commIFDL_NetLinkStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommIFDL_NetLinkStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Identifies the Flight of the peer.; [Minimum inclusive: 1] [Maximum inclusive: 999] */
         typedef uci::base::UnsignedIntAccessor PeerFlightID;
         //  The type of primitive data accessed by PeerFlightID
         typedef xs::UnsignedInt PeerFlightIDValue;

         /** Identifies the position in the flight of the peer.; [Minimum inclusive: 1] [Maximum inclusive: 4] */
         typedef uci::base::UnsignedIntAccessor PeerFlightPosition;
         //  The type of primitive data accessed by PeerFlightPosition
         typedef xs::UnsignedInt PeerFlightPositionValue;

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


         /** Returns the value of the SimplePrimitive data type that is identified by the PeerFlightID.
           *
           * @return The value of the simple primitive data type identified by PeerFlightID.
           */
         virtual uci::type::CommIFDL_NetLinkStatusType::PeerFlightIDValue getPeerFlightID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeerFlightID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeerFlightID(uci::type::CommIFDL_NetLinkStatusType::PeerFlightIDValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeerFlightPosition.
           *
           * @return The value of the simple primitive data type identified by PeerFlightPosition.
           */
         virtual uci::type::CommIFDL_NetLinkStatusType::PeerFlightPositionValue getPeerFlightPosition() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeerFlightPosition.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeerFlightPosition(uci::type::CommIFDL_NetLinkStatusType::PeerFlightPositionValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommunicationMode.
           *
           * @return The value of the enumeration identified by CommunicationMode.
           */
         virtual const uci::type::CommIFDL_CommModeEnum& getCommunicationMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommunicationMode.
           *
           * @return The value of the enumeration identified by CommunicationMode.
           */
         virtual uci::type::CommIFDL_CommModeEnum& getCommunicationMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommunicationMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommunicationMode(const uci::type::CommIFDL_CommModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommunicationMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommunicationMode(uci::type::CommIFDL_CommModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmissionRate.
           *
           * @return The value of the enumeration identified by TransmissionRate.
           */
         virtual const uci::type::CommIFDL_TxRateEnum& getTransmissionRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TransmissionRate.
           *
           * @return The value of the enumeration identified by TransmissionRate.
           */
         virtual uci::type::CommIFDL_TxRateEnum& getTransmissionRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmissionRate.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmissionRate(const uci::type::CommIFDL_TxRateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TransmissionRate.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTransmissionRate(uci::type::CommIFDL_TxRateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TransmissionRateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTransmissionRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTransmissionRate will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commIFDL_TxRateEnum This Accessor's accessor type
           */
         virtual void enableTransmissionRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::commIFDL_TxRateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTransmissionRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaxRangeApproached.
           *
           * @return The value of the simple primitive data type identified by MaxRangeApproached.
           */
         virtual xs::Boolean getMaxRangeApproached() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaxRangeApproached.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaxRangeApproached(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaxRangeApproached exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaxRangeApproached is emabled or not
           */
         virtual bool hasMaxRangeApproached() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaxRangeApproached
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableMaxRangeApproached(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaxRangeApproached */
         virtual void clearMaxRangeApproached()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommIFDL_NetLinkStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommIFDL_NetLinkStatusType to copy from
           */
         CommIFDL_NetLinkStatusType(const CommIFDL_NetLinkStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommIFDL_NetLinkStatusType to the contents of the
           * CommIFDL_NetLinkStatusType on the right hand side (rhs) of the assignment operator.CommIFDL_NetLinkStatusType [only
           * available to derived classes]
           *
           * @param rhs The CommIFDL_NetLinkStatusType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommIFDL_NetLinkStatusType
           * @return A constant reference to this CommIFDL_NetLinkStatusType.
           */
         const CommIFDL_NetLinkStatusType& operator=(const CommIFDL_NetLinkStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommIFDL_NetLinkStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommIFDL_NetLinkStatusType_h

