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
#ifndef Uci__Type__ProductProcessingPlanMDT_h
#define Uci__Type__ProductProcessingPlanMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductProcessingPlanType_h)
# include "uci/type/ProductProcessingPlanType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductProcessingPlanMDT sequence accessor class */
      class ProductProcessingPlanMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductProcessingPlanMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductProcessingPlanMDT
           *
           * @return This accessor's type constant, i.e. ProductProcessingPlanMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productProcessingPlanMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductProcessingPlanMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This field represents a list of plans for each product type that the system will process. If multiple instances are
           * given, each should be of a different product type as indicated by the child element. [Maximum occurrences: 78]
           */
         typedef uci::base::BoundedList<uci::type::ProductProcessingPlanType, uci::type::accessorType::productProcessingPlanType> ProductProcessingPlan;

         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductProcessingPlan.
           *
           * @return The bounded list identified by ProductProcessingPlan.
           */
         virtual const uci::type::ProductProcessingPlanMDT::ProductProcessingPlan& getProductProcessingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductProcessingPlan.
           *
           * @return The bounded list identified by ProductProcessingPlan.
           */
         virtual uci::type::ProductProcessingPlanMDT::ProductProcessingPlan& getProductProcessingPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductProcessingPlan.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductProcessingPlan(const uci::type::ProductProcessingPlanMDT::ProductProcessingPlan& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductProcessingPlanMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductProcessingPlanMDT to copy from
           */
         ProductProcessingPlanMDT(const ProductProcessingPlanMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductProcessingPlanMDT to the contents of the
           * ProductProcessingPlanMDT on the right hand side (rhs) of the assignment operator.ProductProcessingPlanMDT [only
           * available to derived classes]
           *
           * @param rhs The ProductProcessingPlanMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ProductProcessingPlanMDT
           * @return A constant reference to this ProductProcessingPlanMDT.
           */
         const ProductProcessingPlanMDT& operator=(const ProductProcessingPlanMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductProcessingPlanMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductProcessingPlanMDT_h

