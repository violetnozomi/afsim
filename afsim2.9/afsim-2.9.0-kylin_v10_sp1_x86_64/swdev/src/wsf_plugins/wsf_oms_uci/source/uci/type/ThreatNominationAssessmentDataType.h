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
#ifndef Uci__Type__ThreatNominationAssessmentDataType_h
#define Uci__Type__ThreatNominationAssessmentDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ThreatNominationResultType_h)
# include "uci/type/ThreatNominationResultType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The results of an assessment that determines which entity poses the highest threat to a system or systems. */
      class ThreatNominationAssessmentDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreatNominationAssessmentDataType()
         { }

         /** Returns this accessor's type constant, i.e. ThreatNominationAssessmentDataType
           *
           * @return This accessor's type constant, i.e. ThreatNominationAssessmentDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatNominationAssessmentDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatNominationAssessmentDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * NominationWithoutSuppression.
           *
           * @return The acecssor that provides access to the complex content that is identified by NominationWithoutSuppression.
           */
         virtual const uci::type::ThreatNominationResultType& getNominationWithoutSuppression() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * NominationWithoutSuppression.
           *
           * @return The acecssor that provides access to the complex content that is identified by NominationWithoutSuppression.
           */
         virtual uci::type::ThreatNominationResultType& getNominationWithoutSuppression()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NominationWithoutSuppression to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NominationWithoutSuppression
           */
         virtual void setNominationWithoutSuppression(const uci::type::ThreatNominationResultType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NominationWithoutSuppression exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NominationWithoutSuppression is emabled or not
           */
         virtual bool hasNominationWithoutSuppression() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NominationWithoutSuppression
           *
           * @param type = uci::type::accessorType::threatNominationResultType This Accessor's accessor type
           */
         virtual void enableNominationWithoutSuppression(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatNominationResultType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NominationWithoutSuppression */
         virtual void clearNominationWithoutSuppression()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NominationWithSuppression.
           *
           * @return The acecssor that provides access to the complex content that is identified by NominationWithSuppression.
           */
         virtual const uci::type::ThreatNominationResultType& getNominationWithSuppression() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NominationWithSuppression.
           *
           * @return The acecssor that provides access to the complex content that is identified by NominationWithSuppression.
           */
         virtual uci::type::ThreatNominationResultType& getNominationWithSuppression()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NominationWithSuppression to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NominationWithSuppression
           */
         virtual void setNominationWithSuppression(const uci::type::ThreatNominationResultType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NominationWithSuppression exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NominationWithSuppression is emabled or not
           */
         virtual bool hasNominationWithSuppression() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NominationWithSuppression
           *
           * @param type = uci::type::accessorType::threatNominationResultType This Accessor's accessor type
           */
         virtual void enableNominationWithSuppression(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatNominationResultType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NominationWithSuppression */
         virtual void clearNominationWithSuppression()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatNominationAssessmentDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatNominationAssessmentDataType to copy from
           */
         ThreatNominationAssessmentDataType(const ThreatNominationAssessmentDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatNominationAssessmentDataType to the contents of the
           * ThreatNominationAssessmentDataType on the right hand side (rhs) of the assignment
           * operator.ThreatNominationAssessmentDataType [only available to derived classes]
           *
           * @param rhs The ThreatNominationAssessmentDataType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ThreatNominationAssessmentDataType
           * @return A constant reference to this ThreatNominationAssessmentDataType.
           */
         const ThreatNominationAssessmentDataType& operator=(const ThreatNominationAssessmentDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatNominationAssessmentDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatNominationAssessmentDataType_h

