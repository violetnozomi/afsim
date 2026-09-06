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
#ifndef Uci__Type__EA_ActivityType_h
#define Uci__Type__EA_ActivityType_h 1

#if !defined(Uci__Type__SharedRF_ApertureActivityBaseType_h)
# include "uci/type/SharedRF_ApertureActivityBaseType.h"
#endif

#if !defined(Uci__Type__EA_DetailsType_h)
# include "uci/type/EA_DetailsType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ConsentEnum_h)
# include "uci/type/ConsentEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_ActivityType sequence accessor class */
      class EA_ActivityType : public virtual uci::type::SharedRF_ApertureActivityBaseType {
      public:

         /** The destructor */
         virtual ~EA_ActivityType()
         { }

         /** Returns this accessor's type constant, i.e. EA_ActivityType
           *
           * @return This accessor's type constant, i.e. EA_ActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_ActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_ActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of a system to be protected by the electronic attack. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ProtectedEntityID;

         /** Returns the accessor that provides access to the complex content that is identified by the EA_Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by EA_Details.
           */
         virtual const uci::type::EA_DetailsType& getEA_Details() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EA_Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by EA_Details.
           */
         virtual uci::type::EA_DetailsType& getEA_Details()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EA_Details to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EA_Details
           */
         virtual void setEA_Details(const uci::type::EA_DetailsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProtectedEntityID.
           *
           * @return The bounded list identified by ProtectedEntityID.
           */
         virtual const uci::type::EA_ActivityType::ProtectedEntityID& getProtectedEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProtectedEntityID.
           *
           * @return The bounded list identified by ProtectedEntityID.
           */
         virtual uci::type::EA_ActivityType::ProtectedEntityID& getProtectedEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProtectedEntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProtectedEntityID(const uci::type::EA_ActivityType::ProtectedEntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ConsentRequired.
           *
           * @return The value of the simple primitive data type identified by ConsentRequired.
           */
         virtual xs::Boolean getConsentRequired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ConsentRequired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setConsentRequired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConsentRequired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConsentRequired is emabled or not
           */
         virtual bool hasConsentRequired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConsentRequired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableConsentRequired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConsentRequired */
         virtual void clearConsentRequired()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConsentState.
           *
           * @return The value of the enumeration identified by ConsentState.
           */
         virtual const uci::type::ConsentEnum& getConsentState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConsentState.
           *
           * @return The value of the enumeration identified by ConsentState.
           */
         virtual uci::type::ConsentEnum& getConsentState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConsentState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConsentState(const uci::type::ConsentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConsentState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConsentState(uci::type::ConsentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ConsentStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasConsentState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getConsentState will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::consentEnum This Accessor's accessor type
           */
         virtual void enableConsentState(uci::base::accessorType::AccessorType type = uci::type::accessorType::consentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearConsentState()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_ActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_ActivityType to copy from
           */
         EA_ActivityType(const EA_ActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_ActivityType to the contents of the EA_ActivityType on the
           * right hand side (rhs) of the assignment operator.EA_ActivityType [only available to derived classes]
           *
           * @param rhs The EA_ActivityType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::EA_ActivityType
           * @return A constant reference to this EA_ActivityType.
           */
         const EA_ActivityType& operator=(const EA_ActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_ActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_ActivityType_h

