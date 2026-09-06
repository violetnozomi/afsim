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
#ifndef Uci__Type__Link16MetadataType_h
#define Uci__Type__Link16MetadataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

#if !defined(Uci__Type__TrackNumberType_h)
# include "uci/type/TrackNumberType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__TrackNumberOrEntityType_h)
# include "uci/type/TrackNumberOrEntityType.h"
#endif

#if !defined(Uci__Type__GenericUnitTypeEnum_h)
# include "uci/type/GenericUnitTypeEnum.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__PositionTimeQualityEnum_h)
# include "uci/type/PositionTimeQualityEnum.h"
#endif

#if !defined(Uci__Type__SiteEnum_h)
# include "uci/type/SiteEnum.h"
#endif

#if !defined(Uci__Type__OperationalCapabilityEnum_h)
# include "uci/type/OperationalCapabilityEnum.h"
#endif

#if !defined(Uci__Type__ConnectionStatusEnum_h)
# include "uci/type/ConnectionStatusEnum.h"
#endif

#if !defined(Uci__Type__ReconnaissanceStatusType_h)
# include "uci/type/ReconnaissanceStatusType.h"
#endif

#if !defined(Uci__Type__NPSI_Enum_h)
# include "uci/type/NPSI_Enum.h"
#endif

#if !defined(Uci__Type__RadarChannelIndicatorEnum_h)
# include "uci/type/RadarChannelIndicatorEnum.h"
#endif

#if !defined(Uci__Type__RadarChannelType_h)
# include "uci/type/RadarChannelType.h"
#endif

#if !defined(Uci__Type__MissileChannelType_h)
# include "uci/type/MissileChannelType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DefensiveJammerSystemInfoType_h)
# include "uci/type/DefensiveJammerSystemInfoType.h"
#endif

#if !defined(Uci__Type__JammerStationType_h)
# include "uci/type/JammerStationType.h"
#endif

#if !defined(Uci__Type__CommCountermeasureSetEnum_h)
# include "uci/type/CommCountermeasureSetEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the Link16MetadataType sequence accessor class */
      class Link16MetadataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~Link16MetadataType()
         { }

         /** Returns this accessor's type constant, i.e. Link16MetadataType
           *
           * @return This accessor's type constant, i.e. Link16MetadataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::link16MetadataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const Link16MetadataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] [Maximum inclusive: 127] */
         typedef uci::base::UnsignedIntAccessor ControlChannel;
         //  The type of primitive data accessed by ControlChannel
         typedef xs::UnsignedInt ControlChannelValue;

         /** Indicates the type and status of a defensive jammer. [Minimum occurrences: 0] [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::DefensiveJammerSystemInfoType, uci::type::accessorType::defensiveJammerSystemInfoType> DefensiveJammerSystemInfo;

         /** Indicates the type and status of an external offensive jammer. [Minimum occurrences: 0] [Maximum occurrences: 10] */
         typedef uci::base::BoundedList<uci::type::JammerStationType, uci::type::accessorType::jammerStationType> JammerStation;

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


         /** Returns the value of the string data type that is identified by the SourceTrack.
           *
           * @return The value of the string data type identified by SourceTrack.
           */
         virtual const uci::type::TrackNumberType& getSourceTrack() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SourceTrack.
           *
           * @return The value of the string data type identified by SourceTrack.
           */
         virtual uci::type::TrackNumberType& getSourceTrack()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SourceTrack to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSourceTrack(const uci::type::TrackNumberType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SourceTrack to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSourceTrack(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SourceTrack to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSourceTrack(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SourceTrack exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SourceTrack is emabled or not
           */
         virtual bool hasSourceTrack() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SourceTrack
           *
           * @param type = uci::type::accessorType::trackNumberType This Accessor's accessor type
           */
         virtual void enableSourceTrack(uci::base::accessorType::AccessorType type = uci::type::accessorType::trackNumberType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SourceTrack */
         virtual void clearSourceTrack()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Airborne.
           *
           * @return The value of the simple primitive data type identified by Airborne.
           */
         virtual xs::Boolean getAirborne() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Airborne.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAirborne(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Airborne exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Airborne is emabled or not
           */
         virtual bool hasAirborne() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Airborne
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAirborne(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Airborne */
         virtual void clearAirborne()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ControlChannel.
           *
           * @return The value of the simple primitive data type identified by ControlChannel.
           */
         virtual uci::type::Link16MetadataType::ControlChannelValue getControlChannel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ControlChannel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setControlChannel(uci::type::Link16MetadataType::ControlChannelValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ControlChannel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ControlChannel is emabled or not
           */
         virtual bool hasControlChannel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ControlChannel
           *
           * @param type = uci::type::link16MetadataType::accessorType::controlChannel This Accessor's accessor type
           */
         virtual void enableControlChannel(uci::base::accessorType::AccessorType type = uci::type::link16MetadataType::accessorType::controlChannel)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ControlChannel */
         virtual void clearControlChannel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FlightLeadIndicator.
           *
           * @return The value of the simple primitive data type identified by FlightLeadIndicator.
           */
         virtual xs::Boolean getFlightLeadIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FlightLeadIndicator.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFlightLeadIndicator(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FlightLeadIndicator exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FlightLeadIndicator is emabled or not
           */
         virtual bool hasFlightLeadIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FlightLeadIndicator
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableFlightLeadIndicator(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FlightLeadIndicator */
         virtual void clearFlightLeadIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FlightLead.
           *
           * @return The acecssor that provides access to the complex content that is identified by FlightLead.
           */
         virtual const uci::type::TrackNumberOrEntityType& getFlightLead() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FlightLead.
           *
           * @return The acecssor that provides access to the complex content that is identified by FlightLead.
           */
         virtual uci::type::TrackNumberOrEntityType& getFlightLead()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FlightLead to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FlightLead
           */
         virtual void setFlightLead(const uci::type::TrackNumberOrEntityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FlightLead exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FlightLead is emabled or not
           */
         virtual bool hasFlightLead() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FlightLead
           *
           * @param type = uci::type::accessorType::trackNumberOrEntityType This Accessor's accessor type
           */
         virtual void enableFlightLead(uci::base::accessorType::AccessorType type = uci::type::accessorType::trackNumberOrEntityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FlightLead */
         virtual void clearFlightLead()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GenericUnitType.
           *
           * @return The value of the enumeration identified by GenericUnitType.
           */
         virtual const uci::type::GenericUnitTypeEnum& getGenericUnitType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GenericUnitType.
           *
           * @return The value of the enumeration identified by GenericUnitType.
           */
         virtual uci::type::GenericUnitTypeEnum& getGenericUnitType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GenericUnitType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGenericUnitType(const uci::type::GenericUnitTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GenericUnitType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGenericUnitType(uci::type::GenericUnitTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield GenericUnitTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasGenericUnitType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getGenericUnitType will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::genericUnitTypeEnum This Accessor's accessor type
           */
         virtual void enableGenericUnitType(uci::base::accessorType::AccessorType type = uci::type::accessorType::genericUnitTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearGenericUnitType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MissionCommanderIndicator.
           *
           * @return The value of the simple primitive data type identified by MissionCommanderIndicator.
           */
         virtual xs::Boolean getMissionCommanderIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MissionCommanderIndicator.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMissionCommanderIndicator(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionCommanderIndicator exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionCommanderIndicator is emabled or not
           */
         virtual bool hasMissionCommanderIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionCommanderIndicator
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableMissionCommanderIndicator(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionCommanderIndicator */
         virtual void clearMissionCommanderIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PrecisionTime.
           *
           * @return The value of the simple primitive data type identified by PrecisionTime.
           */
         virtual uci::type::DateTimeTypeValue getPrecisionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PrecisionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPrecisionTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PrecisionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PrecisionTime is emabled or not
           */
         virtual bool hasPrecisionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PrecisionTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enablePrecisionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PrecisionTime */
         virtual void clearPrecisionTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PositionTimeQuality.
           *
           * @return The value of the enumeration identified by PositionTimeQuality.
           */
         virtual const uci::type::PositionTimeQualityEnum& getPositionTimeQuality() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PositionTimeQuality.
           *
           * @return The value of the enumeration identified by PositionTimeQuality.
           */
         virtual uci::type::PositionTimeQualityEnum& getPositionTimeQuality()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PositionTimeQuality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPositionTimeQuality(const uci::type::PositionTimeQualityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PositionTimeQuality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPositionTimeQuality(uci::type::PositionTimeQualityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PositionTimeQualityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPositionTimeQuality() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPositionTimeQuality will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::positionTimeQualityEnum This Accessor's accessor type
           */
         virtual void enablePositionTimeQuality(uci::base::accessorType::AccessorType type = uci::type::accessorType::positionTimeQualityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPositionTimeQuality()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Site.
           *
           * @return The value of the enumeration identified by Site.
           */
         virtual const uci::type::SiteEnum& getSite() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Site.
           *
           * @return The value of the enumeration identified by Site.
           */
         virtual uci::type::SiteEnum& getSite()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Site.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSite(const uci::type::SiteEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Site.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSite(uci::type::SiteEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Siteis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSite() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSite will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::siteEnum This Accessor's accessor type
           */
         virtual void enableSite(uci::base::accessorType::AccessorType type = uci::type::accessorType::siteEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSite()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalCapability.
           *
           * @return The value of the enumeration identified by OperationalCapability.
           */
         virtual const uci::type::OperationalCapabilityEnum& getOperationalCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalCapability.
           *
           * @return The value of the enumeration identified by OperationalCapability.
           */
         virtual uci::type::OperationalCapabilityEnum& getOperationalCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalCapability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalCapability(const uci::type::OperationalCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalCapability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalCapability(uci::type::OperationalCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield OperationalCapabilityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasOperationalCapability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getOperationalCapability will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::operationalCapabilityEnum This Accessor's accessor type
           */
         virtual void enableOperationalCapability(uci::base::accessorType::AccessorType type = uci::type::accessorType::operationalCapabilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearOperationalCapability()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Link16_Status.
           *
           * @return The value of the enumeration identified by Link16_Status.
           */
         virtual const uci::type::ConnectionStatusEnum& getLink16_Status() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Link16_Status.
           *
           * @return The value of the enumeration identified by Link16_Status.
           */
         virtual uci::type::ConnectionStatusEnum& getLink16_Status()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Link16_Status.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLink16_Status(const uci::type::ConnectionStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Link16_Status.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLink16_Status(uci::type::ConnectionStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Link16_Statusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLink16_Status() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLink16_Status will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::connectionStatusEnum This Accessor's accessor type
           */
         virtual void enableLink16_Status(uci::base::accessorType::AccessorType type = uci::type::accessorType::connectionStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLink16_Status()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReconnaissanceStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReconnaissanceStatus.
           */
         virtual const uci::type::ReconnaissanceStatusType& getReconnaissanceStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReconnaissanceStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReconnaissanceStatus.
           */
         virtual uci::type::ReconnaissanceStatusType& getReconnaissanceStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReconnaissanceStatus to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReconnaissanceStatus
           */
         virtual void setReconnaissanceStatus(const uci::type::ReconnaissanceStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReconnaissanceStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReconnaissanceStatus is emabled or not
           */
         virtual bool hasReconnaissanceStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReconnaissanceStatus
           *
           * @param type = uci::type::accessorType::reconnaissanceStatusType This Accessor's accessor type
           */
         virtual void enableReconnaissanceStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::reconnaissanceStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReconnaissanceStatus */
         virtual void clearReconnaissanceStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NetworkParticipationStatusIndicator.
           *
           * @return The value of the enumeration identified by NetworkParticipationStatusIndicator.
           */
         virtual const uci::type::NPSI_Enum& getNetworkParticipationStatusIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NetworkParticipationStatusIndicator.
           *
           * @return The value of the enumeration identified by NetworkParticipationStatusIndicator.
           */
         virtual uci::type::NPSI_Enum& getNetworkParticipationStatusIndicator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NetworkParticipationStatusIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNetworkParticipationStatusIndicator(const uci::type::NPSI_Enum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NetworkParticipationStatusIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNetworkParticipationStatusIndicator(uci::type::NPSI_Enum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield NetworkParticipationStatusIndicatoris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasNetworkParticipationStatusIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getNetworkParticipationStatusIndicator will return the optional field and not
           * throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::nPSI_Enum This Accessor's accessor type
           */
         virtual void enableNetworkParticipationStatusIndicator(uci::base::accessorType::AccessorType type = uci::type::accessorType::nPSI_Enum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearNetworkParticipationStatusIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RadarChannelIndicator.
           *
           * @return The value of the enumeration identified by RadarChannelIndicator.
           */
         virtual const uci::type::RadarChannelIndicatorEnum& getRadarChannelIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RadarChannelIndicator.
           *
           * @return The value of the enumeration identified by RadarChannelIndicator.
           */
         virtual uci::type::RadarChannelIndicatorEnum& getRadarChannelIndicator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RadarChannelIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRadarChannelIndicator(const uci::type::RadarChannelIndicatorEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RadarChannelIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRadarChannelIndicator(uci::type::RadarChannelIndicatorEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield RadarChannelIndicatoris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasRadarChannelIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getRadarChannelIndicator will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::radarChannelIndicatorEnum This Accessor's accessor type
           */
         virtual void enableRadarChannelIndicator(uci::base::accessorType::AccessorType type = uci::type::accessorType::radarChannelIndicatorEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearRadarChannelIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RadarChannel.
           *
           * @return The value of the simple primitive data type identified by RadarChannel.
           */
         virtual uci::type::RadarChannelTypeValue getRadarChannel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RadarChannel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRadarChannel(uci::type::RadarChannelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RadarChannel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RadarChannel is emabled or not
           */
         virtual bool hasRadarChannel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RadarChannel
           *
           * @param type = uci::type::accessorType::radarChannelType This Accessor's accessor type
           */
         virtual void enableRadarChannel(uci::base::accessorType::AccessorType type = uci::type::accessorType::radarChannelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RadarChannel */
         virtual void clearRadarChannel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MissileChannel.
           *
           * @return The value of the simple primitive data type identified by MissileChannel.
           */
         virtual uci::type::MissileChannelTypeValue getMissileChannel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MissileChannel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMissileChannel(uci::type::MissileChannelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissileChannel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissileChannel is emabled or not
           */
         virtual bool hasMissileChannel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissileChannel
           *
           * @param type = uci::type::accessorType::missileChannelType This Accessor's accessor type
           */
         virtual void enableMissileChannel(uci::base::accessorType::AccessorType type = uci::type::accessorType::missileChannelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissileChannel */
         virtual void clearMissileChannel()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DefensiveJammerSystemInfo.
           *
           * @return The bounded list identified by DefensiveJammerSystemInfo.
           */
         virtual const uci::type::Link16MetadataType::DefensiveJammerSystemInfo& getDefensiveJammerSystemInfo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DefensiveJammerSystemInfo.
           *
           * @return The bounded list identified by DefensiveJammerSystemInfo.
           */
         virtual uci::type::Link16MetadataType::DefensiveJammerSystemInfo& getDefensiveJammerSystemInfo()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DefensiveJammerSystemInfo.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDefensiveJammerSystemInfo(const uci::type::Link16MetadataType::DefensiveJammerSystemInfo& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the JammerStation.
           *
           * @return The bounded list identified by JammerStation.
           */
         virtual const uci::type::Link16MetadataType::JammerStation& getJammerStation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the JammerStation.
           *
           * @return The bounded list identified by JammerStation.
           */
         virtual uci::type::Link16MetadataType::JammerStation& getJammerStation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the JammerStation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setJammerStation(const uci::type::Link16MetadataType::JammerStation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommCountermeasureSet.
           *
           * @return The value of the enumeration identified by CommCountermeasureSet.
           */
         virtual const uci::type::CommCountermeasureSetEnum& getCommCountermeasureSet() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommCountermeasureSet.
           *
           * @return The value of the enumeration identified by CommCountermeasureSet.
           */
         virtual uci::type::CommCountermeasureSetEnum& getCommCountermeasureSet()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommCountermeasureSet.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommCountermeasureSet(const uci::type::CommCountermeasureSetEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommCountermeasureSet.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommCountermeasureSet(uci::type::CommCountermeasureSetEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield CommCountermeasureSetis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasCommCountermeasureSet() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getCommCountermeasureSet will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::commCountermeasureSetEnum This Accessor's accessor type
           */
         virtual void enableCommCountermeasureSet(uci::base::accessorType::AccessorType type = uci::type::accessorType::commCountermeasureSetEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCommCountermeasureSet()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         Link16MetadataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Link16MetadataType to copy from
           */
         Link16MetadataType(const Link16MetadataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Link16MetadataType to the contents of the Link16MetadataType on
           * the right hand side (rhs) of the assignment operator.Link16MetadataType [only available to derived classes]
           *
           * @param rhs The Link16MetadataType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::Link16MetadataType
           * @return A constant reference to this Link16MetadataType.
           */
         const Link16MetadataType& operator=(const Link16MetadataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Link16MetadataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__Link16MetadataType_h

