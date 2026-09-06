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
#ifndef Uci__Type__ThreatTargetPrioritizationPriorityType_h
#define Uci__Type__ThreatTargetPrioritizationPriorityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MetaTaskTypeEnum_h)
# include "uci/type/MetaTaskTypeEnum.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThreatTargetPrioritizationPriorityType sequence accessor class */
      class ThreatTargetPrioritizationPriorityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreatTargetPrioritizationPriorityType()
         { }

         /** Returns this accessor's type constant, i.e. ThreatTargetPrioritizationPriorityType
           *
           * @return This accessor's type constant, i.e. ThreatTargetPrioritizationPriorityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatTargetPrioritizationPriorityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatTargetPrioritizationPriorityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PriorityType.
           *
           * @return The value of the enumeration identified by PriorityType.
           */
         virtual const uci::type::MetaTaskTypeEnum& getPriorityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PriorityType.
           *
           * @return The value of the enumeration identified by PriorityType.
           */
         virtual uci::type::MetaTaskTypeEnum& getPriorityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PriorityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPriorityType(const uci::type::MetaTaskTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PriorityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPriorityType(uci::type::MetaTaskTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PriorityTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPriorityType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPriorityType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::metaTaskTypeEnum This Accessor's accessor type
           */
         virtual void enablePriorityType(uci::base::accessorType::AccessorType type = uci::type::accessorType::metaTaskTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPriorityType()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PriorityValue.
           *
           * @return The acecssor that provides access to the complex content that is identified by PriorityValue.
           */
         virtual const uci::type::RankingType& getPriorityValue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PriorityValue.
           *
           * @return The acecssor that provides access to the complex content that is identified by PriorityValue.
           */
         virtual uci::type::RankingType& getPriorityValue()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PriorityValue to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PriorityValue
           */
         virtual void setPriorityValue(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatTargetPrioritizationPriorityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatTargetPrioritizationPriorityType to copy from
           */
         ThreatTargetPrioritizationPriorityType(const ThreatTargetPrioritizationPriorityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatTargetPrioritizationPriorityType to the contents of the
           * ThreatTargetPrioritizationPriorityType on the right hand side (rhs) of the assignment
           * operator.ThreatTargetPrioritizationPriorityType [only available to derived classes]
           *
           * @param rhs The ThreatTargetPrioritizationPriorityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ThreatTargetPrioritizationPriorityType
           * @return A constant reference to this ThreatTargetPrioritizationPriorityType.
           */
         const ThreatTargetPrioritizationPriorityType& operator=(const ThreatTargetPrioritizationPriorityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatTargetPrioritizationPriorityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatTargetPrioritizationPriorityType_h

