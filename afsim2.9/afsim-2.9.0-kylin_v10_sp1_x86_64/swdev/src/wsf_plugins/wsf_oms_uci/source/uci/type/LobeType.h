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
#ifndef Uci__Type__LobeType_h
#define Uci__Type__LobeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LobeType sequence accessor class */
      class LobeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LobeType()
         { }

         /** Returns this accessor's type constant, i.e. LobeType
           *
           * @return This accessor's type constant, i.e. LobeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lobeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LobeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeakAmplitude.
           *
           * @return The value of the simple primitive data type identified by PeakAmplitude.
           */
         virtual uci::type::DecibelTypeValue getPeakAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeakAmplitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeakAmplitude(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PeakAmplitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PeakAmplitude is emabled or not
           */
         virtual bool hasPeakAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PeakAmplitude
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enablePeakAmplitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PeakAmplitude */
         virtual void clearPeakAmplitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PeakTime.
           *
           * @return The value of the simple primitive data type identified by PeakTime.
           */
         virtual uci::type::DateTimeTypeValue getPeakTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PeakTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPeakTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PeakTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PeakTime is emabled or not
           */
         virtual bool hasPeakTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PeakTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enablePeakTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PeakTime */
         virtual void clearPeakTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TimeInMainBeam.
           *
           * @return The value of the simple primitive data type identified by TimeInMainBeam.
           */
         virtual uci::type::DurationTypeValue getTimeInMainBeam() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeInMainBeam.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeInMainBeam(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeInMainBeam exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeInMainBeam is emabled or not
           */
         virtual bool hasTimeInMainBeam() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeInMainBeam
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableTimeInMainBeam(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeInMainBeam */
         virtual void clearTimeInMainBeam()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GreatDepth.
           *
           * @return The value of the simple primitive data type identified by GreatDepth.
           */
         virtual xs::Boolean getGreatDepth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GreatDepth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGreatDepth(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GreatDepth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GreatDepth is emabled or not
           */
         virtual bool hasGreatDepth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GreatDepth
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableGreatDepth(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GreatDepth */
         virtual void clearGreatDepth()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LobeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LobeType to copy from
           */
         LobeType(const LobeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LobeType to the contents of the LobeType on the right hand side
           * (rhs) of the assignment operator.LobeType [only available to derived classes]
           *
           * @param rhs The LobeType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::LobeType
           * @return A constant reference to this LobeType.
           */
         const LobeType& operator=(const LobeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LobeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LobeType_h

