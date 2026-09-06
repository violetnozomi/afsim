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
#ifndef Uci__Type__EntityMDT_h
#define Uci__Type__EntityMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeSigmaType_h)
# include "uci/type/DateTimeSigmaType.h"
#endif

#if !defined(Uci__Type__EntitySourceType_h)
# include "uci/type/EntitySourceType.h"
#endif

#if !defined(Uci__Type__EntityStatusEnum_h)
# include "uci/type/EntityStatusEnum.h"
#endif

#if !defined(Uci__Type__EntityIdentityType_h)
# include "uci/type/EntityIdentityType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MobilityEnum_h)
# include "uci/type/MobilityEnum.h"
#endif

#if !defined(Uci__Type__KinematicsType_h)
# include "uci/type/KinematicsType.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__PlatformStatusType_h)
# include "uci/type/PlatformStatusType.h"
#endif

#if !defined(Uci__Type__EntitySignalSummaryType_h)
# include "uci/type/EntitySignalSummaryType.h"
#endif

#if !defined(Uci__Type__PulseDataID_Type_h)
# include "uci/type/PulseDataID_Type.h"
#endif

#if !defined(Uci__Type__StrengthType_h)
# include "uci/type/StrengthType.h"
#endif

#if !defined(Uci__Type__ActivityAgainstType_h)
# include "uci/type/ActivityAgainstType.h"
#endif

#if !defined(Uci__Type__ActivityByType_h)
# include "uci/type/ActivityByType.h"
#endif

#if !defined(Uci__Type__VoiceControlType_h)
# include "uci/type/VoiceControlType.h"
#endif

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

#if !defined(Uci__Type__EntityRemoveInfoType_h)
# include "uci/type/EntityRemoveInfoType.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityMDT sequence accessor class */
      class EntityMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityMDT()
         { }

         /** Returns this accessor's type constant, i.e. EntityMDT
           *
           * @return This accessor's type constant, i.e. EntityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of an Entity that is a part of this Site Entity. When omitted, the Entity is not a Site. When
           * one or more are given, this Entity is a Site. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> SiteEntityID;

         /** Indicates a summary of an RF Signal from the Entity. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntitySignalSummaryType, uci::type::accessorType::entitySignalSummaryType> SignalSummary;

         /** Indicates the unique ID of a PulseData collection associated with the Entity. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PulseDataID_Type, uci::type::accessorType::pulseDataID_Type> PulseDataID;

         /** This element indicates an activity, that isn't otherwise represented by UCI Tasks, Capability commands, etc. being
           * performed on/against the Entity. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ActivityAgainstType, uci::type::accessorType::activityAgainstType> ActivityAgainst;

         /** This element indicates an activity, that isn't otherwise represented by UCI Tasks, Capability commands, etc. being
           * performed by the Entity. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ActivityByType, uci::type::accessorType::activityByType> ActivityBy;

         /** This element indicates an association (not a fusion or derivation relationship) between this Entity and another UCI
           * message or object. For example, an Entity could be associated with an OpPoint. Data content of the Entity and the
           * associated message/object is maintained independently. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ID_Type, uci::type::accessorType::iD_Type> AssociatedID;

         /** Indicates a capability of the Entity. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual const uci::type::EntityID_Type& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual uci::type::EntityID_Type& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EntityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EntityID
           */
         virtual void setEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CreationTimestamp.
           *
           * @return The acecssor that provides access to the complex content that is identified by CreationTimestamp.
           */
         virtual const uci::type::DateTimeSigmaType& getCreationTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CreationTimestamp.
           *
           * @return The acecssor that provides access to the complex content that is identified by CreationTimestamp.
           */
         virtual uci::type::DateTimeSigmaType& getCreationTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CreationTimestamp to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CreationTimestamp
           */
         virtual void setCreationTimestamp(const uci::type::DateTimeSigmaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual const uci::type::EntitySourceType& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual uci::type::EntitySourceType& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Source to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Source
           */
         virtual void setSource(const uci::type::EntitySourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Status.
           *
           * @return The value of the enumeration identified by Status.
           */
         virtual const uci::type::EntityStatusEnum& getStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Status.
           *
           * @return The value of the enumeration identified by Status.
           */
         virtual uci::type::EntityStatusEnum& getStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Status.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setStatus(const uci::type::EntityStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Status.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setStatus(uci::type::EntityStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Statusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getStatus will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::entityStatusEnum This Accessor's accessor type
           */
         virtual void enableStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::EntityIdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::EntityIdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::EntityIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SiteEntityID.
           *
           * @return The bounded list identified by SiteEntityID.
           */
         virtual const uci::type::EntityMDT::SiteEntityID& getSiteEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SiteEntityID.
           *
           * @return The bounded list identified by SiteEntityID.
           */
         virtual uci::type::EntityMDT::SiteEntityID& getSiteEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SiteEntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSiteEntityID(const uci::type::EntityMDT::SiteEntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mobility.
           *
           * @return The value of the enumeration identified by Mobility.
           */
         virtual const uci::type::MobilityEnum& getMobility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mobility.
           *
           * @return The value of the enumeration identified by Mobility.
           */
         virtual uci::type::MobilityEnum& getMobility()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mobility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMobility(const uci::type::MobilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mobility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMobility(uci::type::MobilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Mobilityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasMobility() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getMobility will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::mobilityEnum This Accessor's accessor type
           */
         virtual void enableMobility(uci::base::accessorType::AccessorType type = uci::type::accessorType::mobilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearMobility()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Kinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Kinematics.
           */
         virtual const uci::type::KinematicsType& getKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Kinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Kinematics.
           */
         virtual uci::type::KinematicsType& getKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Kinematics to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Kinematics
           */
         virtual void setKinematics(const uci::type::KinematicsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Kinematics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Kinematics is emabled or not
           */
         virtual bool hasKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Kinematics
           *
           * @param type = uci::type::accessorType::kinematicsType This Accessor's accessor type
           */
         virtual void enableKinematics(uci::base::accessorType::AccessorType type = uci::type::accessorType::kinematicsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Kinematics */
         virtual void clearKinematics()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimatedKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimatedKinematics.
           */
         virtual const uci::type::KinematicsType& getEstimatedKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimatedKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimatedKinematics.
           */
         virtual uci::type::KinematicsType& getEstimatedKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EstimatedKinematics to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EstimatedKinematics
           */
         virtual void setEstimatedKinematics(const uci::type::KinematicsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedKinematics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedKinematics is emabled or not
           */
         virtual bool hasEstimatedKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedKinematics
           *
           * @param type = uci::type::accessorType::kinematicsType This Accessor's accessor type
           */
         virtual void enableEstimatedKinematics(uci::base::accessorType::AccessorType type = uci::type::accessorType::kinematicsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedKinematics */
         virtual void clearEstimatedKinematics()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DownLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by DownLocation.
           */
         virtual const uci::type::Point2D_Type& getDownLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DownLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by DownLocation.
           */
         virtual uci::type::Point2D_Type& getDownLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DownLocation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DownLocation
           */
         virtual void setDownLocation(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DownLocation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DownLocation is emabled or not
           */
         virtual bool hasDownLocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DownLocation
           *
           * @param type = uci::type::accessorType::point2D_Type This Accessor's accessor type
           */
         virtual void enableDownLocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::point2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DownLocation */
         virtual void clearDownLocation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlatformStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlatformStatus.
           */
         virtual const uci::type::PlatformStatusType& getPlatformStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlatformStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlatformStatus.
           */
         virtual uci::type::PlatformStatusType& getPlatformStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlatformStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlatformStatus
           */
         virtual void setPlatformStatus(const uci::type::PlatformStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlatformStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlatformStatus is emabled or not
           */
         virtual bool hasPlatformStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlatformStatus
           *
           * @param type = uci::type::accessorType::platformStatusType This Accessor's accessor type
           */
         virtual void enablePlatformStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::platformStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlatformStatus */
         virtual void clearPlatformStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SignalSummary.
           *
           * @return The bounded list identified by SignalSummary.
           */
         virtual const uci::type::EntityMDT::SignalSummary& getSignalSummary() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SignalSummary.
           *
           * @return The bounded list identified by SignalSummary.
           */
         virtual uci::type::EntityMDT::SignalSummary& getSignalSummary()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SignalSummary.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSignalSummary(const uci::type::EntityMDT::SignalSummary& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseDataID.
           *
           * @return The bounded list identified by PulseDataID.
           */
         virtual const uci::type::EntityMDT::PulseDataID& getPulseDataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PulseDataID.
           *
           * @return The bounded list identified by PulseDataID.
           */
         virtual uci::type::EntityMDT::PulseDataID& getPulseDataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PulseDataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPulseDataID(const uci::type::EntityMDT::PulseDataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Strength.
           *
           * @return The acecssor that provides access to the complex content that is identified by Strength.
           */
         virtual const uci::type::StrengthType& getStrength() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Strength.
           *
           * @return The acecssor that provides access to the complex content that is identified by Strength.
           */
         virtual uci::type::StrengthType& getStrength()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Strength to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Strength
           */
         virtual void setStrength(const uci::type::StrengthType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Strength exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Strength is emabled or not
           */
         virtual bool hasStrength() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Strength
           *
           * @param type = uci::type::accessorType::strengthType This Accessor's accessor type
           */
         virtual void enableStrength(uci::base::accessorType::AccessorType type = uci::type::accessorType::strengthType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Strength */
         virtual void clearStrength()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityAgainst.
           *
           * @return The bounded list identified by ActivityAgainst.
           */
         virtual const uci::type::EntityMDT::ActivityAgainst& getActivityAgainst() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityAgainst.
           *
           * @return The bounded list identified by ActivityAgainst.
           */
         virtual uci::type::EntityMDT::ActivityAgainst& getActivityAgainst()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActivityAgainst.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivityAgainst(const uci::type::EntityMDT::ActivityAgainst& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityBy.
           *
           * @return The bounded list identified by ActivityBy.
           */
         virtual const uci::type::EntityMDT::ActivityBy& getActivityBy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityBy.
           *
           * @return The bounded list identified by ActivityBy.
           */
         virtual uci::type::EntityMDT::ActivityBy& getActivityBy()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActivityBy.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivityBy(const uci::type::EntityMDT::ActivityBy& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VoiceControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by VoiceControl.
           */
         virtual const uci::type::VoiceControlType& getVoiceControl() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VoiceControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by VoiceControl.
           */
         virtual uci::type::VoiceControlType& getVoiceControl()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VoiceControl to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VoiceControl
           */
         virtual void setVoiceControl(const uci::type::VoiceControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceControl exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceControl is emabled or not
           */
         virtual bool hasVoiceControl() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceControl
           *
           * @param type = uci::type::accessorType::voiceControlType This Accessor's accessor type
           */
         virtual void enableVoiceControl(uci::base::accessorType::AccessorType type = uci::type::accessorType::voiceControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceControl */
         virtual void clearVoiceControl()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedID.
           *
           * @return The bounded list identified by AssociatedID.
           */
         virtual const uci::type::EntityMDT::AssociatedID& getAssociatedID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedID.
           *
           * @return The bounded list identified by AssociatedID.
           */
         virtual uci::type::EntityMDT::AssociatedID& getAssociatedID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedID(const uci::type::EntityMDT::AssociatedID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoveInfo.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoveInfo.
           */
         virtual const uci::type::EntityRemoveInfoType& getRemoveInfo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoveInfo.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoveInfo.
           */
         virtual uci::type::EntityRemoveInfoType& getRemoveInfo()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RemoveInfo to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RemoveInfo
           */
         virtual void setRemoveInfo(const uci::type::EntityRemoveInfoType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemoveInfo exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemoveInfo is emabled or not
           */
         virtual bool hasRemoveInfo() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemoveInfo
           *
           * @param type = uci::type::accessorType::entityRemoveInfoType This Accessor's accessor type
           */
         virtual void enableRemoveInfo(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityRemoveInfoType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemoveInfo */
         virtual void clearRemoveInfo()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::EntityMDT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::EntityMDT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::EntityMDT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityMDT to copy from
           */
         EntityMDT(const EntityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityMDT to the contents of the EntityMDT on the right hand side
           * (rhs) of the assignment operator.EntityMDT [only available to derived classes]
           *
           * @param rhs The EntityMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::EntityMDT
           * @return A constant reference to this EntityMDT.
           */
         const EntityMDT& operator=(const EntityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityMDT_h

