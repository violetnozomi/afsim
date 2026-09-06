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
#ifndef Uci__Type__RF_ResourceRequestMDT_h
#define Uci__Type__RF_ResourceRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ResourceRequestID_Type_h)
# include "uci/type/ResourceRequestID_Type.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

#if !defined(Uci__Type__RequestingFunctionType_h)
# include "uci/type/RequestingFunctionType.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ResourceRequestOptionType_h)
# include "uci/type/ResourceRequestOptionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_ResourceRequestMDT sequence accessor class */
      class RF_ResourceRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_ResourceRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. RF_ResourceRequestMDT
           *
           * @return This accessor's type constant, i.e. RF_ResourceRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ResourceRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ResourceRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Each entry in this array represents an option that can be used to fill the resource request. Only one option will be
           * allocated. They are assumed to be in order of preference. Optional only in the case of REMOVE. If omitted for REMOVE,
           * all allocations to the Activity will be deallocated. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ResourceRequestOptionType, uci::type::accessorType::resourceRequestOptionType> ResourceRequestOptions;

         /** Returns the accessor that provides access to the complex content that is identified by the ResourceRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ResourceRequestID.
           */
         virtual const uci::type::ResourceRequestID_Type& getResourceRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ResourceRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ResourceRequestID.
           */
         virtual uci::type::ResourceRequestID_Type& getResourceRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ResourceRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ResourceRequestID
           */
         virtual void setResourceRequestID(const uci::type::ResourceRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ResourceRequestState.
           *
           * @return The value of the enumeration identified by ResourceRequestState.
           */
         virtual const uci::type::MessageStateEnum& getResourceRequestState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ResourceRequestState.
           *
           * @return The value of the enumeration identified by ResourceRequestState.
           */
         virtual uci::type::MessageStateEnum& getResourceRequestState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ResourceRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setResourceRequestState(const uci::type::MessageStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ResourceRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setResourceRequestState(uci::type::MessageStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestingFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestingFunction.
           */
         virtual const uci::type::RequestingFunctionType& getRequestingFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestingFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestingFunction.
           */
         virtual uci::type::RequestingFunctionType& getRequestingFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequestingFunction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequestingFunction
           */
         virtual void setRequestingFunction(const uci::type::RequestingFunctionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Ranking.
           *
           * @return The acecssor that provides access to the complex content that is identified by Ranking.
           */
         virtual const uci::type::RankingType& getRanking() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Ranking.
           *
           * @return The acecssor that provides access to the complex content that is identified by Ranking.
           */
         virtual uci::type::RankingType& getRanking()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Ranking to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Ranking
           */
         virtual void setRanking(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Ranking exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Ranking is emabled or not
           */
         virtual bool hasRanking() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Ranking
           *
           * @param type = uci::type::accessorType::rankingType This Accessor's accessor type
           */
         virtual void enableRanking(uci::base::accessorType::AccessorType type = uci::type::accessorType::rankingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Ranking */
         virtual void clearRanking()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DurationRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by DurationRange.
           */
         virtual const uci::type::DateTimeRangeType& getDurationRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DurationRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by DurationRange.
           */
         virtual uci::type::DateTimeRangeType& getDurationRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DurationRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DurationRange
           */
         virtual void setDurationRange(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DurationRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DurationRange is emabled or not
           */
         virtual bool hasDurationRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DurationRange
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableDurationRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DurationRange */
         virtual void clearDurationRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestedFrequencyRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestedFrequencyRange.
           */
         virtual const uci::type::FrequencyRangeType& getRequestedFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestedFrequencyRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestedFrequencyRange.
           */
         virtual uci::type::FrequencyRangeType& getRequestedFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequestedFrequencyRange to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequestedFrequencyRange
           */
         virtual void setRequestedFrequencyRange(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RequestedFrequencyRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RequestedFrequencyRange is emabled or not
           */
         virtual bool hasRequestedFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RequestedFrequencyRange
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableRequestedFrequencyRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RequestedFrequencyRange */
         virtual void clearRequestedFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceRequestOptions.
           *
           * @return The bounded list identified by ResourceRequestOptions.
           */
         virtual const uci::type::RF_ResourceRequestMDT::ResourceRequestOptions& getResourceRequestOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceRequestOptions.
           *
           * @return The bounded list identified by ResourceRequestOptions.
           */
         virtual uci::type::RF_ResourceRequestMDT::ResourceRequestOptions& getResourceRequestOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ResourceRequestOptions.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResourceRequestOptions(const uci::type::RF_ResourceRequestMDT::ResourceRequestOptions& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ResourceRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ResourceRequestMDT to copy from
           */
         RF_ResourceRequestMDT(const RF_ResourceRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ResourceRequestMDT to the contents of the RF_ResourceRequestMDT
           * on the right hand side (rhs) of the assignment operator.RF_ResourceRequestMDT [only available to derived classes]
           *
           * @param rhs The RF_ResourceRequestMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RF_ResourceRequestMDT
           * @return A constant reference to this RF_ResourceRequestMDT.
           */
         const RF_ResourceRequestMDT& operator=(const RF_ResourceRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ResourceRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ResourceRequestMDT_h

