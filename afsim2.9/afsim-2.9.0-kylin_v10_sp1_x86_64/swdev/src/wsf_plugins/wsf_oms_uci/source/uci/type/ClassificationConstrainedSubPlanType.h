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
#ifndef Uci__Type__ClassificationConstrainedSubPlanType_h
#define Uci__Type__ClassificationConstrainedSubPlanType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
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

      /** This is the ClassificationConstrainedSubPlanType sequence accessor class */
      class ClassificationConstrainedSubPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ClassificationConstrainedSubPlanType()
         { }

         /** Returns this accessor's type constant, i.e. ClassificationConstrainedSubPlanType
           *
           * @return This accessor's type constant, i.e. ClassificationConstrainedSubPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::classificationConstrainedSubPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ClassificationConstrainedSubPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filters are used to determine if the constrained subplan applies. All of the filter criteria within a constrained
           * subplan must be satisfied to apply the subplan. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductFilterType, uci::type::accessorType::productFilterType> Filter;

         /** Returns the accessor that provides access to the complex content that is identified by the SecurityInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityInformation.
           */
         virtual const uci::type::SecurityInformationType& getSecurityInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityInformation.
           */
         virtual uci::type::SecurityInformationType& getSecurityInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SecurityInformation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SecurityInformation
           */
         virtual void setSecurityInformation(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


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


         /** Returns the bounded list that is identified by the Filter.
           *
           * @return The bounded list identified by Filter.
           */
         virtual const uci::type::ClassificationConstrainedSubPlanType::Filter& getFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Filter.
           *
           * @return The bounded list identified by Filter.
           */
         virtual uci::type::ClassificationConstrainedSubPlanType::Filter& getFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Filter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFilter(const uci::type::ClassificationConstrainedSubPlanType::Filter& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ClassificationConstrainedSubPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ClassificationConstrainedSubPlanType to copy from
           */
         ClassificationConstrainedSubPlanType(const ClassificationConstrainedSubPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ClassificationConstrainedSubPlanType to the contents of the
           * ClassificationConstrainedSubPlanType on the right hand side (rhs) of the assignment
           * operator.ClassificationConstrainedSubPlanType [only available to derived classes]
           *
           * @param rhs The ClassificationConstrainedSubPlanType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ClassificationConstrainedSubPlanType
           * @return A constant reference to this ClassificationConstrainedSubPlanType.
           */
         const ClassificationConstrainedSubPlanType& operator=(const ClassificationConstrainedSubPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ClassificationConstrainedSubPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ClassificationConstrainedSubPlanType_h

