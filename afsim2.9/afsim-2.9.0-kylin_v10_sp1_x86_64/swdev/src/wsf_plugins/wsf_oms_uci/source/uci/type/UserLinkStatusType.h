// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__UserLinkStatusType_h
#define Uci__Type__UserLinkStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommLinkPortMapType_h)
# include "uci/type/CommLinkPortMapType.h"
#endif

#if !defined(Uci__Type__CommConnectionStateEnum_h)
# include "uci/type/CommConnectionStateEnum.h"
#endif

#if !defined(Uci__Type__CommLinkStatisticsType_h)
# include "uci/type/CommLinkStatisticsType.h"
#endif

#if !defined(Uci__Type__CommControlType_h)
# include "uci/type/CommControlType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the UserLinkStatusType sequence accessor class */
      class UserLinkStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~UserLinkStatusType()
         { }

         /** Returns this accessor's type constant, i.e. UserLinkStatusType
           *
           * @return This accessor's type constant, i.e. UserLinkStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::userLinkStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const UserLinkStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Provides a reflection of what port assignments (IO and RF) are associated with this link. It is possible to have more
           * than one to reflect multiple simultaneous paths. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommLinkPortMapType, uci::type::accessorType::commLinkPortMapType> LinkPortMap;

         /** Returns the accessor that provides access to the complex content that is identified by the NetworkLinkID.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetworkLinkID.
           */
         virtual const uci::type::NetworkLinkID_Type& getNetworkLinkID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetworkLinkID.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetworkLinkID.
           */
         virtual uci::type::NetworkLinkID_Type& getNetworkLinkID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NetworkLinkID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NetworkLinkID
           */
         virtual void setNetworkLinkID(const uci::type::NetworkLinkID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkPortMap.
           *
           * @return The bounded list identified by LinkPortMap.
           */
         virtual const uci::type::UserLinkStatusType::LinkPortMap& getLinkPortMap() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkPortMap.
           *
           * @return The bounded list identified by LinkPortMap.
           */
         virtual uci::type::UserLinkStatusType::LinkPortMap& getLinkPortMap()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LinkPortMap.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLinkPortMap(const uci::type::UserLinkStatusType::LinkPortMap& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LinkConnectionState.
           *
           * @return The value of the enumeration identified by LinkConnectionState.
           */
         virtual const uci::type::CommConnectionStateEnum& getLinkConnectionState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LinkConnectionState.
           *
           * @return The value of the enumeration identified by LinkConnectionState.
           */
         virtual uci::type::CommConnectionStateEnum& getLinkConnectionState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LinkConnectionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLinkConnectionState(const uci::type::CommConnectionStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LinkConnectionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLinkConnectionState(uci::type::CommConnectionStateEnum::EnumerationItem value)
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



      protected:

         /** The constructor [only available to derived classes]. */
         UserLinkStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The UserLinkStatusType to copy from
           */
         UserLinkStatusType(const UserLinkStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this UserLinkStatusType to the contents of the UserLinkStatusType on
           * the right hand side (rhs) of the assignment operator.UserLinkStatusType [only available to derived classes]
           *
           * @param rhs The UserLinkStatusType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::UserLinkStatusType
           * @return A constant reference to this UserLinkStatusType.
           */
         const UserLinkStatusType& operator=(const UserLinkStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // UserLinkStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__UserLinkStatusType_h

