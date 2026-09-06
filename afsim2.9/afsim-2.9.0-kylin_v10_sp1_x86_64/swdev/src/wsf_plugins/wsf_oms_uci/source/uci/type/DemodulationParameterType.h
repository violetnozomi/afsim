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
#ifndef Uci__Type__DemodulationParameterType_h
#define Uci__Type__DemodulationParameterType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__TimeWindowType_h)
# include "uci/type/TimeWindowType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates the parameters used to define product processing which includes demodulation. */
      class DemodulationParameterType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DemodulationParameterType()
         { }

         /** Returns this accessor's type constant, i.e. DemodulationParameterType
           *
           * @return This accessor's type constant, i.e. DemodulationParameterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::demodulationParameterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DemodulationParameterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DemodulationUsed.
           *
           * @return The value of the enumeration identified by DemodulationUsed.
           */
         virtual const uci::type::ModulationEnum& getDemodulationUsed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DemodulationUsed.
           *
           * @return The value of the enumeration identified by DemodulationUsed.
           */
         virtual uci::type::ModulationEnum& getDemodulationUsed()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DemodulationUsed.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDemodulationUsed(const uci::type::ModulationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DemodulationUsed.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDemodulationUsed(uci::type::ModulationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield DemodulationUsedis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasDemodulationUsed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getDemodulationUsed will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::modulationEnum This Accessor's accessor type
           */
         virtual void enableDemodulationUsed(uci::base::accessorType::AccessorType type = uci::type::accessorType::modulationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearDemodulationUsed()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AudioAmplitude.
           *
           * @return The value of the simple primitive data type identified by AudioAmplitude.
           */
         virtual uci::type::DecibelTypeValue getAudioAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AudioAmplitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAudioAmplitude(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AudioAmplitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AudioAmplitude is emabled or not
           */
         virtual bool hasAudioAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AudioAmplitude
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableAudioAmplitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AudioAmplitude */
         virtual void clearAudioAmplitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ChannelBandwidth.
           *
           * @return The value of the simple primitive data type identified by ChannelBandwidth.
           */
         virtual uci::type::FrequencyTypeValue getChannelBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ChannelBandwidth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setChannelBandwidth(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChannelBandwidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChannelBandwidth is emabled or not
           */
         virtual bool hasChannelBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChannelBandwidth
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableChannelBandwidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChannelBandwidth */
         virtual void clearChannelBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeWindow.
           */
         virtual const uci::type::TimeWindowType& getTimeWindow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeWindow.
           */
         virtual uci::type::TimeWindowType& getTimeWindow()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimeWindow to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimeWindow
           */
         virtual void setTimeWindow(const uci::type::TimeWindowType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeWindow exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeWindow is emabled or not
           */
         virtual bool hasTimeWindow() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeWindow
           *
           * @param type = uci::type::accessorType::timeWindowType This Accessor's accessor type
           */
         virtual void enableTimeWindow(uci::base::accessorType::AccessorType type = uci::type::accessorType::timeWindowType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeWindow */
         virtual void clearTimeWindow()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DemodulationParameterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DemodulationParameterType to copy from
           */
         DemodulationParameterType(const DemodulationParameterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DemodulationParameterType to the contents of the
           * DemodulationParameterType on the right hand side (rhs) of the assignment operator.DemodulationParameterType [only
           * available to derived classes]
           *
           * @param rhs The DemodulationParameterType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::DemodulationParameterType
           * @return A constant reference to this DemodulationParameterType.
           */
         const DemodulationParameterType& operator=(const DemodulationParameterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DemodulationParameterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DemodulationParameterType_h

