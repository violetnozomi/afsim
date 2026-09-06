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
#ifndef Uci__Type__AssessmentRequestMDT_h
#define Uci__Type__AssessmentRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AssessmentRequestID_Type_h)
# include "uci/type/AssessmentRequestID_Type.h"
#endif

#if !defined(Uci__Type__RepeatEnum_h)
# include "uci/type/RepeatEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__AssessmentRequestType_h)
# include "uci/type/AssessmentRequestType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AssessmentRequestMDT sequence accessor class */
      class AssessmentRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AssessmentRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. AssessmentRequestMDT
           *
           * @return This accessor's type constant, i.e. AssessmentRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::assessmentRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AssessmentRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


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


         /** Returns the value of the enumeration that is identified by the RequestFrequencyType.
           *
           * @return The value of the enumeration identified by RequestFrequencyType.
           */
         virtual const uci::type::RepeatEnum& getRequestFrequencyType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestFrequencyType.
           *
           * @return The value of the enumeration identified by RequestFrequencyType.
           */
         virtual uci::type::RepeatEnum& getRequestFrequencyType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestFrequencyType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestFrequencyType(const uci::type::RepeatEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestFrequencyType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestFrequencyType(uci::type::RepeatEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResultsInAssessmentMessage.
           *
           * @return The value of the simple primitive data type identified by ResultsInAssessmentMessage.
           */
         virtual xs::Boolean getResultsInAssessmentMessage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResultsInAssessmentMessage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResultsInAssessmentMessage(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Category.
           *
           * @return The acecssor that provides access to the complex content that is identified by Category.
           */
         virtual const uci::type::AssessmentRequestType& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Category.
           *
           * @return The acecssor that provides access to the complex content that is identified by Category.
           */
         virtual uci::type::AssessmentRequestType& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Category to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Category
           */
         virtual void setCategory(const uci::type::AssessmentRequestType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AssessmentRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AssessmentRequestMDT to copy from
           */
         AssessmentRequestMDT(const AssessmentRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AssessmentRequestMDT to the contents of the AssessmentRequestMDT
           * on the right hand side (rhs) of the assignment operator.AssessmentRequestMDT [only available to derived classes]
           *
           * @param rhs The AssessmentRequestMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AssessmentRequestMDT
           * @return A constant reference to this AssessmentRequestMDT.
           */
         const AssessmentRequestMDT& operator=(const AssessmentRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AssessmentRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AssessmentRequestMDT_h

