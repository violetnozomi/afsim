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
#ifndef Uci__Type__DMPI_DamageEstimateType_h
#define Uci__Type__DMPI_DamageEstimateType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

#if !defined(Uci__Type__DMPI_LocationType_h)
# include "uci/type/DMPI_LocationType.h"
#endif

#if !defined(Uci__Type__WeaponeeringType_h)
# include "uci/type/WeaponeeringType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_DamageEstimateType sequence accessor class */
      class DMPI_DamageEstimateType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DMPI_DamageEstimateType()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_DamageEstimateType
           *
           * @return This accessor's type constant, i.e. DMPI_DamageEstimateType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_DamageEstimateType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_DamageEstimateType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_ID.
           */
         virtual const uci::type::DMPI_ID_Type& getDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_ID.
           */
         virtual uci::type::DMPI_ID_Type& getDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DMPI_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DMPI_ID
           */
         virtual void setDMPI_ID(const uci::type::DMPI_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_Location.
           */
         virtual const uci::type::DMPI_LocationType& getDMPI_Location() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_Location.
           */
         virtual uci::type::DMPI_LocationType& getDMPI_Location()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DMPI_Location to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DMPI_Location
           */
         virtual void setDMPI_Location(const uci::type::DMPI_LocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DMPI_Location exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DMPI_Location is emabled or not
           */
         virtual bool hasDMPI_Location() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DMPI_Location
           *
           * @param type = uci::type::accessorType::dMPI_LocationType This Accessor's accessor type
           */
         virtual void enableDMPI_Location(uci::base::accessorType::AccessorType type = uci::type::accessorType::dMPI_LocationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DMPI_Location */
         virtual void clearDMPI_Location()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Weaponeering.
           *
           * @return The acecssor that provides access to the complex content that is identified by Weaponeering.
           */
         virtual const uci::type::WeaponeeringType& getWeaponeering() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Weaponeering.
           *
           * @return The acecssor that provides access to the complex content that is identified by Weaponeering.
           */
         virtual uci::type::WeaponeeringType& getWeaponeering()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Weaponeering to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Weaponeering
           */
         virtual void setWeaponeering(const uci::type::WeaponeeringType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Weaponeering exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Weaponeering is emabled or not
           */
         virtual bool hasWeaponeering() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Weaponeering
           *
           * @param type = uci::type::accessorType::weaponeeringType This Accessor's accessor type
           */
         virtual void enableWeaponeering(uci::base::accessorType::AccessorType type = uci::type::accessorType::weaponeeringType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Weaponeering */
         virtual void clearWeaponeering()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedBlastRadius.
           *
           * @return The value of the simple primitive data type identified by EstimatedBlastRadius.
           */
         virtual uci::type::DistanceTypeValue getEstimatedBlastRadius() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedBlastRadius.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedBlastRadius(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedBlastRadius exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedBlastRadius is emabled or not
           */
         virtual bool hasEstimatedBlastRadius() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedBlastRadius
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableEstimatedBlastRadius(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedBlastRadius */
         virtual void clearEstimatedBlastRadius()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_DamageEstimateType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_DamageEstimateType to copy from
           */
         DMPI_DamageEstimateType(const DMPI_DamageEstimateType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_DamageEstimateType to the contents of the
           * DMPI_DamageEstimateType on the right hand side (rhs) of the assignment operator.DMPI_DamageEstimateType [only
           * available to derived classes]
           *
           * @param rhs The DMPI_DamageEstimateType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DMPI_DamageEstimateType
           * @return A constant reference to this DMPI_DamageEstimateType.
           */
         const DMPI_DamageEstimateType& operator=(const DMPI_DamageEstimateType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_DamageEstimateType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_DamageEstimateType_h

