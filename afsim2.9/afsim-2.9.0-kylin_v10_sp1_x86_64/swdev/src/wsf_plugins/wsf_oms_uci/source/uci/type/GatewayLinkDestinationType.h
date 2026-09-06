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
#ifndef Uci__Type__GatewayLinkDestinationType_h
#define Uci__Type__GatewayLinkDestinationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MessageDestinationType_h)
# include "uci/type/MessageDestinationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Forms the unique pair up to assist in the mapping of Links, Communications Capabilities / Activities, and
        * Destinations.
        */
      class GatewayLinkDestinationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GatewayLinkDestinationType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayLinkDestinationType
           *
           * @return This accessor's type constant, i.e. GatewayLinkDestinationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayLinkDestinationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayLinkDestinationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Helps to map On-board IDs to data-link specific IDs. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MessageDestinationType, uci::type::accessorType::messageDestinationType> DataDestination;

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


         /** Returns the accessor that provides access to the complex content that is identified by the CommCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommCapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCommCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommCapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCommCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommCapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommCapabilityID
           */
         virtual void setCommCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommCapabilityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommCapabilityID is emabled or not
           */
         virtual bool hasCommCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommCapabilityID
           *
           * @param type = uci::type::accessorType::capabilityID_Type This Accessor's accessor type
           */
         virtual void enableCommCapabilityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommCapabilityID */
         virtual void clearCommCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommActivityID.
           */
         virtual const uci::type::ActivityID_Type& getCommActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommActivityID.
           */
         virtual uci::type::ActivityID_Type& getCommActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommActivityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommActivityID
           */
         virtual void setCommActivityID(const uci::type::ActivityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommActivityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommActivityID is emabled or not
           */
         virtual bool hasCommActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommActivityID
           *
           * @param type = uci::type::accessorType::activityID_Type This Accessor's accessor type
           */
         virtual void enableCommActivityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::activityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommActivityID */
         virtual void clearCommActivityID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataDestination.
           *
           * @return The bounded list identified by DataDestination.
           */
         virtual const uci::type::GatewayLinkDestinationType::DataDestination& getDataDestination() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataDestination.
           *
           * @return The bounded list identified by DataDestination.
           */
         virtual uci::type::GatewayLinkDestinationType::DataDestination& getDataDestination()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataDestination.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataDestination(const uci::type::GatewayLinkDestinationType::DataDestination& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayLinkDestinationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayLinkDestinationType to copy from
           */
         GatewayLinkDestinationType(const GatewayLinkDestinationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayLinkDestinationType to the contents of the
           * GatewayLinkDestinationType on the right hand side (rhs) of the assignment operator.GatewayLinkDestinationType [only
           * available to derived classes]
           *
           * @param rhs The GatewayLinkDestinationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::GatewayLinkDestinationType
           * @return A constant reference to this GatewayLinkDestinationType.
           */
         const GatewayLinkDestinationType& operator=(const GatewayLinkDestinationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayLinkDestinationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayLinkDestinationType_h

