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
#ifndef Uci__Type__MessageConfigurationType_h
#define Uci__Type__MessageConfigurationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Type__CommDataDirectionFiltrationEnum_h)
# include "uci/type/CommDataDirectionFiltrationEnum.h"
#endif

#if !defined(Uci__Type__InclusionEnum_h)
# include "uci/type/InclusionEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__DataLinkNativeFilterPET_h)
# include "uci/type/DataLinkNativeFilterPET.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A filter for messages. Each component has an 'and' relationship with the others. */
      class MessageConfigurationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MessageConfigurationType()
         { }

         /** Returns this accessor's type constant, i.e. MessageConfigurationType
           *
           * @return This accessor's type constant, i.e. MessageConfigurationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::messageConfigurationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MessageConfigurationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Specifies a Critical Abstraction Layer Connection ID. This allows this message configuration to be applied to a
           * specific CAL Connection therefore only applying to message traffic on that connection. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> ConnectionID;

         /** System ID of a remote System. If System ID is empty, It is assumed this is a broadcast. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> RemoteSystemID;

         /** Returns the accessor that provides access to the complex content that is identified by the Filter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Filter.
           */
         virtual const uci::type::QueryFilterPET& getFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Filter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Filter.
           */
         virtual uci::type::QueryFilterPET& getFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Filter to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Filter
           */
         virtual void setFilter(const uci::type::QueryFilterPET& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilterDirection.
           *
           * @return The value of the enumeration identified by FilterDirection.
           */
         virtual const uci::type::CommDataDirectionFiltrationEnum& getFilterDirection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilterDirection.
           *
           * @return The value of the enumeration identified by FilterDirection.
           */
         virtual uci::type::CommDataDirectionFiltrationEnum& getFilterDirection()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilterDirection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilterDirection(const uci::type::CommDataDirectionFiltrationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilterDirection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilterDirection(uci::type::CommDataDirectionFiltrationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilterInclusionType.
           *
           * @return The value of the enumeration identified by FilterInclusionType.
           */
         virtual const uci::type::InclusionEnum& getFilterInclusionType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FilterInclusionType.
           *
           * @return The value of the enumeration identified by FilterInclusionType.
           */
         virtual uci::type::InclusionEnum& getFilterInclusionType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilterInclusionType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilterInclusionType(const uci::type::InclusionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FilterInclusionType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFilterInclusionType(uci::type::InclusionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConnectionID.
           *
           * @return The bounded list identified by ConnectionID.
           */
         virtual const uci::type::MessageConfigurationType::ConnectionID& getConnectionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConnectionID.
           *
           * @return The bounded list identified by ConnectionID.
           */
         virtual uci::type::MessageConfigurationType::ConnectionID& getConnectionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConnectionID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConnectionID(const uci::type::MessageConfigurationType::ConnectionID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RemoteSystemID.
           *
           * @return The bounded list identified by RemoteSystemID.
           */
         virtual const uci::type::MessageConfigurationType::RemoteSystemID& getRemoteSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RemoteSystemID.
           *
           * @return The bounded list identified by RemoteSystemID.
           */
         virtual uci::type::MessageConfigurationType::RemoteSystemID& getRemoteSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RemoteSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRemoteSystemID(const uci::type::MessageConfigurationType::RemoteSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TransmitPeriodicity.
           *
           * @return The value of the simple primitive data type identified by TransmitPeriodicity.
           */
         virtual uci::type::DurationTypeValue getTransmitPeriodicity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TransmitPeriodicity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTransmitPeriodicity(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TransmitPeriodicity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TransmitPeriodicity is emabled or not
           */
         virtual bool hasTransmitPeriodicity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TransmitPeriodicity
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableTransmitPeriodicity(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TransmitPeriodicity */
         virtual void clearTransmitPeriodicity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataLinkNativeFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataLinkNativeFilter.
           */
         virtual const uci::type::DataLinkNativeFilterPET& getDataLinkNativeFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DataLinkNativeFilter.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataLinkNativeFilter.
           */
         virtual uci::type::DataLinkNativeFilterPET& getDataLinkNativeFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DataLinkNativeFilter to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DataLinkNativeFilter
           */
         virtual void setDataLinkNativeFilter(const uci::type::DataLinkNativeFilterPET& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DataLinkNativeFilter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DataLinkNativeFilter is emabled or not
           */
         virtual bool hasDataLinkNativeFilter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DataLinkNativeFilter
           *
           * @param type = uci::type::accessorType::dataLinkNativeFilterPET This Accessor's accessor type
           */
         virtual void enableDataLinkNativeFilter(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataLinkNativeFilterPET)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DataLinkNativeFilter */
         virtual void clearDataLinkNativeFilter()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MessageConfigurationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MessageConfigurationType to copy from
           */
         MessageConfigurationType(const MessageConfigurationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MessageConfigurationType to the contents of the
           * MessageConfigurationType on the right hand side (rhs) of the assignment operator.MessageConfigurationType [only
           * available to derived classes]
           *
           * @param rhs The MessageConfigurationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::MessageConfigurationType
           * @return A constant reference to this MessageConfigurationType.
           */
         const MessageConfigurationType& operator=(const MessageConfigurationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MessageConfigurationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MessageConfigurationType_h

