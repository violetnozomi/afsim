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
#ifndef Uci__Type__CommCDL_ControlType_h
#define Uci__Type__CommCDL_ControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__CommCDL_RemoteMessageProcessingEnum_h)
# include "uci/type/CommCDL_RemoteMessageProcessingEnum.h"
#endif

#if !defined(Uci__Type__DataPortType_h)
# include "uci/type/DataPortType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to perform CDL Specific tuning of a communications capability. */
      class CommCDL_ControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommCDL_ControlType()
         { }

         /** Returns this accessor's type constant, i.e. CommCDL_ControlType
           *
           * @return This accessor's type constant, i.e. CommCDL_ControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commCDL_ControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommCDL_ControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] [Maximum inclusive: 255] */
         typedef uci::base::IntAccessor LinkChannelPriority;
         //  The type of primitive data accessed by LinkChannelPriority
         typedef xs::Int LinkChannelPriorityValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 255] */
         typedef uci::base::IntAccessor UDP_MonitorPeriod;
         //  The type of primitive data accessed by UDP_MonitorPeriod
         typedef xs::Int UDP_MonitorPeriodValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the LinkChannelPriority.
           *
           * @return The value of the simple primitive data type identified by LinkChannelPriority.
           */
         virtual uci::type::CommCDL_ControlType::LinkChannelPriorityValue getLinkChannelPriority() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LinkChannelPriority.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLinkChannelPriority(uci::type::CommCDL_ControlType::LinkChannelPriorityValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LinkChannelPriority exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LinkChannelPriority is emabled or not
           */
         virtual bool hasLinkChannelPriority() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LinkChannelPriority
           *
           * @param type = uci::type::commCDL_ControlType::accessorType::linkChannelPriority This Accessor's accessor type
           */
         virtual void enableLinkChannelPriority(uci::base::accessorType::AccessorType type = uci::type::commCDL_ControlType::accessorType::linkChannelPriority)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LinkChannelPriority */
         virtual void clearLinkChannelPriority()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RF_CommandProcessing.
           *
           * @return The value of the enumeration identified by RF_CommandProcessing.
           */
         virtual const uci::type::CommCDL_RemoteMessageProcessingEnum& getRF_CommandProcessing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RF_CommandProcessing.
           *
           * @return The value of the enumeration identified by RF_CommandProcessing.
           */
         virtual uci::type::CommCDL_RemoteMessageProcessingEnum& getRF_CommandProcessing()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RF_CommandProcessing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRF_CommandProcessing(const uci::type::CommCDL_RemoteMessageProcessingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RF_CommandProcessing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRF_CommandProcessing(uci::type::CommCDL_RemoteMessageProcessingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield RF_CommandProcessingis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasRF_CommandProcessing() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getRF_CommandProcessing will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::commCDL_RemoteMessageProcessingEnum This Accessor's accessor type
           */
         virtual void enableRF_CommandProcessing(uci::base::accessorType::AccessorType type = uci::type::accessorType::commCDL_RemoteMessageProcessingEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearRF_CommandProcessing()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UDP_MonitorPeriod.
           *
           * @return The value of the simple primitive data type identified by UDP_MonitorPeriod.
           */
         virtual uci::type::CommCDL_ControlType::UDP_MonitorPeriodValue getUDP_MonitorPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UDP_MonitorPeriod.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUDP_MonitorPeriod(uci::type::CommCDL_ControlType::UDP_MonitorPeriodValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UDP_MonitorPeriod exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UDP_MonitorPeriod is emabled or not
           */
         virtual bool hasUDP_MonitorPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UDP_MonitorPeriod
           *
           * @param type = uci::type::commCDL_ControlType::accessorType::uDP_MonitorPeriod This Accessor's accessor type
           */
         virtual void enableUDP_MonitorPeriod(uci::base::accessorType::AccessorType type = uci::type::commCDL_ControlType::accessorType::uDP_MonitorPeriod)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UDP_MonitorPeriod */
         virtual void clearUDP_MonitorPeriod()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAN_PortSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAN_PortSettings.
           */
         virtual const uci::type::DataPortType& getLAN_PortSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAN_PortSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAN_PortSettings.
           */
         virtual uci::type::DataPortType& getLAN_PortSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LAN_PortSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LAN_PortSettings
           */
         virtual void setLAN_PortSettings(const uci::type::DataPortType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LAN_PortSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LAN_PortSettings is emabled or not
           */
         virtual bool hasLAN_PortSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LAN_PortSettings
           *
           * @param type = uci::type::accessorType::dataPortType This Accessor's accessor type
           */
         virtual void enableLAN_PortSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataPortType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LAN_PortSettings */
         virtual void clearLAN_PortSettings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommCDL_ControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommCDL_ControlType to copy from
           */
         CommCDL_ControlType(const CommCDL_ControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommCDL_ControlType to the contents of the CommCDL_ControlType on
           * the right hand side (rhs) of the assignment operator.CommCDL_ControlType [only available to derived classes]
           *
           * @param rhs The CommCDL_ControlType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CommCDL_ControlType
           * @return A constant reference to this CommCDL_ControlType.
           */
         const CommCDL_ControlType& operator=(const CommCDL_ControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommCDL_ControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommCDL_ControlType_h

