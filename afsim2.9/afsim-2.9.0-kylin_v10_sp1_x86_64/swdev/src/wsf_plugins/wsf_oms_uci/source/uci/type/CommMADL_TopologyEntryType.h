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
#ifndef Uci__Type__CommMADL_TopologyEntryType_h
#define Uci__Type__CommMADL_TopologyEntryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommMADL_NodeRangeType_h)
# include "uci/type/CommMADL_NodeRangeType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_FlightGroupType_h)
# include "uci/type/CommMADL_FlightGroupType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__Point4D_Type_h)
# include "uci/type/Point4D_Type.h"
#endif

#if !defined(Uci__Type__CommMADL_TimeSourceEnum_h)
# include "uci/type/CommMADL_TimeSourceEnum.h"
#endif

#if !defined(Uci__Type__CommMADL_DataRateTypeEnum_h)
# include "uci/type/CommMADL_DataRateTypeEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommMADL_SecurityKeyType_h)
# include "uci/type/CommMADL_SecurityKeyType.h"
#endif

#if !defined(Uci__Type__CommMADL_NodeDataValidityEnum_h)
# include "uci/type/CommMADL_NodeDataValidityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommMADL_TopologyEntryType sequence accessor class */
      class CommMADL_TopologyEntryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_TopologyEntryType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_TopologyEntryType
           *
           * @return This accessor's type constant, i.e. CommMADL_TopologyEntryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_TopologyEntryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_TopologyEntryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 1] [Maximum inclusive: 65535] */
         typedef uci::base::UnsignedIntAccessor MemberMADL_UniqueID;
         //  The type of primitive data accessed by MemberMADL_UniqueID
         typedef xs::UnsignedInt MemberMADL_UniqueIDValue;

         /** Identifies the security categories and bandwidths associated with the member. [Minimum occurrences: 0] [Maximum
           * occurrences: 4]
           */
         typedef uci::base::BoundedList<uci::type::CommMADL_SecurityKeyType, uci::type::accessorType::commMADL_SecurityKeyType> SecurityKey;

         /** Returns the value of the SimplePrimitive data type that is identified by the MemberNodeID.
           *
           * @return The value of the simple primitive data type identified by MemberNodeID.
           */
         virtual uci::type::CommMADL_NodeRangeTypeValue getMemberNodeID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MemberNodeID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMemberNodeID(uci::type::CommMADL_NodeRangeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MemberMADL_UniqueID.
           *
           * @return The value of the simple primitive data type identified by MemberMADL_UniqueID.
           */
         virtual uci::type::CommMADL_TopologyEntryType::MemberMADL_UniqueIDValue getMemberMADL_UniqueID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MemberMADL_UniqueID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMemberMADL_UniqueID(uci::type::CommMADL_TopologyEntryType::MemberMADL_UniqueIDValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MemberMADL_UniqueID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MemberMADL_UniqueID is emabled or not
           */
         virtual bool hasMemberMADL_UniqueID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MemberMADL_UniqueID
           *
           * @param type = uci::type::commMADL_TopologyEntryType::accessorType::memberMADL_UniqueID This Accessor's accessor type
           */
         virtual void enableMemberMADL_UniqueID(uci::base::accessorType::AccessorType type = uci::type::commMADL_TopologyEntryType::accessorType::memberMADL_UniqueID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MemberMADL_UniqueID */
         virtual void clearMemberMADL_UniqueID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MemberFlightGroupID.
           *
           * @return The value of the string data type identified by MemberFlightGroupID.
           */
         virtual const uci::type::CommMADL_FlightGroupType& getMemberFlightGroupID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MemberFlightGroupID.
           *
           * @return The value of the string data type identified by MemberFlightGroupID.
           */
         virtual uci::type::CommMADL_FlightGroupType& getMemberFlightGroupID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MemberFlightGroupID to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMemberFlightGroupID(const uci::type::CommMADL_FlightGroupType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MemberFlightGroupID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMemberFlightGroupID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MemberFlightGroupID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMemberFlightGroupID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MemberFlightGroupID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MemberFlightGroupID is emabled or not
           */
         virtual bool hasMemberFlightGroupID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MemberFlightGroupID
           *
           * @param type = uci::type::accessorType::commMADL_FlightGroupType This Accessor's accessor type
           */
         virtual void enableMemberFlightGroupID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_FlightGroupType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MemberFlightGroupID */
         virtual void clearMemberFlightGroupID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DataCapable.
           *
           * @return The value of the simple primitive data type identified by DataCapable.
           */
         virtual xs::Boolean getDataCapable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DataCapable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDataCapable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataCapable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataCapable is emabled or not
           */
         virtual bool hasDataCapable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataCapable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableDataCapable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataCapable */
         virtual void clearDataCapable()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceCapable.
           *
           * @return The value of the simple primitive data type identified by VoiceCapable.
           */
         virtual xs::Boolean getVoiceCapable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceCapable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceCapable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceCapable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceCapable is emabled or not
           */
         virtual bool hasVoiceCapable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceCapable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableVoiceCapable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceCapable */
         virtual void clearVoiceCapable()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MemberPosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by MemberPosition.
           */
         virtual const uci::type::Point4D_Type& getMemberPosition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MemberPosition.
           *
           * @return The acecssor that provides access to the complex content that is identified by MemberPosition.
           */
         virtual uci::type::Point4D_Type& getMemberPosition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MemberPosition to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MemberPosition
           */
         virtual void setMemberPosition(const uci::type::Point4D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MemberPosition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MemberPosition is emabled or not
           */
         virtual bool hasMemberPosition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MemberPosition
           *
           * @param type = uci::type::accessorType::point4D_Type This Accessor's accessor type
           */
         virtual void enableMemberPosition(uci::base::accessorType::AccessorType type = uci::type::accessorType::point4D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MemberPosition */
         virtual void clearMemberPosition()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MemberTimeSource.
           *
           * @return The value of the enumeration identified by MemberTimeSource.
           */
         virtual const uci::type::CommMADL_TimeSourceEnum& getMemberTimeSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MemberTimeSource.
           *
           * @return The value of the enumeration identified by MemberTimeSource.
           */
         virtual uci::type::CommMADL_TimeSourceEnum& getMemberTimeSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MemberTimeSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMemberTimeSource(const uci::type::CommMADL_TimeSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MemberTimeSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMemberTimeSource(uci::type::CommMADL_TimeSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield MemberTimeSourceis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasMemberTimeSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getMemberTimeSource will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commMADL_TimeSourceEnum This Accessor's accessor type
           */
         virtual void enableMemberTimeSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_TimeSourceEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearMemberTimeSource()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NetTimeFineSyncAchieved.
           *
           * @return The value of the simple primitive data type identified by NetTimeFineSyncAchieved.
           */
         virtual xs::Boolean getNetTimeFineSyncAchieved() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NetTimeFineSyncAchieved.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNetTimeFineSyncAchieved(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NetTimeFineSyncAchieved exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NetTimeFineSyncAchieved is emabled or not
           */
         virtual bool hasNetTimeFineSyncAchieved() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NetTimeFineSyncAchieved
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableNetTimeFineSyncAchieved(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NetTimeFineSyncAchieved */
         virtual void clearNetTimeFineSyncAchieved()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PathLatencyRelay.
           *
           * @return The value of the simple primitive data type identified by PathLatencyRelay.
           */
         virtual uci::type::CommMADL_NodeRangeTypeValue getPathLatencyRelay() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PathLatencyRelay.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPathLatencyRelay(uci::type::CommMADL_NodeRangeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PathLatencyRelay exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PathLatencyRelay is emabled or not
           */
         virtual bool hasPathLatencyRelay() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PathLatencyRelay
           *
           * @param type = uci::type::accessorType::commMADL_NodeRangeType This Accessor's accessor type
           */
         virtual void enablePathLatencyRelay(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_NodeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PathLatencyRelay */
         virtual void clearPathLatencyRelay()
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


         /** Returns the value of the SimplePrimitive data type that is identified by the HigerDataRateCapable.
           *
           * @return The value of the simple primitive data type identified by HigerDataRateCapable.
           */
         virtual xs::Boolean getHigerDataRateCapable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HigerDataRateCapable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHigerDataRateCapable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HigerDataRateCapable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HigerDataRateCapable is emabled or not
           */
         virtual bool hasHigerDataRateCapable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HigerDataRateCapable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableHigerDataRateCapable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HigerDataRateCapable */
         virtual void clearHigerDataRateCapable()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecurityKey.
           *
           * @return The bounded list identified by SecurityKey.
           */
         virtual const uci::type::CommMADL_TopologyEntryType::SecurityKey& getSecurityKey() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecurityKey.
           *
           * @return The bounded list identified by SecurityKey.
           */
         virtual uci::type::CommMADL_TopologyEntryType::SecurityKey& getSecurityKey()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SecurityKey.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSecurityKey(const uci::type::CommMADL_TopologyEntryType::SecurityKey& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NodeDataValidity.
           *
           * @return The value of the enumeration identified by NodeDataValidity.
           */
         virtual const uci::type::CommMADL_NodeDataValidityEnum& getNodeDataValidity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NodeDataValidity.
           *
           * @return The value of the enumeration identified by NodeDataValidity.
           */
         virtual uci::type::CommMADL_NodeDataValidityEnum& getNodeDataValidity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NodeDataValidity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNodeDataValidity(const uci::type::CommMADL_NodeDataValidityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NodeDataValidity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNodeDataValidity(uci::type::CommMADL_NodeDataValidityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_TopologyEntryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_TopologyEntryType to copy from
           */
         CommMADL_TopologyEntryType(const CommMADL_TopologyEntryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_TopologyEntryType to the contents of the
           * CommMADL_TopologyEntryType on the right hand side (rhs) of the assignment operator.CommMADL_TopologyEntryType [only
           * available to derived classes]
           *
           * @param rhs The CommMADL_TopologyEntryType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommMADL_TopologyEntryType
           * @return A constant reference to this CommMADL_TopologyEntryType.
           */
         const CommMADL_TopologyEntryType& operator=(const CommMADL_TopologyEntryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_TopologyEntryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_TopologyEntryType_h

