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
#ifndef Uci__Type__OpZoneCategoryType_h
#define Uci__Type__OpZoneCategoryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__VehicleCommandDataType_h)
# include "uci/type/VehicleCommandDataType.h"
#endif

#if !defined(Uci__Type__IngressEgressType_h)
# include "uci/type/IngressEgressType.h"
#endif

#if !defined(Uci__Type__OpZoneNoFireType_h)
# include "uci/type/OpZoneNoFireType.h"
#endif

#if !defined(Uci__Type__OpZoneNoFlyType_h)
# include "uci/type/OpZoneNoFlyType.h"
#endif

#if !defined(Uci__Type__OpZoneJammingType_h)
# include "uci/type/OpZoneJammingType.h"
#endif

#if !defined(Uci__Type__OpZoneWeaponRestrictionType_h)
# include "uci/type/OpZoneWeaponRestrictionType.h"
#endif

#if !defined(Uci__Type__OpZoneWeatherType_h)
# include "uci/type/OpZoneWeatherType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OpZoneFilterAreaPET_h)
# include "uci/type/OpZoneFilterAreaPET.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OpZoneCategoryType choice accessor class */
      class OpZoneCategoryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OpZoneCategoryType()
         { }

         /** Returns this accessor's type constant, i.e. OpZoneCategoryType
           *
           * @return This accessor's type constant, i.e. OpZoneCategoryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::opZoneCategoryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OpZoneCategoryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates that the OpZone can be a zone filter type. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::OpZoneFilterAreaPET, uci::type::accessorType::opZoneFilterAreaPET> FilterArea;

         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum OpZoneCategoryTypeChoice {
            OPZONECATEGORYTYPE_CHOICE_NONE,
            OPZONECATEGORYTYPE_CHOICE_MISSIONAREA,
            OPZONECATEGORYTYPE_CHOICE_VEHICLECONFIGURATION,
            OPZONECATEGORYTYPE_CHOICE_EOB,
            OPZONECATEGORYTYPE_CHOICE_KEEPIN,
            OPZONECATEGORYTYPE_CHOICE_NOCOMM,
            OPZONECATEGORYTYPE_CHOICE_KILL,
            OPZONECATEGORYTYPE_CHOICE_SHORAD,
            OPZONECATEGORYTYPE_CHOICE_HOSTILEWEAPON,
            OPZONECATEGORYTYPE_CHOICE_HOSTILETACTICALAREA,
            OPZONECATEGORYTYPE_CHOICE_MISSILEENGAGEMENT,
            OPZONECATEGORYTYPE_CHOICE_CONTAMINATED,
            OPZONECATEGORYTYPE_CHOICE_GROUNDAOR,
            OPZONECATEGORYTYPE_CHOICE_FIGHTERENGAGEAOR,
            OPZONECATEGORYTYPE_CHOICE_NOFIRE,
            OPZONECATEGORYTYPE_CHOICE_NOFLY,
            OPZONECATEGORYTYPE_CHOICE_NOIMAGE,
            OPZONECATEGORYTYPE_CHOICE_DIRECTEDSEARCHAREA,
            OPZONECATEGORYTYPE_CHOICE_JAMMING,
            OPZONECATEGORYTYPE_CHOICE_WEAPONRESTRICTION,
            OPZONECATEGORYTYPE_CHOICE_WEATHERCONDITIONS,
            OPZONECATEGORYTYPE_CHOICE_FILTERAREA,
            OPZONECATEGORYTYPE_CHOICE_REFERENCE
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual OpZoneCategoryTypeChoice getOpZoneCategoryTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getOpZoneCategoryTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
           * access methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setOpZoneCategoryTypeChoiceOrdinal(OpZoneCategoryTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionArea.
           *
           * @return The value of the string data type identified by MissionArea.
           */
         virtual const uci::type::EmptyType& getMissionArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionArea.
           *
           * @return The value of the string data type identified by MissionArea.
           */
         virtual uci::type::EmptyType& getMissionArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionArea to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissionArea(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionArea to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionArea(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionArea to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionArea(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleConfiguration.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleConfiguration.
           */
         virtual const uci::type::VehicleCommandDataType& getVehicleConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleConfiguration.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleConfiguration.
           */
         virtual uci::type::VehicleCommandDataType& getVehicleConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VehicleConfiguration to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VehicleConfiguration
           */
         virtual void setVehicleConfiguration(const uci::type::VehicleCommandDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the EOB.
           *
           * @return The value of the string data type identified by EOB.
           */
         virtual const uci::type::EmptyType& getEOB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the EOB.
           *
           * @return The value of the string data type identified by EOB.
           */
         virtual uci::type::EmptyType& getEOB()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EOB to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setEOB(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EOB to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setEOB(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EOB to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setEOB(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KeepIn.
           *
           * @return The acecssor that provides access to the complex content that is identified by KeepIn.
           */
         virtual const uci::type::IngressEgressType& getKeepIn() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KeepIn.
           *
           * @return The acecssor that provides access to the complex content that is identified by KeepIn.
           */
         virtual uci::type::IngressEgressType& getKeepIn()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the KeepIn to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by KeepIn
           */
         virtual void setKeepIn(const uci::type::IngressEgressType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NoComm.
           *
           * @return The value of the string data type identified by NoComm.
           */
         virtual const uci::type::EmptyType& getNoComm() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NoComm.
           *
           * @return The value of the string data type identified by NoComm.
           */
         virtual uci::type::EmptyType& getNoComm()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NoComm to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setNoComm(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NoComm to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNoComm(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NoComm to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNoComm(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Kill.
           *
           * @return The value of the string data type identified by Kill.
           */
         virtual const uci::type::EmptyType& getKill() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Kill.
           *
           * @return The value of the string data type identified by Kill.
           */
         virtual uci::type::EmptyType& getKill()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Kill to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setKill(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Kill to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setKill(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Kill to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setKill(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SHORAD.
           *
           * @return The value of the string data type identified by SHORAD.
           */
         virtual const uci::type::EmptyType& getSHORAD() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SHORAD.
           *
           * @return The value of the string data type identified by SHORAD.
           */
         virtual uci::type::EmptyType& getSHORAD()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SHORAD to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSHORAD(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SHORAD to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSHORAD(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SHORAD to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSHORAD(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the HostileWeapon.
           *
           * @return The value of the string data type identified by HostileWeapon.
           */
         virtual const uci::type::EmptyType& getHostileWeapon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the HostileWeapon.
           *
           * @return The value of the string data type identified by HostileWeapon.
           */
         virtual uci::type::EmptyType& getHostileWeapon()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HostileWeapon to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setHostileWeapon(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HostileWeapon to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setHostileWeapon(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HostileWeapon to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setHostileWeapon(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the HostileTacticalArea.
           *
           * @return The value of the string data type identified by HostileTacticalArea.
           */
         virtual const uci::type::EmptyType& getHostileTacticalArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the HostileTacticalArea.
           *
           * @return The value of the string data type identified by HostileTacticalArea.
           */
         virtual uci::type::EmptyType& getHostileTacticalArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HostileTacticalArea to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setHostileTacticalArea(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HostileTacticalArea to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setHostileTacticalArea(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the HostileTacticalArea to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setHostileTacticalArea(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissileEngagement.
           *
           * @return The value of the string data type identified by MissileEngagement.
           */
         virtual const uci::type::EmptyType& getMissileEngagement() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissileEngagement.
           *
           * @return The value of the string data type identified by MissileEngagement.
           */
         virtual uci::type::EmptyType& getMissileEngagement()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissileEngagement to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissileEngagement(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissileEngagement to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissileEngagement(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissileEngagement to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissileEngagement(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Contaminated.
           *
           * @return The value of the string data type identified by Contaminated.
           */
         virtual const uci::type::EmptyType& getContaminated() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Contaminated.
           *
           * @return The value of the string data type identified by Contaminated.
           */
         virtual uci::type::EmptyType& getContaminated()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Contaminated to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setContaminated(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Contaminated to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setContaminated(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Contaminated to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setContaminated(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the GroundAOR.
           *
           * @return The value of the string data type identified by GroundAOR.
           */
         virtual const uci::type::EmptyType& getGroundAOR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the GroundAOR.
           *
           * @return The value of the string data type identified by GroundAOR.
           */
         virtual uci::type::EmptyType& getGroundAOR()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the GroundAOR to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setGroundAOR(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the GroundAOR to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setGroundAOR(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the GroundAOR to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setGroundAOR(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FighterEngageAOR.
           *
           * @return The value of the string data type identified by FighterEngageAOR.
           */
         virtual const uci::type::EmptyType& getFighterEngageAOR() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FighterEngageAOR.
           *
           * @return The value of the string data type identified by FighterEngageAOR.
           */
         virtual uci::type::EmptyType& getFighterEngageAOR()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FighterEngageAOR to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFighterEngageAOR(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FighterEngageAOR to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFighterEngageAOR(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FighterEngageAOR to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFighterEngageAOR(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NoFire.
           *
           * @return The acecssor that provides access to the complex content that is identified by NoFire.
           */
         virtual const uci::type::OpZoneNoFireType& getNoFire() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NoFire.
           *
           * @return The acecssor that provides access to the complex content that is identified by NoFire.
           */
         virtual uci::type::OpZoneNoFireType& getNoFire()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NoFire to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NoFire
           */
         virtual void setNoFire(const uci::type::OpZoneNoFireType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NoFly.
           *
           * @return The acecssor that provides access to the complex content that is identified by NoFly.
           */
         virtual const uci::type::OpZoneNoFlyType& getNoFly() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NoFly.
           *
           * @return The acecssor that provides access to the complex content that is identified by NoFly.
           */
         virtual uci::type::OpZoneNoFlyType& getNoFly()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NoFly to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NoFly
           */
         virtual void setNoFly(const uci::type::OpZoneNoFlyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NoImage.
           *
           * @return The value of the string data type identified by NoImage.
           */
         virtual const uci::type::EmptyType& getNoImage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NoImage.
           *
           * @return The value of the string data type identified by NoImage.
           */
         virtual uci::type::EmptyType& getNoImage()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NoImage to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setNoImage(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NoImage to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNoImage(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NoImage to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNoImage(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DirectedSearchArea.
           *
           * @return The value of the string data type identified by DirectedSearchArea.
           */
         virtual const uci::type::EmptyType& getDirectedSearchArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the DirectedSearchArea.
           *
           * @return The value of the string data type identified by DirectedSearchArea.
           */
         virtual uci::type::EmptyType& getDirectedSearchArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DirectedSearchArea to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDirectedSearchArea(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DirectedSearchArea to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDirectedSearchArea(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the DirectedSearchArea to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDirectedSearchArea(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Jamming.
           *
           * @return The acecssor that provides access to the complex content that is identified by Jamming.
           */
         virtual const uci::type::OpZoneJammingType& getJamming() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Jamming.
           *
           * @return The acecssor that provides access to the complex content that is identified by Jamming.
           */
         virtual uci::type::OpZoneJammingType& getJamming()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Jamming to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Jamming
           */
         virtual void setJamming(const uci::type::OpZoneJammingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponRestriction.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponRestriction.
           */
         virtual const uci::type::OpZoneWeaponRestrictionType& getWeaponRestriction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponRestriction.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponRestriction.
           */
         virtual uci::type::OpZoneWeaponRestrictionType& getWeaponRestriction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeaponRestriction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeaponRestriction
           */
         virtual void setWeaponRestriction(const uci::type::OpZoneWeaponRestrictionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherConditions.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherConditions.
           */
         virtual const uci::type::OpZoneWeatherType& getWeatherConditions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherConditions.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherConditions.
           */
         virtual uci::type::OpZoneWeatherType& getWeatherConditions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherConditions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherConditions
           */
         virtual void setWeatherConditions(const uci::type::OpZoneWeatherType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FilterArea.
           *
           * @return The bounded list identified by FilterArea.
           */
         virtual const uci::type::OpZoneCategoryType::FilterArea& getFilterArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FilterArea.
           *
           * @return The bounded list identified by FilterArea.
           */
         virtual uci::type::OpZoneCategoryType::FilterArea& getFilterArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FilterArea.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFilterArea(const uci::type::OpZoneCategoryType::FilterArea& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Reference.
           *
           * @return The value of the string data type identified by Reference.
           */
         virtual const uci::type::EmptyType& getReference() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Reference.
           *
           * @return The value of the string data type identified by Reference.
           */
         virtual uci::type::EmptyType& getReference()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reference to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setReference(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reference to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReference(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reference to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReference(const char* value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OpZoneCategoryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OpZoneCategoryType to copy from
           */
         OpZoneCategoryType(const OpZoneCategoryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OpZoneCategoryType to the contents of the OpZoneCategoryType on
           * the right hand side (rhs) of the assignment operator.OpZoneCategoryType [only available to derived classes]
           *
           * @param rhs The OpZoneCategoryType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::OpZoneCategoryType
           * @return A constant reference to this OpZoneCategoryType.
           */
         const OpZoneCategoryType& operator=(const OpZoneCategoryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OpZoneCategoryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OpZoneCategoryType_h

