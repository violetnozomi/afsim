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
#ifndef Uci__Type__ProductDisseminationDestinationQFT_h
#define Uci__Type__ProductDisseminationDestinationQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__ProductTypeEnum_h)
# include "uci/type/ProductTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductDisseminationDestinationQFT sequence accessor class */
      class ProductDisseminationDestinationQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ProductDisseminationDestinationQFT()
         { }

         /** Returns this accessor's type constant, i.e. ProductDisseminationDestinationQFT
           *
           * @return This accessor's type constant, i.e. ProductDisseminationDestinationQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productDisseminationDestinationQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductDisseminationDestinationQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the System ID [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Filter on the handle. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::VisibleString256Type, uci::type::accessorType::visibleString256Type> Handle;

         /** Filter on the type of product [Minimum occurrences: 0] [Maximum occurrences: 78] */
         typedef uci::base::BoundedList<uci::type::ProductTypeEnum, uci::type::accessorType::productTypeEnum> ProductType;

         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::ProductDisseminationDestinationQFT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::ProductDisseminationDestinationQFT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::ProductDisseminationDestinationQFT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Handle.
           *
           * @return The bounded list identified by Handle.
           */
         virtual const uci::type::ProductDisseminationDestinationQFT::Handle& getHandle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Handle.
           *
           * @return The bounded list identified by Handle.
           */
         virtual uci::type::ProductDisseminationDestinationQFT::Handle& getHandle()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Handle.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setHandle(const uci::type::ProductDisseminationDestinationQFT::Handle& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductType.
           *
           * @return The bounded list identified by ProductType.
           */
         virtual const uci::type::ProductDisseminationDestinationQFT::ProductType& getProductType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductType.
           *
           * @return The bounded list identified by ProductType.
           */
         virtual uci::type::ProductDisseminationDestinationQFT::ProductType& getProductType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductType(const uci::type::ProductDisseminationDestinationQFT::ProductType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductDisseminationDestinationQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductDisseminationDestinationQFT to copy from
           */
         ProductDisseminationDestinationQFT(const ProductDisseminationDestinationQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductDisseminationDestinationQFT to the contents of the
           * ProductDisseminationDestinationQFT on the right hand side (rhs) of the assignment
           * operator.ProductDisseminationDestinationQFT [only available to derived classes]
           *
           * @param rhs The ProductDisseminationDestinationQFT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ProductDisseminationDestinationQFT
           * @return A constant reference to this ProductDisseminationDestinationQFT.
           */
         const ProductDisseminationDestinationQFT& operator=(const ProductDisseminationDestinationQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductDisseminationDestinationQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductDisseminationDestinationQFT_h

