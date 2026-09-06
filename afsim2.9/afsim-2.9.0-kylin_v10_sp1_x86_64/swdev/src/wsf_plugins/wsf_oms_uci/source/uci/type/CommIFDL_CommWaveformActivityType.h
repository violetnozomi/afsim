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
#ifndef Uci__Type__CommIFDL_CommWaveformActivityType_h
#define Uci__Type__CommIFDL_CommWaveformActivityType_h 1

#if !defined(Uci__Type__CommWaveformActivityPET_h)
# include "uci/type/CommWaveformActivityPET.h"
#endif

#if !defined(Uci__Type__CommIFDL_ControlType_h)
# include "uci/type/CommIFDL_ControlType.h"
#endif

#if !defined(Uci__Type__CommIFDL_FlightSizeEnum_h)
# include "uci/type/CommIFDL_FlightSizeEnum.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommIFDL_NetLinkStatusType_h)
# include "uci/type/CommIFDL_NetLinkStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base CommWaveformActivityPET for communication
        * IFDL parameters.
        */
      class CommIFDL_CommWaveformActivityType : public virtual uci::type::CommWaveformActivityPET {
      public:

         /** The destructor */
         virtual ~CommIFDL_CommWaveformActivityType()
         { }

         /** Returns this accessor's type constant, i.e. CommIFDL_CommWaveformActivityType
           *
           * @return This accessor's type constant, i.e. CommIFDL_CommWaveformActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commIFDL_CommWaveformActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommIFDL_CommWaveformActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Identifies the frequency channel number used for A links.; [Minimum inclusive: 1] [Maximum inclusive: 127] */
         typedef uci::base::UnsignedIntAccessor ChannelA_Number;
         //  The type of primitive data accessed by ChannelA_Number
         typedef xs::UnsignedInt ChannelA_NumberValue;

         /** Identifies the frequency channel number used for B links.; [Minimum inclusive: 1] [Maximum inclusive: 127] */
         typedef uci::base::UnsignedIntAccessor ChannelB_Number;
         //  The type of primitive data accessed by ChannelB_Number
         typedef xs::UnsignedInt ChannelB_NumberValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 999] */
         typedef uci::base::UnsignedIntAccessor WideNetID1;
         //  The type of primitive data accessed by WideNetID1
         typedef xs::UnsignedInt WideNetID1Value;

         /** [Minimum inclusive: 0] [Maximum inclusive: 999] */
         typedef uci::base::UnsignedIntAccessor WideNetID2;
         //  The type of primitive data accessed by WideNetID2
         typedef xs::UnsignedInt WideNetID2Value;

         /** [Minimum inclusive: 0] [Maximum inclusive: 999] */
         typedef uci::base::UnsignedIntAccessor WideNetID3;
         //  The type of primitive data accessed by WideNetID3
         typedef xs::UnsignedInt WideNetID3Value;

         /** Encapsulates the user link information that indicates the state and configuration of the individual user links, which
           * the waveform uses to transport the payload data over the communication medium. [Minimum occurrences: 0] [Maximum
           * occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::CommIFDL_NetLinkStatusType, uci::type::accessorType::commIFDL_NetLinkStatusType> PeerLinkStatus;

         /** Returns the accessor that provides access to the complex content that is identified by the SettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SettingsStatus.
           */
         virtual const uci::type::CommIFDL_ControlType& getSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SettingsStatus.
           */
         virtual uci::type::CommIFDL_ControlType& getSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SettingsStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SettingsStatus
           */
         virtual void setSettingsStatus(const uci::type::CommIFDL_ControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FlightSize.
           *
           * @return The value of the enumeration identified by FlightSize.
           */
         virtual const uci::type::CommIFDL_FlightSizeEnum& getFlightSize() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FlightSize.
           *
           * @return The value of the enumeration identified by FlightSize.
           */
         virtual uci::type::CommIFDL_FlightSizeEnum& getFlightSize()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FlightSize.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFlightSize(const uci::type::CommIFDL_FlightSizeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FlightSize.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFlightSize(uci::type::CommIFDL_FlightSizeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ChannelA_Number.
           *
           * @return The value of the simple primitive data type identified by ChannelA_Number.
           */
         virtual uci::type::CommIFDL_CommWaveformActivityType::ChannelA_NumberValue getChannelA_Number() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ChannelA_Number.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setChannelA_Number(uci::type::CommIFDL_CommWaveformActivityType::ChannelA_NumberValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ChannelB_Number.
           *
           * @return The value of the simple primitive data type identified by ChannelB_Number.
           */
         virtual uci::type::CommIFDL_CommWaveformActivityType::ChannelB_NumberValue getChannelB_Number() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ChannelB_Number.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setChannelB_Number(uci::type::CommIFDL_CommWaveformActivityType::ChannelB_NumberValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AcquisitionActive.
           *
           * @return The value of the simple primitive data type identified by AcquisitionActive.
           */
         virtual xs::Boolean getAcquisitionActive() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AcquisitionActive.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAcquisitionActive(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AcquisitionActive exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AcquisitionActive is emabled or not
           */
         virtual bool hasAcquisitionActive() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AcquisitionActive
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAcquisitionActive(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AcquisitionActive */
         virtual void clearAcquisitionActive()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceTransmit.
           *
           * @return The value of the simple primitive data type identified by VoiceTransmit.
           */
         virtual xs::Boolean getVoiceTransmit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceTransmit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceTransmit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceTransmit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceTransmit is emabled or not
           */
         virtual bool hasVoiceTransmit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceTransmit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableVoiceTransmit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceTransmit */
         virtual void clearVoiceTransmit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceReceive.
           *
           * @return The value of the simple primitive data type identified by VoiceReceive.
           */
         virtual xs::Boolean getVoiceReceive() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceReceive.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceReceive(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceReceive exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceReceive is emabled or not
           */
         virtual bool hasVoiceReceive() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceReceive
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableVoiceReceive(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceReceive */
         virtual void clearVoiceReceive()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the WideNetID1.
           *
           * @return The value of the simple primitive data type identified by WideNetID1.
           */
         virtual uci::type::CommIFDL_CommWaveformActivityType::WideNetID1Value getWideNetID1() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WideNetID1.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWideNetID1(uci::type::CommIFDL_CommWaveformActivityType::WideNetID1Value value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WideNetID1 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WideNetID1 is emabled or not
           */
         virtual bool hasWideNetID1() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WideNetID1
           *
           * @param type = uci::type::commIFDL_CommWaveformActivityType::accessorType::wideNetID1 This Accessor's accessor type
           */
         virtual void enableWideNetID1(uci::base::accessorType::AccessorType type = uci::type::commIFDL_CommWaveformActivityType::accessorType::wideNetID1)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WideNetID1 */
         virtual void clearWideNetID1()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the WideNetID2.
           *
           * @return The value of the simple primitive data type identified by WideNetID2.
           */
         virtual uci::type::CommIFDL_CommWaveformActivityType::WideNetID2Value getWideNetID2() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WideNetID2.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWideNetID2(uci::type::CommIFDL_CommWaveformActivityType::WideNetID2Value value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WideNetID2 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WideNetID2 is emabled or not
           */
         virtual bool hasWideNetID2() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WideNetID2
           *
           * @param type = uci::type::commIFDL_CommWaveformActivityType::accessorType::wideNetID2 This Accessor's accessor type
           */
         virtual void enableWideNetID2(uci::base::accessorType::AccessorType type = uci::type::commIFDL_CommWaveformActivityType::accessorType::wideNetID2)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WideNetID2 */
         virtual void clearWideNetID2()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the WideNetID3.
           *
           * @return The value of the simple primitive data type identified by WideNetID3.
           */
         virtual uci::type::CommIFDL_CommWaveformActivityType::WideNetID3Value getWideNetID3() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WideNetID3.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWideNetID3(uci::type::CommIFDL_CommWaveformActivityType::WideNetID3Value value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WideNetID3 exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WideNetID3 is emabled or not
           */
         virtual bool hasWideNetID3() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WideNetID3
           *
           * @param type = uci::type::commIFDL_CommWaveformActivityType::accessorType::wideNetID3 This Accessor's accessor type
           */
         virtual void enableWideNetID3(uci::base::accessorType::AccessorType type = uci::type::commIFDL_CommWaveformActivityType::accessorType::wideNetID3)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WideNetID3 */
         virtual void clearWideNetID3()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PeerLinkStatus.
           *
           * @return The bounded list identified by PeerLinkStatus.
           */
         virtual const uci::type::CommIFDL_CommWaveformActivityType::PeerLinkStatus& getPeerLinkStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PeerLinkStatus.
           *
           * @return The bounded list identified by PeerLinkStatus.
           */
         virtual uci::type::CommIFDL_CommWaveformActivityType::PeerLinkStatus& getPeerLinkStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PeerLinkStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPeerLinkStatus(const uci::type::CommIFDL_CommWaveformActivityType::PeerLinkStatus& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommIFDL_CommWaveformActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommIFDL_CommWaveformActivityType to copy from
           */
         CommIFDL_CommWaveformActivityType(const CommIFDL_CommWaveformActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommIFDL_CommWaveformActivityType to the contents of the
           * CommIFDL_CommWaveformActivityType on the right hand side (rhs) of the assignment
           * operator.CommIFDL_CommWaveformActivityType [only available to derived classes]
           *
           * @param rhs The CommIFDL_CommWaveformActivityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommIFDL_CommWaveformActivityType
           * @return A constant reference to this CommIFDL_CommWaveformActivityType.
           */
         const CommIFDL_CommWaveformActivityType& operator=(const CommIFDL_CommWaveformActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommIFDL_CommWaveformActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommIFDL_CommWaveformActivityType_h

