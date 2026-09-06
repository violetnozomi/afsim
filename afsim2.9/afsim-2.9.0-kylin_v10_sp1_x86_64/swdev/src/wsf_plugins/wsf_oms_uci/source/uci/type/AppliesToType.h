// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AppliesToType_h
#define Uci__Type__AppliesToType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AppliesToType sequence accessor class */
      class AppliesToType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AppliesToType()
         { }

         /** Returns this accessor's type constant, i.e. AppliesToType
           *
           * @return This accessor's type constant, i.e. AppliesToType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::appliesToType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AppliesToType& accessor)
            throw(uci::base::UCIException) = 0;


         /** If specified, indicates the Systems that this request should apply to. If unspecified, then the request is applicable
           * to all Systems. This element is logically ANDed with its siblings. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** If specified, indicates the Subsystems that this request should apply to. If unspecified, then the request is
           * applicable to all Subsystems. This element is logically ANDed with its siblings. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemID_Type, uci::type::accessorType::subsystemID_Type> SubsystemID;

         /** If specified, indicates the Services that this request should apply to. If unspecified, then the request is
           * applicable to all Services. This element is logically ANDed with its siblings. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ServiceID;

         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::AppliesToType::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::AppliesToType::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::AppliesToType::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual const uci::type::AppliesToType::SubsystemID& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual uci::type::AppliesToType::SubsystemID& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubsystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubsystemID(const uci::type::AppliesToType::SubsystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual const uci::type::AppliesToType::ServiceID& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual uci::type::AppliesToType::ServiceID& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setServiceID(const uci::type::AppliesToType::ServiceID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AppliesToType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AppliesToType to copy from
           */
         AppliesToType(const AppliesToType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AppliesToType to the contents of the AppliesToType on the right
           * hand side (rhs) of the assignment operator.AppliesToType [only available to derived classes]
           *
           * @param rhs The AppliesToType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::AppliesToType
           * @return A constant reference to this AppliesToType.
           */
         const AppliesToType& operator=(const AppliesToType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AppliesToType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AppliesToType_h

