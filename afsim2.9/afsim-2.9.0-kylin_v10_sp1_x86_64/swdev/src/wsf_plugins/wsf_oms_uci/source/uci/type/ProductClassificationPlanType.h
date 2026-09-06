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
#ifndef Uci__Type__ProductClassificationPlanType_h
#define Uci__Type__ProductClassificationPlanType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ClassificationConstrainedSubPlanType_h)
# include "uci/type/ClassificationConstrainedSubPlanType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type represents the system classification plan for a type of product. */
      class ProductClassificationPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductClassificationPlanType()
         { }

         /** Returns this accessor's type constant, i.e. ProductClassificationPlanType
           *
           * @return This accessor's type constant, i.e. ProductClassificationPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productClassificationPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductClassificationPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A constrained subplan specifies classifications that should be used instead of the default if the filters for those
           * classifications are satisfied. If the filters are satisfied in a way that determines multiple classifications could
           * be applied then the ConstrainedSubplan Rank can be used to determine which classification to apply. If subplans have
           * the same rank, the applicable subplan with the highest classification should be applied. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ClassificationConstrainedSubPlanType, uci::type::accessorType::classificationConstrainedSubPlanType> ConstrainedSubPlan;

         /** Returns the accessor that provides access to the complex content that is identified by the DefaultSubPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultSubPlan.
           */
         virtual const uci::type::SecurityInformationType& getDefaultSubPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultSubPlan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultSubPlan.
           */
         virtual uci::type::SecurityInformationType& getDefaultSubPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultSubPlan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultSubPlan
           */
         virtual void setDefaultSubPlan(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConstrainedSubPlan.
           *
           * @return The bounded list identified by ConstrainedSubPlan.
           */
         virtual const uci::type::ProductClassificationPlanType::ConstrainedSubPlan& getConstrainedSubPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConstrainedSubPlan.
           *
           * @return The bounded list identified by ConstrainedSubPlan.
           */
         virtual uci::type::ProductClassificationPlanType::ConstrainedSubPlan& getConstrainedSubPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConstrainedSubPlan.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConstrainedSubPlan(const uci::type::ProductClassificationPlanType::ConstrainedSubPlan& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductClassificationPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductClassificationPlanType to copy from
           */
         ProductClassificationPlanType(const ProductClassificationPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductClassificationPlanType to the contents of the
           * ProductClassificationPlanType on the right hand side (rhs) of the assignment operator.ProductClassificationPlanType
           * [only available to derived classes]
           *
           * @param rhs The ProductClassificationPlanType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductClassificationPlanType
           * @return A constant reference to this ProductClassificationPlanType.
           */
         const ProductClassificationPlanType& operator=(const ProductClassificationPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductClassificationPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductClassificationPlanType_h

