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
#ifndef Uci__Type__OrderOfBattleTimestampsType_h
#define Uci__Type__OrderOfBattleTimestampsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeSigmaType_h)
# include "uci/type/DateTimeSigmaType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates relevant temporal information. */
      class OrderOfBattleTimestampsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrderOfBattleTimestampsType()
         { }

         /** Returns this accessor's type constant, i.e. OrderOfBattleTimestampsType
           *
           * @return This accessor's type constant, i.e. OrderOfBattleTimestampsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orderOfBattleTimestampsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrderOfBattleTimestampsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CreationTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by CreationTime.
           */
         virtual const uci::type::DateTimeSigmaType& getCreationTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CreationTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by CreationTime.
           */
         virtual uci::type::DateTimeSigmaType& getCreationTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CreationTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CreationTime
           */
         virtual void setCreationTime(const uci::type::DateTimeSigmaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LastUpdateTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by LastUpdateTime.
           */
         virtual const uci::type::DateTimeSigmaType& getLastUpdateTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LastUpdateTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by LastUpdateTime.
           */
         virtual uci::type::DateTimeSigmaType& getLastUpdateTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LastUpdateTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LastUpdateTime
           */
         virtual void setLastUpdateTime(const uci::type::DateTimeSigmaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastUpdateTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastUpdateTime is emabled or not
           */
         virtual bool hasLastUpdateTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastUpdateTime
           *
           * @param type = uci::type::accessorType::dateTimeSigmaType This Accessor's accessor type
           */
         virtual void enableLastUpdateTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeSigmaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastUpdateTime */
         virtual void clearLastUpdateTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FirstObservationTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by FirstObservationTime.
           */
         virtual const uci::type::DateTimeSigmaType& getFirstObservationTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FirstObservationTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by FirstObservationTime.
           */
         virtual uci::type::DateTimeSigmaType& getFirstObservationTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FirstObservationTime to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FirstObservationTime
           */
         virtual void setFirstObservationTime(const uci::type::DateTimeSigmaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FirstObservationTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FirstObservationTime is emabled or not
           */
         virtual bool hasFirstObservationTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FirstObservationTime
           *
           * @param type = uci::type::accessorType::dateTimeSigmaType This Accessor's accessor type
           */
         virtual void enableFirstObservationTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeSigmaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FirstObservationTime */
         virtual void clearFirstObservationTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LastObservationTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by LastObservationTime.
           */
         virtual const uci::type::DateTimeSigmaType& getLastObservationTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LastObservationTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by LastObservationTime.
           */
         virtual uci::type::DateTimeSigmaType& getLastObservationTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LastObservationTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LastObservationTime
           */
         virtual void setLastObservationTime(const uci::type::DateTimeSigmaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastObservationTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastObservationTime is emabled or not
           */
         virtual bool hasLastObservationTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastObservationTime
           *
           * @param type = uci::type::accessorType::dateTimeSigmaType This Accessor's accessor type
           */
         virtual void enableLastObservationTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeSigmaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastObservationTime */
         virtual void clearLastObservationTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LastReviewedTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by LastReviewedTime.
           */
         virtual const uci::type::DateTimeSigmaType& getLastReviewedTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LastReviewedTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by LastReviewedTime.
           */
         virtual uci::type::DateTimeSigmaType& getLastReviewedTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LastReviewedTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LastReviewedTime
           */
         virtual void setLastReviewedTime(const uci::type::DateTimeSigmaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastReviewedTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastReviewedTime is emabled or not
           */
         virtual bool hasLastReviewedTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastReviewedTime
           *
           * @param type = uci::type::accessorType::dateTimeSigmaType This Accessor's accessor type
           */
         virtual void enableLastReviewedTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeSigmaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastReviewedTime */
         virtual void clearLastReviewedTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrderOfBattleTimestampsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrderOfBattleTimestampsType to copy from
           */
         OrderOfBattleTimestampsType(const OrderOfBattleTimestampsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrderOfBattleTimestampsType to the contents of the
           * OrderOfBattleTimestampsType on the right hand side (rhs) of the assignment operator.OrderOfBattleTimestampsType [only
           * available to derived classes]
           *
           * @param rhs The OrderOfBattleTimestampsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::OrderOfBattleTimestampsType
           * @return A constant reference to this OrderOfBattleTimestampsType.
           */
         const OrderOfBattleTimestampsType& operator=(const OrderOfBattleTimestampsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrderOfBattleTimestampsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrderOfBattleTimestampsType_h

