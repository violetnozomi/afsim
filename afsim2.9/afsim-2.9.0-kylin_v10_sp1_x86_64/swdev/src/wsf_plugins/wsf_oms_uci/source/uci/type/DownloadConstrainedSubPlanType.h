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
#ifndef Uci__Type__DownloadConstrainedSubPlanType_h
#define Uci__Type__DownloadConstrainedSubPlanType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductFilterType_h)
# include "uci/type/ProductFilterType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DownloadConstrainedSubPlanType sequence accessor class */
      class DownloadConstrainedSubPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DownloadConstrainedSubPlanType()
         { }

         /** Returns this accessor's type constant, i.e. DownloadConstrainedSubPlanType
           *
           * @return This accessor's type constant, i.e. DownloadConstrainedSubPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::downloadConstrainedSubPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DownloadConstrainedSubPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filters are used to determine if the constrained subplan applies. All of the filter criteria within a constrained
           * subplan must be satisfied to apply the subplan. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductFilterType, uci::type::accessorType::productFilterType> Filter;

         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedSubPlanRank.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedSubPlanRank.
           */
         virtual const uci::type::RankingType& getConstrainedSubPlanRank() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedSubPlanRank.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedSubPlanRank.
           */
         virtual uci::type::RankingType& getConstrainedSubPlanRank()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConstrainedSubPlanRank to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConstrainedSubPlanRank
           */
         virtual void setConstrainedSubPlanRank(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AutoDownload.
           *
           * @return The value of the simple primitive data type identified by AutoDownload.
           */
         virtual xs::Boolean getAutoDownload() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AutoDownload.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAutoDownload(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Filter.
           *
           * @return The bounded list identified by Filter.
           */
         virtual const uci::type::DownloadConstrainedSubPlanType::Filter& getFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Filter.
           *
           * @return The bounded list identified by Filter.
           */
         virtual uci::type::DownloadConstrainedSubPlanType::Filter& getFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Filter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFilter(const uci::type::DownloadConstrainedSubPlanType::Filter& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DownloadConstrainedSubPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DownloadConstrainedSubPlanType to copy from
           */
         DownloadConstrainedSubPlanType(const DownloadConstrainedSubPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DownloadConstrainedSubPlanType to the contents of the
           * DownloadConstrainedSubPlanType on the right hand side (rhs) of the assignment operator.DownloadConstrainedSubPlanType
           * [only available to derived classes]
           *
           * @param rhs The DownloadConstrainedSubPlanType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::DownloadConstrainedSubPlanType
           * @return A constant reference to this DownloadConstrainedSubPlanType.
           */
         const DownloadConstrainedSubPlanType& operator=(const DownloadConstrainedSubPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DownloadConstrainedSubPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DownloadConstrainedSubPlanType_h

