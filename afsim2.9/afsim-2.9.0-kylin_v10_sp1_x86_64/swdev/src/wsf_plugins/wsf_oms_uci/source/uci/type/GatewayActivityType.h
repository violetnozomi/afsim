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
#ifndef Uci__Type__GatewayActivityType_h
#define Uci__Type__GatewayActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Type__CommConnectionStateEnum_h)
# include "uci/type/CommConnectionStateEnum.h"
#endif

#if !defined(Uci__Type__GatewayControlType_h)
# include "uci/type/GatewayControlType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__GatewayConfigurationType_h)
# include "uci/type/GatewayConfigurationType.h"
#endif

#if !defined(Uci__Type__DataDeliveryType_h)
# include "uci/type/DataDeliveryType.h"
#endif

#if !defined(Uci__Type__CommProfileID_Type_h)
# include "uci/type/CommProfileID_Type.h"
#endif

#if !defined(Uci__Type__GatewayLinkStatisticsType_h)
# include "uci/type/GatewayLinkStatisticsType.h"
#endif

#if !defined(Uci__Type__VerboseStatusType_h)
# include "uci/type/VerboseStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the GatewayActivityType sequence accessor class */
      class GatewayActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~GatewayActivityType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayActivityType
           *
           * @return This accessor's type constant, i.e. GatewayActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Provides a mechanism for a gateway to status its Gateway configuration. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::GatewayConfigurationType, uci::type::accessorType::gatewayConfigurationType> GatewayConfiguration;

         /** Provides a mechanism for a gateway to provide assurance that it has transmitted tactical and or message data within
           * the time-span this activity message covers. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DataDeliveryType, uci::type::accessorType::dataDeliveryType> AssuredTransmission;

         /** Provides a mechanism for a gateway to provide assurance that it has delivered tactical and or message data within the
           * time-span this activity message covers. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DataDeliveryType, uci::type::accessorType::dataDeliveryType> AssuredDelivery;

         /** For the periodicity of this message this is a report of the statistics provided by this capability and its links. If
           * a NetworkLinkID is specified then the granularity is that specific link. If not, It is assumed to be for the gateway
           * as a whole. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::GatewayLinkStatisticsType, uci::type::accessorType::gatewayLinkStatisticsType> LinkStatistics;

         /** Returns the value of the enumeration that is identified by the ConnectionStatus.
           *
           * @return The value of the enumeration identified by ConnectionStatus.
           */
         virtual const uci::type::CommConnectionStateEnum& getConnectionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConnectionStatus.
           *
           * @return The value of the enumeration identified by ConnectionStatus.
           */
         virtual uci::type::CommConnectionStateEnum& getConnectionStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConnectionStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConnectionStatus(const uci::type::CommConnectionStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConnectionStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConnectionStatus(uci::type::CommConnectionStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ConnectionStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasConnectionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getConnectionStatus will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commConnectionStateEnum This Accessor's accessor type
           */
         virtual void enableConnectionStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::commConnectionStateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearConnectionStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettingsStatus.
           */
         virtual const uci::type::GatewayControlType& getCommonSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettingsStatus.
           */
         virtual uci::type::GatewayControlType& getCommonSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommonSettingsStatus to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommonSettingsStatus
           */
         virtual void setCommonSettingsStatus(const uci::type::GatewayControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommonSettingsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommonSettingsStatus is emabled or not
           */
         virtual bool hasCommonSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommonSettingsStatus
           *
           * @param type = uci::type::accessorType::gatewayControlType This Accessor's accessor type
           */
         virtual void enableCommonSettingsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::gatewayControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommonSettingsStatus */
         virtual void clearCommonSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GatewayConfiguration.
           *
           * @return The bounded list identified by GatewayConfiguration.
           */
         virtual const uci::type::GatewayActivityType::GatewayConfiguration& getGatewayConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GatewayConfiguration.
           *
           * @return The bounded list identified by GatewayConfiguration.
           */
         virtual uci::type::GatewayActivityType::GatewayConfiguration& getGatewayConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GatewayConfiguration.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGatewayConfiguration(const uci::type::GatewayActivityType::GatewayConfiguration& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssuredTransmission.
           *
           * @return The bounded list identified by AssuredTransmission.
           */
         virtual const uci::type::GatewayActivityType::AssuredTransmission& getAssuredTransmission() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssuredTransmission.
           *
           * @return The bounded list identified by AssuredTransmission.
           */
         virtual uci::type::GatewayActivityType::AssuredTransmission& getAssuredTransmission()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssuredTransmission.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssuredTransmission(const uci::type::GatewayActivityType::AssuredTransmission& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssuredDelivery.
           *
           * @return The bounded list identified by AssuredDelivery.
           */
         virtual const uci::type::GatewayActivityType::AssuredDelivery& getAssuredDelivery() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssuredDelivery.
           *
           * @return The bounded list identified by AssuredDelivery.
           */
         virtual uci::type::GatewayActivityType::AssuredDelivery& getAssuredDelivery()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssuredDelivery.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssuredDelivery(const uci::type::GatewayActivityType::AssuredDelivery& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentProfileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentProfileID.
           */
         virtual const uci::type::CommProfileID_Type& getCurrentProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentProfileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentProfileID.
           */
         virtual uci::type::CommProfileID_Type& getCurrentProfileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CurrentProfileID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CurrentProfileID
           */
         virtual void setCurrentProfileID(const uci::type::CommProfileID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CurrentProfileID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CurrentProfileID is emabled or not
           */
         virtual bool hasCurrentProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CurrentProfileID
           *
           * @param type = uci::type::accessorType::commProfileID_Type This Accessor's accessor type
           */
         virtual void enableCurrentProfileID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commProfileID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CurrentProfileID */
         virtual void clearCurrentProfileID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkStatistics.
           *
           * @return The bounded list identified by LinkStatistics.
           */
         virtual const uci::type::GatewayActivityType::LinkStatistics& getLinkStatistics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkStatistics.
           *
           * @return The bounded list identified by LinkStatistics.
           */
         virtual uci::type::GatewayActivityType::LinkStatistics& getLinkStatistics()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LinkStatistics.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLinkStatistics(const uci::type::GatewayActivityType::LinkStatistics& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VerboseStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by VerboseStatus.
           */
         virtual const uci::type::VerboseStatusType& getVerboseStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VerboseStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by VerboseStatus.
           */
         virtual uci::type::VerboseStatusType& getVerboseStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VerboseStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VerboseStatus
           */
         virtual void setVerboseStatus(const uci::type::VerboseStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VerboseStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VerboseStatus is emabled or not
           */
         virtual bool hasVerboseStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VerboseStatus
           *
           * @param type = uci::type::accessorType::verboseStatusType This Accessor's accessor type
           */
         virtual void enableVerboseStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::verboseStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VerboseStatus */
         virtual void clearVerboseStatus()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayActivityType to copy from
           */
         GatewayActivityType(const GatewayActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayActivityType to the contents of the GatewayActivityType on
           * the right hand side (rhs) of the assignment operator.GatewayActivityType [only available to derived classes]
           *
           * @param rhs The GatewayActivityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::GatewayActivityType
           * @return A constant reference to this GatewayActivityType.
           */
         const GatewayActivityType& operator=(const GatewayActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayActivityType_h

