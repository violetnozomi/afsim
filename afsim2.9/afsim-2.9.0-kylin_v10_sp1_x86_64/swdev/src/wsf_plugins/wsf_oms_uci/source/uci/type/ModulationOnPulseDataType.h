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
#ifndef Uci__Type__ModulationOnPulseDataType_h
#define Uci__Type__ModulationOnPulseDataType_h 1

#if !defined(Uci__Type__ModulationOnPulseDataBaseType_h)
# include "uci/type/ModulationOnPulseDataBaseType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__PMOP_SequenceType_h)
# include "uci/type/PMOP_SequenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ModulationOnPulseDataType sequence accessor class */
      class ModulationOnPulseDataType : public virtual uci::type::ModulationOnPulseDataBaseType {
      public:

         /** The destructor */
         virtual ~ModulationOnPulseDataType()
         { }

         /** Returns this accessor's type constant, i.e. ModulationOnPulseDataType
           *
           * @return This accessor's type constant, i.e. ModulationOnPulseDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::modulationOnPulseDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ModulationOnPulseDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FMOP_Span.
           *
           * @return The value of the simple primitive data type identified by FMOP_Span.
           */
         virtual uci::type::DurationTypeValue getFMOP_Span() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FMOP_Span.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFMOP_Span(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FMOP_Span exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FMOP_Span is emabled or not
           */
         virtual bool hasFMOP_Span() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FMOP_Span
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableFMOP_Span(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FMOP_Span */
         virtual void clearFMOP_Span()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FMOP_Range.
           *
           * @return The acecssor that provides access to the complex content that is identified by FMOP_Range.
           */
         virtual const uci::type::FrequencyRangeType& getFMOP_Range() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FMOP_Range.
           *
           * @return The acecssor that provides access to the complex content that is identified by FMOP_Range.
           */
         virtual uci::type::FrequencyRangeType& getFMOP_Range()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FMOP_Range to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FMOP_Range
           */
         virtual void setFMOP_Range(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FMOP_Range exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FMOP_Range is emabled or not
           */
         virtual bool hasFMOP_Range() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FMOP_Range
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableFMOP_Range(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FMOP_Range */
         virtual void clearFMOP_Range()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PMOP_BitDuration.
           *
           * @return The value of the simple primitive data type identified by PMOP_BitDuration.
           */
         virtual uci::type::DurationTypeValue getPMOP_BitDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PMOP_BitDuration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPMOP_BitDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PMOP_BitDuration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PMOP_BitDuration is emabled or not
           */
         virtual bool hasPMOP_BitDuration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PMOP_BitDuration
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePMOP_BitDuration(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PMOP_BitDuration */
         virtual void clearPMOP_BitDuration()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PMOP_Sequence.
           *
           * @return The acecssor that provides access to the complex content that is identified by PMOP_Sequence.
           */
         virtual const uci::type::PMOP_SequenceType& getPMOP_Sequence() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PMOP_Sequence.
           *
           * @return The acecssor that provides access to the complex content that is identified by PMOP_Sequence.
           */
         virtual uci::type::PMOP_SequenceType& getPMOP_Sequence()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PMOP_Sequence to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PMOP_Sequence
           */
         virtual void setPMOP_Sequence(const uci::type::PMOP_SequenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PMOP_Sequence exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PMOP_Sequence is emabled or not
           */
         virtual bool hasPMOP_Sequence() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PMOP_Sequence
           *
           * @param type = uci::type::accessorType::pMOP_SequenceType This Accessor's accessor type
           */
         virtual void enablePMOP_Sequence(uci::base::accessorType::AccessorType type = uci::type::accessorType::pMOP_SequenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PMOP_Sequence */
         virtual void clearPMOP_Sequence()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ModulationOnPulseDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ModulationOnPulseDataType to copy from
           */
         ModulationOnPulseDataType(const ModulationOnPulseDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ModulationOnPulseDataType to the contents of the
           * ModulationOnPulseDataType on the right hand side (rhs) of the assignment operator.ModulationOnPulseDataType [only
           * available to derived classes]
           *
           * @param rhs The ModulationOnPulseDataType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ModulationOnPulseDataType
           * @return A constant reference to this ModulationOnPulseDataType.
           */
         const ModulationOnPulseDataType& operator=(const ModulationOnPulseDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ModulationOnPulseDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ModulationOnPulseDataType_h

