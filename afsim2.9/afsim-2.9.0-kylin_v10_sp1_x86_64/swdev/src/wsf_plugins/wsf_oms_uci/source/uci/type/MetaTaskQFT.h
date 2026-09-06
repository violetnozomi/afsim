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
#ifndef Uci__Type__MetaTaskQFT_h
#define Uci__Type__MetaTaskQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MetaTaskID_Type_h)
# include "uci/type/MetaTaskID_Type.h"
#endif

#if !defined(Uci__Type__MetaTaskTypeEnum_h)
# include "uci/type/MetaTaskTypeEnum.h"
#endif

#if !defined(Uci__Type__MetaTaskStatusEnum_h)
# include "uci/type/MetaTaskStatusEnum.h"
#endif

#if !defined(Uci__Type__MetaTaskTargetType_h)
# include "uci/type/MetaTaskTargetType.h"
#endif

#if !defined(Uci__Type__GeoFiltersType_h)
# include "uci/type/GeoFiltersType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MetaTaskQFT sequence accessor class */
      class MetaTaskQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~MetaTaskQFT()
         { }

         /** Returns this accessor's type constant, i.e. MetaTaskQFT
           *
           * @return This accessor's type constant, i.e. MetaTaskQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::metaTaskQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MetaTaskQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MetaTaskID_Type, uci::type::accessorType::metaTaskID_Type> MetaTaskID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 8] */
         typedef uci::base::BoundedList<uci::type::MetaTaskTypeEnum, uci::type::accessorType::metaTaskTypeEnum> MetaTaskType;

         /** [Minimum occurrences: 0] [Maximum occurrences: 5] */
         typedef uci::base::BoundedList<uci::type::MetaTaskStatusEnum, uci::type::accessorType::metaTaskStatusEnum> MetaTaskStatus;

         /** This element lets a geographical filter be applied to the query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::GeoFiltersType, uci::type::accessorType::geoFiltersType> GeoFilter;

         /** This element lets a temporal filter be applied to the query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeType, uci::type::accessorType::dateTimeRangeType> TemporalFilter;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @return The value of the simple primitive data type identified by IncludeStatus.
           */
         virtual xs::Boolean getIncludeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeStatus(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MetaTaskID.
           *
           * @return The bounded list identified by MetaTaskID.
           */
         virtual const uci::type::MetaTaskQFT::MetaTaskID& getMetaTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MetaTaskID.
           *
           * @return The bounded list identified by MetaTaskID.
           */
         virtual uci::type::MetaTaskQFT::MetaTaskID& getMetaTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MetaTaskID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMetaTaskID(const uci::type::MetaTaskQFT::MetaTaskID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MetaTaskType.
           *
           * @return The bounded list identified by MetaTaskType.
           */
         virtual const uci::type::MetaTaskQFT::MetaTaskType& getMetaTaskType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MetaTaskType.
           *
           * @return The bounded list identified by MetaTaskType.
           */
         virtual uci::type::MetaTaskQFT::MetaTaskType& getMetaTaskType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MetaTaskType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMetaTaskType(const uci::type::MetaTaskQFT::MetaTaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MetaTaskStatus.
           *
           * @return The bounded list identified by MetaTaskStatus.
           */
         virtual const uci::type::MetaTaskQFT::MetaTaskStatus& getMetaTaskStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MetaTaskStatus.
           *
           * @return The bounded list identified by MetaTaskStatus.
           */
         virtual uci::type::MetaTaskQFT::MetaTaskStatus& getMetaTaskStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MetaTaskStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMetaTaskStatus(const uci::type::MetaTaskQFT::MetaTaskStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual const uci::type::MetaTaskTargetType& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual uci::type::MetaTaskTargetType& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Target to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Target
           */
         virtual void setTarget(const uci::type::MetaTaskTargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Target exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Target is emabled or not
           */
         virtual bool hasTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Target
           *
           * @param type = uci::type::accessorType::metaTaskTargetType This Accessor's accessor type
           */
         virtual void enableTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::metaTaskTargetType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Target */
         virtual void clearTarget()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual const uci::type::MetaTaskQFT::GeoFilter& getGeoFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the GeoFilter.
           *
           * @return The bounded list identified by GeoFilter.
           */
         virtual uci::type::MetaTaskQFT::GeoFilter& getGeoFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the GeoFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setGeoFilter(const uci::type::MetaTaskQFT::GeoFilter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual const uci::type::MetaTaskQFT::TemporalFilter& getTemporalFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TemporalFilter.
           *
           * @return The bounded list identified by TemporalFilter.
           */
         virtual uci::type::MetaTaskQFT::TemporalFilter& getTemporalFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TemporalFilter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTemporalFilter(const uci::type::MetaTaskQFT::TemporalFilter& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MetaTaskQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MetaTaskQFT to copy from
           */
         MetaTaskQFT(const MetaTaskQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MetaTaskQFT to the contents of the MetaTaskQFT on the right hand
           * side (rhs) of the assignment operator.MetaTaskQFT [only available to derived classes]
           *
           * @param rhs The MetaTaskQFT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::MetaTaskQFT
           * @return A constant reference to this MetaTaskQFT.
           */
         const MetaTaskQFT& operator=(const MetaTaskQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MetaTaskQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MetaTaskQFT_h

