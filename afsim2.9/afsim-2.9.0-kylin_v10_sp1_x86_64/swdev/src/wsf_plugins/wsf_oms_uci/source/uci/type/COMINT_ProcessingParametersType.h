// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__COMINT_ProcessingParametersType_h
#define Uci__Type__COMINT_ProcessingParametersType_h 1

#if !defined(Uci__Type__ProcessingParametersPET_h)
# include "uci/type/ProcessingParametersPET.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base ProcessingParametersType. */
      class COMINT_ProcessingParametersType : public virtual uci::type::ProcessingParametersPET {
      public:

         /** The destructor */
         virtual ~COMINT_ProcessingParametersType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_ProcessingParametersType
           *
           * @return This accessor's type constant, i.e. COMINT_ProcessingParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_ProcessingParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_ProcessingParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CenterFrequency.
           *
           * @return The value of the simple primitive data type identified by CenterFrequency.
           */
         virtual uci::type::FrequencyTypeValue getCenterFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CenterFrequency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCenterFrequency(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CenterFrequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CenterFrequency is emabled or not
           */
         virtual bool hasCenterFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CenterFrequency
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableCenterFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CenterFrequency */
         virtual void clearCenterFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionBandwidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionBandwidth.
           */
         virtual const uci::type::FrequencyRangeType& getCollectionBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionBandwidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionBandwidth.
           */
         virtual uci::type::FrequencyRangeType& getCollectionBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionBandwidth to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionBandwidth
           */
         virtual void setCollectionBandwidth(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionBandwidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionBandwidth is emabled or not
           */
         virtual bool hasCollectionBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionBandwidth
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableCollectionBandwidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionBandwidth */
         virtual void clearCollectionBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalBandwidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalBandwidth.
           */
         virtual const uci::type::FrequencyRangeType& getSignalBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalBandwidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalBandwidth.
           */
         virtual uci::type::FrequencyRangeType& getSignalBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalBandwidth to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalBandwidth
           */
         virtual void setSignalBandwidth(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalBandwidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalBandwidth is emabled or not
           */
         virtual bool hasSignalBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalBandwidth
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableSignalBandwidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalBandwidth */
         virtual void clearSignalBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaxLOB_TimeOffset.
           *
           * @return The value of the simple primitive data type identified by MaxLOB_TimeOffset.
           */
         virtual uci::type::DurationTypeValue getMaxLOB_TimeOffset() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaxLOB_TimeOffset.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaxLOB_TimeOffset(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaxLOB_TimeOffset exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaxLOB_TimeOffset is emabled or not
           */
         virtual bool hasMaxLOB_TimeOffset() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaxLOB_TimeOffset
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableMaxLOB_TimeOffset(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaxLOB_TimeOffset */
         virtual void clearMaxLOB_TimeOffset()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_ProcessingParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_ProcessingParametersType to copy from
           */
         COMINT_ProcessingParametersType(const COMINT_ProcessingParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_ProcessingParametersType to the contents of the
           * COMINT_ProcessingParametersType on the right hand side (rhs) of the assignment
           * operator.COMINT_ProcessingParametersType [only available to derived classes]
           *
           * @param rhs The COMINT_ProcessingParametersType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::COMINT_ProcessingParametersType
           * @return A constant reference to this COMINT_ProcessingParametersType.
           */
         const COMINT_ProcessingParametersType& operator=(const COMINT_ProcessingParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_ProcessingParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_ProcessingParametersType_h

