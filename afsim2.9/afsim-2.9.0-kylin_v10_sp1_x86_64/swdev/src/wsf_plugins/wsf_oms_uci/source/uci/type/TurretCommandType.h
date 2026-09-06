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
#ifndef Uci__Type__TurretCommandType_h
#define Uci__Type__TurretCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FixedPointingEnum_h)
# include "uci/type/FixedPointingEnum.h"
#endif

#if !defined(Uci__Type__TurretCommandPositionType_h)
# include "uci/type/TurretCommandPositionType.h"
#endif

#if !defined(Uci__Type__LOS_D_Type_h)
# include "uci/type/LOS_D_Type.h"
#endif

#if !defined(Uci__Type__POST_AirTargetVolumeCommandType_h)
# include "uci/type/POST_AirTargetVolumeCommandType.h"
#endif

#if !defined(Uci__Type__TargetType_h)
# include "uci/type/TargetType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the TurretCommandType choice accessor class */
      class TurretCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TurretCommandType()
         { }

         /** Returns this accessor's type constant, i.e. TurretCommandType
           *
           * @return This accessor's type constant, i.e. TurretCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::turretCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TurretCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum TurretCommandTypeChoice {
            TURRETCOMMANDTYPE_CHOICE_NONE,
            TURRETCOMMANDTYPE_CHOICE_FIXEDMODE,
            TURRETCOMMANDTYPE_CHOICE_POSITION,
            TURRETCOMMANDTYPE_CHOICE_LOSPOSITION,
            TURRETCOMMANDTYPE_CHOICE_VOLUME,
            TURRETCOMMANDTYPE_CHOICE_GEOSPATIAL
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual TurretCommandTypeChoice getTurretCommandTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getTurretCommandTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
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
         virtual void setTurretCommandTypeChoiceOrdinal(TurretCommandTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FixedMode.
           *
           * @return The value of the enumeration identified by FixedMode.
           */
         virtual const uci::type::FixedPointingEnum& getFixedMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FixedMode.
           *
           * @return The value of the enumeration identified by FixedMode.
           */
         virtual uci::type::FixedPointingEnum& getFixedMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FixedMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFixedMode(const uci::type::FixedPointingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FixedMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFixedMode(uci::type::FixedPointingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual const uci::type::TurretCommandPositionType& getPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Position.
           *
           * @return The acecssor that provides access to the complex content that is identified by Position.
           */
         virtual uci::type::TurretCommandPositionType& getPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Position to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Position
           */
         virtual void setPosition(const uci::type::TurretCommandPositionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LOSPosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by LOSPosition.
           */
         virtual const uci::type::LOS_D_Type& getLOSPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LOSPosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by LOSPosition.
           */
         virtual uci::type::LOS_D_Type& getLOSPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LOSPosition to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LOSPosition
           */
         virtual void setLOSPosition(const uci::type::LOS_D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Volume.
           *
           * @return The acecssor that provides access to the complex content that is identified by Volume.
           */
         virtual const uci::type::POST_AirTargetVolumeCommandType& getVolume() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Volume.
           *
           * @return The acecssor that provides access to the complex content that is identified by Volume.
           */
         virtual uci::type::POST_AirTargetVolumeCommandType& getVolume()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Volume to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Volume
           */
         virtual void setVolume(const uci::type::POST_AirTargetVolumeCommandType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Geospatial.
           *
           * @return The acecssor that provides access to the complex content that is identified by Geospatial.
           */
         virtual const uci::type::TargetType& getGeospatial() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Geospatial.
           *
           * @return The acecssor that provides access to the complex content that is identified by Geospatial.
           */
         virtual uci::type::TargetType& getGeospatial()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Geospatial to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Geospatial
           */
         virtual void setGeospatial(const uci::type::TargetType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TurretCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TurretCommandType to copy from
           */
         TurretCommandType(const TurretCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TurretCommandType to the contents of the TurretCommandType on the
           * right hand side (rhs) of the assignment operator.TurretCommandType [only available to derived classes]
           *
           * @param rhs The TurretCommandType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::TurretCommandType
           * @return A constant reference to this TurretCommandType.
           */
         const TurretCommandType& operator=(const TurretCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TurretCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TurretCommandType_h

