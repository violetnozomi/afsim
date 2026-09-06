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
#ifndef Uci__Type__SearchTrackActivityDetailsType_h
#define Uci__Type__SearchTrackActivityDetailsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SearchTrackActivityDetailsType sequence accessor class */
      class SearchTrackActivityDetailsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SearchTrackActivityDetailsType()
         { }

         /** Returns this accessor's type constant, i.e. SearchTrackActivityDetailsType
           *
           * @return This accessor's type constant, i.e. SearchTrackActivityDetailsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::searchTrackActivityDetailsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SearchTrackActivityDetailsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfTimesSearched.
           *
           * @return The value of the simple primitive data type identified by NumberOfTimesSearched.
           */
         virtual xs::UnsignedInt getNumberOfTimesSearched() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfTimesSearched.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfTimesSearched(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NumberOfTimesSearched exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NumberOfTimesSearched is emabled or not
           */
         virtual bool hasNumberOfTimesSearched() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NumberOfTimesSearched
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableNumberOfTimesSearched(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NumberOfTimesSearched */
         virtual void clearNumberOfTimesSearched()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TimeLastSearched.
           *
           * @return The value of the simple primitive data type identified by TimeLastSearched.
           */
         virtual uci::type::DateTimeTypeValue getTimeLastSearched() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeLastSearched.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeLastSearched(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeLastSearched exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeLastSearched is emabled or not
           */
         virtual bool hasTimeLastSearched() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeLastSearched
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableTimeLastSearched(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeLastSearched */
         virtual void clearTimeLastSearched()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Attempts.
           *
           * @return The value of the simple primitive data type identified by Attempts.
           */
         virtual xs::UnsignedInt getAttempts() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Attempts.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAttempts(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Attempts exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Attempts is emabled or not
           */
         virtual bool hasAttempts() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Attempts
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableAttempts(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Attempts */
         virtual void clearAttempts()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualRevisitPeriod.
           *
           * @return The value of the simple primitive data type identified by ActualRevisitPeriod.
           */
         virtual uci::type::DurationTypeValue getActualRevisitPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualRevisitPeriod.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualRevisitPeriod(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualRevisitPeriod exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualRevisitPeriod is emabled or not
           */
         virtual bool hasActualRevisitPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualRevisitPeriod
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableActualRevisitPeriod(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualRevisitPeriod */
         virtual void clearActualRevisitPeriod()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentageInFOR.
           *
           * @return The value of the simple primitive data type identified by PercentageInFOR.
           */
         virtual uci::type::PercentTypeValue getPercentageInFOR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentageInFOR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentageInFOR(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentageInFOR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentageInFOR is emabled or not
           */
         virtual bool hasPercentageInFOR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentageInFOR
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentageInFOR(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentageInFOR */
         virtual void clearPercentageInFOR()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualStartTime.
           *
           * @return The value of the simple primitive data type identified by ActualStartTime.
           */
         virtual uci::type::DateTimeTypeValue getActualStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualStartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualStartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualStartTime is emabled or not
           */
         virtual bool hasActualStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualStartTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualStartTime */
         virtual void clearActualStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualStopTime.
           *
           * @return The value of the simple primitive data type identified by ActualStopTime.
           */
         virtual uci::type::DateTimeTypeValue getActualStopTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualStopTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualStopTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualStopTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualStopTime is emabled or not
           */
         virtual bool hasActualStopTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualStopTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualStopTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualStopTime */
         virtual void clearActualStopTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentSearched.
           *
           * @return The value of the simple primitive data type identified by PercentSearched.
           */
         virtual uci::type::PercentTypeValue getPercentSearched() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentSearched.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentSearched(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentSearched exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentSearched is emabled or not
           */
         virtual bool hasPercentSearched() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentSearched
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentSearched(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentSearched */
         virtual void clearPercentSearched()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EntitiesReported.
           *
           * @return The value of the simple primitive data type identified by EntitiesReported.
           */
         virtual xs::UnsignedInt getEntitiesReported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EntitiesReported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEntitiesReported(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EntitiesReported exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EntitiesReported is emabled or not
           */
         virtual bool hasEntitiesReported() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EntitiesReported
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableEntitiesReported(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EntitiesReported */
         virtual void clearEntitiesReported()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CueAcquired.
           *
           * @return The value of the simple primitive data type identified by CueAcquired.
           */
         virtual xs::Boolean getCueAcquired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CueAcquired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCueAcquired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CueAcquired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CueAcquired is emabled or not
           */
         virtual bool hasCueAcquired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CueAcquired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableCueAcquired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CueAcquired */
         virtual void clearCueAcquired()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ProductsProduced.
           *
           * @return The value of the simple primitive data type identified by ProductsProduced.
           */
         virtual xs::UnsignedInt getProductsProduced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ProductsProduced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setProductsProduced(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductsProduced exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductsProduced is emabled or not
           */
         virtual bool hasProductsProduced() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductsProduced
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableProductsProduced(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductsProduced */
         virtual void clearProductsProduced()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HitsReported.
           *
           * @return The value of the simple primitive data type identified by HitsReported.
           */
         virtual xs::UnsignedInt getHitsReported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HitsReported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHitsReported(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HitsReported exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HitsReported is emabled or not
           */
         virtual bool hasHitsReported() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HitsReported
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableHitsReported(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HitsReported */
         virtual void clearHitsReported()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SearchTrackActivityDetailsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SearchTrackActivityDetailsType to copy from
           */
         SearchTrackActivityDetailsType(const SearchTrackActivityDetailsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SearchTrackActivityDetailsType to the contents of the
           * SearchTrackActivityDetailsType on the right hand side (rhs) of the assignment operator.SearchTrackActivityDetailsType
           * [only available to derived classes]
           *
           * @param rhs The SearchTrackActivityDetailsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SearchTrackActivityDetailsType
           * @return A constant reference to this SearchTrackActivityDetailsType.
           */
         const SearchTrackActivityDetailsType& operator=(const SearchTrackActivityDetailsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SearchTrackActivityDetailsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SearchTrackActivityDetailsType_h

