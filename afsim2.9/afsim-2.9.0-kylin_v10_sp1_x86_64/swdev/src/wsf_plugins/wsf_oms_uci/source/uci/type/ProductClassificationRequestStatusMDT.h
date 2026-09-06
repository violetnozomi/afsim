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
#ifndef Uci__Type__ProductClassificationRequestStatusMDT_h
#define Uci__Type__ProductClassificationRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductClassificationRequestID_Type_h)
# include "uci/type/ProductClassificationRequestID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ProcessingStatusEnum_h)
# include "uci/type/ProcessingStatusEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductClassificationStatusLabelType_h)
# include "uci/type/ProductClassificationStatusLabelType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductClassificationRequestStatusMDT sequence accessor class */
      class ProductClassificationRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductClassificationRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductClassificationRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. ProductClassificationRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productClassificationRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductClassificationRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element is used to provide more detailed status of requests that involve more than one product. It is also used
           * to return a classification label if the request was to return a classification label generated per the Product
           * Classification Plan. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductClassificationStatusLabelType, uci::type::accessorType::productClassificationStatusLabelType> ProductClassificationStatus;

         /** Returns the accessor that provides access to the complex content that is identified by the
           * ProductClassificationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ProductClassificationRequestID.
           */
         virtual const uci::type::ProductClassificationRequestID_Type& getProductClassificationRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ProductClassificationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ProductClassificationRequestID.
           */
         virtual uci::type::ProductClassificationRequestID_Type& getProductClassificationRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductClassificationRequestID to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductClassificationRequestID
           */
         virtual void setProductClassificationRequestID(const uci::type::ProductClassificationRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


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


         /** Returns the value of the enumeration that is identified by the ProductClassificationRequestState.
           *
           * @return The value of the enumeration identified by ProductClassificationRequestState.
           */
         virtual const uci::type::ProcessingStatusEnum& getProductClassificationRequestState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ProductClassificationRequestState.
           *
           * @return The value of the enumeration identified by ProductClassificationRequestState.
           */
         virtual uci::type::ProcessingStatusEnum& getProductClassificationRequestState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductClassificationRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductClassificationRequestState(const uci::type::ProcessingStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductClassificationRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductClassificationRequestState(uci::type::ProcessingStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductClassificationStatus.
           *
           * @return The bounded list identified by ProductClassificationStatus.
           */
         virtual const uci::type::ProductClassificationRequestStatusMDT::ProductClassificationStatus& getProductClassificationStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductClassificationStatus.
           *
           * @return The bounded list identified by ProductClassificationStatus.
           */
         virtual uci::type::ProductClassificationRequestStatusMDT::ProductClassificationStatus& getProductClassificationStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductClassificationStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductClassificationStatus(const uci::type::ProductClassificationRequestStatusMDT::ProductClassificationStatus& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductClassificationRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductClassificationRequestStatusMDT to copy from
           */
         ProductClassificationRequestStatusMDT(const ProductClassificationRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductClassificationRequestStatusMDT to the contents of the
           * ProductClassificationRequestStatusMDT on the right hand side (rhs) of the assignment
           * operator.ProductClassificationRequestStatusMDT [only available to derived classes]
           *
           * @param rhs The ProductClassificationRequestStatusMDT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ProductClassificationRequestStatusMDT
           * @return A constant reference to this ProductClassificationRequestStatusMDT.
           */
         const ProductClassificationRequestStatusMDT& operator=(const ProductClassificationRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductClassificationRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductClassificationRequestStatusMDT_h

