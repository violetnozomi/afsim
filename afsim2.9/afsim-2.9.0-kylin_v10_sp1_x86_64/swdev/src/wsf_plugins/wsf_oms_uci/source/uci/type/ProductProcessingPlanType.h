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
#ifndef Uci__Type__ProductProcessingPlanType_h
#define Uci__Type__ProductProcessingPlanType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductTypeEnum_h)
# include "uci/type/ProductTypeEnum.h"
#endif

#if !defined(Uci__Type__ProductProcessingType_h)
# include "uci/type/ProductProcessingType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProcessingConstrainedSubPlanType_h)
# include "uci/type/ProcessingConstrainedSubPlanType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type represents the system classification plan for a type of product. */
      class ProductProcessingPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductProcessingPlanType()
         { }

         /** Returns this accessor's type constant, i.e. ProductProcessingPlanType
           *
           * @return This accessor's type constant, i.e. ProductProcessingPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productProcessingPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductProcessingPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This field overrides the default subplan based on a list of filters which include things like time, classification,
           * system ID, and zones. If the filters are satisfied in a way that determines multiple processing types and parameters
           * could be applied then then the constrainedSubplanRank can be used to determine which to apply. If subplans have the
           * same rank, the product may be processed in accordance with each applicable subplan for creation of multiple processed
           * products. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProcessingConstrainedSubPlanType, uci::type::accessorType::processingConstrainedSubPlanType> ConstrainedSubPlan;

         /** Returns the value of the enumeration that is identified by the ProductType.
           *
           * @return The value of the enumeration identified by ProductType.
           */
         virtual const uci::type::ProductTypeEnum& getProductType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ProductType.
           *
           * @return The value of the enumeration identified by ProductType.
           */
         virtual uci::type::ProductTypeEnum& getProductType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductType(const uci::type::ProductTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductType(uci::type::ProductTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultSubPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultSubPlan.
           */
         virtual const uci::type::ProductProcessingType& getDefaultSubPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultSubPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultSubPlan.
           */
         virtual uci::type::ProductProcessingType& getDefaultSubPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultSubPlan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultSubPlan
           */
         virtual void setDefaultSubPlan(const uci::type::ProductProcessingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConstrainedSubPlan.
           *
           * @return The bounded list identified by ConstrainedSubPlan.
           */
         virtual const uci::type::ProductProcessingPlanType::ConstrainedSubPlan& getConstrainedSubPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConstrainedSubPlan.
           *
           * @return The bounded list identified by ConstrainedSubPlan.
           */
         virtual uci::type::ProductProcessingPlanType::ConstrainedSubPlan& getConstrainedSubPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConstrainedSubPlan.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConstrainedSubPlan(const uci::type::ProductProcessingPlanType::ConstrainedSubPlan& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductProcessingPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductProcessingPlanType to copy from
           */
         ProductProcessingPlanType(const ProductProcessingPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductProcessingPlanType to the contents of the
           * ProductProcessingPlanType on the right hand side (rhs) of the assignment operator.ProductProcessingPlanType [only
           * available to derived classes]
           *
           * @param rhs The ProductProcessingPlanType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ProductProcessingPlanType
           * @return A constant reference to this ProductProcessingPlanType.
           */
         const ProductProcessingPlanType& operator=(const ProductProcessingPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductProcessingPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductProcessingPlanType_h

