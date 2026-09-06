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
#ifndef Uci__Type__WeatherAugmentationParametersType_h
#define Uci__Type__WeatherAugmentationParametersType_h 1

#if !defined(Uci__Type__AugmentationProcessingParametersType_h)
# include "uci/type/AugmentationProcessingParametersType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type extends AugmentationProcessingParametersType to provide augmentation parameters specific to weather. */
      class WeatherAugmentationParametersType : public virtual uci::type::AugmentationProcessingParametersType {
      public:

         /** The destructor */
         virtual ~WeatherAugmentationParametersType()
         { }

         /** Returns this accessor's type constant, i.e. WeatherAugmentationParametersType
           *
           * @return This accessor's type constant, i.e. WeatherAugmentationParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherAugmentationParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherAugmentationParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of a System for which the weather-specific augmentation parameters are applicable. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ApplicableSystemID;

         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual const uci::type::WeatherAugmentationParametersType::ApplicableSystemID& getApplicableSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual uci::type::WeatherAugmentationParametersType::ApplicableSystemID& getApplicableSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableSystemID(const uci::type::WeatherAugmentationParametersType::ApplicableSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeriodicInterval.
           *
           * @return The value of the simple primitive data type identified by PeriodicInterval.
           */
         virtual uci::type::DurationTypeValue getPeriodicInterval() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeriodicInterval.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeriodicInterval(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PeriodicInterval exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PeriodicInterval is emabled or not
           */
         virtual bool hasPeriodicInterval() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PeriodicInterval
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePeriodicInterval(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PeriodicInterval */
         virtual void clearPeriodicInterval()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherAugmentationParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherAugmentationParametersType to copy from
           */
         WeatherAugmentationParametersType(const WeatherAugmentationParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherAugmentationParametersType to the contents of the
           * WeatherAugmentationParametersType on the right hand side (rhs) of the assignment
           * operator.WeatherAugmentationParametersType [only available to derived classes]
           *
           * @param rhs The WeatherAugmentationParametersType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::WeatherAugmentationParametersType
           * @return A constant reference to this WeatherAugmentationParametersType.
           */
         const WeatherAugmentationParametersType& operator=(const WeatherAugmentationParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherAugmentationParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherAugmentationParametersType_h

