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
#ifndef Uci__Type__DefensiveJammerSystemInfoType_h
#define Uci__Type__DefensiveJammerSystemInfoType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DefensiveJammerSystemTypeEnum_h)
# include "uci/type/DefensiveJammerSystemTypeEnum.h"
#endif

#if !defined(Uci__Type__DefensiveJammerSystemStatusEnum_h)
# include "uci/type/DefensiveJammerSystemStatusEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the type and status of a defensive jammer system. */
      class DefensiveJammerSystemInfoType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DefensiveJammerSystemInfoType()
         { }

         /** Returns this accessor's type constant, i.e. DefensiveJammerSystemInfoType
           *
           * @return This accessor's type constant, i.e. DefensiveJammerSystemInfoType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::defensiveJammerSystemInfoType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DefensiveJammerSystemInfoType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DefensiveJammerSystemType.
           *
           * @return The value of the enumeration identified by DefensiveJammerSystemType.
           */
         virtual const uci::type::DefensiveJammerSystemTypeEnum& getDefensiveJammerSystemType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DefensiveJammerSystemType.
           *
           * @return The value of the enumeration identified by DefensiveJammerSystemType.
           */
         virtual uci::type::DefensiveJammerSystemTypeEnum& getDefensiveJammerSystemType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DefensiveJammerSystemType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDefensiveJammerSystemType(const uci::type::DefensiveJammerSystemTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DefensiveJammerSystemType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDefensiveJammerSystemType(uci::type::DefensiveJammerSystemTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DefensiveJammerSystemStatus.
           *
           * @return The value of the enumeration identified by DefensiveJammerSystemStatus.
           */
         virtual const uci::type::DefensiveJammerSystemStatusEnum& getDefensiveJammerSystemStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DefensiveJammerSystemStatus.
           *
           * @return The value of the enumeration identified by DefensiveJammerSystemStatus.
           */
         virtual uci::type::DefensiveJammerSystemStatusEnum& getDefensiveJammerSystemStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DefensiveJammerSystemStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDefensiveJammerSystemStatus(const uci::type::DefensiveJammerSystemStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DefensiveJammerSystemStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDefensiveJammerSystemStatus(uci::type::DefensiveJammerSystemStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DefensiveJammerSystemInfoType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DefensiveJammerSystemInfoType to copy from
           */
         DefensiveJammerSystemInfoType(const DefensiveJammerSystemInfoType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DefensiveJammerSystemInfoType to the contents of the
           * DefensiveJammerSystemInfoType on the right hand side (rhs) of the assignment operator.DefensiveJammerSystemInfoType
           * [only available to derived classes]
           *
           * @param rhs The DefensiveJammerSystemInfoType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::DefensiveJammerSystemInfoType
           * @return A constant reference to this DefensiveJammerSystemInfoType.
           */
         const DefensiveJammerSystemInfoType& operator=(const DefensiveJammerSystemInfoType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DefensiveJammerSystemInfoType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DefensiveJammerSystemInfoType_h

