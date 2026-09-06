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
#ifndef Uci__Type__StalenessDomainMobilityType_h
#define Uci__Type__StalenessDomainMobilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EnvironmentEnum_h)
# include "uci/type/EnvironmentEnum.h"
#endif

#if !defined(Uci__Type__MobilityEnum_h)
# include "uci/type/MobilityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the StalenessDomainMobilityType sequence accessor class */
      class StalenessDomainMobilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StalenessDomainMobilityType()
         { }

         /** Returns this accessor's type constant, i.e. StalenessDomainMobilityType
           *
           * @return This accessor's type constant, i.e. StalenessDomainMobilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::stalenessDomainMobilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StalenessDomainMobilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Domain.
           *
           * @return The value of the enumeration identified by Domain.
           */
         virtual const uci::type::EnvironmentEnum& getDomain() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Domain.
           *
           * @return The value of the enumeration identified by Domain.
           */
         virtual uci::type::EnvironmentEnum& getDomain()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Domain.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDomain(const uci::type::EnvironmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Domain.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDomain(uci::type::EnvironmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Domainis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasDomain() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getDomain will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::environmentEnum This Accessor's accessor type
           */
         virtual void enableDomain(uci::base::accessorType::AccessorType type = uci::type::accessorType::environmentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearDomain()
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



      protected:

         /** The constructor [only available to derived classes]. */
         StalenessDomainMobilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StalenessDomainMobilityType to copy from
           */
         StalenessDomainMobilityType(const StalenessDomainMobilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StalenessDomainMobilityType to the contents of the
           * StalenessDomainMobilityType on the right hand side (rhs) of the assignment operator.StalenessDomainMobilityType [only
           * available to derived classes]
           *
           * @param rhs The StalenessDomainMobilityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::StalenessDomainMobilityType
           * @return A constant reference to this StalenessDomainMobilityType.
           */
         const StalenessDomainMobilityType& operator=(const StalenessDomainMobilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StalenessDomainMobilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StalenessDomainMobilityType_h

