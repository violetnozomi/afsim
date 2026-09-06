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
#ifndef Uci__Type__EOB_SignalType_h
#define Uci__Type__EOB_SignalType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__FrequencyDataBaseType_h)
# include "uci/type/FrequencyDataBaseType.h"
#endif

#if !defined(Uci__Type__PRI_DataBaseType_h)
# include "uci/type/PRI_DataBaseType.h"
#endif

#if !defined(Uci__Type__SignalPulseWidthBaseType_h)
# include "uci/type/SignalPulseWidthBaseType.h"
#endif

#if !defined(Uci__Type__ScanDataBaseType_h)
# include "uci/type/ScanDataBaseType.h"
#endif

#if !defined(Uci__Type__ModulationOnPulseDataBaseType_h)
# include "uci/type/ModulationOnPulseDataBaseType.h"
#endif

#if !defined(Uci__Type__SignalAmplitudeType_h)
# include "uci/type/SignalAmplitudeType.h"
#endif

#if !defined(Uci__Type__EmitterFunctionType_h)
# include "uci/type/EmitterFunctionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EOB_SignalType sequence accessor class */
      class EOB_SignalType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EOB_SignalType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_SignalType
           *
           * @return This accessor's type constant, i.e. EOB_SignalType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_SignalType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_SignalType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LastHeardTimestamp.
           *
           * @return The value of the simple primitive data type identified by LastHeardTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getLastHeardTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LastHeardTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLastHeardTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastHeardTimestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastHeardTimestamp is emabled or not
           */
         virtual bool hasLastHeardTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastHeardTimestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableLastHeardTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastHeardTimestamp */
         virtual void clearLastHeardTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual const uci::type::FrequencyDataBaseType& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual uci::type::FrequencyDataBaseType& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Frequency to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Frequency
           */
         virtual void setFrequency(const uci::type::FrequencyDataBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Frequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Frequency is emabled or not
           */
         virtual bool hasFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Frequency
           *
           * @param type = uci::type::accessorType::frequencyDataBaseType This Accessor's accessor type
           */
         virtual void enableFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyDataBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Frequency */
         virtual void clearFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PilotFrequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by PilotFrequency.
           */
         virtual const uci::type::FrequencyDataBaseType& getPilotFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PilotFrequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by PilotFrequency.
           */
         virtual uci::type::FrequencyDataBaseType& getPilotFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PilotFrequency to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PilotFrequency
           */
         virtual void setPilotFrequency(const uci::type::FrequencyDataBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PilotFrequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PilotFrequency is emabled or not
           */
         virtual bool hasPilotFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PilotFrequency
           *
           * @param type = uci::type::accessorType::frequencyDataBaseType This Accessor's accessor type
           */
         virtual void enablePilotFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyDataBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PilotFrequency */
         virtual void clearPilotFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PRI.
           *
           * @return The acecssor that provides access to the complex content that is identified by PRI.
           */
         virtual const uci::type::PRI_DataBaseType& getPRI() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PRI.
           *
           * @return The acecssor that provides access to the complex content that is identified by PRI.
           */
         virtual uci::type::PRI_DataBaseType& getPRI()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PRI to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PRI
           */
         virtual void setPRI(const uci::type::PRI_DataBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI is emabled or not
           */
         virtual bool hasPRI() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI
           *
           * @param type = uci::type::accessorType::pRI_DataBaseType This Accessor's accessor type
           */
         virtual void enablePRI(uci::base::accessorType::AccessorType type = uci::type::accessorType::pRI_DataBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI */
         virtual void clearPRI()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PulseWidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by PulseWidth.
           */
         virtual const uci::type::SignalPulseWidthBaseType& getPulseWidth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PulseWidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by PulseWidth.
           */
         virtual uci::type::SignalPulseWidthBaseType& getPulseWidth()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PulseWidth to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PulseWidth
           */
         virtual void setPulseWidth(const uci::type::SignalPulseWidthBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidth is emabled or not
           */
         virtual bool hasPulseWidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidth
           *
           * @param type = uci::type::accessorType::signalPulseWidthBaseType This Accessor's accessor type
           */
         virtual void enablePulseWidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalPulseWidthBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidth */
         virtual void clearPulseWidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Scan.
           *
           * @return The acecssor that provides access to the complex content that is identified by Scan.
           */
         virtual const uci::type::ScanDataBaseType& getScan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Scan.
           *
           * @return The acecssor that provides access to the complex content that is identified by Scan.
           */
         virtual uci::type::ScanDataBaseType& getScan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Scan to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Scan
           */
         virtual void setScan(const uci::type::ScanDataBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Scan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Scan is emabled or not
           */
         virtual bool hasScan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Scan
           *
           * @param type = uci::type::accessorType::scanDataBaseType This Accessor's accessor type
           */
         virtual void enableScan(uci::base::accessorType::AccessorType type = uci::type::accessorType::scanDataBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Scan */
         virtual void clearScan()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModulationOnPulse.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModulationOnPulse.
           */
         virtual const uci::type::ModulationOnPulseDataBaseType& getModulationOnPulse() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModulationOnPulse.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModulationOnPulse.
           */
         virtual uci::type::ModulationOnPulseDataBaseType& getModulationOnPulse()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ModulationOnPulse to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ModulationOnPulse
           */
         virtual void setModulationOnPulse(const uci::type::ModulationOnPulseDataBaseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ModulationOnPulse exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ModulationOnPulse is emabled or not
           */
         virtual bool hasModulationOnPulse() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ModulationOnPulse
           *
           * @param type = uci::type::accessorType::modulationOnPulseDataBaseType This Accessor's accessor type
           */
         virtual void enableModulationOnPulse(uci::base::accessorType::AccessorType type = uci::type::accessorType::modulationOnPulseDataBaseType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ModulationOnPulse */
         virtual void clearModulationOnPulse()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Amplitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by Amplitude.
           */
         virtual const uci::type::SignalAmplitudeType& getAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Amplitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by Amplitude.
           */
         virtual uci::type::SignalAmplitudeType& getAmplitude()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Amplitude to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Amplitude
           */
         virtual void setAmplitude(const uci::type::SignalAmplitudeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Amplitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Amplitude is emabled or not
           */
         virtual bool hasAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Amplitude
           *
           * @param type = uci::type::accessorType::signalAmplitudeType This Accessor's accessor type
           */
         virtual void enableAmplitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalAmplitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Amplitude */
         virtual void clearAmplitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterFunction.
           */
         virtual const uci::type::EmitterFunctionType& getEmitterFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterFunction.
           */
         virtual uci::type::EmitterFunctionType& getEmitterFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterFunction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterFunction
           */
         virtual void setEmitterFunction(const uci::type::EmitterFunctionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterFunction exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterFunction is emabled or not
           */
         virtual bool hasEmitterFunction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterFunction
           *
           * @param type = uci::type::accessorType::emitterFunctionType This Accessor's accessor type
           */
         virtual void enableEmitterFunction(uci::base::accessorType::AccessorType type = uci::type::accessorType::emitterFunctionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterFunction */
         virtual void clearEmitterFunction()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_SignalType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_SignalType to copy from
           */
         EOB_SignalType(const EOB_SignalType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_SignalType to the contents of the EOB_SignalType on the right
           * hand side (rhs) of the assignment operator.EOB_SignalType [only available to derived classes]
           *
           * @param rhs The EOB_SignalType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::EOB_SignalType
           * @return A constant reference to this EOB_SignalType.
           */
         const EOB_SignalType& operator=(const EOB_SignalType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_SignalType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_SignalType_h

