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
#ifndef Uci__Type__CommTerminalActivityType_h
#define Uci__Type__CommTerminalActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Type__CommConnectionStateEnum_h)
# include "uci/type/CommConnectionStateEnum.h"
#endif

#if !defined(Uci__Type__CommControlType_h)
# include "uci/type/CommControlType.h"
#endif

#if !defined(Uci__Type__CommProfileID_Type_h)
# include "uci/type/CommProfileID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__UserLinkStatusType_h)
# include "uci/type/UserLinkStatusType.h"
#endif

#if !defined(Uci__Type__CommLinkPortMapType_h)
# include "uci/type/CommLinkPortMapType.h"
#endif

#if !defined(Uci__Type__CommLinkStatisticsType_h)
# include "uci/type/CommLinkStatisticsType.h"
#endif

#if !defined(Uci__Type__CommWaveformActivityPET_h)
# include "uci/type/CommWaveformActivityPET.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__VerboseStatusType_h)
# include "uci/type/VerboseStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommTerminalActivityType sequence accessor class */
      class CommTerminalActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~CommTerminalActivityType()
         { }

         /** Returns this accessor's type constant, i.e. CommTerminalActivityType
           *
           * @return This accessor's type constant, i.e. CommTerminalActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commTerminalActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommTerminalActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Encapsulates the user link information that indicates the state and configuration of the individual user links, which
           * the waveform uses to transport the payload data over the communication medium. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::UserLinkStatusType, uci::type::accessorType::userLinkStatusType> UserLinkStatus;

         /** Returns the value of the enumeration that is identified by the ConnectionStatus.
           *
           * @return The value of the enumeration identified by ConnectionStatus.
           */
         virtual const uci::type::CommConnectionStateEnum& getConnectionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConnectionStatus.
           *
           * @return The value of the enumeration identified by ConnectionStatus.
           */
         virtual uci::type::CommConnectionStateEnum& getConnectionStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConnectionStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConnectionStatus(const uci::type::CommConnectionStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConnectionStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConnectionStatus(uci::type::CommConnectionStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ConnectionStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasConnectionStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getConnectionStatus will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commConnectionStateEnum This Accessor's accessor type
           */
         virtual void enableConnectionStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::commConnectionStateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearConnectionStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettingsStatus.
           */
         virtual const uci::type::CommControlType& getCommonSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettingsStatus.
           */
         virtual uci::type::CommControlType& getCommonSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommonSettingsStatus to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommonSettingsStatus
           */
         virtual void setCommonSettingsStatus(const uci::type::CommControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommonSettingsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommonSettingsStatus is emabled or not
           */
         virtual bool hasCommonSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommonSettingsStatus
           *
           * @param type = uci::type::accessorType::commControlType This Accessor's accessor type
           */
         virtual void enableCommonSettingsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::commControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommonSettingsStatus */
         virtual void clearCommonSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentProfileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentProfileID.
           */
         virtual const uci::type::CommProfileID_Type& getCurrentProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentProfileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentProfileID.
           */
         virtual uci::type::CommProfileID_Type& getCurrentProfileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CurrentProfileID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CurrentProfileID
           */
         virtual void setCurrentProfileID(const uci::type::CommProfileID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CurrentProfileID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CurrentProfileID is emabled or not
           */
         virtual bool hasCurrentProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CurrentProfileID
           *
           * @param type = uci::type::accessorType::commProfileID_Type This Accessor's accessor type
           */
         virtual void enableCurrentProfileID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commProfileID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CurrentProfileID */
         virtual void clearCurrentProfileID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UserLinkStatus.
           *
           * @return The bounded list identified by UserLinkStatus.
           */
         virtual const uci::type::CommTerminalActivityType::UserLinkStatus& getUserLinkStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UserLinkStatus.
           *
           * @return The bounded list identified by UserLinkStatus.
           */
         virtual uci::type::CommTerminalActivityType::UserLinkStatus& getUserLinkStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the UserLinkStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setUserLinkStatus(const uci::type::CommTerminalActivityType::UserLinkStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkPortMap.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkPortMap.
           */
         virtual const uci::type::CommLinkPortMapType& getLinkPortMap() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkPortMap.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkPortMap.
           */
         virtual uci::type::CommLinkPortMapType& getLinkPortMap()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LinkPortMap to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LinkPortMap
           */
         virtual void setLinkPortMap(const uci::type::CommLinkPortMapType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LinkPortMap exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LinkPortMap is emabled or not
           */
         virtual bool hasLinkPortMap() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LinkPortMap
           *
           * @param type = uci::type::accessorType::commLinkPortMapType This Accessor's accessor type
           */
         virtual void enableLinkPortMap(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLinkPortMapType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LinkPortMap */
         virtual void clearLinkPortMap()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkStatistics.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkStatistics.
           */
         virtual const uci::type::CommLinkStatisticsType& getLinkStatistics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkStatistics.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkStatistics.
           */
         virtual uci::type::CommLinkStatisticsType& getLinkStatistics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LinkStatistics to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LinkStatistics
           */
         virtual void setLinkStatistics(const uci::type::CommLinkStatisticsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LinkStatistics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LinkStatistics is emabled or not
           */
         virtual bool hasLinkStatistics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LinkStatistics
           *
           * @param type = uci::type::accessorType::commLinkStatisticsType This Accessor's accessor type
           */
         virtual void enableLinkStatistics(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLinkStatisticsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LinkStatistics */
         virtual void clearLinkStatistics()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * WaveformSpecificSettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      WaveformSpecificSettingsStatus.
           */
         virtual const uci::type::CommWaveformActivityPET& getWaveformSpecificSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * WaveformSpecificSettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      WaveformSpecificSettingsStatus.
           */
         virtual uci::type::CommWaveformActivityPET& getWaveformSpecificSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WaveformSpecificSettingsStatus to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WaveformSpecificSettingsStatus
           */
         virtual void setWaveformSpecificSettingsStatus(const uci::type::CommWaveformActivityPET& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WaveformSpecificSettingsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WaveformSpecificSettingsStatus is emabled or not
           */
         virtual bool hasWaveformSpecificSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WaveformSpecificSettingsStatus
           *
           * @param type = uci::type::accessorType::commWaveformActivityPET This Accessor's accessor type
           */
         virtual void enableWaveformSpecificSettingsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::commWaveformActivityPET)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WaveformSpecificSettingsStatus */
         virtual void clearWaveformSpecificSettingsStatus()
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


         /** Returns the accessor that provides access to the complex content that is identified by the VerboseStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by VerboseStatus.
           */
         virtual const uci::type::VerboseStatusType& getVerboseStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VerboseStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by VerboseStatus.
           */
         virtual uci::type::VerboseStatusType& getVerboseStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VerboseStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VerboseStatus
           */
         virtual void setVerboseStatus(const uci::type::VerboseStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VerboseStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VerboseStatus is emabled or not
           */
         virtual bool hasVerboseStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VerboseStatus
           *
           * @param type = uci::type::accessorType::verboseStatusType This Accessor's accessor type
           */
         virtual void enableVerboseStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::verboseStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VerboseStatus */
         virtual void clearVerboseStatus()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommTerminalActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommTerminalActivityType to copy from
           */
         CommTerminalActivityType(const CommTerminalActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommTerminalActivityType to the contents of the
           * CommTerminalActivityType on the right hand side (rhs) of the assignment operator.CommTerminalActivityType [only
           * available to derived classes]
           *
           * @param rhs The CommTerminalActivityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommTerminalActivityType
           * @return A constant reference to this CommTerminalActivityType.
           */
         const CommTerminalActivityType& operator=(const CommTerminalActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommTerminalActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommTerminalActivityType_h

