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
#ifndef Uci__Type__ProductDisseminationTaskStatusMDT_h
#define Uci__Type__ProductDisseminationTaskStatusMDT_h 1

#if !defined(Uci__Type__ProductManagementTaskStatusDataType_h)
# include "uci/type/ProductManagementTaskStatusDataType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductDisseminationStatusType_h)
# include "uci/type/ProductDisseminationStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductDisseminationTaskStatusMDT sequence accessor class */
      class ProductDisseminationTaskStatusMDT : public virtual uci::type::ProductManagementTaskStatusDataType {
      public:

         /** The destructor */
         virtual ~ProductDisseminationTaskStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductDisseminationTaskStatusMDT
           *
           * @return This accessor's type constant, i.e. ProductDisseminationTaskStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productDisseminationTaskStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductDisseminationTaskStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Because a single dissemination request can result in many products being disseminated, the status of individual
           * products is given. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductDisseminationStatusType, uci::type::accessorType::productDisseminationStatusType> ProductDisseminationStatus;

         /** Returns the bounded list that is identified by the ProductDisseminationStatus.
           *
           * @return The bounded list identified by ProductDisseminationStatus.
           */
         virtual const uci::type::ProductDisseminationTaskStatusMDT::ProductDisseminationStatus& getProductDisseminationStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductDisseminationStatus.
           *
           * @return The bounded list identified by ProductDisseminationStatus.
           */
         virtual uci::type::ProductDisseminationTaskStatusMDT::ProductDisseminationStatus& getProductDisseminationStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductDisseminationStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductDisseminationStatus(const uci::type::ProductDisseminationTaskStatusMDT::ProductDisseminationStatus& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductDisseminationTaskStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductDisseminationTaskStatusMDT to copy from
           */
         ProductDisseminationTaskStatusMDT(const ProductDisseminationTaskStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductDisseminationTaskStatusMDT to the contents of the
           * ProductDisseminationTaskStatusMDT on the right hand side (rhs) of the assignment
           * operator.ProductDisseminationTaskStatusMDT [only available to derived classes]
           *
           * @param rhs The ProductDisseminationTaskStatusMDT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ProductDisseminationTaskStatusMDT
           * @return A constant reference to this ProductDisseminationTaskStatusMDT.
           */
         const ProductDisseminationTaskStatusMDT& operator=(const ProductDisseminationTaskStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductDisseminationTaskStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductDisseminationTaskStatusMDT_h

