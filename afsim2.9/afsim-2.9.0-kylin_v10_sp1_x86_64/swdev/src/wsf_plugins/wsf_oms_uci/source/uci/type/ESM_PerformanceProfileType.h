// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ESM_PerformanceProfileType_h
#define Uci__Type__ESM_PerformanceProfileType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FieldOfRegardType_h)
# include "uci/type/FieldOfRegardType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ESM_PerformanceProfileType sequence accessor class */
      class ESM_PerformanceProfileType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ESM_PerformanceProfileType()
         { }

         /** Returns this accessor's type constant, i.e. ESM_PerformanceProfileType
           *
           * @return This accessor's type constant, i.e. ESM_PerformanceProfileType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eSM_PerformanceProfileType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ESM_PerformanceProfileType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Field of Regard of the Capability relative to Body Coordinates. This element can be multiple if multiple instances of
           * the same antenna are installed with different Fields Of Regard. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FieldOfRegardType, uci::type::accessorType::fieldOfRegardType> FieldOfRegard;

         /** Indicates the bandwidth of the Capability. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> Band;

         /** Returns the bounded list that is identified by the FieldOfRegard.
           *
           * @return The bounded list identified by FieldOfRegard.
           */
         virtual const uci::type::ESM_PerformanceProfileType::FieldOfRegard& getFieldOfRegard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FieldOfRegard.
           *
           * @return The bounded list identified by FieldOfRegard.
           */
         virtual uci::type::ESM_PerformanceProfileType::FieldOfRegard& getFieldOfRegard()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FieldOfRegard.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFieldOfRegard(const uci::type::ESM_PerformanceProfileType::FieldOfRegard& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Band.
           *
           * @return The bounded list identified by Band.
           */
         virtual const uci::type::ESM_PerformanceProfileType::Band& getBand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Band.
           *
           * @return The bounded list identified by Band.
           */
         virtual uci::type::ESM_PerformanceProfileType::Band& getBand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Band.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBand(const uci::type::ESM_PerformanceProfileType::Band& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BeamLimit.
           *
           * @return The value of the simple primitive data type identified by BeamLimit.
           */
         virtual xs::UnsignedInt getBeamLimit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BeamLimit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBeamLimit(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BeamLimit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BeamLimit is emabled or not
           */
         virtual bool hasBeamLimit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BeamLimit
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableBeamLimit(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BeamLimit */
         virtual void clearBeamLimit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyChangeTime.
           *
           * @return The value of the simple primitive data type identified by FrequencyChangeTime.
           */
         virtual uci::type::DurationTypeValue getFrequencyChangeTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyChangeTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyChangeTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyChangeTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyChangeTime is emabled or not
           */
         virtual bool hasFrequencyChangeTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyChangeTime
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableFrequencyChangeTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyChangeTime */
         virtual void clearFrequencyChangeTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ESM_PerformanceProfileType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ESM_PerformanceProfileType to copy from
           */
         ESM_PerformanceProfileType(const ESM_PerformanceProfileType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ESM_PerformanceProfileType to the contents of the
           * ESM_PerformanceProfileType on the right hand side (rhs) of the assignment operator.ESM_PerformanceProfileType [only
           * available to derived classes]
           *
           * @param rhs The ESM_PerformanceProfileType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ESM_PerformanceProfileType
           * @return A constant reference to this ESM_PerformanceProfileType.
           */
         const ESM_PerformanceProfileType& operator=(const ESM_PerformanceProfileType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ESM_PerformanceProfileType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ESM_PerformanceProfileType_h

