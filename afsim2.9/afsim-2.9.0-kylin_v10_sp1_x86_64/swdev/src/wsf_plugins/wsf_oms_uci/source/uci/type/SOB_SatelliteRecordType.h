// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SOB_SatelliteRecordType_h
#define Uci__Type__SOB_SatelliteRecordType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__SOB_SatelliteRecordID_Type_h)
# include "uci/type/SOB_SatelliteRecordID_Type.h"
#endif

#if !defined(Uci__Type__EOB_RecordID_Type_h)
# include "uci/type/EOB_RecordID_Type.h"
#endif

#if !defined(Uci__Type__SatelliteIdentityType_h)
# include "uci/type/SatelliteIdentityType.h"
#endif

#if !defined(Uci__Type__OrbitAltitudeEnum_h)
# include "uci/type/OrbitAltitudeEnum.h"
#endif

#if !defined(Uci__Type__OrderOfBattleTimestampsType_h)
# include "uci/type/OrderOfBattleTimestampsType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__EOB_OperationalStatusEnum_h)
# include "uci/type/EOB_OperationalStatusEnum.h"
#endif

#if !defined(Uci__Type__TLE_BaseType_h)
# include "uci/type/TLE_BaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SOB_MissionTypeEnum_h)
# include "uci/type/SOB_MissionTypeEnum.h"
#endif

#if !defined(Uci__Type__MassType_h)
# include "uci/type/MassType.h"
#endif

#if !defined(Uci__Type__SatelliteEnduranceType_h)
# include "uci/type/SatelliteEnduranceType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__OrderOfBattleEmitterType_h)
# include "uci/type/OrderOfBattleEmitterType.h"
#endif

#if !defined(Uci__Type__VisibleString512Type_h)
# include "uci/type/VisibleString512Type.h"
#endif

#if !defined(Uci__Type__FileLocationMDT_h)
# include "uci/type/FileLocationMDT.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Contains satellite information similar to a space catalog record. */
      class SOB_SatelliteRecordType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SOB_SatelliteRecordType()
         { }

         /** Returns this accessor's type constant, i.e. SOB_SatelliteRecordType
           *
           * @return This accessor's type constant, i.e. SOB_SatelliteRecordType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sOB_SatelliteRecordType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SOB_SatelliteRecordType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The intended mission(s) of the satellite. [Minimum occurrences: 0] [Maximum occurrences: 24] */
         typedef uci::base::BoundedList<uci::type::SOB_MissionTypeEnum, uci::type::accessorType::sOB_MissionTypeEnum> MissionType;

         /** Specifies satellite records associated with this SOB Satellite Record. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SOB_SatelliteRecordID_Type, uci::type::accessorType::sOB_SatelliteRecordID_Type> RelatedSatelliteRecordID;

         /** Indicates modes and parametrics of a previously observed emitter associated with the SOB record. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::OrderOfBattleEmitterType, uci::type::accessorType::orderOfBattleEmitterType> Emitter;

         /** Returns the accessor that provides access to the complex content that is identified by the SecurityAttributes.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityAttributes.
           */
         virtual const uci::type::SecurityInformationType& getSecurityAttributes() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityAttributes.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityAttributes.
           */
         virtual uci::type::SecurityInformationType& getSecurityAttributes()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SecurityAttributes to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SecurityAttributes
           */
         virtual void setSecurityAttributes(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SatelliteRecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SatelliteRecordID.
           */
         virtual const uci::type::SOB_SatelliteRecordID_Type& getSatelliteRecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SatelliteRecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SatelliteRecordID.
           */
         virtual uci::type::SOB_SatelliteRecordID_Type& getSatelliteRecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SatelliteRecordID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SatelliteRecordID
           */
         virtual void setSatelliteRecordID(const uci::type::SOB_SatelliteRecordID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_RecordID.
           */
         virtual const uci::type::EOB_RecordID_Type& getEOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EOB_RecordID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EOB_RecordID.
           */
         virtual uci::type::EOB_RecordID_Type& getEOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EOB_RecordID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EOB_RecordID
           */
         virtual void setEOB_RecordID(const uci::type::EOB_RecordID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EOB_RecordID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EOB_RecordID is emabled or not
           */
         virtual bool hasEOB_RecordID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EOB_RecordID
           *
           * @param type = uci::type::accessorType::eOB_RecordID_Type This Accessor's accessor type
           */
         virtual void enableEOB_RecordID(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_RecordID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EOB_RecordID */
         virtual void clearEOB_RecordID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::SatelliteIdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::SatelliteIdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::SatelliteIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::satelliteIdentityType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::satelliteIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Orbit.
           *
           * @return The value of the enumeration identified by Orbit.
           */
         virtual const uci::type::OrbitAltitudeEnum& getOrbit() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Orbit.
           *
           * @return The value of the enumeration identified by Orbit.
           */
         virtual uci::type::OrbitAltitudeEnum& getOrbit()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Orbit.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOrbit(const uci::type::OrbitAltitudeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Orbit.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOrbit(uci::type::OrbitAltitudeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Orbitis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasOrbit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getOrbit will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::orbitAltitudeEnum This Accessor's accessor type
           */
         virtual void enableOrbit(uci::base::accessorType::AccessorType type = uci::type::accessorType::orbitAltitudeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearOrbit()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Timestamps.
           *
           * @return The acecssor that provides access to the complex content that is identified by Timestamps.
           */
         virtual const uci::type::OrderOfBattleTimestampsType& getTimestamps() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Timestamps.
           *
           * @return The acecssor that provides access to the complex content that is identified by Timestamps.
           */
         virtual uci::type::OrderOfBattleTimestampsType& getTimestamps()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Timestamps to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Timestamps
           */
         virtual void setTimestamps(const uci::type::OrderOfBattleTimestampsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LaunchDate.
           *
           * @return The value of the simple primitive data type identified by LaunchDate.
           */
         virtual uci::type::DateTimeTypeValue getLaunchDate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LaunchDate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLaunchDate(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LaunchDate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LaunchDate is emabled or not
           */
         virtual bool hasLaunchDate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LaunchDate
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableLaunchDate(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LaunchDate */
         virtual void clearLaunchDate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalStatus.
           *
           * @return The value of the enumeration identified by OperationalStatus.
           */
         virtual const uci::type::EOB_OperationalStatusEnum& getOperationalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalStatus.
           *
           * @return The value of the enumeration identified by OperationalStatus.
           */
         virtual uci::type::EOB_OperationalStatusEnum& getOperationalStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalStatus(const uci::type::EOB_OperationalStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalStatus(uci::type::EOB_OperationalStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield OperationalStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasOperationalStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getOperationalStatus will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::eOB_OperationalStatusEnum This Accessor's accessor type
           */
         virtual void enableOperationalStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_OperationalStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearOperationalStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElementSet.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElementSet.
           */
         virtual const uci::type::TLE_BaseType& getElementSet() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElementSet.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElementSet.
           */
         virtual uci::type::TLE_BaseType& getElementSet()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ElementSet to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ElementSet
           */
         virtual void setElementSet(const uci::type::TLE_BaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ElementSet exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ElementSet is emabled or not
           */
         virtual bool hasElementSet() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ElementSet
           *
           * @param type = uci::type::accessorType::tLE_BaseType This Accessor's accessor type
           */
         virtual void enableElementSet(uci::base::accessorType::AccessorType type = uci::type::accessorType::tLE_BaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ElementSet */
         virtual void clearElementSet()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionType.
           *
           * @return The bounded list identified by MissionType.
           */
         virtual const uci::type::SOB_SatelliteRecordType::MissionType& getMissionType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MissionType.
           *
           * @return The bounded list identified by MissionType.
           */
         virtual uci::type::SOB_SatelliteRecordType::MissionType& getMissionType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MissionType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMissionType(const uci::type::SOB_SatelliteRecordType::MissionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Mass.
           *
           * @return The value of the simple primitive data type identified by Mass.
           */
         virtual uci::type::MassTypeValue getMass() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Mass.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMass(uci::type::MassTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Mass exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Mass is emabled or not
           */
         virtual bool hasMass() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Mass
           *
           * @param type = uci::type::accessorType::massType This Accessor's accessor type
           */
         virtual void enableMass(uci::base::accessorType::AccessorType type = uci::type::accessorType::massType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Mass */
         virtual void clearMass()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endurance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endurance.
           */
         virtual const uci::type::SatelliteEnduranceType& getEndurance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endurance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endurance.
           */
         virtual uci::type::SatelliteEnduranceType& getEndurance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Endurance to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Endurance
           */
         virtual void setEndurance(const uci::type::SatelliteEnduranceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Endurance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Endurance is emabled or not
           */
         virtual bool hasEndurance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Endurance
           *
           * @param type = uci::type::accessorType::satelliteEnduranceType This Accessor's accessor type
           */
         virtual void enableEndurance(uci::base::accessorType::AccessorType type = uci::type::accessorType::satelliteEnduranceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Endurance */
         virtual void clearEndurance()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RelatedSatelliteRecordID.
           *
           * @return The bounded list identified by RelatedSatelliteRecordID.
           */
         virtual const uci::type::SOB_SatelliteRecordType::RelatedSatelliteRecordID& getRelatedSatelliteRecordID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RelatedSatelliteRecordID.
           *
           * @return The bounded list identified by RelatedSatelliteRecordID.
           */
         virtual uci::type::SOB_SatelliteRecordType::RelatedSatelliteRecordID& getRelatedSatelliteRecordID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RelatedSatelliteRecordID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRelatedSatelliteRecordID(const uci::type::SOB_SatelliteRecordType::RelatedSatelliteRecordID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the BusType.
           *
           * @return The value of the string data type identified by BusType.
           */
         virtual const uci::type::VisibleString1024Type& getBusType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the BusType.
           *
           * @return The value of the string data type identified by BusType.
           */
         virtual uci::type::VisibleString1024Type& getBusType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the BusType to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setBusType(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the BusType to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setBusType(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the BusType to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setBusType(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BusType exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BusType is emabled or not
           */
         virtual bool hasBusType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BusType
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableBusType(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BusType */
         virtual void clearBusType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PatternOfLife.
           *
           * @return The value of the string data type identified by PatternOfLife.
           */
         virtual const uci::type::VisibleString1024Type& getPatternOfLife() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PatternOfLife.
           *
           * @return The value of the string data type identified by PatternOfLife.
           */
         virtual uci::type::VisibleString1024Type& getPatternOfLife()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PatternOfLife to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPatternOfLife(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PatternOfLife to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPatternOfLife(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PatternOfLife to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPatternOfLife(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PatternOfLife exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PatternOfLife is emabled or not
           */
         virtual bool hasPatternOfLife() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PatternOfLife
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enablePatternOfLife(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PatternOfLife */
         virtual void clearPatternOfLife()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UplinkFrequencyRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by UplinkFrequencyRange.
           */
         virtual const uci::type::FrequencyRangeType& getUplinkFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UplinkFrequencyRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by UplinkFrequencyRange.
           */
         virtual uci::type::FrequencyRangeType& getUplinkFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the UplinkFrequencyRange to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by UplinkFrequencyRange
           */
         virtual void setUplinkFrequencyRange(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UplinkFrequencyRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UplinkFrequencyRange is emabled or not
           */
         virtual bool hasUplinkFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UplinkFrequencyRange
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableUplinkFrequencyRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UplinkFrequencyRange */
         virtual void clearUplinkFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DownlinkFrequencyRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by DownlinkFrequencyRange.
           */
         virtual const uci::type::FrequencyRangeType& getDownlinkFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DownlinkFrequencyRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by DownlinkFrequencyRange.
           */
         virtual uci::type::FrequencyRangeType& getDownlinkFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DownlinkFrequencyRange to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DownlinkFrequencyRange
           */
         virtual void setDownlinkFrequencyRange(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DownlinkFrequencyRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DownlinkFrequencyRange is emabled or not
           */
         virtual bool hasDownlinkFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DownlinkFrequencyRange
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableDownlinkFrequencyRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DownlinkFrequencyRange */
         virtual void clearDownlinkFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual const uci::type::SOB_SatelliteRecordType::Emitter& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual uci::type::SOB_SatelliteRecordType::Emitter& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Emitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitter(const uci::type::SOB_SatelliteRecordType::Emitter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RemarksText.
           *
           * @return The value of the string data type identified by RemarksText.
           */
         virtual const uci::type::VisibleString512Type& getRemarksText() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the RemarksText.
           *
           * @return The value of the string data type identified by RemarksText.
           */
         virtual uci::type::VisibleString512Type& getRemarksText()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RemarksText to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRemarksText(const uci::type::VisibleString512Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RemarksText to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarksText(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the RemarksText to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarksText(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemarksText exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemarksText is emabled or not
           */
         virtual bool hasRemarksText() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemarksText
           *
           * @param type = uci::type::accessorType::visibleString512Type This Accessor's accessor type
           */
         virtual void enableRemarksText(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString512Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemarksText */
         virtual void clearRemarksText()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Graphic.
           *
           * @return The acecssor that provides access to the complex content that is identified by Graphic.
           */
         virtual const uci::type::FileLocationMDT& getGraphic() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Graphic.
           *
           * @return The acecssor that provides access to the complex content that is identified by Graphic.
           */
         virtual uci::type::FileLocationMDT& getGraphic()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Graphic to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Graphic
           */
         virtual void setGraphic(const uci::type::FileLocationMDT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Graphic exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Graphic is emabled or not
           */
         virtual bool hasGraphic() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Graphic
           *
           * @param type = uci::type::accessorType::fileLocationMDT This Accessor's accessor type
           */
         virtual void enableGraphic(uci::base::accessorType::AccessorType type = uci::type::accessorType::fileLocationMDT)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Graphic */
         virtual void clearGraphic()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SOB_SatelliteRecordType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SOB_SatelliteRecordType to copy from
           */
         SOB_SatelliteRecordType(const SOB_SatelliteRecordType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SOB_SatelliteRecordType to the contents of the
           * SOB_SatelliteRecordType on the right hand side (rhs) of the assignment operator.SOB_SatelliteRecordType [only
           * available to derived classes]
           *
           * @param rhs The SOB_SatelliteRecordType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SOB_SatelliteRecordType
           * @return A constant reference to this SOB_SatelliteRecordType.
           */
         const SOB_SatelliteRecordType& operator=(const SOB_SatelliteRecordType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SOB_SatelliteRecordType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SOB_SatelliteRecordType_h

