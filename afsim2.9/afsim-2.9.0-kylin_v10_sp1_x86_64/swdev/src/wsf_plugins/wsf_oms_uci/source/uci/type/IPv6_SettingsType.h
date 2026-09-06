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
#ifndef Uci__Type__IPv6_SettingsType_h
#define Uci__Type__IPv6_SettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the IPv6_SettingsType sequence accessor class */
      class IPv6_SettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~IPv6_SettingsType()
         { }

         /** Returns this accessor's type constant, i.e. IPv6_SettingsType
           *
           * @return This accessor's type constant, i.e. IPv6_SettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::iPv6_SettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IPv6_SettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StaticIP_Address.
           *
           * @return The acecssor that provides access to the complex content that is identified by StaticIP_Address.
           */
         virtual const uci::type::ForeignKeyType& getStaticIP_Address() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StaticIP_Address.
           *
           * @return The acecssor that provides access to the complex content that is identified by StaticIP_Address.
           */
         virtual uci::type::ForeignKeyType& getStaticIP_Address()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the StaticIP_Address to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by StaticIP_Address
           */
         virtual void setStaticIP_Address(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StaticIP_Address exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StaticIP_Address is emabled or not
           */
         virtual bool hasStaticIP_Address() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StaticIP_Address
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableStaticIP_Address(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StaticIP_Address */
         virtual void clearStaticIP_Address()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PreferredDNS_Server.
           *
           * @return The acecssor that provides access to the complex content that is identified by PreferredDNS_Server.
           */
         virtual const uci::type::ForeignKeyType& getPreferredDNS_Server() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PreferredDNS_Server.
           *
           * @return The acecssor that provides access to the complex content that is identified by PreferredDNS_Server.
           */
         virtual uci::type::ForeignKeyType& getPreferredDNS_Server()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PreferredDNS_Server to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PreferredDNS_Server
           */
         virtual void setPreferredDNS_Server(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PreferredDNS_Server exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PreferredDNS_Server is emabled or not
           */
         virtual bool hasPreferredDNS_Server() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PreferredDNS_Server
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enablePreferredDNS_Server(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PreferredDNS_Server */
         virtual void clearPreferredDNS_Server()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PortNumber.
           *
           * @return The value of the simple primitive data type identified by PortNumber.
           */
         virtual xs::Int getPortNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PortNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPortNumber(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PortNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PortNumber is emabled or not
           */
         virtual bool hasPortNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PortNumber
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enablePortNumber(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PortNumber */
         virtual void clearPortNumber()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IPv6_SettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IPv6_SettingsType to copy from
           */
         IPv6_SettingsType(const IPv6_SettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IPv6_SettingsType to the contents of the IPv6_SettingsType on the
           * right hand side (rhs) of the assignment operator.IPv6_SettingsType [only available to derived classes]
           *
           * @param rhs The IPv6_SettingsType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::IPv6_SettingsType
           * @return A constant reference to this IPv6_SettingsType.
           */
         const IPv6_SettingsType& operator=(const IPv6_SettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IPv6_SettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IPv6_SettingsType_h

