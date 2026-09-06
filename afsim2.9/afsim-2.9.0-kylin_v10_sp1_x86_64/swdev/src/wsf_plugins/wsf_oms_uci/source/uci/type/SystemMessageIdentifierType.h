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
#ifndef Uci__Type__SystemMessageIdentifierType_h
#define Uci__Type__SystemMessageIdentifierType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Type__NetworkLinkID_Type_h)
# include "uci/type/NetworkLinkID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Identifies a system and service as an object in a message. */
      class SystemMessageIdentifierType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SystemMessageIdentifierType()
         { }

         /** Returns this accessor's type constant, i.e. SystemMessageIdentifierType
           *
           * @return This accessor's type constant, i.e. SystemMessageIdentifierType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemMessageIdentifierType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemMessageIdentifierType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual const uci::type::ServiceID_Type& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual uci::type::ServiceID_Type& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceID
           */
         virtual void setServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ServiceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ServiceID is emabled or not
           */
         virtual bool hasServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ServiceID
           *
           * @param type = uci::type::accessorType::serviceID_Type This Accessor's accessor type
           */
         virtual void enableServiceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::serviceID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ServiceID */
         virtual void clearServiceID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual const uci::type::ActivityID_Type& getActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual uci::type::ActivityID_Type& getActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityID
           */
         virtual void setActivityID(const uci::type::ActivityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActivityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActivityID is emabled or not
           */
         virtual bool hasActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActivityID
           *
           * @param type = uci::type::accessorType::activityID_Type This Accessor's accessor type
           */
         virtual void enableActivityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::activityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActivityID */
         virtual void clearActivityID()
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


         /** Returns whether the Element that is identified by NetworkLinkID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NetworkLinkID is emabled or not
           */
         virtual bool hasNetworkLinkID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NetworkLinkID
           *
           * @param type = uci::type::accessorType::networkLinkID_Type This Accessor's accessor type
           */
         virtual void enableNetworkLinkID(uci::base::accessorType::AccessorType type = uci::type::accessorType::networkLinkID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NetworkLinkID */
         virtual void clearNetworkLinkID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemMessageIdentifierType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemMessageIdentifierType to copy from
           */
         SystemMessageIdentifierType(const SystemMessageIdentifierType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemMessageIdentifierType to the contents of the
           * SystemMessageIdentifierType on the right hand side (rhs) of the assignment operator.SystemMessageIdentifierType [only
           * available to derived classes]
           *
           * @param rhs The SystemMessageIdentifierType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SystemMessageIdentifierType
           * @return A constant reference to this SystemMessageIdentifierType.
           */
         const SystemMessageIdentifierType& operator=(const SystemMessageIdentifierType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemMessageIdentifierType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemMessageIdentifierType_h

