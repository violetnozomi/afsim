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
#ifndef Uci__Type__CommMADL_LinkStatusType_h
#define Uci__Type__CommMADL_LinkStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommMADL_LinkStateEnum_h)
# include "uci/type/CommMADL_LinkStateEnum.h"
#endif

#if !defined(Uci__Type__CommMADL_DataRateTypeEnum_h)
# include "uci/type/CommMADL_DataRateTypeEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_VoiceRxIndicatorTypeEnum_h)
# include "uci/type/CommMADL_VoiceRxIndicatorTypeEnum.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_PeerNodeDataType_h)
# include "uci/type/CommMADL_PeerNodeDataType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommMADL_TopologyEntryType_h)
# include "uci/type/CommMADL_TopologyEntryType.h"
#endif

#if !defined(Uci__Type__CommMADL_SearchTypeEnum_h)
# include "uci/type/CommMADL_SearchTypeEnum.h"
#endif

#if !defined(Uci__Type__CommMADL_TimeSlotOE_PhaseTypeEnum_h)
# include "uci/type/CommMADL_TimeSlotOE_PhaseTypeEnum.h"
#endif

#if !defined(Uci__Type__CommMADL_TimeSlotTxPhaseTypeEnum_h)
# include "uci/type/CommMADL_TimeSlotTxPhaseTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommMADL_LinkStatusType sequence accessor class */
      class CommMADL_LinkStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_LinkStatusType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_LinkStatusType
           *
           * @return This accessor's type constant, i.e. CommMADL_LinkStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_LinkStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_LinkStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] [Maximum inclusive: 65535] */
         typedef uci::base::UnsignedIntAccessor VoiceSource;
         //  The type of primitive data accessed by VoiceSource
         typedef xs::UnsignedInt VoiceSourceValue;

         /** If link connected, information on the network topology in this direction. This data is likely limited to verbose
           * Activity report. [Minimum occurrences: 0] [Maximum occurrences: 63]
           */
         typedef uci::base::BoundedList<uci::type::CommMADL_TopologyEntryType, uci::type::accessorType::commMADL_TopologyEntryType> MemberTopology;

         /** [Minimum inclusive: 0] [Maximum inclusive: 65535] */
         typedef uci::base::UnsignedIntAccessor HopOffset;
         //  The type of primitive data accessed by HopOffset
         typedef xs::UnsignedInt HopOffsetValue;

         /** Returns the value of the enumeration that is identified by the LinkState.
           *
           * @return The value of the enumeration identified by LinkState.
           */
         virtual const uci::type::CommMADL_LinkStateEnum& getLinkState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LinkState.
           *
           * @return The value of the enumeration identified by LinkState.
           */
         virtual uci::type::CommMADL_LinkStateEnum& getLinkState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LinkState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLinkState(const uci::type::CommMADL_LinkStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LinkState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLinkState(uci::type::CommMADL_LinkStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ReceivedDataRate.
           *
           * @return The value of the enumeration identified by ReceivedDataRate.
           */
         virtual const uci::type::CommMADL_DataRateTypeEnum& getReceivedDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ReceivedDataRate.
           *
           * @return The value of the enumeration identified by ReceivedDataRate.
           */
         virtual uci::type::CommMADL_DataRateTypeEnum& getReceivedDataRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ReceivedDataRate.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setReceivedDataRate(const uci::type::CommMADL_DataRateTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ReceivedDataRate.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setReceivedDataRate(uci::type::CommMADL_DataRateTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ReceivedDataRateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasReceivedDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getReceivedDataRate will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commMADL_DataRateTypeEnum This Accessor's accessor type
           */
         virtual void enableReceivedDataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_DataRateTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearReceivedDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ActualTxDataRate.
           *
           * @return The value of the enumeration identified by ActualTxDataRate.
           */
         virtual const uci::type::CommMADL_DataRateTypeEnum& getActualTxDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ActualTxDataRate.
           *
           * @return The value of the enumeration identified by ActualTxDataRate.
           */
         virtual uci::type::CommMADL_DataRateTypeEnum& getActualTxDataRate()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ActualTxDataRate.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActualTxDataRate(const uci::type::CommMADL_DataRateTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ActualTxDataRate.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActualTxDataRate(uci::type::CommMADL_DataRateTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ActualTxDataRateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasActualTxDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getActualTxDataRate will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commMADL_DataRateTypeEnum This Accessor's accessor type
           */
         virtual void enableActualTxDataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_DataRateTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearActualTxDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HigherDataRateCapable.
           *
           * @return The value of the simple primitive data type identified by HigherDataRateCapable.
           */
         virtual xs::Boolean getHigherDataRateCapable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HigherDataRateCapable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHigherDataRateCapable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HigherDataRateCapable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HigherDataRateCapable is emabled or not
           */
         virtual bool hasHigherDataRateCapable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HigherDataRateCapable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableHigherDataRateCapable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HigherDataRateCapable */
         virtual void clearHigherDataRateCapable()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the VoiceReceiptIndicator.
           *
           * @return The value of the enumeration identified by VoiceReceiptIndicator.
           */
         virtual const uci::type::CommMADL_VoiceRxIndicatorTypeEnum& getVoiceReceiptIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the VoiceReceiptIndicator.
           *
           * @return The value of the enumeration identified by VoiceReceiptIndicator.
           */
         virtual uci::type::CommMADL_VoiceRxIndicatorTypeEnum& getVoiceReceiptIndicator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the VoiceReceiptIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setVoiceReceiptIndicator(const uci::type::CommMADL_VoiceRxIndicatorTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the VoiceReceiptIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setVoiceReceiptIndicator(uci::type::CommMADL_VoiceRxIndicatorTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield VoiceReceiptIndicatoris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasVoiceReceiptIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getVoiceReceiptIndicator will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::commMADL_VoiceRxIndicatorTypeEnum This Accessor's accessor type
           */
         virtual void enableVoiceReceiptIndicator(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_VoiceRxIndicatorTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearVoiceReceiptIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceSource.
           *
           * @return The value of the simple primitive data type identified by VoiceSource.
           */
         virtual uci::type::CommMADL_LinkStatusType::VoiceSourceValue getVoiceSource() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceSource.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceSource(uci::type::CommMADL_LinkStatusType::VoiceSourceValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceSource exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceSource is emabled or not
           */
         virtual bool hasVoiceSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceSource
           *
           * @param type = uci::type::commMADL_LinkStatusType::accessorType::voiceSource This Accessor's accessor type
           */
         virtual void enableVoiceSource(uci::base::accessorType::AccessorType type = uci::type::commMADL_LinkStatusType::accessorType::voiceSource)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceSource */
         virtual void clearVoiceSource()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PeerNodeData.
           *
           * @return The acecssor that provides access to the complex content that is identified by PeerNodeData.
           */
         virtual const uci::type::CommMADL_PeerNodeDataType& getPeerNodeData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PeerNodeData.
           *
           * @return The acecssor that provides access to the complex content that is identified by PeerNodeData.
           */
         virtual uci::type::CommMADL_PeerNodeDataType& getPeerNodeData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PeerNodeData to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PeerNodeData
           */
         virtual void setPeerNodeData(const uci::type::CommMADL_PeerNodeDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PeerNodeData exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PeerNodeData is emabled or not
           */
         virtual bool hasPeerNodeData() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PeerNodeData
           *
           * @param type = uci::type::accessorType::commMADL_PeerNodeDataType This Accessor's accessor type
           */
         virtual void enablePeerNodeData(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_PeerNodeDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PeerNodeData */
         virtual void clearPeerNodeData()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MemberTopology.
           *
           * @return The bounded list identified by MemberTopology.
           */
         virtual const uci::type::CommMADL_LinkStatusType::MemberTopology& getMemberTopology() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MemberTopology.
           *
           * @return The bounded list identified by MemberTopology.
           */
         virtual uci::type::CommMADL_LinkStatusType::MemberTopology& getMemberTopology()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MemberTopology.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMemberTopology(const uci::type::CommMADL_LinkStatusType::MemberTopology& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SearchPattern.
           *
           * @return The value of the enumeration identified by SearchPattern.
           */
         virtual const uci::type::CommMADL_SearchTypeEnum& getSearchPattern() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SearchPattern.
           *
           * @return The value of the enumeration identified by SearchPattern.
           */
         virtual uci::type::CommMADL_SearchTypeEnum& getSearchPattern()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SearchPattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSearchPattern(const uci::type::CommMADL_SearchTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SearchPattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSearchPattern(uci::type::CommMADL_SearchTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SearchPatternis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSearchPattern() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSearchPattern will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commMADL_SearchTypeEnum This Accessor's accessor type
           */
         virtual void enableSearchPattern(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_SearchTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSearchPattern()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SearchVector.
           *
           * @return The value of the simple primitive data type identified by SearchVector.
           */
         virtual xs::Boolean getSearchVector() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SearchVector.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSearchVector(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SearchVector exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SearchVector is emabled or not
           */
         virtual bool hasSearchVector() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SearchVector
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableSearchVector(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SearchVector */
         virtual void clearSearchVector()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EvenOddPhase.
           *
           * @return The value of the enumeration identified by EvenOddPhase.
           */
         virtual const uci::type::CommMADL_TimeSlotOE_PhaseTypeEnum& getEvenOddPhase() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EvenOddPhase.
           *
           * @return The value of the enumeration identified by EvenOddPhase.
           */
         virtual uci::type::CommMADL_TimeSlotOE_PhaseTypeEnum& getEvenOddPhase()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EvenOddPhase.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEvenOddPhase(const uci::type::CommMADL_TimeSlotOE_PhaseTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EvenOddPhase.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEvenOddPhase(uci::type::CommMADL_TimeSlotOE_PhaseTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EvenOddPhaseis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEvenOddPhase() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEvenOddPhase will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commMADL_TimeSlotOE_PhaseTypeEnum This Accessor's accessor type
           */
         virtual void enableEvenOddPhase(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_TimeSlotOE_PhaseTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEvenOddPhase()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EarlyLatePhase.
           *
           * @return The value of the enumeration identified by EarlyLatePhase.
           */
         virtual const uci::type::CommMADL_TimeSlotTxPhaseTypeEnum& getEarlyLatePhase() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EarlyLatePhase.
           *
           * @return The value of the enumeration identified by EarlyLatePhase.
           */
         virtual uci::type::CommMADL_TimeSlotTxPhaseTypeEnum& getEarlyLatePhase()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EarlyLatePhase.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEarlyLatePhase(const uci::type::CommMADL_TimeSlotTxPhaseTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EarlyLatePhase.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEarlyLatePhase(uci::type::CommMADL_TimeSlotTxPhaseTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EarlyLatePhaseis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEarlyLatePhase() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEarlyLatePhase will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commMADL_TimeSlotTxPhaseTypeEnum This Accessor's accessor type
           */
         virtual void enableEarlyLatePhase(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_TimeSlotTxPhaseTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEarlyLatePhase()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HopOffset.
           *
           * @return The value of the simple primitive data type identified by HopOffset.
           */
         virtual uci::type::CommMADL_LinkStatusType::HopOffsetValue getHopOffset() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HopOffset.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHopOffset(uci::type::CommMADL_LinkStatusType::HopOffsetValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HopOffset exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HopOffset is emabled or not
           */
         virtual bool hasHopOffset() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HopOffset
           *
           * @param type = uci::type::commMADL_LinkStatusType::accessorType::hopOffset This Accessor's accessor type
           */
         virtual void enableHopOffset(uci::base::accessorType::AccessorType type = uci::type::commMADL_LinkStatusType::accessorType::hopOffset)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HopOffset */
         virtual void clearHopOffset()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_LinkStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_LinkStatusType to copy from
           */
         CommMADL_LinkStatusType(const CommMADL_LinkStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_LinkStatusType to the contents of the
           * CommMADL_LinkStatusType on the right hand side (rhs) of the assignment operator.CommMADL_LinkStatusType [only
           * available to derived classes]
           *
           * @param rhs The CommMADL_LinkStatusType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommMADL_LinkStatusType
           * @return A constant reference to this CommMADL_LinkStatusType.
           */
         const CommMADL_LinkStatusType& operator=(const CommMADL_LinkStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_LinkStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_LinkStatusType_h

