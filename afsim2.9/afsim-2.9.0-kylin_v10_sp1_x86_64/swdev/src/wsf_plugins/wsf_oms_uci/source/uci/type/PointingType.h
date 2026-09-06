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
#ifndef Uci__Type__PointingType_h
#define Uci__Type__PointingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TargetType_h)
# include "uci/type/TargetType.h"
#endif

#if !defined(Uci__Type__LOS_D_Type_h)
# include "uci/type/LOS_D_Type.h"
#endif

#if !defined(Uci__Type__POST_AirTargetVolumeCommandType_h)
# include "uci/type/POST_AirTargetVolumeCommandType.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Type__FixedPointingEnum_h)
# include "uci/type/FixedPointingEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the position or location to point the XX Subsystem to do a XX collection, track, or search as part of an XX
        * Activity. If the XX Subsystem cannot point itself, then the TurretSlaved Type is used. If the system wants to allow
        * the subsystem to control its own LOS then the ActivitySlavedID is used. FixedPointing is used to point to a
        * predetermined location defined by its Enum values.
        */
      class PointingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PointingType()
         { }

         /** Returns this accessor's type constant, i.e. PointingType
           *
           * @return This accessor's type constant, i.e. PointingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pointingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PointingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the source of or explicit values for geospatial characteristics of the target of the Command. When multiple
           * points/targets are given, a best-fit, multi-track or similar behavior is expected. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TargetType, uci::type::accessorType::targetType> Geospatial;

         /** Indicates the volume or specific Entity that defines the extents of the spatial region covered by the command. For
           * non-cued Capabilities, one air volume instance of this element is expected. For cued Capabilities, two instances of
           * this element are allowed: one with an Entity specified and a second with an air volume that define the extents of the
           * cue volume around the Entity. For cued Capabilities, a single instance is allowed if it is an Entity in which case
           * the Entity location uncertainty defines the extents of the cue volume. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_AirTargetVolumeCommandType, uci::type::accessorType::pOST_AirTargetVolumeCommandType> Volume;

         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum PointingTypeChoice {
            POINTINGTYPE_CHOICE_NONE,
            POINTINGTYPE_CHOICE_GEOSPATIAL,
            POINTINGTYPE_CHOICE_LOSOPTION,
            POINTINGTYPE_CHOICE_VOLUME,
            POINTINGTYPE_CHOICE_TURRETSLAVED,
            POINTINGTYPE_CHOICE_ACTIVITYSLAVEDID,
            POINTINGTYPE_CHOICE_FIXEDPOINTING
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual PointingTypeChoice getPointingTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getPointingTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the access
           * methods associated with the chosen element will be enabled and will provide access to the chosen element.
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
         virtual void setPointingTypeChoiceOrdinal(PointingTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Geospatial.
           *
           * @return The bounded list identified by Geospatial.
           */
         virtual const uci::type::PointingType::Geospatial& getGeospatial() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Geospatial.
           *
           * @return The bounded list identified by Geospatial.
           */
         virtual uci::type::PointingType::Geospatial& getGeospatial()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Geospatial.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeospatial(const uci::type::PointingType::Geospatial& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LOSOption.
           *
           * @return The acecssor that provides access to the complex content that is identified by LOSOption.
           */
         virtual const uci::type::LOS_D_Type& getLOSOption() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LOSOption.
           *
           * @return The acecssor that provides access to the complex content that is identified by LOSOption.
           */
         virtual uci::type::LOS_D_Type& getLOSOption()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LOSOption to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LOSOption
           */
         virtual void setLOSOption(const uci::type::LOS_D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Volume.
           *
           * @return The bounded list identified by Volume.
           */
         virtual const uci::type::PointingType::Volume& getVolume() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Volume.
           *
           * @return The bounded list identified by Volume.
           */
         virtual uci::type::PointingType::Volume& getVolume()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Volume.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVolume(const uci::type::PointingType::Volume& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TurretSlaved.
           *
           * @return The value of the string data type identified by TurretSlaved.
           */
         virtual const uci::type::EmptyType& getTurretSlaved() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the TurretSlaved.
           *
           * @return The value of the string data type identified by TurretSlaved.
           */
         virtual uci::type::EmptyType& getTurretSlaved()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TurretSlaved to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTurretSlaved(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TurretSlaved to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTurretSlaved(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the TurretSlaved to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTurretSlaved(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivitySlavedID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivitySlavedID.
           */
         virtual const uci::type::ActivityID_Type& getActivitySlavedID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivitySlavedID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivitySlavedID.
           */
         virtual uci::type::ActivityID_Type& getActivitySlavedID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivitySlavedID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivitySlavedID
           */
         virtual void setActivitySlavedID(const uci::type::ActivityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FixedPointing.
           *
           * @return The value of the enumeration identified by FixedPointing.
           */
         virtual const uci::type::FixedPointingEnum& getFixedPointing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FixedPointing.
           *
           * @return The value of the enumeration identified by FixedPointing.
           */
         virtual uci::type::FixedPointingEnum& getFixedPointing()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FixedPointing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFixedPointing(const uci::type::FixedPointingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FixedPointing.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFixedPointing(uci::type::FixedPointingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PointingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PointingType to copy from
           */
         PointingType(const PointingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PointingType to the contents of the PointingType on the right hand
           * side (rhs) of the assignment operator.PointingType [only available to derived classes]
           *
           * @param rhs The PointingType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::PointingType
           * @return A constant reference to this PointingType.
           */
         const PointingType& operator=(const PointingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PointingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PointingType_h

