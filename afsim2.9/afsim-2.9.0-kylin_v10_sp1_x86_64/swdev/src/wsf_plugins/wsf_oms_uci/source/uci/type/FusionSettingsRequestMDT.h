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
#ifndef Uci__Type__FusionSettingsRequestMDT_h
#define Uci__Type__FusionSettingsRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FusionSettingsRequestID_Type_h)
# include "uci/type/FusionSettingsRequestID_Type.h"
#endif

#if !defined(Uci__Type__FusionSettingsRequestType_h)
# include "uci/type/FusionSettingsRequestType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FusionSettingsRequestMDT sequence accessor class */
      class FusionSettingsRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FusionSettingsRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. FusionSettingsRequestMDT
           *
           * @return This accessor's type constant, i.e. FusionSettingsRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fusionSettingsRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FusionSettingsRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** One or more Systems to which the request applies. If omitted, the request applies to all Systems. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** One or more Services to which the request applies. If omitted, the request applies to all Services (or all services
           * within the system specified by the sibling SystemID element). [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ServiceID;

         /** Returns the accessor that provides access to the complex content that is identified by the FusionSettingsRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FusionSettingsRequestID.
           */
         virtual const uci::type::FusionSettingsRequestID_Type& getFusionSettingsRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FusionSettingsRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FusionSettingsRequestID.
           */
         virtual uci::type::FusionSettingsRequestID_Type& getFusionSettingsRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FusionSettingsRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FusionSettingsRequestID
           */
         virtual void setFusionSettingsRequestID(const uci::type::FusionSettingsRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestType.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestType.
           */
         virtual const uci::type::FusionSettingsRequestType& getRequestType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestType.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestType.
           */
         virtual uci::type::FusionSettingsRequestType& getRequestType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequestType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequestType
           */
         virtual void setRequestType(const uci::type::FusionSettingsRequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::FusionSettingsRequestMDT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::FusionSettingsRequestMDT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::FusionSettingsRequestMDT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual const uci::type::FusionSettingsRequestMDT::ServiceID& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual uci::type::FusionSettingsRequestMDT::ServiceID& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setServiceID(const uci::type::FusionSettingsRequestMDT::ServiceID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FusionSettingsRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FusionSettingsRequestMDT to copy from
           */
         FusionSettingsRequestMDT(const FusionSettingsRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FusionSettingsRequestMDT to the contents of the
           * FusionSettingsRequestMDT on the right hand side (rhs) of the assignment operator.FusionSettingsRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The FusionSettingsRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::FusionSettingsRequestMDT
           * @return A constant reference to this FusionSettingsRequestMDT.
           */
         const FusionSettingsRequestMDT& operator=(const FusionSettingsRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FusionSettingsRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FusionSettingsRequestMDT_h

