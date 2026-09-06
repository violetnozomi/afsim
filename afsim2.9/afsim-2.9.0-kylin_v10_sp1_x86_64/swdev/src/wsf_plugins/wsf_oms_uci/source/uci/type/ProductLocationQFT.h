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
#ifndef Uci__Type__ProductLocationQFT_h
#define Uci__Type__ProductLocationQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductLocationID_Type_h)
# include "uci/type/ProductLocationID_Type.h"
#endif

#if !defined(Uci__Type__ProductMetadataID_Type_h)
# include "uci/type/ProductMetadataID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductLocationQFT sequence accessor class */
      class ProductLocationQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ProductLocationQFT()
         { }

         /** Returns this accessor's type constant, i.e. ProductLocationQFT
           *
           * @return This accessor's type constant, i.e. ProductLocationQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productLocationQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductLocationQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ProductLocationID_Type, uci::type::accessorType::productLocationID_Type> ProductLocationID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ProductMetadataID_Type, uci::type::accessorType::productMetadataID_Type> ProductMetadataID;

         /** Returns the bounded list that is identified by the ProductLocationID.
           *
           * @return The bounded list identified by ProductLocationID.
           */
         virtual const uci::type::ProductLocationQFT::ProductLocationID& getProductLocationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductLocationID.
           *
           * @return The bounded list identified by ProductLocationID.
           */
         virtual uci::type::ProductLocationQFT::ProductLocationID& getProductLocationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductLocationID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductLocationID(const uci::type::ProductLocationQFT::ProductLocationID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual const uci::type::ProductLocationQFT::ProductMetadataID& getProductMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductMetadataID.
           *
           * @return The bounded list identified by ProductMetadataID.
           */
         virtual uci::type::ProductLocationQFT::ProductMetadataID& getProductMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductMetadataID(const uci::type::ProductLocationQFT::ProductMetadataID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductLocationQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductLocationQFT to copy from
           */
         ProductLocationQFT(const ProductLocationQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductLocationQFT to the contents of the ProductLocationQFT on
           * the right hand side (rhs) of the assignment operator.ProductLocationQFT [only available to derived classes]
           *
           * @param rhs The ProductLocationQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ProductLocationQFT
           * @return A constant reference to this ProductLocationQFT.
           */
         const ProductLocationQFT& operator=(const ProductLocationQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductLocationQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductLocationQFT_h

