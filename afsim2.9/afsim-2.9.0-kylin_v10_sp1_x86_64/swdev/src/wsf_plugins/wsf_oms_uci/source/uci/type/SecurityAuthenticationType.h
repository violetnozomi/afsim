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
#ifndef Uci__Type__SecurityAuthenticationType_h
#define Uci__Type__SecurityAuthenticationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SecurityAuthenticationEnum_h)
# include "uci/type/SecurityAuthenticationEnum.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__MDF_ID_Type_h)
# include "uci/type/MDF_ID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SecurityAuthenticationType sequence accessor class */
      class SecurityAuthenticationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SecurityAuthenticationType()
         { }

         /** Returns this accessor's type constant, i.e. SecurityAuthenticationType
           *
           * @return This accessor's type constant, i.e. SecurityAuthenticationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::securityAuthenticationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SecurityAuthenticationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Category.
           *
           * @return The value of the enumeration identified by Category.
           */
         virtual const uci::type::SecurityAuthenticationEnum& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Category.
           *
           * @return The value of the enumeration identified by Category.
           */
         virtual uci::type::SecurityAuthenticationEnum& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Category.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCategory(const uci::type::SecurityAuthenticationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Category.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCategory(uci::type::SecurityAuthenticationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubsystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubsystemID is emabled or not
           */
         virtual bool hasSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubsystemID
           *
           * @param type = uci::type::accessorType::subsystemID_Type This Accessor's accessor type
           */
         virtual void enableSubsystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::subsystemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubsystemID */
         virtual void clearSubsystemID()
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


         /** Returns the accessor that provides access to the complex content that is identified by the MDF_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MDF_ID.
           */
         virtual const uci::type::MDF_ID_Type& getMDF_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MDF_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MDF_ID.
           */
         virtual uci::type::MDF_ID_Type& getMDF_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MDF_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MDF_ID
           */
         virtual void setMDF_ID(const uci::type::MDF_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MDF_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MDF_ID is emabled or not
           */
         virtual bool hasMDF_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MDF_ID
           *
           * @param type = uci::type::accessorType::mDF_ID_Type This Accessor's accessor type
           */
         virtual void enableMDF_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::mDF_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MDF_ID */
         virtual void clearMDF_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Details.
           *
           * @return The value of the string data type identified by Details.
           */
         virtual const uci::type::VisibleString1024Type& getDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Details.
           *
           * @return The value of the string data type identified by Details.
           */
         virtual uci::type::VisibleString1024Type& getDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Details to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDetails(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Details to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDetails(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Details to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDetails(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Details exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Details is emabled or not
           */
         virtual bool hasDetails() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Details
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableDetails(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Details */
         virtual void clearDetails()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SecurityAuthenticationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SecurityAuthenticationType to copy from
           */
         SecurityAuthenticationType(const SecurityAuthenticationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SecurityAuthenticationType to the contents of the
           * SecurityAuthenticationType on the right hand side (rhs) of the assignment operator.SecurityAuthenticationType [only
           * available to derived classes]
           *
           * @param rhs The SecurityAuthenticationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SecurityAuthenticationType
           * @return A constant reference to this SecurityAuthenticationType.
           */
         const SecurityAuthenticationType& operator=(const SecurityAuthenticationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SecurityAuthenticationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SecurityAuthenticationType_h

