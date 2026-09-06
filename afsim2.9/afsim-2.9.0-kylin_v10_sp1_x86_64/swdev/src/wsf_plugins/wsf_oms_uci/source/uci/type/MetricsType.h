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
#ifndef Uci__Type__MetricsType_h
#define Uci__Type__MetricsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EnduranceType_h)
# include "uci/type/EnduranceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ExpendableType_h)
# include "uci/type/ExpendableType.h"
#endif

#if !defined(Uci__Type__WeaponStoreType_h)
# include "uci/type/WeaponStoreType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MetricsType sequence accessor class */
      class MetricsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MetricsType()
         { }

         /** Returns this accessor's type constant, i.e. MetricsType
           *
           * @return This accessor's type constant, i.e. MetricsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::metricsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MetricsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Estimated expendables usage. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ExpendableType, uci::type::accessorType::expendableType> Expendables;

         /** Estimated Weapons usage. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::WeaponStoreType, uci::type::accessorType::weaponStoreType> Weapons;

         /** Returns the accessor that provides access to the complex content that is identified by the EnduranceUsage.
           *
           * @return The acecssor that provides access to the complex content that is identified by EnduranceUsage.
           */
         virtual const uci::type::EnduranceType& getEnduranceUsage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EnduranceUsage.
           *
           * @return The acecssor that provides access to the complex content that is identified by EnduranceUsage.
           */
         virtual uci::type::EnduranceType& getEnduranceUsage()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EnduranceUsage to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EnduranceUsage
           */
         virtual void setEnduranceUsage(const uci::type::EnduranceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EnduranceUsage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EnduranceUsage is emabled or not
           */
         virtual bool hasEnduranceUsage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EnduranceUsage
           *
           * @param type = uci::type::accessorType::enduranceType This Accessor's accessor type
           */
         virtual void enableEnduranceUsage(uci::base::accessorType::AccessorType type = uci::type::accessorType::enduranceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EnduranceUsage */
         virtual void clearEnduranceUsage()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Expendables.
           *
           * @return The bounded list identified by Expendables.
           */
         virtual const uci::type::MetricsType::Expendables& getExpendables() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Expendables.
           *
           * @return The bounded list identified by Expendables.
           */
         virtual uci::type::MetricsType::Expendables& getExpendables()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Expendables.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setExpendables(const uci::type::MetricsType::Expendables& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Weapons.
           *
           * @return The bounded list identified by Weapons.
           */
         virtual const uci::type::MetricsType::Weapons& getWeapons() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Weapons.
           *
           * @return The bounded list identified by Weapons.
           */
         virtual uci::type::MetricsType::Weapons& getWeapons()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Weapons.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWeapons(const uci::type::MetricsType::Weapons& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MetricsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MetricsType to copy from
           */
         MetricsType(const MetricsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MetricsType to the contents of the MetricsType on the right hand
           * side (rhs) of the assignment operator.MetricsType [only available to derived classes]
           *
           * @param rhs The MetricsType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::MetricsType
           * @return A constant reference to this MetricsType.
           */
         const MetricsType& operator=(const MetricsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MetricsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MetricsType_h

