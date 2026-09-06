// This file was generated  on 10/18/2017 at 5:35:43 AM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* DISTRIBUTION STATEMENT D.  Distribution authorized to the Department 
*    of Defense and U.S. DoD contractors only; Critical Technology; 26 Mar 13.
*    Other requests shall be referred to the Air Force Rapid Capabilities 
*    Office, 202-404-5497 or 202-404-5498.
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
#ifndef Uci__Type__ThreatAssessmentDetectionType_h
#define Uci__Type__ThreatAssessmentDetectionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThreatAssessmentDetectionType sequence accessor class */
      class ThreatAssessmentDetectionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreatAssessmentDetectionType()
         { }

         /** Returns this accessor's type constant, i.e. ThreatAssessmentDetectionType
           *
           * @return This accessor's type constant, i.e. ThreatAssessmentDetectionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatAssessmentDetectionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatAssessmentDetectionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EffectivePD.
           *
           * @return The value of the simple primitive data type identified by EffectivePD.
           */
         virtual uci::type::PercentTypeValue getEffectivePD() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EffectivePD.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEffectivePD(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EffectivePD exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EffectivePD is emabled or not
           */
         virtual bool hasEffectivePD() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EffectivePD
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableEffectivePD(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EffectivePD */
         virtual void clearEffectivePD()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EffectivePT.
           *
           * @return The value of the simple primitive data type identified by EffectivePT.
           */
         virtual uci::type::PercentTypeValue getEffectivePT() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EffectivePT.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEffectivePT(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EffectivePT exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EffectivePT is emabled or not
           */
         virtual bool hasEffectivePT() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EffectivePT
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableEffectivePT(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EffectivePT */
         virtual void clearEffectivePT()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ExposureTime.
           *
           * @return The value of the simple primitive data type identified by ExposureTime.
           */
         virtual uci::type::DurationTypeValue getExposureTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ExposureTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExposureTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ExposureTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ExposureTime is emabled or not
           */
         virtual bool hasExposureTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ExposureTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableExposureTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ExposureTime */
         virtual void clearExposureTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatAssessmentDetectionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatAssessmentDetectionType to copy from
           */
         ThreatAssessmentDetectionType(const ThreatAssessmentDetectionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatAssessmentDetectionType to the contents of the
           * ThreatAssessmentDetectionType on the right hand side (rhs) of the assignment operator.ThreatAssessmentDetectionType
           * [only available to derived classes]
           *
           * @param rhs The ThreatAssessmentDetectionType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ThreatAssessmentDetectionType
           * @return A constant reference to this ThreatAssessmentDetectionType.
           */
         const ThreatAssessmentDetectionType& operator=(const ThreatAssessmentDetectionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatAssessmentDetectionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatAssessmentDetectionType_h

