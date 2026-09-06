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
#ifndef Uci__Type__DamageProbabilityZoneType_h
#define Uci__Type__DamageProbabilityZoneType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageProbabilityZoneType sequence accessor class */
      class DamageProbabilityZoneType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageProbabilityZoneType()
         { }

         /** Returns this accessor's type constant, i.e. DamageProbabilityZoneType
           *
           * @return This accessor's type constant, i.e. DamageProbabilityZoneType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageProbabilityZoneType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageProbabilityZoneType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumDamageProbability.
           *
           * @return The value of the simple primitive data type identified by MinimumDamageProbability.
           */
         virtual uci::type::PercentTypeValue getMinimumDamageProbability() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumDamageProbability.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumDamageProbability(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumDamageProbability.
           *
           * @return The value of the simple primitive data type identified by MaximumDamageProbability.
           */
         virtual uci::type::PercentTypeValue getMaximumDamageProbability() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumDamageProbability.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumDamageProbability(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageZone.
           */
         virtual const uci::type::ZoneType& getDamageZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageZone.
           */
         virtual uci::type::ZoneType& getDamageZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamageZone to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamageZone
           */
         virtual void setDamageZone(const uci::type::ZoneType& value)
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


         /** Returns whether the Element that is identified by DMPI_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DMPI_ID is emabled or not
           */
         virtual bool hasDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DMPI_ID
           *
           * @param type = uci::type::accessorType::dMPI_ID_Type This Accessor's accessor type
           */
         virtual void enableDMPI_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::dMPI_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DMPI_ID */
         virtual void clearDMPI_ID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageProbabilityZoneType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageProbabilityZoneType to copy from
           */
         DamageProbabilityZoneType(const DamageProbabilityZoneType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageProbabilityZoneType to the contents of the
           * DamageProbabilityZoneType on the right hand side (rhs) of the assignment operator.DamageProbabilityZoneType [only
           * available to derived classes]
           *
           * @param rhs The DamageProbabilityZoneType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DamageProbabilityZoneType
           * @return A constant reference to this DamageProbabilityZoneType.
           */
         const DamageProbabilityZoneType& operator=(const DamageProbabilityZoneType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageProbabilityZoneType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageProbabilityZoneType_h

