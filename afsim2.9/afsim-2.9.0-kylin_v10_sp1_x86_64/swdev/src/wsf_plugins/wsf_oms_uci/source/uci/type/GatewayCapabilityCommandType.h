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
#ifndef Uci__Type__GatewayCapabilityCommandType_h
#define Uci__Type__GatewayCapabilityCommandType_h 1

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__CommConnectionCommandEnum_h)
# include "uci/type/CommConnectionCommandEnum.h"
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

#if !defined(Uci__Type__CommProfileID_Type_h)
# include "uci/type/CommProfileID_Type.h"
#endif

#if !defined(Uci__Type__VerboseStatusType_h)
# include "uci/type/VerboseStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the GatewayCapabilityCommandType sequence accessor class */
      class GatewayCapabilityCommandType : public virtual uci::type::CapabilityCommandBaseType {
      public:

         /** The destructor */
         virtual ~GatewayCapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayCapabilityCommandType
           *
           * @return This accessor's type constant, i.e. GatewayCapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayCapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayCapabilityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Provides a mechanism to command change to the configuration of this gateway in regards to Message and Product
           * Processing. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::GatewayConfigurationType, uci::type::accessorType::gatewayConfigurationType> GatewayConfiguration;

         /** Returns the value of the enumeration that is identified by the ConnectionRequest.
           *
           * @return The value of the enumeration identified by ConnectionRequest.
           */
         virtual const uci::type::CommConnectionCommandEnum& getConnectionRequest() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConnectionRequest.
           *
           * @return The value of the enumeration identified by ConnectionRequest.
           */
         virtual uci::type::CommConnectionCommandEnum& getConnectionRequest()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConnectionRequest.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConnectionRequest(const uci::type::CommConnectionCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConnectionRequest.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConnectionRequest(uci::type::CommConnectionCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ConnectionRequestis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasConnectionRequest() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getConnectionRequest will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commConnectionCommandEnum This Accessor's accessor type
           */
         virtual void enableConnectionRequest(uci::base::accessorType::AccessorType type = uci::type::accessorType::commConnectionCommandEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearConnectionRequest()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettingsChange.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettingsChange.
           */
         virtual const uci::type::GatewayControlType& getCommonSettingsChange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettingsChange.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettingsChange.
           */
         virtual uci::type::GatewayControlType& getCommonSettingsChange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommonSettingsChange to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommonSettingsChange
           */
         virtual void setCommonSettingsChange(const uci::type::GatewayControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommonSettingsChange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommonSettingsChange is emabled or not
           */
         virtual bool hasCommonSettingsChange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommonSettingsChange
           *
           * @param type = uci::type::accessorType::gatewayControlType This Accessor's accessor type
           */
         virtual void enableCommonSettingsChange(uci::base::accessorType::AccessorType type = uci::type::accessorType::gatewayControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommonSettingsChange */
         virtual void clearCommonSettingsChange()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GatewayConfiguration.
           *
           * @return The bounded list identified by GatewayConfiguration.
           */
         virtual const uci::type::GatewayCapabilityCommandType::GatewayConfiguration& getGatewayConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GatewayConfiguration.
           *
           * @return The bounded list identified by GatewayConfiguration.
           */
         virtual uci::type::GatewayCapabilityCommandType::GatewayConfiguration& getGatewayConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GatewayConfiguration.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGatewayConfiguration(const uci::type::GatewayCapabilityCommandType::GatewayConfiguration& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivateProfileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivateProfileID.
           */
         virtual const uci::type::CommProfileID_Type& getActivateProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivateProfileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivateProfileID.
           */
         virtual uci::type::CommProfileID_Type& getActivateProfileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivateProfileID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivateProfileID
           */
         virtual void setActivateProfileID(const uci::type::CommProfileID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActivateProfileID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActivateProfileID is emabled or not
           */
         virtual bool hasActivateProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActivateProfileID
           *
           * @param type = uci::type::accessorType::commProfileID_Type This Accessor's accessor type
           */
         virtual void enableActivateProfileID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commProfileID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActivateProfileID */
         virtual void clearActivateProfileID()
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
         GatewayCapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayCapabilityCommandType to copy from
           */
         GatewayCapabilityCommandType(const GatewayCapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayCapabilityCommandType to the contents of the
           * GatewayCapabilityCommandType on the right hand side (rhs) of the assignment operator.GatewayCapabilityCommandType
           * [only available to derived classes]
           *
           * @param rhs The GatewayCapabilityCommandType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::GatewayCapabilityCommandType
           * @return A constant reference to this GatewayCapabilityCommandType.
           */
         const GatewayCapabilityCommandType& operator=(const GatewayCapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayCapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayCapabilityCommandType_h

