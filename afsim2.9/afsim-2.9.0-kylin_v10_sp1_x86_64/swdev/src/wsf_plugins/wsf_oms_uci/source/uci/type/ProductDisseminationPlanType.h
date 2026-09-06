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
#ifndef Uci__Type__ProductDisseminationPlanType_h
#define Uci__Type__ProductDisseminationPlanType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductTypeEnum_h)
# include "uci/type/ProductTypeEnum.h"
#endif

#if !defined(Uci__Type__DisseminationSubplanType_h)
# include "uci/type/DisseminationSubplanType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DisseminationConstrainedSubplanType_h)
# include "uci/type/DisseminationConstrainedSubplanType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductDisseminationPlanType sequence accessor class */
      class ProductDisseminationPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductDisseminationPlanType()
         { }

         /** Returns this accessor's type constant, i.e. ProductDisseminationPlanType
           *
           * @return This accessor's type constant, i.e. ProductDisseminationPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productDisseminationPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductDisseminationPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This field overrides the default subplan based on a list of filters which include things like time, classification,
           * system ID, and zones. If the filters are satisfied in a way that determines multiple destinations could be applied
           * then then the constrainedSubplanRank can be used to determine which destination to apply. If subplans have the same
           * rank, the product may be disseminated to all applicable subplan destinations. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DisseminationConstrainedSubplanType, uci::type::accessorType::disseminationConstrainedSubplanType> ConstrainedSubplan;

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


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultSubplan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultSubplan.
           */
         virtual const uci::type::DisseminationSubplanType& getDefaultSubplan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DefaultSubplan.
           *
           * @return The acecssor that provides access to the complex content that is identified by DefaultSubplan.
           */
         virtual uci::type::DisseminationSubplanType& getDefaultSubplan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DefaultSubplan to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DefaultSubplan
           */
         virtual void setDefaultSubplan(const uci::type::DisseminationSubplanType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConstrainedSubplan.
           *
           * @return The bounded list identified by ConstrainedSubplan.
           */
         virtual const uci::type::ProductDisseminationPlanType::ConstrainedSubplan& getConstrainedSubplan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConstrainedSubplan.
           *
           * @return The bounded list identified by ConstrainedSubplan.
           */
         virtual uci::type::ProductDisseminationPlanType::ConstrainedSubplan& getConstrainedSubplan()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConstrainedSubplan.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConstrainedSubplan(const uci::type::ProductDisseminationPlanType::ConstrainedSubplan& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductDisseminationPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductDisseminationPlanType to copy from
           */
         ProductDisseminationPlanType(const ProductDisseminationPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductDisseminationPlanType to the contents of the
           * ProductDisseminationPlanType on the right hand side (rhs) of the assignment operator.ProductDisseminationPlanType
           * [only available to derived classes]
           *
           * @param rhs The ProductDisseminationPlanType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductDisseminationPlanType
           * @return A constant reference to this ProductDisseminationPlanType.
           */
         const ProductDisseminationPlanType& operator=(const ProductDisseminationPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductDisseminationPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductDisseminationPlanType_h

