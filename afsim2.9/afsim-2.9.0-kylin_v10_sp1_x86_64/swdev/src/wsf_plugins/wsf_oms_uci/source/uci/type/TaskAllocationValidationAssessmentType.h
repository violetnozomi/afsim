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
#ifndef Uci__Type__TaskAllocationValidationAssessmentType_h
#define Uci__Type__TaskAllocationValidationAssessmentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TaskAllocationValidationRequestType_h)
# include "uci/type/TaskAllocationValidationRequestType.h"
#endif

#if !defined(Uci__Type__TaskAllocationValidationAssessmentDataType_h)
# include "uci/type/TaskAllocationValidationAssessmentDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Base data structure containing both the inputs and outputs for a Task Allocation Validation Assessment. */
      class TaskAllocationValidationAssessmentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~TaskAllocationValidationAssessmentType()
         { }

         /** Returns this accessor's type constant, i.e. TaskAllocationValidationAssessmentType
           *
           * @return This accessor's type constant, i.e. TaskAllocationValidationAssessmentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskAllocationValidationAssessmentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskAllocationValidationAssessmentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Parameters.
           *
           * @return The acecssor that provides access to the complex content that is identified by Parameters.
           */
         virtual const uci::type::TaskAllocationValidationRequestType& getParameters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Parameters.
           *
           * @return The acecssor that provides access to the complex content that is identified by Parameters.
           */
         virtual uci::type::TaskAllocationValidationRequestType& getParameters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Parameters to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Parameters
           */
         virtual void setParameters(const uci::type::TaskAllocationValidationRequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Parameters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Parameters is emabled or not
           */
         virtual bool hasParameters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Parameters
           *
           * @param type = uci::type::accessorType::taskAllocationValidationRequestType This Accessor's accessor type
           */
         virtual void enableParameters(uci::base::accessorType::AccessorType type = uci::type::accessorType::taskAllocationValidationRequestType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Parameters */
         virtual void clearParameters()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Data.
           *
           * @return The acecssor that provides access to the complex content that is identified by Data.
           */
         virtual const uci::type::TaskAllocationValidationAssessmentDataType& getData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Data.
           *
           * @return The acecssor that provides access to the complex content that is identified by Data.
           */
         virtual uci::type::TaskAllocationValidationAssessmentDataType& getData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Data to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Data
           */
         virtual void setData(const uci::type::TaskAllocationValidationAssessmentDataType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskAllocationValidationAssessmentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskAllocationValidationAssessmentType to copy from
           */
         TaskAllocationValidationAssessmentType(const TaskAllocationValidationAssessmentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskAllocationValidationAssessmentType to the contents of the
           * TaskAllocationValidationAssessmentType on the right hand side (rhs) of the assignment
           * operator.TaskAllocationValidationAssessmentType [only available to derived classes]
           *
           * @param rhs The TaskAllocationValidationAssessmentType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::TaskAllocationValidationAssessmentType
           * @return A constant reference to this TaskAllocationValidationAssessmentType.
           */
         const TaskAllocationValidationAssessmentType& operator=(const TaskAllocationValidationAssessmentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskAllocationValidationAssessmentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskAllocationValidationAssessmentType_h

