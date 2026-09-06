// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SystemEstimationRequestStatusMDT_h
#define Uci__Type__SystemEstimationRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemEstimationRequestID_Type_h)
# include "uci/type/SystemEstimationRequestID_Type.h"
#endif

#if !defined(Uci__Type__ProcessingStatusEnum_h)
# include "uci/type/ProcessingStatusEnum.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EstimationResultType_h)
# include "uci/type/EstimationResultType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SystemEstimationRequestStatusMDT sequence accessor class */
      class SystemEstimationRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SystemEstimationRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. SystemEstimationRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. SystemEstimationRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemEstimationRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemEstimationRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The result of the system estimation. If a SINGLE response was requested there will be multiple results for each
           * timestep. This field will be populated if ResultsInNativeMessage=true and the RequestStatus is COMPLETED. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EstimationResultType, uci::type::accessorType::estimationResultType> EstimationResult;

         /** Returns the accessor that provides access to the complex content that is identified by the SystemEstimationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemEstimationRequestID.
           */
         virtual const uci::type::SystemEstimationRequestID_Type& getSystemEstimationRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemEstimationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemEstimationRequestID.
           */
         virtual uci::type::SystemEstimationRequestID_Type& getSystemEstimationRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemEstimationRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemEstimationRequestID
           */
         virtual void setSystemEstimationRequestID(const uci::type::SystemEstimationRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestStatus.
           *
           * @return The value of the enumeration identified by RequestStatus.
           */
         virtual const uci::type::ProcessingStatusEnum& getRequestStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestStatus.
           *
           * @return The value of the enumeration identified by RequestStatus.
           */
         virtual uci::type::ProcessingStatusEnum& getRequestStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestStatus(const uci::type::ProcessingStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestStatus(uci::type::ProcessingStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FailureReason.
           *
           * @return The value of the string data type identified by FailureReason.
           */
         virtual const uci::type::VisibleString256Type& getFailureReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FailureReason.
           *
           * @return The value of the string data type identified by FailureReason.
           */
         virtual uci::type::VisibleString256Type& getFailureReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FailureReason to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFailureReason(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FailureReason to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFailureReason(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FailureReason to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFailureReason(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FailureReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FailureReason is emabled or not
           */
         virtual bool hasFailureReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FailureReason
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableFailureReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FailureReason */
         virtual void clearFailureReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EstimationResult.
           *
           * @return The bounded list identified by EstimationResult.
           */
         virtual const uci::type::SystemEstimationRequestStatusMDT::EstimationResult& getEstimationResult() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EstimationResult.
           *
           * @return The bounded list identified by EstimationResult.
           */
         virtual uci::type::SystemEstimationRequestStatusMDT::EstimationResult& getEstimationResult()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EstimationResult.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEstimationResult(const uci::type::SystemEstimationRequestStatusMDT::EstimationResult& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemEstimationRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemEstimationRequestStatusMDT to copy from
           */
         SystemEstimationRequestStatusMDT(const SystemEstimationRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemEstimationRequestStatusMDT to the contents of the
           * SystemEstimationRequestStatusMDT on the right hand side (rhs) of the assignment
           * operator.SystemEstimationRequestStatusMDT [only available to derived classes]
           *
           * @param rhs The SystemEstimationRequestStatusMDT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SystemEstimationRequestStatusMDT
           * @return A constant reference to this SystemEstimationRequestStatusMDT.
           */
         const SystemEstimationRequestStatusMDT& operator=(const SystemEstimationRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemEstimationRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemEstimationRequestStatusMDT_h

