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
#ifndef Uci__Type__CommMADL_ControlType_h
#define Uci__Type__CommMADL_ControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommMADL_NodeRangeType_h)
# include "uci/type/CommMADL_NodeRangeType.h"
#endif

#if !defined(Uci__Type__CommMADL_FlightGroupType_h)
# include "uci/type/CommMADL_FlightGroupType.h"
#endif

#if !defined(Uci__Type__CommMADL_NetworkType_h)
# include "uci/type/CommMADL_NetworkType.h"
#endif

#if !defined(Uci__Type__CommMADL_NetworkLayoutType_h)
# include "uci/type/CommMADL_NetworkLayoutType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_LinkCommandListType_h)
# include "uci/type/CommMADL_LinkCommandListType.h"
#endif

#if !defined(Uci__Type__CommMADL_NetChangeRequestType_h)
# include "uci/type/CommMADL_NetChangeRequestType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to perform MADL Specific tuning of a communications capability. */
      class CommMADL_ControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_ControlType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_ControlType
           *
           * @return This accessor's type constant, i.e. CommMADL_ControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_ControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_ControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 1] [Maximum inclusive: 367] */
         typedef uci::base::UnsignedIntAccessor MADL_DayOfYear;
         //  The type of primitive data accessed by MADL_DayOfYear
         typedef xs::UnsignedInt MADL_DayOfYearValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the Node.
           *
           * @return The value of the simple primitive data type identified by Node.
           */
         virtual uci::type::CommMADL_NodeRangeTypeValue getNode() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Node.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNode(uci::type::CommMADL_NodeRangeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FlightGroupID.
           *
           * @return The value of the string data type identified by FlightGroupID.
           */
         virtual const uci::type::CommMADL_FlightGroupType& getFlightGroupID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FlightGroupID.
           *
           * @return The value of the string data type identified by FlightGroupID.
           */
         virtual uci::type::CommMADL_FlightGroupType& getFlightGroupID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FlightGroupID to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFlightGroupID(const uci::type::CommMADL_FlightGroupType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FlightGroupID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFlightGroupID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FlightGroupID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFlightGroupID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FlightGroupID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FlightGroupID is emabled or not
           */
         virtual bool hasFlightGroupID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FlightGroupID
           *
           * @param type = uci::type::accessorType::commMADL_FlightGroupType This Accessor's accessor type
           */
         virtual void enableFlightGroupID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_FlightGroupType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FlightGroupID */
         virtual void clearFlightGroupID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetworkID.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetworkID.
           */
         virtual const uci::type::CommMADL_NetworkType& getNetworkID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetworkID.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetworkID.
           */
         virtual uci::type::CommMADL_NetworkType& getNetworkID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NetworkID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NetworkID
           */
         virtual void setNetworkID(const uci::type::CommMADL_NetworkType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetworkLayout.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetworkLayout.
           */
         virtual const uci::type::CommMADL_NetworkLayoutType& getNetworkLayout() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetworkLayout.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetworkLayout.
           */
         virtual uci::type::CommMADL_NetworkLayoutType& getNetworkLayout()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NetworkLayout to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NetworkLayout
           */
         virtual void setNetworkLayout(const uci::type::CommMADL_NetworkLayoutType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MADL_DayOfYear.
           *
           * @return The value of the simple primitive data type identified by MADL_DayOfYear.
           */
         virtual uci::type::CommMADL_ControlType::MADL_DayOfYearValue getMADL_DayOfYear() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MADL_DayOfYear.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMADL_DayOfYear(uci::type::CommMADL_ControlType::MADL_DayOfYearValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MADL_DayOfYear exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MADL_DayOfYear is emabled or not
           */
         virtual bool hasMADL_DayOfYear() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MADL_DayOfYear
           *
           * @param type = uci::type::commMADL_ControlType::accessorType::mADL_DayOfYear This Accessor's accessor type
           */
         virtual void enableMADL_DayOfYear(uci::base::accessorType::AccessorType type = uci::type::commMADL_ControlType::accessorType::mADL_DayOfYear)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MADL_DayOfYear */
         virtual void clearMADL_DayOfYear()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceTxExtent.
           *
           * @return The value of the simple primitive data type identified by VoiceTxExtent.
           */
         virtual xs::Boolean getVoiceTxExtent() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceTxExtent.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceTxExtent(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceTxExtent exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceTxExtent is emabled or not
           */
         virtual bool hasVoiceTxExtent() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceTxExtent
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableVoiceTxExtent(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceTxExtent */
         virtual void clearVoiceTxExtent()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkCommandedState.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkCommandedState.
           */
         virtual const uci::type::CommMADL_LinkCommandListType& getLinkCommandedState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkCommandedState.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkCommandedState.
           */
         virtual uci::type::CommMADL_LinkCommandListType& getLinkCommandedState()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LinkCommandedState to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LinkCommandedState
           */
         virtual void setLinkCommandedState(const uci::type::CommMADL_LinkCommandListType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LinkCommandedState exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LinkCommandedState is emabled or not
           */
         virtual bool hasLinkCommandedState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LinkCommandedState
           *
           * @param type = uci::type::accessorType::commMADL_LinkCommandListType This Accessor's accessor type
           */
         virtual void enableLinkCommandedState(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_LinkCommandListType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LinkCommandedState */
         virtual void clearLinkCommandedState()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetChangeRequest.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetChangeRequest.
           */
         virtual const uci::type::CommMADL_NetChangeRequestType& getNetChangeRequest() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetChangeRequest.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetChangeRequest.
           */
         virtual uci::type::CommMADL_NetChangeRequestType& getNetChangeRequest()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NetChangeRequest to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NetChangeRequest
           */
         virtual void setNetChangeRequest(const uci::type::CommMADL_NetChangeRequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NetChangeRequest exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NetChangeRequest is emabled or not
           */
         virtual bool hasNetChangeRequest() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NetChangeRequest
           *
           * @param type = uci::type::accessorType::commMADL_NetChangeRequestType This Accessor's accessor type
           */
         virtual void enableNetChangeRequest(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_NetChangeRequestType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NetChangeRequest */
         virtual void clearNetChangeRequest()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_ControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_ControlType to copy from
           */
         CommMADL_ControlType(const CommMADL_ControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_ControlType to the contents of the CommMADL_ControlType
           * on the right hand side (rhs) of the assignment operator.CommMADL_ControlType [only available to derived classes]
           *
           * @param rhs The CommMADL_ControlType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CommMADL_ControlType
           * @return A constant reference to this CommMADL_ControlType.
           */
         const CommMADL_ControlType& operator=(const CommMADL_ControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_ControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_ControlType_h

