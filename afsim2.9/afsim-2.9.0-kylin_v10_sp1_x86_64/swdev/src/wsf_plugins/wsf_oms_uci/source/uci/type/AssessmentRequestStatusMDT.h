// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AssessmentRequestStatusMDT_h
#define Uci__Type__AssessmentRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AssessmentRequestID_Type_h)
# include "uci/type/AssessmentRequestID_Type.h"
#endif

#if !defined(Uci__Type__ProcessingStatusEnum_h)
# include "uci/type/ProcessingStatusEnum.h"
#endif

#if !defined(Uci__Type__CompletionStatusType_h)
# include "uci/type/CompletionStatusType.h"
#endif

#if !defined(Uci__Type__AssessmentType_h)
# include "uci/type/AssessmentType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__IncompleteProcessingType_h)
# include "uci/type/IncompleteProcessingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AssessmentRequestStatusMDT sequence accessor class */
      class AssessmentRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AssessmentRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. AssessmentRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. AssessmentRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::assessmentRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AssessmentRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element indicates any assessments that failed to complete. It is used to indicate details about why a specific
           * assessment target failed to process. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::IncompleteProcessingType, uci::type::accessorType::incompleteProcessingType> IncompleteAssessment;

         /** Returns the accessor that provides access to the complex content that is identified by the AssessmentRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssessmentRequestID.
           */
         virtual const uci::type::AssessmentRequestID_Type& getAssessmentRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssessmentRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssessmentRequestID.
           */
         virtual uci::type::AssessmentRequestID_Type& getAssessmentRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AssessmentRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AssessmentRequestID
           */
         virtual void setAssessmentRequestID(const uci::type::AssessmentRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ProcessingState.
           *
           * @return The value of the enumeration identified by ProcessingState.
           */
         virtual const uci::type::ProcessingStatusEnum& getProcessingState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ProcessingState.
           *
           * @return The value of the enumeration identified by ProcessingState.
           */
         virtual uci::type::ProcessingStatusEnum& getProcessingState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProcessingState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProcessingState(const uci::type::ProcessingStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ProcessingState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setProcessingState(uci::type::ProcessingStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CompletionStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CompletionStatus.
           */
         virtual const uci::type::CompletionStatusType& getCompletionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CompletionStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CompletionStatus.
           */
         virtual uci::type::CompletionStatusType& getCompletionStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CompletionStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CompletionStatus
           */
         virtual void setCompletionStatus(const uci::type::CompletionStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CompletionStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CompletionStatus is emabled or not
           */
         virtual bool hasCompletionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CompletionStatus
           *
           * @param type = uci::type::accessorType::completionStatusType This Accessor's accessor type
           */
         virtual void enableCompletionStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::completionStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CompletionStatus */
         virtual void clearCompletionStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Assessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by Assessment.
           */
         virtual const uci::type::AssessmentType& getAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Assessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by Assessment.
           */
         virtual uci::type::AssessmentType& getAssessment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Assessment to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Assessment
           */
         virtual void setAssessment(const uci::type::AssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Assessment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Assessment is emabled or not
           */
         virtual bool hasAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Assessment
           *
           * @param type = uci::type::accessorType::assessmentType This Accessor's accessor type
           */
         virtual void enableAssessment(uci::base::accessorType::AccessorType type = uci::type::accessorType::assessmentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Assessment */
         virtual void clearAssessment()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the IncompleteAssessment.
           *
           * @return The bounded list identified by IncompleteAssessment.
           */
         virtual const uci::type::AssessmentRequestStatusMDT::IncompleteAssessment& getIncompleteAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the IncompleteAssessment.
           *
           * @return The bounded list identified by IncompleteAssessment.
           */
         virtual uci::type::AssessmentRequestStatusMDT::IncompleteAssessment& getIncompleteAssessment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the IncompleteAssessment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setIncompleteAssessment(const uci::type::AssessmentRequestStatusMDT::IncompleteAssessment& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AssessmentRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AssessmentRequestStatusMDT to copy from
           */
         AssessmentRequestStatusMDT(const AssessmentRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AssessmentRequestStatusMDT to the contents of the
           * AssessmentRequestStatusMDT on the right hand side (rhs) of the assignment operator.AssessmentRequestStatusMDT [only
           * available to derived classes]
           *
           * @param rhs The AssessmentRequestStatusMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::AssessmentRequestStatusMDT
           * @return A constant reference to this AssessmentRequestStatusMDT.
           */
         const AssessmentRequestStatusMDT& operator=(const AssessmentRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AssessmentRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AssessmentRequestStatusMDT_h

