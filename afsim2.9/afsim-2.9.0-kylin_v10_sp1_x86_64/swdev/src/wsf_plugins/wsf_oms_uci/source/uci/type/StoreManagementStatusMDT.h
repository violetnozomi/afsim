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
#ifndef Uci__Type__StoreManagementStatusMDT_h
#define Uci__Type__StoreManagementStatusMDT_h 1

#if !defined(Uci__Type__SupportCapabilityStatusBaseType_h)
# include "uci/type/SupportCapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__StoreStationType_h)
# include "uci/type/StoreStationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This types describes the stores management supporting Capability of a System. A stores management supporting
        * Capability is associated with other primary Capabilities that it controls such as Strike Capabilities.
        */
      class StoreManagementStatusMDT : public virtual uci::type::SupportCapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~StoreManagementStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. StoreManagementStatusMDT
           *
           * @return This accessor's type constant, i.e. StoreManagementStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::storeManagementStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StoreManagementStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an individual store station or position under control of the Store Management supporting Capability. All
           * stations must be reported, even if a store is not currently installed; the number of instances of this element is the
           * maximum number of stores under control of the Store Management supporting Capability. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::StoreStationType, uci::type::accessorType::storeStationType> StoreStation;

         /** Returns the bounded list that is identified by the StoreStation.
           *
           * @return The bounded list identified by StoreStation.
           */
         virtual const uci::type::StoreManagementStatusMDT::StoreStation& getStoreStation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StoreStation.
           *
           * @return The bounded list identified by StoreStation.
           */
         virtual uci::type::StoreManagementStatusMDT::StoreStation& getStoreStation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the StoreStation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStoreStation(const uci::type::StoreManagementStatusMDT::StoreStation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StoreManagementStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StoreManagementStatusMDT to copy from
           */
         StoreManagementStatusMDT(const StoreManagementStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StoreManagementStatusMDT to the contents of the
           * StoreManagementStatusMDT on the right hand side (rhs) of the assignment operator.StoreManagementStatusMDT [only
           * available to derived classes]
           *
           * @param rhs The StoreManagementStatusMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::StoreManagementStatusMDT
           * @return A constant reference to this StoreManagementStatusMDT.
           */
         const StoreManagementStatusMDT& operator=(const StoreManagementStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StoreManagementStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StoreManagementStatusMDT_h

