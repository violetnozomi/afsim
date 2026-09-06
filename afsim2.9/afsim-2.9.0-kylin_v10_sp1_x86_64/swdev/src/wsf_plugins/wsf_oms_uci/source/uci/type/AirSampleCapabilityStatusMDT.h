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
#ifndef Uci__Type__AirSampleCapabilityStatusMDT_h
#define Uci__Type__AirSampleCapabilityStatusMDT_h 1

#if !defined(Uci__Type__CapabilityStatusBaseType_h)
# include "uci/type/CapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityStatusType_h)
# include "uci/type/CapabilityStatusType.h"
#endif

#if !defined(Uci__Type__RF_ProfileType_h)
# include "uci/type/RF_ProfileType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AirSampleCapabilityStatusMDT sequence accessor class */
      class AirSampleCapabilityStatusMDT : public virtual uci::type::CapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~AirSampleCapabilityStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. AirSampleCapabilityStatusMDT
           *
           * @return This accessor's type constant, i.e. AirSampleCapabilityStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::airSampleCapabilityStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AirSampleCapabilityStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the high level availability/state of a Capability instance. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityStatusType, uci::type::accessorType::capabilityStatusType> CapabilityStatus;

         /** Indicates a RF Emission Profile currently applied to the Capability. If omitted, there are no current profiles and
           * the Capability is operating per its defaults and other constraints. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RF_ProfileType, uci::type::accessorType::rF_ProfileType> RF_Profile;

         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual const uci::type::AirSampleCapabilityStatusMDT::CapabilityStatus& getCapabilityStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual uci::type::AirSampleCapabilityStatusMDT::CapabilityStatus& getCapabilityStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityStatus(const uci::type::AirSampleCapabilityStatusMDT::CapabilityStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Profile.
           *
           * @return The bounded list identified by RF_Profile.
           */
         virtual const uci::type::AirSampleCapabilityStatusMDT::RF_Profile& getRF_Profile() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Profile.
           *
           * @return The bounded list identified by RF_Profile.
           */
         virtual uci::type::AirSampleCapabilityStatusMDT::RF_Profile& getRF_Profile()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_Profile.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_Profile(const uci::type::AirSampleCapabilityStatusMDT::RF_Profile& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AirSampleCapabilityStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AirSampleCapabilityStatusMDT to copy from
           */
         AirSampleCapabilityStatusMDT(const AirSampleCapabilityStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AirSampleCapabilityStatusMDT to the contents of the
           * AirSampleCapabilityStatusMDT on the right hand side (rhs) of the assignment operator.AirSampleCapabilityStatusMDT
           * [only available to derived classes]
           *
           * @param rhs The AirSampleCapabilityStatusMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::AirSampleCapabilityStatusMDT
           * @return A constant reference to this AirSampleCapabilityStatusMDT.
           */
         const AirSampleCapabilityStatusMDT& operator=(const AirSampleCapabilityStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AirSampleCapabilityStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AirSampleCapabilityStatusMDT_h

