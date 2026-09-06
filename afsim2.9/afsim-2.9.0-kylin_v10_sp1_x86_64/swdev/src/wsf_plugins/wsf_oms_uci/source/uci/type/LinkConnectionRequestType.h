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
#ifndef Uci__Type__LinkConnectionRequestType_h
#define Uci__Type__LinkConnectionRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommConnectionCommandEnum_h)
# include "uci/type/CommConnectionCommandEnum.h"
#endif

#if !defined(Uci__Type__CommControlType_h)
# include "uci/type/CommControlType.h"
#endif

#if !defined(Uci__Type__CommLinkPortMapType_h)
# include "uci/type/CommLinkPortMapType.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LinkConnectionRequestType sequence accessor class */
      class LinkConnectionRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LinkConnectionRequestType()
         { }

         /** Returns this accessor's type constant, i.e. LinkConnectionRequestType
           *
           * @return This accessor's type constant, i.e. LinkConnectionRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::linkConnectionRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LinkConnectionRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConnectCommand.
           *
           * @return The value of the enumeration identified by ConnectCommand.
           */
         virtual const uci::type::CommConnectionCommandEnum& getConnectCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConnectCommand.
           *
           * @return The value of the enumeration identified by ConnectCommand.
           */
         virtual uci::type::CommConnectionCommandEnum& getConnectCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConnectCommand.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConnectCommand(const uci::type::CommConnectionCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConnectCommand.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConnectCommand(uci::type::CommConnectionCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettingsChange.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettingsChange.
           */
         virtual const uci::type::CommControlType& getCommonSettingsChange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommonSettingsChange.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommonSettingsChange.
           */
         virtual uci::type::CommControlType& getCommonSettingsChange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommonSettingsChange to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommonSettingsChange
           */
         virtual void setCommonSettingsChange(const uci::type::CommControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommonSettingsChange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommonSettingsChange is emabled or not
           */
         virtual bool hasCommonSettingsChange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommonSettingsChange
           *
           * @param type = uci::type::accessorType::commControlType This Accessor's accessor type
           */
         virtual void enableCommonSettingsChange(uci::base::accessorType::AccessorType type = uci::type::accessorType::commControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommonSettingsChange */
         virtual void clearCommonSettingsChange()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkPortAssignments.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkPortAssignments.
           */
         virtual const uci::type::CommLinkPortMapType& getLinkPortAssignments() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LinkPortAssignments.
           *
           * @return The acecssor that provides access to the complex content that is identified by LinkPortAssignments.
           */
         virtual uci::type::CommLinkPortMapType& getLinkPortAssignments()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LinkPortAssignments to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LinkPortAssignments
           */
         virtual void setLinkPortAssignments(const uci::type::CommLinkPortMapType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LinkPortAssignments exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LinkPortAssignments is emabled or not
           */
         virtual bool hasLinkPortAssignments() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LinkPortAssignments
           *
           * @param type = uci::type::accessorType::commLinkPortMapType This Accessor's accessor type
           */
         virtual void enableLinkPortAssignments(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLinkPortMapType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LinkPortAssignments */
         virtual void clearLinkPortAssignments()
            throw(uci::base::UCIException) = 0;


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



      protected:

         /** The constructor [only available to derived classes]. */
         LinkConnectionRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LinkConnectionRequestType to copy from
           */
         LinkConnectionRequestType(const LinkConnectionRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LinkConnectionRequestType to the contents of the
           * LinkConnectionRequestType on the right hand side (rhs) of the assignment operator.LinkConnectionRequestType [only
           * available to derived classes]
           *
           * @param rhs The LinkConnectionRequestType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::LinkConnectionRequestType
           * @return A constant reference to this LinkConnectionRequestType.
           */
         const LinkConnectionRequestType& operator=(const LinkConnectionRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LinkConnectionRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LinkConnectionRequestType_h

