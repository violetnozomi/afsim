// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__PlannedCommCapabilityType_h
#define Uci__Type__PlannedCommCapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NetworkLinkActionTypeEnum_h)
# include "uci/type/NetworkLinkActionTypeEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityCommandRankingType_h)
# include "uci/type/CapabilityCommandRankingType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__CommConnectionCommandEnum_h)
# include "uci/type/CommConnectionCommandEnum.h"
#endif

#if !defined(Uci__Type__CommProfileID_Type_h)
# include "uci/type/CommProfileID_Type.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

#if !defined(Uci__Type__CommControlType_h)
# include "uci/type/CommControlType.h"
#endif

#if !defined(Uci__Type__CryptoLocationType_h)
# include "uci/type/CryptoLocationType.h"
#endif

#if !defined(Uci__Type__CryptoRolloverCommandType_h)
# include "uci/type/CryptoRolloverCommandType.h"
#endif

#if !defined(Uci__Type__CommLinkPortMapType_h)
# include "uci/type/CommLinkPortMapType.h"
#endif

#if !defined(Uci__Type__VerboseStatusType_h)
# include "uci/type/VerboseStatusType.h"
#endif

#if !defined(Uci__Type__CommWaveformCapabilityCommandPET_h)
# include "uci/type/CommWaveformCapabilityCommandPET.h"
#endif

#if !defined(Uci__Type__LinkConnectionRequestType_h)
# include "uci/type/LinkConnectionRequestType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PlannedCommCapabilityType sequence accessor class */
      class PlannedCommCapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PlannedCommCapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. PlannedCommCapabilityType
           *
           * @return This accessor's type constant, i.e. PlannedCommCapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::plannedCommCapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PlannedCommCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a Capability instance which the communication effectivity has been allocated to. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Allows the Communication Planner to apply the key at a specified slot. Such as selecting the appropriate VINSON Key
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CryptoLocationType, uci::type::accessorType::cryptoLocationType> CryptoKey;

         /** Allows the Communication Planner to Zeroize crypto slots. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CryptoLocationType, uci::type::accessorType::cryptoLocationType> CryptoZeroize;

         /** Indicates a command to change the connection status of the individual data links managed by the waveform. Generally,
           * a link's connection status must be CONNECTED for the waveform to transmit payload data over the communication medium
           * or for the waveform to receive transmissions from the communication medium. Note that this parameter is not available
           * until after a CommActivity is created and may not be available until the waveform has joined the network. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::LinkConnectionRequestType, uci::type::accessorType::linkConnectionRequestType> LinkConnectionRequest;

         /** Returns the value of the enumeration that is identified by the Action.
           *
           * @return The value of the enumeration identified by Action.
           */
         virtual const uci::type::NetworkLinkActionTypeEnum& getAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Action.
           *
           * @return The value of the enumeration identified by Action.
           */
         virtual uci::type::NetworkLinkActionTypeEnum& getAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Action.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAction(const uci::type::NetworkLinkActionTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Action.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAction(uci::type::NetworkLinkActionTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::PlannedCommCapabilityType::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::PlannedCommCapabilityType::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::PlannedCommCapabilityType::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubsystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubsystemID is emabled or not
           */
         virtual bool hasSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubsystemID
           *
           * @param type = uci::type::accessorType::subsystemID_Type This Accessor's accessor type
           */
         virtual void enableSubsystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::subsystemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubsystemID */
         virtual void clearSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Ranking.
           *
           * @return The acecssor that provides access to the complex content that is identified by Ranking.
           */
         virtual const uci::type::CapabilityCommandRankingType& getRanking() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Ranking.
           *
           * @return The acecssor that provides access to the complex content that is identified by Ranking.
           */
         virtual uci::type::CapabilityCommandRankingType& getRanking()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Ranking to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Ranking
           */
         virtual void setRanking(const uci::type::CapabilityCommandRankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Ranking exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Ranking is emabled or not
           */
         virtual bool hasRanking() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Ranking
           *
           * @param type = uci::type::accessorType::capabilityCommandRankingType This Accessor's accessor type
           */
         virtual void enableRanking(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityCommandRankingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Ranking */
         virtual void clearRanking()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Classification.
           *
           * @return The acecssor that provides access to the complex content that is identified by Classification.
           */
         virtual const uci::type::SecurityInformationType& getClassification() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Classification.
           *
           * @return The acecssor that provides access to the complex content that is identified by Classification.
           */
         virtual uci::type::SecurityInformationType& getClassification()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Classification to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Classification
           */
         virtual void setClassification(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Classification exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Classification is emabled or not
           */
         virtual bool hasClassification() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Classification
           *
           * @param type = uci::type::accessorType::securityInformationType This Accessor's accessor type
           */
         virtual void enableClassification(uci::base::accessorType::AccessorType type = uci::type::accessorType::securityInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Classification */
         virtual void clearClassification()
            throw(uci::base::UCIException) = 0;


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


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettings.
           */
         virtual const uci::type::CommControlType& getCommonSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettings.
           */
         virtual uci::type::CommControlType& getCommonSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommonSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommonSettings
           */
         virtual void setCommonSettings(const uci::type::CommControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommonSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommonSettings is emabled or not
           */
         virtual bool hasCommonSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommonSettings
           *
           * @param type = uci::type::accessorType::commControlType This Accessor's accessor type
           */
         virtual void enableCommonSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::commControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommonSettings */
         virtual void clearCommonSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoKey.
           *
           * @return The bounded list identified by CryptoKey.
           */
         virtual const uci::type::PlannedCommCapabilityType::CryptoKey& getCryptoKey() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoKey.
           *
           * @return The bounded list identified by CryptoKey.
           */
         virtual uci::type::PlannedCommCapabilityType::CryptoKey& getCryptoKey()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CryptoKey.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCryptoKey(const uci::type::PlannedCommCapabilityType::CryptoKey& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoZeroize.
           *
           * @return The bounded list identified by CryptoZeroize.
           */
         virtual const uci::type::PlannedCommCapabilityType::CryptoZeroize& getCryptoZeroize() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoZeroize.
           *
           * @return The bounded list identified by CryptoZeroize.
           */
         virtual uci::type::PlannedCommCapabilityType::CryptoZeroize& getCryptoZeroize()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CryptoZeroize.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCryptoZeroize(const uci::type::PlannedCommCapabilityType::CryptoZeroize& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CryptoRollover.
           *
           * @return The acecssor that provides access to the complex content that is identified by CryptoRollover.
           */
         virtual const uci::type::CryptoRolloverCommandType& getCryptoRollover() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CryptoRollover.
           *
           * @return The acecssor that provides access to the complex content that is identified by CryptoRollover.
           */
         virtual uci::type::CryptoRolloverCommandType& getCryptoRollover()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CryptoRollover to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CryptoRollover
           */
         virtual void setCryptoRollover(const uci::type::CryptoRolloverCommandType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CryptoRollover exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CryptoRollover is emabled or not
           */
         virtual bool hasCryptoRollover() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CryptoRollover
           *
           * @param type = uci::type::accessorType::cryptoRolloverCommandType This Accessor's accessor type
           */
         virtual void enableCryptoRollover(uci::base::accessorType::AccessorType type = uci::type::accessorType::cryptoRolloverCommandType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CryptoRollover */
         virtual void clearCryptoRollover()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkPortAssignments.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkPortAssignments.
           */
         virtual const uci::type::CommLinkPortMapType& getLinkPortAssignments() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkPortAssignments.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkPortAssignments.
           */
         virtual uci::type::CommLinkPortMapType& getLinkPortAssignments()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LinkPortAssignments to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LinkPortAssignments
           */
         virtual void setLinkPortAssignments(const uci::type::CommLinkPortMapType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LinkPortAssignments exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LinkPortAssignments is emabled or not
           */
         virtual bool hasLinkPortAssignments() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LinkPortAssignments
           *
           * @param type = uci::type::accessorType::commLinkPortMapType This Accessor's accessor type
           */
         virtual void enableLinkPortAssignments(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLinkPortMapType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LinkPortAssignments */
         virtual void clearLinkPortAssignments()
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


         /** Returns the accessor that provides access to the complex content that is identified by the WaveformSpecificSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by WaveformSpecificSettings.
           */
         virtual const uci::type::CommWaveformCapabilityCommandPET& getWaveformSpecificSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WaveformSpecificSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by WaveformSpecificSettings.
           */
         virtual uci::type::CommWaveformCapabilityCommandPET& getWaveformSpecificSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WaveformSpecificSettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WaveformSpecificSettings
           */
         virtual void setWaveformSpecificSettings(const uci::type::CommWaveformCapabilityCommandPET& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WaveformSpecificSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WaveformSpecificSettings is emabled or not
           */
         virtual bool hasWaveformSpecificSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WaveformSpecificSettings
           *
           * @param type = uci::type::accessorType::commWaveformCapabilityCommandPET This Accessor's accessor type
           */
         virtual void enableWaveformSpecificSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::commWaveformCapabilityCommandPET)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WaveformSpecificSettings */
         virtual void clearWaveformSpecificSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkConnectionRequest.
           *
           * @return The bounded list identified by LinkConnectionRequest.
           */
         virtual const uci::type::PlannedCommCapabilityType::LinkConnectionRequest& getLinkConnectionRequest() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkConnectionRequest.
           *
           * @return The bounded list identified by LinkConnectionRequest.
           */
         virtual uci::type::PlannedCommCapabilityType::LinkConnectionRequest& getLinkConnectionRequest()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LinkConnectionRequest.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLinkConnectionRequest(const uci::type::PlannedCommCapabilityType::LinkConnectionRequest& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PlannedCommCapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PlannedCommCapabilityType to copy from
           */
         PlannedCommCapabilityType(const PlannedCommCapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PlannedCommCapabilityType to the contents of the
           * PlannedCommCapabilityType on the right hand side (rhs) of the assignment operator.PlannedCommCapabilityType [only
           * available to derived classes]
           *
           * @param rhs The PlannedCommCapabilityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::PlannedCommCapabilityType
           * @return A constant reference to this PlannedCommCapabilityType.
           */
         const PlannedCommCapabilityType& operator=(const PlannedCommCapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PlannedCommCapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PlannedCommCapabilityType_h

