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
#ifndef Uci__Type__AugmentationProcessingParametersType_h
#define Uci__Type__AugmentationProcessingParametersType_h 1

#if !defined(Uci__Type__ProcessingParametersPET_h)
# include "uci/type/ProcessingParametersPET.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base ProcessingParametersPET for augmentation
        * of products. Specific algorithms will have an extended type for additional parameters. Available extension types
        * include: ImageAugmentationParametersType, VideoAugmentationParametersType, WeatherAugmentationParametersType.
        */
      class AugmentationProcessingParametersType : public virtual uci::type::ProcessingParametersPET {
      public:

         /** The destructor */
         virtual ~AugmentationProcessingParametersType()
         { }

         /** Returns this accessor's type constant, i.e. AugmentationProcessingParametersType
           *
           * @return This accessor's type constant, i.e. AugmentationProcessingParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::augmentationProcessingParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AugmentationProcessingParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableRange.
           */
         virtual const uci::type::DateTimeRangeType& getApplicableRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableRange.
           */
         virtual uci::type::DateTimeRangeType& getApplicableRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApplicableRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApplicableRange
           */
         virtual void setApplicableRange(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ApplicableRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ApplicableRange is emabled or not
           */
         virtual bool hasApplicableRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ApplicableRange
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableApplicableRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ApplicableRange */
         virtual void clearApplicableRange()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AugmentationProcessingParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AugmentationProcessingParametersType to copy from
           */
         AugmentationProcessingParametersType(const AugmentationProcessingParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AugmentationProcessingParametersType to the contents of the
           * AugmentationProcessingParametersType on the right hand side (rhs) of the assignment
           * operator.AugmentationProcessingParametersType [only available to derived classes]
           *
           * @param rhs The AugmentationProcessingParametersType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::AugmentationProcessingParametersType
           * @return A constant reference to this AugmentationProcessingParametersType.
           */
         const AugmentationProcessingParametersType& operator=(const AugmentationProcessingParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AugmentationProcessingParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AugmentationProcessingParametersType_h

