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
#ifndef Uci__Type__ProductProcessingRequestStatusMDT_h
#define Uci__Type__ProductProcessingRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductProcessingRequestID_Type_h)
# include "uci/type/ProductProcessingRequestID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ProcessingStatusEnum_h)
# include "uci/type/ProcessingStatusEnum.h"
#endif

#if !defined(Uci__Type__ProductProcessingStatusType_h)
# include "uci/type/ProductProcessingStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductProcessingRequestStatusMDT sequence accessor class */
      class ProductProcessingRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductProcessingRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ProductProcessingRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. ProductProcessingRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productProcessingRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductProcessingRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ProductProcessingRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductProcessingRequestID.
           */
         virtual const uci::type::ProductProcessingRequestID_Type& getProductProcessingRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ProductProcessingRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductProcessingRequestID.
           */
         virtual uci::type::ProductProcessingRequestID_Type& getProductProcessingRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductProcessingRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductProcessingRequestID
           */
         virtual void setProductProcessingRequestID(const uci::type::ProductProcessingRequestID_Type& value)
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


         /** Returns the value of the enumeration that is identified by the ProductProcessingRequestState.
           *
           * @return The value of the enumeration identified by ProductProcessingRequestState.
           */
         virtual const uci::type::ProcessingStatusEnum& getProductProcessingRequestState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ProductProcessingRequestState.
           *
           * @return The value of the enumeration identified by ProductProcessingRequestState.
           */
         virtual uci::type::ProcessingStatusEnum& getProductProcessingRequestState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductProcessingRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductProcessingRequestState(const uci::type::ProcessingStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProductProcessingRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProductProcessingRequestState(uci::type::ProcessingStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductProcessingStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductProcessingStatus.
           */
         virtual const uci::type::ProductProcessingStatusType& getProductProcessingStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductProcessingStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductProcessingStatus.
           */
         virtual uci::type::ProductProcessingStatusType& getProductProcessingStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductProcessingStatus to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductProcessingStatus
           */
         virtual void setProductProcessingStatus(const uci::type::ProductProcessingStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductProcessingStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductProcessingStatus is emabled or not
           */
         virtual bool hasProductProcessingStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductProcessingStatus
           *
           * @param type = uci::type::accessorType::productProcessingStatusType This Accessor's accessor type
           */
         virtual void enableProductProcessingStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::productProcessingStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductProcessingStatus */
         virtual void clearProductProcessingStatus()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductProcessingRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductProcessingRequestStatusMDT to copy from
           */
         ProductProcessingRequestStatusMDT(const ProductProcessingRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductProcessingRequestStatusMDT to the contents of the
           * ProductProcessingRequestStatusMDT on the right hand side (rhs) of the assignment
           * operator.ProductProcessingRequestStatusMDT [only available to derived classes]
           *
           * @param rhs The ProductProcessingRequestStatusMDT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ProductProcessingRequestStatusMDT
           * @return A constant reference to this ProductProcessingRequestStatusMDT.
           */
         const ProductProcessingRequestStatusMDT& operator=(const ProductProcessingRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductProcessingRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductProcessingRequestStatusMDT_h

