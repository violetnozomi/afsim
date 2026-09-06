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
#ifndef Uci__Type__ContactDetailsType_h
#define Uci__Type__ContactDetailsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OperatorNameType_h)
# include "uci/type/OperatorNameType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OperatorPhoneNumberType_h)
# include "uci/type/OperatorPhoneNumberType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Provides operator contact information. */
      class ContactDetailsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ContactDetailsType()
         { }

         /** Returns this accessor's type constant, i.e. ContactDetailsType
           *
           * @return This accessor's type constant, i.e. ContactDetailsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::contactDetailsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ContactDetailsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Provides for up to a 20-digit US phone number. The operator may have several phone numbers including extensions and
           * valid phone numbers can include dots, hyphens and/or spaces. Example formats include: 555-555-5555x5555, 5555555555,
           * 555 555 5555x555555, 555.555.5555. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::OperatorPhoneNumberType, uci::type::accessorType::operatorPhoneNumberType> PhoneNumber;

         /** Returns the accessor that provides access to the complex content that is identified by the OperatorName.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorName.
           */
         virtual const uci::type::OperatorNameType& getOperatorName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorName.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorName.
           */
         virtual uci::type::OperatorNameType& getOperatorName()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperatorName to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperatorName
           */
         virtual void setOperatorName(const uci::type::OperatorNameType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PhoneNumber.
           *
           * @return The bounded list identified by PhoneNumber.
           */
         virtual const uci::type::ContactDetailsType::PhoneNumber& getPhoneNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PhoneNumber.
           *
           * @return The bounded list identified by PhoneNumber.
           */
         virtual uci::type::ContactDetailsType::PhoneNumber& getPhoneNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PhoneNumber.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPhoneNumber(const uci::type::ContactDetailsType::PhoneNumber& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ContactDetailsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ContactDetailsType to copy from
           */
         ContactDetailsType(const ContactDetailsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ContactDetailsType to the contents of the ContactDetailsType on
           * the right hand side (rhs) of the assignment operator.ContactDetailsType [only available to derived classes]
           *
           * @param rhs The ContactDetailsType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ContactDetailsType
           * @return A constant reference to this ContactDetailsType.
           */
         const ContactDetailsType& operator=(const ContactDetailsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ContactDetailsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ContactDetailsType_h

