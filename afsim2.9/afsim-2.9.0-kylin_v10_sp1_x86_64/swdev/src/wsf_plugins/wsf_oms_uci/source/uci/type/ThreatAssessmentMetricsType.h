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
#ifndef Uci__Type__ThreatAssessmentMetricsType_h
#define Uci__Type__ThreatAssessmentMetricsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ThreatAssessmentProbabilityType_h)
# include "uci/type/ThreatAssessmentProbabilityType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DetectionEnvelopeType_h)
# include "uci/type/DetectionEnvelopeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThreatAssessmentMetricsType sequence accessor class */
      class ThreatAssessmentMetricsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreatAssessmentMetricsType()
         { }

         /** Returns this accessor's type constant, i.e. ThreatAssessmentMetricsType
           *
           * @return This accessor's type constant, i.e. ThreatAssessmentMetricsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatAssessmentMetricsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatAssessmentMetricsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Defines the detection characteristics of the vehicle as a "bugsplat". [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DetectionEnvelopeType, uci::type::accessorType::detectionEnvelopeType> DetectionEnvelope;

         /** Returns the accessor that provides access to the complex content that is identified by the Acquisition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Acquisition.
           */
         virtual const uci::type::ThreatAssessmentProbabilityType& getAcquisition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Acquisition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Acquisition.
           */
         virtual uci::type::ThreatAssessmentProbabilityType& getAcquisition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Acquisition to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Acquisition
           */
         virtual void setAcquisition(const uci::type::ThreatAssessmentProbabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Acquisition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Acquisition is emabled or not
           */
         virtual bool hasAcquisition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Acquisition
           *
           * @param type = uci::type::accessorType::threatAssessmentProbabilityType This Accessor's accessor type
           */
         virtual void enableAcquisition(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatAssessmentProbabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Acquisition */
         virtual void clearAcquisition()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Track.
           *
           * @return The acecssor that provides access to the complex content that is identified by Track.
           */
         virtual const uci::type::ThreatAssessmentProbabilityType& getTrack() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Track.
           *
           * @return The acecssor that provides access to the complex content that is identified by Track.
           */
         virtual uci::type::ThreatAssessmentProbabilityType& getTrack()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Track to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Track
           */
         virtual void setTrack(const uci::type::ThreatAssessmentProbabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Track exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Track is emabled or not
           */
         virtual bool hasTrack() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Track
           *
           * @param type = uci::type::accessorType::threatAssessmentProbabilityType This Accessor's accessor type
           */
         virtual void enableTrack(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatAssessmentProbabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Track */
         virtual void clearTrack()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Launch.
           *
           * @return The acecssor that provides access to the complex content that is identified by Launch.
           */
         virtual const uci::type::ThreatAssessmentProbabilityType& getLaunch() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Launch.
           *
           * @return The acecssor that provides access to the complex content that is identified by Launch.
           */
         virtual uci::type::ThreatAssessmentProbabilityType& getLaunch()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Launch to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Launch
           */
         virtual void setLaunch(const uci::type::ThreatAssessmentProbabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Launch exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Launch is emabled or not
           */
         virtual bool hasLaunch() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Launch
           *
           * @param type = uci::type::accessorType::threatAssessmentProbabilityType This Accessor's accessor type
           */
         virtual void enableLaunch(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatAssessmentProbabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Launch */
         virtual void clearLaunch()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Intercept.
           *
           * @return The acecssor that provides access to the complex content that is identified by Intercept.
           */
         virtual const uci::type::ThreatAssessmentProbabilityType& getIntercept() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Intercept.
           *
           * @return The acecssor that provides access to the complex content that is identified by Intercept.
           */
         virtual uci::type::ThreatAssessmentProbabilityType& getIntercept()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Intercept to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Intercept
           */
         virtual void setIntercept(const uci::type::ThreatAssessmentProbabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Intercept exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Intercept is emabled or not
           */
         virtual bool hasIntercept() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Intercept
           *
           * @param type = uci::type::accessorType::threatAssessmentProbabilityType This Accessor's accessor type
           */
         virtual void enableIntercept(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatAssessmentProbabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Intercept */
         virtual void clearIntercept()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Kill.
           *
           * @return The acecssor that provides access to the complex content that is identified by Kill.
           */
         virtual const uci::type::ThreatAssessmentProbabilityType& getKill() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Kill.
           *
           * @return The acecssor that provides access to the complex content that is identified by Kill.
           */
         virtual uci::type::ThreatAssessmentProbabilityType& getKill()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Kill to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Kill
           */
         virtual void setKill(const uci::type::ThreatAssessmentProbabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Kill exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Kill is emabled or not
           */
         virtual bool hasKill() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Kill
           *
           * @param type = uci::type::accessorType::threatAssessmentProbabilityType This Accessor's accessor type
           */
         virtual void enableKill(uci::base::accessorType::AccessorType type = uci::type::accessorType::threatAssessmentProbabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Kill */
         virtual void clearKill()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DetectionEnvelope.
           *
           * @return The bounded list identified by DetectionEnvelope.
           */
         virtual const uci::type::ThreatAssessmentMetricsType::DetectionEnvelope& getDetectionEnvelope() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DetectionEnvelope.
           *
           * @return The bounded list identified by DetectionEnvelope.
           */
         virtual uci::type::ThreatAssessmentMetricsType::DetectionEnvelope& getDetectionEnvelope()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DetectionEnvelope.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDetectionEnvelope(const uci::type::ThreatAssessmentMetricsType::DetectionEnvelope& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatAssessmentMetricsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatAssessmentMetricsType to copy from
           */
         ThreatAssessmentMetricsType(const ThreatAssessmentMetricsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatAssessmentMetricsType to the contents of the
           * ThreatAssessmentMetricsType on the right hand side (rhs) of the assignment operator.ThreatAssessmentMetricsType [only
           * available to derived classes]
           *
           * @param rhs The ThreatAssessmentMetricsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ThreatAssessmentMetricsType
           * @return A constant reference to this ThreatAssessmentMetricsType.
           */
         const ThreatAssessmentMetricsType& operator=(const ThreatAssessmentMetricsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatAssessmentMetricsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatAssessmentMetricsType_h

