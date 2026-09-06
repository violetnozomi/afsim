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
#ifndef Uci__Type__GatewayCapabilityStatusMDT_h
#define Uci__Type__GatewayCapabilityStatusMDT_h 1

#if !defined(Uci__Type__CapabilityStatusBaseType_h)
# include "uci/type/CapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__GatewayCapabilityStatusType_h)
# include "uci/type/GatewayCapabilityStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class GatewayCapabilityStatusMDT : public virtual uci::type::CapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~GatewayCapabilityStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. GatewayCapabilityStatusMDT
           *
           * @return This accessor's type constant, i.e. GatewayCapabilityStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayCapabilityStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayCapabilityStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the high level availability/state of a Capability instance. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::GatewayCapabilityStatusType, uci::type::accessorType::gatewayCapabilityStatusType> CapabilityStatus;

         /** Returns the accessor that provides access to the complex content that is identified by the GatewayServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by GatewayServiceID.
           */
         virtual const uci::type::ServiceID_Type& getGatewayServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GatewayServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by GatewayServiceID.
           */
         virtual uci::type::ServiceID_Type& getGatewayServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GatewayServiceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GatewayServiceID
           */
         virtual void setGatewayServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual const uci::type::GatewayCapabilityStatusMDT::CapabilityStatus& getCapabilityStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual uci::type::GatewayCapabilityStatusMDT::CapabilityStatus& getCapabilityStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityStatus(const uci::type::GatewayCapabilityStatusMDT::CapabilityStatus& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayCapabilityStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayCapabilityStatusMDT to copy from
           */
         GatewayCapabilityStatusMDT(const GatewayCapabilityStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayCapabilityStatusMDT to the contents of the
           * GatewayCapabilityStatusMDT on the right hand side (rhs) of the assignment operator.GatewayCapabilityStatusMDT [only
           * available to derived classes]
           *
           * @param rhs The GatewayCapabilityStatusMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::GatewayCapabilityStatusMDT
           * @return A constant reference to this GatewayCapabilityStatusMDT.
           */
         const GatewayCapabilityStatusMDT& operator=(const GatewayCapabilityStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayCapabilityStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayCapabilityStatusMDT_h

