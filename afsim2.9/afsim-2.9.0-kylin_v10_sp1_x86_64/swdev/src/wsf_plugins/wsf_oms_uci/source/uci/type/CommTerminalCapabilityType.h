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
#ifndef Uci__Type__CommTerminalCapabilityType_h
#define Uci__Type__CommTerminalCapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CommCapabilityEnum_h)
# include "uci/type/CommCapabilityEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityControlInterfacesEnum_h)
# include "uci/type/CapabilityControlInterfacesEnum.h"
#endif

#if !defined(Uci__Type__CommCapabilityOptionsType_h)
# include "uci/type/CommCapabilityOptionsType.h"
#endif

#if !defined(Uci__Type__CommProfileID_Type_h)
# include "uci/type/CommProfileID_Type.h"
#endif

#if !defined(Uci__Type__CommDataFormatEnum_h)
# include "uci/type/CommDataFormatEnum.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioRangeType_h)
# include "uci/type/MilliwattPowerRatioRangeType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__CryptoTypeEnum_h)
# include "uci/type/CryptoTypeEnum.h"
#endif

#if !defined(Uci__Type__CommUserIO_ID_Type_h)
# include "uci/type/CommUserIO_ID_Type.h"
#endif

#if !defined(Uci__Type__CommUserLinkType_h)
# include "uci/type/CommUserLinkType.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommTerminalCapabilityType sequence accessor class */
      class CommTerminalCapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommTerminalCapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. CommTerminalCapabilityType
           *
           * @return This accessor's type constant, i.e. CommTerminalCapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commTerminalCapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommTerminalCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an accepted control interface for the Capability. See enumeration annotations for further details. [Maximum
           * occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityControlInterfacesEnum, uci::type::accessorType::capabilityControlInterfacesEnum> AcceptedInterface;

         /** Indicates a Profile (preset) that is applicable to this Capability. Only the profiles currently available and
           * applicable to this Capability should be listed. A Profile (or preset) is a logical collection of data, whose
           * implementation is not specified, that is used to tailor, configure or otherwise affect the behavior of a Capability.
           * Profiles can vary from mission to mission or even by phases of a single mission. They are generally developed in
           * pre-mission planning and are unique for each Subsystem model. Their content is not standardized by UCI, but the
           * mechanism for advertising their availability and activating them is. Their content is abstracted by the UCI concept
           * of Capability. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommProfileID_Type, uci::type::accessorType::commProfileID_Type> AvailableProfileID;

         /** Indicates the data formats this capability is capable of exchanging via the data link. [Minimum occurrences: 0]
           * [Maximum occurrences: 16]
           */
         typedef uci::base::BoundedList<uci::type::CommDataFormatEnum, uci::type::accessorType::commDataFormatEnum> DataFormat;

         /** Indicates the frequency bands used by the waveform for transmissions over the network medium. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> TransmitFrequencyRange;

         /** Indicates the frequency bands used by the waveform to receive. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> ReceiveFrequencyRange;

         /** Indicates the Capability’s first tier type in the Crypto taxonomy. Reflects crypto modes supported by the capability
           * given the current configuration of MDF, Keys, code version, etc. [Minimum occurrences: 0] [Maximum occurrences: 14]
           */
         typedef uci::base::BoundedList<uci::type::CryptoTypeEnum, uci::type::accessorType::cryptoTypeEnum> CryptoType;

         /** Identifies User I/O components that the waveform may be dynamically configured to use a data source (in the case of
           * data to be transmitted) or a data sink (in the case of data received and decrypted). [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommUserIO_ID_Type, uci::type::accessorType::commUserIO_ID_Type> AssociatedUserIO_ID;

         /** Identifies network components, or logical RF ports, that the waveform may be dynamically configured to use as a data
           * sink (in the case of data to be transmitted) or a data source (in the case of received data). [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommUserLinkType, uci::type::accessorType::commUserLinkType> AssociatedUserLink;

         /** Indicates a command and control relevant Antenna Component associated with the Capability. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> AssociatedAntennaID;

         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::CommCapabilityEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::CommCapabilityEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::CommCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::CommCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubCapabilityType.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCapabilityType.
           */
         virtual const uci::type::ForeignKeyType& getSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubCapabilityType.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCapabilityType.
           */
         virtual uci::type::ForeignKeyType& getSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubCapabilityType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubCapabilityType
           */
         virtual void setSubCapabilityType(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubCapabilityType exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubCapabilityType is emabled or not
           */
         virtual bool hasSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubCapabilityType
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableSubCapabilityType(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubCapabilityType */
         virtual void clearSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::CommTerminalCapabilityType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::CommTerminalCapabilityType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::CommTerminalCapabilityType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual const uci::type::CommCapabilityOptionsType& getCapabilityOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual uci::type::CommCapabilityOptionsType& getCapabilityOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityOptions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityOptions
           */
         virtual void setCapabilityOptions(const uci::type::CommCapabilityOptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableProfileID.
           *
           * @return The bounded list identified by AvailableProfileID.
           */
         virtual const uci::type::CommTerminalCapabilityType::AvailableProfileID& getAvailableProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AvailableProfileID.
           *
           * @return The bounded list identified by AvailableProfileID.
           */
         virtual uci::type::CommTerminalCapabilityType::AvailableProfileID& getAvailableProfileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AvailableProfileID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAvailableProfileID(const uci::type::CommTerminalCapabilityType::AvailableProfileID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataFormat.
           *
           * @return The bounded list identified by DataFormat.
           */
         virtual const uci::type::CommTerminalCapabilityType::DataFormat& getDataFormat() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DataFormat.
           *
           * @return The bounded list identified by DataFormat.
           */
         virtual uci::type::CommTerminalCapabilityType::DataFormat& getDataFormat()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DataFormat.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDataFormat(const uci::type::CommTerminalCapabilityType::DataFormat& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TerminalUniqueID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TerminalUniqueID.
           */
         virtual const uci::type::ForeignKeyType& getTerminalUniqueID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TerminalUniqueID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TerminalUniqueID.
           */
         virtual uci::type::ForeignKeyType& getTerminalUniqueID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TerminalUniqueID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TerminalUniqueID
           */
         virtual void setTerminalUniqueID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TerminalUniqueID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TerminalUniqueID is emabled or not
           */
         virtual bool hasTerminalUniqueID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TerminalUniqueID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableTerminalUniqueID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TerminalUniqueID */
         virtual void clearTerminalUniqueID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TxPowerRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by TxPowerRange.
           */
         virtual const uci::type::MilliwattPowerRatioRangeType& getTxPowerRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TxPowerRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by TxPowerRange.
           */
         virtual uci::type::MilliwattPowerRatioRangeType& getTxPowerRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TxPowerRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TxPowerRange
           */
         virtual void setTxPowerRange(const uci::type::MilliwattPowerRatioRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TxPowerRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TxPowerRange is emabled or not
           */
         virtual bool hasTxPowerRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TxPowerRange
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioRangeType This Accessor's accessor type
           */
         virtual void enableTxPowerRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TxPowerRange */
         virtual void clearTxPowerRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransmitFrequencyRange.
           *
           * @return The bounded list identified by TransmitFrequencyRange.
           */
         virtual const uci::type::CommTerminalCapabilityType::TransmitFrequencyRange& getTransmitFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TransmitFrequencyRange.
           *
           * @return The bounded list identified by TransmitFrequencyRange.
           */
         virtual uci::type::CommTerminalCapabilityType::TransmitFrequencyRange& getTransmitFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TransmitFrequencyRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTransmitFrequencyRange(const uci::type::CommTerminalCapabilityType::TransmitFrequencyRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiveFrequencyRange.
           *
           * @return The bounded list identified by ReceiveFrequencyRange.
           */
         virtual const uci::type::CommTerminalCapabilityType::ReceiveFrequencyRange& getReceiveFrequencyRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ReceiveFrequencyRange.
           *
           * @return The bounded list identified by ReceiveFrequencyRange.
           */
         virtual uci::type::CommTerminalCapabilityType::ReceiveFrequencyRange& getReceiveFrequencyRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ReceiveFrequencyRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setReceiveFrequencyRange(const uci::type::CommTerminalCapabilityType::ReceiveFrequencyRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoType.
           *
           * @return The bounded list identified by CryptoType.
           */
         virtual const uci::type::CommTerminalCapabilityType::CryptoType& getCryptoType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoType.
           *
           * @return The bounded list identified by CryptoType.
           */
         virtual uci::type::CommTerminalCapabilityType::CryptoType& getCryptoType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CryptoType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCryptoType(const uci::type::CommTerminalCapabilityType::CryptoType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedUserIO_ID.
           *
           * @return The bounded list identified by AssociatedUserIO_ID.
           */
         virtual const uci::type::CommTerminalCapabilityType::AssociatedUserIO_ID& getAssociatedUserIO_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedUserIO_ID.
           *
           * @return The bounded list identified by AssociatedUserIO_ID.
           */
         virtual uci::type::CommTerminalCapabilityType::AssociatedUserIO_ID& getAssociatedUserIO_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedUserIO_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedUserIO_ID(const uci::type::CommTerminalCapabilityType::AssociatedUserIO_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedUserLink.
           *
           * @return The bounded list identified by AssociatedUserLink.
           */
         virtual const uci::type::CommTerminalCapabilityType::AssociatedUserLink& getAssociatedUserLink() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedUserLink.
           *
           * @return The bounded list identified by AssociatedUserLink.
           */
         virtual uci::type::CommTerminalCapabilityType::AssociatedUserLink& getAssociatedUserLink()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedUserLink.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedUserLink(const uci::type::CommTerminalCapabilityType::AssociatedUserLink& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedAntennaID.
           *
           * @return The bounded list identified by AssociatedAntennaID.
           */
         virtual const uci::type::CommTerminalCapabilityType::AssociatedAntennaID& getAssociatedAntennaID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedAntennaID.
           *
           * @return The bounded list identified by AssociatedAntennaID.
           */
         virtual uci::type::CommTerminalCapabilityType::AssociatedAntennaID& getAssociatedAntennaID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedAntennaID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedAntennaID(const uci::type::CommTerminalCapabilityType::AssociatedAntennaID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommTerminalCapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommTerminalCapabilityType to copy from
           */
         CommTerminalCapabilityType(const CommTerminalCapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommTerminalCapabilityType to the contents of the
           * CommTerminalCapabilityType on the right hand side (rhs) of the assignment operator.CommTerminalCapabilityType [only
           * available to derived classes]
           *
           * @param rhs The CommTerminalCapabilityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommTerminalCapabilityType
           * @return A constant reference to this CommTerminalCapabilityType.
           */
         const CommTerminalCapabilityType& operator=(const CommTerminalCapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommTerminalCapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommTerminalCapabilityType_h

