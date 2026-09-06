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
#ifndef Uci__Type__RFDC_StatusMDT_h
#define Uci__Type__RFDC_StatusMDT_h 1

#if !defined(Uci__Type__SupportCapabilityStatusBaseType_h)
# include "uci/type/SupportCapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__PowerType_h)
# include "uci/type/PowerType.h"
#endif

#if !defined(Uci__Type__RF_DistributionType_h)
# include "uci/type/RF_DistributionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type represents the status and settings for an RFDC that is relevant to mission operations. */
      class RFDC_StatusMDT : public virtual uci::type::SupportCapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~RFDC_StatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. RFDC_StatusMDT
           *
           * @return This accessor's type constant, i.e. RFDC_StatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rFDC_StatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RFDC_StatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the ID of a Capability supported by the RFDC's SupportingCapability [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** RF Distribution (RFD) component feedback for all the RFD threads. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::RF_DistributionType, uci::type::accessorType::rF_DistributionType> RF_Distribution;

         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::RFDC_StatusMDT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::RFDC_StatusMDT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::RFDC_StatusMDT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PowerConsumption.
           *
           * @return The value of the simple primitive data type identified by PowerConsumption.
           */
         virtual uci::type::PowerTypeValue getPowerConsumption() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PowerConsumption.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPowerConsumption(uci::type::PowerTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PowerConsumption exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PowerConsumption is emabled or not
           */
         virtual bool hasPowerConsumption() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PowerConsumption
           *
           * @param type = uci::type::accessorType::powerType This Accessor's accessor type
           */
         virtual void enablePowerConsumption(uci::base::accessorType::AccessorType type = uci::type::accessorType::powerType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PowerConsumption */
         virtual void clearPowerConsumption()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Distribution.
           *
           * @return The bounded list identified by RF_Distribution.
           */
         virtual const uci::type::RFDC_StatusMDT::RF_Distribution& getRF_Distribution() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Distribution.
           *
           * @return The bounded list identified by RF_Distribution.
           */
         virtual uci::type::RFDC_StatusMDT::RF_Distribution& getRF_Distribution()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_Distribution.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_Distribution(const uci::type::RFDC_StatusMDT::RF_Distribution& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RFDC_StatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RFDC_StatusMDT to copy from
           */
         RFDC_StatusMDT(const RFDC_StatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RFDC_StatusMDT to the contents of the RFDC_StatusMDT on the right
           * hand side (rhs) of the assignment operator.RFDC_StatusMDT [only available to derived classes]
           *
           * @param rhs The RFDC_StatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::RFDC_StatusMDT
           * @return A constant reference to this RFDC_StatusMDT.
           */
         const RFDC_StatusMDT& operator=(const RFDC_StatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RFDC_StatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RFDC_StatusMDT_h

