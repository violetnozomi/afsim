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
#ifndef Uci__Type__DamageWeaponEmploymentType_h
#define Uci__Type__DamageWeaponEmploymentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__WeaponeeringType_h)
# include "uci/type/WeaponeeringType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__Point2D_Type_h)
# include "uci/type/Point2D_Type.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageWeaponEmploymentType sequence accessor class */
      class DamageWeaponEmploymentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageWeaponEmploymentType()
         { }

         /** Returns this accessor's type constant, i.e. DamageWeaponEmploymentType
           *
           * @return This accessor's type constant, i.e. DamageWeaponEmploymentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageWeaponEmploymentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageWeaponEmploymentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Weapons.
           *
           * @return The acecssor that provides access to the complex content that is identified by Weapons.
           */
         virtual const uci::type::WeaponeeringType& getWeapons() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Weapons.
           *
           * @return The acecssor that provides access to the complex content that is identified by Weapons.
           */
         virtual uci::type::WeaponeeringType& getWeapons()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Weapons to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Weapons
           */
         virtual void setWeapons(const uci::type::WeaponeeringType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponSourceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponSourceID.
           */
         virtual const uci::type::SystemID_Type& getWeaponSourceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeaponSourceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeaponSourceID.
           */
         virtual uci::type::SystemID_Type& getWeaponSourceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeaponSourceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeaponSourceID
           */
         virtual void setWeaponSourceID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WeaponSourceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WeaponSourceID is emabled or not
           */
         virtual bool hasWeaponSourceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WeaponSourceID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableWeaponSourceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WeaponSourceID */
         virtual void clearWeaponSourceID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ImpactTimeRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by ImpactTimeRange.
           */
         virtual const uci::type::DateTimeRangeType& getImpactTimeRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ImpactTimeRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by ImpactTimeRange.
           */
         virtual uci::type::DateTimeRangeType& getImpactTimeRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ImpactTimeRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ImpactTimeRange
           */
         virtual void setImpactTimeRange(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ImpactTimeRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ImpactTimeRange is emabled or not
           */
         virtual bool hasImpactTimeRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ImpactTimeRange
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableImpactTimeRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ImpactTimeRange */
         virtual void clearImpactTimeRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual const uci::type::Point2D_Type& getLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual uci::type::Point2D_Type& getLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Location to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Location
           */
         virtual void setLocation(const uci::type::Point2D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Location exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Location is emabled or not
           */
         virtual bool hasLocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Location
           *
           * @param type = uci::type::accessorType::point2D_Type This Accessor's accessor type
           */
         virtual void enableLocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::point2D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Location */
         virtual void clearLocation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual const uci::type::TaskID_Type& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual uci::type::TaskID_Type& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskID
           */
         virtual void setTaskID(const uci::type::TaskID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TaskID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TaskID is emabled or not
           */
         virtual bool hasTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TaskID
           *
           * @param type = uci::type::accessorType::taskID_Type This Accessor's accessor type
           */
         virtual void enableTaskID(uci::base::accessorType::AccessorType type = uci::type::accessorType::taskID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TaskID */
         virtual void clearTaskID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageWeaponEmploymentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageWeaponEmploymentType to copy from
           */
         DamageWeaponEmploymentType(const DamageWeaponEmploymentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageWeaponEmploymentType to the contents of the
           * DamageWeaponEmploymentType on the right hand side (rhs) of the assignment operator.DamageWeaponEmploymentType [only
           * available to derived classes]
           *
           * @param rhs The DamageWeaponEmploymentType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DamageWeaponEmploymentType
           * @return A constant reference to this DamageWeaponEmploymentType.
           */
         const DamageWeaponEmploymentType& operator=(const DamageWeaponEmploymentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageWeaponEmploymentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageWeaponEmploymentType_h

