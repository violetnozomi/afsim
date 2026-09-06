// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__RFDC_StatusQFT_h
#define Uci__Type__RFDC_StatusQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__RFD_ComponentIdentifierType_h)
# include "uci/type/RFD_ComponentIdentifierType.h"
#endif

#if !defined(Uci__Type__RF_ThreadIdentifierType_h)
# include "uci/type/RF_ThreadIdentifierType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RFDC_StatusQFT sequence accessor class */
      class RFDC_StatusQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~RFDC_StatusQFT()
         { }

         /** Returns this accessor's type constant, i.e. RFDC_StatusQFT
           *
           * @return This accessor's type constant, i.e. RFDC_StatusQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rFDC_StatusQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RFDC_StatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SubsystemID_Type, uci::type::accessorType::subsystemID_Type> SubsystemID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::RFD_ComponentIdentifierType, uci::type::accessorType::rFD_ComponentIdentifierType> RFD_ComponentIndex;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::RF_ThreadIdentifierType, uci::type::accessorType::rF_ThreadIdentifierType> RF_ThreadIndex;

         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual const uci::type::RFDC_StatusQFT::SubsystemID& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual uci::type::RFDC_StatusQFT::SubsystemID& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubsystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubsystemID(const uci::type::RFDC_StatusQFT::SubsystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::RFDC_StatusQFT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::RFDC_StatusQFT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::RFDC_StatusQFT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RFD_ComponentIndex.
           *
           * @return The bounded list identified by RFD_ComponentIndex.
           */
         virtual const uci::type::RFDC_StatusQFT::RFD_ComponentIndex& getRFD_ComponentIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RFD_ComponentIndex.
           *
           * @return The bounded list identified by RFD_ComponentIndex.
           */
         virtual uci::type::RFDC_StatusQFT::RFD_ComponentIndex& getRFD_ComponentIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RFD_ComponentIndex.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRFD_ComponentIndex(const uci::type::RFDC_StatusQFT::RFD_ComponentIndex& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_ThreadIndex.
           *
           * @return The bounded list identified by RF_ThreadIndex.
           */
         virtual const uci::type::RFDC_StatusQFT::RF_ThreadIndex& getRF_ThreadIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_ThreadIndex.
           *
           * @return The bounded list identified by RF_ThreadIndex.
           */
         virtual uci::type::RFDC_StatusQFT::RF_ThreadIndex& getRF_ThreadIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_ThreadIndex.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_ThreadIndex(const uci::type::RFDC_StatusQFT::RF_ThreadIndex& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RFDC_StatusQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RFDC_StatusQFT to copy from
           */
         RFDC_StatusQFT(const RFDC_StatusQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RFDC_StatusQFT to the contents of the RFDC_StatusQFT on the right
           * hand side (rhs) of the assignment operator.RFDC_StatusQFT [only available to derived classes]
           *
           * @param rhs The RFDC_StatusQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::RFDC_StatusQFT
           * @return A constant reference to this RFDC_StatusQFT.
           */
         const RFDC_StatusQFT& operator=(const RFDC_StatusQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RFDC_StatusQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RFDC_StatusQFT_h

