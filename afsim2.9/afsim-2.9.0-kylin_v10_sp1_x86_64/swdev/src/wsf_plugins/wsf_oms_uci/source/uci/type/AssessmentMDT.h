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
#ifndef Uci__Type__AssessmentMDT_h
#define Uci__Type__AssessmentMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AssessmentID_Type_h)
# include "uci/type/AssessmentID_Type.h"
#endif

#if !defined(Uci__Type__AssessmentRequestID_Type_h)
# include "uci/type/AssessmentRequestID_Type.h"
#endif

#if !defined(Uci__Type__AssessmentType_h)
# include "uci/type/AssessmentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AssessmentMDT sequence accessor class */
      class AssessmentMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AssessmentMDT()
         { }

         /** Returns this accessor's type constant, i.e. AssessmentMDT
           *
           * @return This accessor's type constant, i.e. AssessmentMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::assessmentMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AssessmentMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssessmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssessmentID.
           */
         virtual const uci::type::AssessmentID_Type& getAssessmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssessmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssessmentID.
           */
         virtual uci::type::AssessmentID_Type& getAssessmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AssessmentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AssessmentID
           */
         virtual void setAssessmentID(const uci::type::AssessmentID_Type& value)
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


         /** Returns whether the Element that is identified by AssessmentRequestID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AssessmentRequestID is emabled or not
           */
         virtual bool hasAssessmentRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AssessmentRequestID
           *
           * @param type = uci::type::accessorType::assessmentRequestID_Type This Accessor's accessor type
           */
         virtual void enableAssessmentRequestID(uci::base::accessorType::AccessorType type = uci::type::accessorType::assessmentRequestID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AssessmentRequestID */
         virtual void clearAssessmentRequestID()
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



      protected:

         /** The constructor [only available to derived classes]. */
         AssessmentMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AssessmentMDT to copy from
           */
         AssessmentMDT(const AssessmentMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AssessmentMDT to the contents of the AssessmentMDT on the right
           * hand side (rhs) of the assignment operator.AssessmentMDT [only available to derived classes]
           *
           * @param rhs The AssessmentMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::AssessmentMDT
           * @return A constant reference to this AssessmentMDT.
           */
         const AssessmentMDT& operator=(const AssessmentMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AssessmentMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AssessmentMDT_h

