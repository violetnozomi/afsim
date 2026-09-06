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
#ifndef Uci__Type__CryptoStatusMDT_h
#define Uci__Type__CryptoStatusMDT_h 1

#if !defined(Uci__Type__SupportCapabilityStatusBaseType_h)
# include "uci/type/SupportCapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Type__CryptoSlotGroupType_h)
# include "uci/type/CryptoSlotGroupType.h"
#endif

#if !defined(Uci__Type__CryptoPortStatusType_h)
# include "uci/type/CryptoPortStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type represents the status and settings for a crypto key slot that is relevant to mission operations. */
      class CryptoStatusMDT : public virtual uci::type::SupportCapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~CryptoStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. CryptoStatusMDT
           *
           * @return This accessor's type constant, i.e. CryptoStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cryptoStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CryptoStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The ID of the Activity (Instance) of the communication capability which this is applicable to. If empty, means all.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ActivityID_Type, uci::type::accessorType::activityID_Type> SupportActivityID;

         /** Indicates the status of groups of crypto keys loaded in or used by the subsystem. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CryptoSlotGroupType, uci::type::accessorType::cryptoSlotGroupType> CryptoSlotGroup;

         /** Crypto Ports provide a construct for command and control and status reporting of crypto components. Crypto components
           * are used to encrypt and/or decrypt payload data. Typically a crypto device provides RED/BLACK separation to a
           * subsystem and a port resides on either the RED or BLACK side of the subsystem. If the port is on the RED side, the
           * port will encrypt data and output the encrypted payload to the corresponding crypto port on the BLACK side of the
           * crypto component. If the port is on the BLACK side, the port will decrypt data and output the decrypted payload to
           * the corresponding crypto port on the RED side of the crypto component. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CryptoPortStatusType, uci::type::accessorType::cryptoPortStatusType> CryptoPort;

         /** Returns the bounded list that is identified by the SupportActivityID.
           *
           * @return The bounded list identified by SupportActivityID.
           */
         virtual const uci::type::CryptoStatusMDT::SupportActivityID& getSupportActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportActivityID.
           *
           * @return The bounded list identified by SupportActivityID.
           */
         virtual uci::type::CryptoStatusMDT::SupportActivityID& getSupportActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportActivityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportActivityID(const uci::type::CryptoStatusMDT::SupportActivityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoSlotGroup.
           *
           * @return The bounded list identified by CryptoSlotGroup.
           */
         virtual const uci::type::CryptoStatusMDT::CryptoSlotGroup& getCryptoSlotGroup() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoSlotGroup.
           *
           * @return The bounded list identified by CryptoSlotGroup.
           */
         virtual uci::type::CryptoStatusMDT::CryptoSlotGroup& getCryptoSlotGroup()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CryptoSlotGroup.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCryptoSlotGroup(const uci::type::CryptoStatusMDT::CryptoSlotGroup& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoPort.
           *
           * @return The bounded list identified by CryptoPort.
           */
         virtual const uci::type::CryptoStatusMDT::CryptoPort& getCryptoPort() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CryptoPort.
           *
           * @return The bounded list identified by CryptoPort.
           */
         virtual uci::type::CryptoStatusMDT::CryptoPort& getCryptoPort()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CryptoPort.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCryptoPort(const uci::type::CryptoStatusMDT::CryptoPort& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CryptoStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CryptoStatusMDT to copy from
           */
         CryptoStatusMDT(const CryptoStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CryptoStatusMDT to the contents of the CryptoStatusMDT on the
           * right hand side (rhs) of the assignment operator.CryptoStatusMDT [only available to derived classes]
           *
           * @param rhs The CryptoStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::CryptoStatusMDT
           * @return A constant reference to this CryptoStatusMDT.
           */
         const CryptoStatusMDT& operator=(const CryptoStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CryptoStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CryptoStatusMDT_h

