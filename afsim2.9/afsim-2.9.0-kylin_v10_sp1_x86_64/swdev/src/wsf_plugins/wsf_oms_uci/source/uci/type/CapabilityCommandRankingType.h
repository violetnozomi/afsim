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
#ifndef Uci__Type__CapabilityCommandRankingType_h
#define Uci__Type__CapabilityCommandRankingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityCommandRankingType sequence accessor class */
      class CapabilityCommandRankingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityCommandRankingType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityCommandRankingType
           *
           * @return This accessor's type constant, i.e. CapabilityCommandRankingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityCommandRankingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityCommandRankingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Rank.
           *
           * @return The acecssor that provides access to the complex content that is identified by Rank.
           */
         virtual const uci::type::RankingType& getRank() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Rank.
           *
           * @return The acecssor that provides access to the complex content that is identified by Rank.
           */
         virtual uci::type::RankingType& getRank()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Rank to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Rank
           */
         virtual void setRank(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InterruptOtherActivities.
           *
           * @return The value of the simple primitive data type identified by InterruptOtherActivities.
           */
         virtual xs::Boolean getInterruptOtherActivities() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InterruptOtherActivities.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInterruptOtherActivities(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InterruptOtherActivities exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InterruptOtherActivities is emabled or not
           */
         virtual bool hasInterruptOtherActivities() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InterruptOtherActivities
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableInterruptOtherActivities(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InterruptOtherActivities */
         virtual void clearInterruptOtherActivities()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CapabilityPrecedenceOverride.
           *
           * @return The value of the simple primitive data type identified by CapabilityPrecedenceOverride.
           */
         virtual xs::UnsignedInt getCapabilityPrecedenceOverride() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CapabilityPrecedenceOverride.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCapabilityPrecedenceOverride(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CapabilityPrecedenceOverride exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CapabilityPrecedenceOverride is emabled or not
           */
         virtual bool hasCapabilityPrecedenceOverride() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CapabilityPrecedenceOverride
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableCapabilityPrecedenceOverride(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CapabilityPrecedenceOverride */
         virtual void clearCapabilityPrecedenceOverride()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityCommandRankingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityCommandRankingType to copy from
           */
         CapabilityCommandRankingType(const CapabilityCommandRankingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityCommandRankingType to the contents of the
           * CapabilityCommandRankingType on the right hand side (rhs) of the assignment operator.CapabilityCommandRankingType
           * [only available to derived classes]
           *
           * @param rhs The CapabilityCommandRankingType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CapabilityCommandRankingType
           * @return A constant reference to this CapabilityCommandRankingType.
           */
         const CapabilityCommandRankingType& operator=(const CapabilityCommandRankingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityCommandRankingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityCommandRankingType_h

