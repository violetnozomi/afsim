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
#ifndef Uci__Type__SystemManagementRequestMDT_h
#define Uci__Type__SystemManagementRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemManagementRequestID_Type_h)
# include "uci/type/SystemManagementRequestID_Type.h"
#endif

#if !defined(Uci__Type__EntityManagementRequestAuthorityEnum_h)
# include "uci/type/EntityManagementRequestAuthorityEnum.h"
#endif

#if !defined(Uci__Type__RequestSourceEnum_h)
# include "uci/type/RequestSourceEnum.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemManagementRequestType_h)
# include "uci/type/SystemManagementRequestType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__SystemMessageIdentifierType_h)
# include "uci/type/SystemMessageIdentifierType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This message is used to request changes to a system’s status or configurable settings. */
      class SystemManagementRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SystemManagementRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. SystemManagementRequestMDT
           *
           * @return This accessor's type constant, i.e. SystemManagementRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemManagementRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemManagementRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the type of request to perform on a system. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemManagementRequestType, uci::type::accessorType::systemManagementRequestType> RequestType;

         /** Returns the accessor that provides access to the complex content that is identified by the SystemManagementRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemManagementRequestID.
           */
         virtual const uci::type::SystemManagementRequestID_Type& getSystemManagementRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemManagementRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemManagementRequestID.
           */
         virtual uci::type::SystemManagementRequestID_Type& getSystemManagementRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemManagementRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemManagementRequestID
           */
         virtual void setSystemManagementRequestID(const uci::type::SystemManagementRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AuthorityType.
           *
           * @return The value of the enumeration identified by AuthorityType.
           */
         virtual const uci::type::EntityManagementRequestAuthorityEnum& getAuthorityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AuthorityType.
           *
           * @return The value of the enumeration identified by AuthorityType.
           */
         virtual uci::type::EntityManagementRequestAuthorityEnum& getAuthorityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AuthorityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAuthorityType(const uci::type::EntityManagementRequestAuthorityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AuthorityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAuthorityType(uci::type::EntityManagementRequestAuthorityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield AuthorityTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasAuthorityType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getAuthorityType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::entityManagementRequestAuthorityEnum This Accessor's accessor type
           */
         virtual void enableAuthorityType(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityManagementRequestAuthorityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearAuthorityType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestSource.
           *
           * @return The value of the enumeration identified by RequestSource.
           */
         virtual const uci::type::RequestSourceEnum& getRequestSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestSource.
           *
           * @return The value of the enumeration identified by RequestSource.
           */
         virtual uci::type::RequestSourceEnum& getRequestSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestSource(const uci::type::RequestSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestSource(uci::type::RequestSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield RequestSourceis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasRequestSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getRequestSource will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::requestSourceEnum This Accessor's accessor type
           */
         virtual void enableRequestSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::requestSourceEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearRequestSource()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceSystemID.
           */
         virtual const uci::type::SystemID_Type& getReferenceSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReferenceSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReferenceSystemID.
           */
         virtual uci::type::SystemID_Type& getReferenceSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReferenceSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReferenceSystemID
           */
         virtual void setReferenceSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequestType.
           *
           * @return The bounded list identified by RequestType.
           */
         virtual const uci::type::SystemManagementRequestMDT::RequestType& getRequestType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RequestType.
           *
           * @return The bounded list identified by RequestType.
           */
         virtual uci::type::SystemManagementRequestMDT::RequestType& getRequestType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RequestType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRequestType(const uci::type::SystemManagementRequestMDT::RequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UpdateFromControllingUnit.
           *
           * @return The value of the simple primitive data type identified by UpdateFromControllingUnit.
           */
         virtual xs::Boolean getUpdateFromControllingUnit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UpdateFromControllingUnit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUpdateFromControllingUnit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UpdateFromControllingUnit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UpdateFromControllingUnit is emabled or not
           */
         virtual bool hasUpdateFromControllingUnit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UpdateFromControllingUnit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableUpdateFromControllingUnit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UpdateFromControllingUnit */
         virtual void clearUpdateFromControllingUnit()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual const uci::type::SystemMessageIdentifierType& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual uci::type::SystemMessageIdentifierType& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Source to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Source
           */
         virtual void setSource(const uci::type::SystemMessageIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Source exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Source is emabled or not
           */
         virtual bool hasSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Source
           *
           * @param type = uci::type::accessorType::systemMessageIdentifierType This Accessor's accessor type
           */
         virtual void enableSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemMessageIdentifierType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Source */
         virtual void clearSource()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemManagementRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemManagementRequestMDT to copy from
           */
         SystemManagementRequestMDT(const SystemManagementRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemManagementRequestMDT to the contents of the
           * SystemManagementRequestMDT on the right hand side (rhs) of the assignment operator.SystemManagementRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The SystemManagementRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SystemManagementRequestMDT
           * @return A constant reference to this SystemManagementRequestMDT.
           */
         const SystemManagementRequestMDT& operator=(const SystemManagementRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemManagementRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemManagementRequestMDT_h

