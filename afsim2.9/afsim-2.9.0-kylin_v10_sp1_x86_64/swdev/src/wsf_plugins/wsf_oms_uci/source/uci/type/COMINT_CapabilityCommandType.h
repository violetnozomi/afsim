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
#ifndef Uci__Type__COMINT_CapabilityCommandType_h
#define Uci__Type__COMINT_CapabilityCommandType_h 1

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__COMINT_SubCapabilityEnum_h)
# include "uci/type/COMINT_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__COMINT_SubcapabilityChoiceType_h)
# include "uci/type/COMINT_SubcapabilityChoiceType.h"
#endif

#if !defined(Uci__Type__ProductOutputCommandBasicType_h)
# include "uci/type/ProductOutputCommandBasicType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_CapabilityCommandType sequence accessor class */
      class COMINT_CapabilityCommandType : public virtual uci::type::CapabilityCommandBaseType {
      public:

         /** The destructor */
         virtual ~COMINT_CapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_CapabilityCommandType
           *
           * @return This accessor's type constant, i.e. COMINT_CapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_CapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_CapabilityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a specific SubCapability being invoked by the command. If omitted, all SubCapabilities of the sibling
           * CapabilityID are to be invoked. If specified, only the specified SubCapabilities specified are required in order to
           * satisfy the command. Only SubCapabilities of the Capability indicated by the sibling CapabilityID should be
           * specified. [Minimum occurrences: 0] [Maximum occurrences: 16]
           */
         typedef uci::base::BoundedList<uci::type::COMINT_SubCapabilityEnum, uci::type::accessorType::cOMINT_SubCapabilityEnum> SubCapability;

         /** Indicates Command details for a Subcapability of the Capability indicated by the sibling CapabilityID. One instance
           * of this element for each Subcapability category (acquisition, ID, geolocation, measurement or pulse data collection)
           * of that Capability is required. If multiple instances are given, each must be of a different category in the
           * descendant choice statement. [Maximum occurrences: 5]
           */
         typedef uci::base::BoundedList<uci::type::COMINT_SubcapabilityChoiceType, uci::type::accessorType::cOMINT_SubcapabilityChoiceType> SubcapabilityDetails;

         /** Indicates a required output Product of the Command. Only Products consistent with the Subsystem and supported by the
           * commanded Capability should be requested. If omitted, no output Product should be produced. See ProductTypeEnum for
           * further details. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCommandBasicType, uci::type::accessorType::productOutputCommandBasicType> ProductOutput;

         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual const uci::type::COMINT_CapabilityCommandType::SubCapability& getSubCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapability.
           *
           * @return The bounded list identified by SubCapability.
           */
         virtual uci::type::COMINT_CapabilityCommandType::SubCapability& getSubCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapability(const uci::type::COMINT_CapabilityCommandType::SubCapability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubcapabilityDetails.
           *
           * @return The bounded list identified by SubcapabilityDetails.
           */
         virtual const uci::type::COMINT_CapabilityCommandType::SubcapabilityDetails& getSubcapabilityDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubcapabilityDetails.
           *
           * @return The bounded list identified by SubcapabilityDetails.
           */
         virtual uci::type::COMINT_CapabilityCommandType::SubcapabilityDetails& getSubcapabilityDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubcapabilityDetails.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubcapabilityDetails(const uci::type::COMINT_CapabilityCommandType::SubcapabilityDetails& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual const uci::type::COMINT_CapabilityCommandType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::COMINT_CapabilityCommandType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::COMINT_CapabilityCommandType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_CapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_CapabilityCommandType to copy from
           */
         COMINT_CapabilityCommandType(const COMINT_CapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_CapabilityCommandType to the contents of the
           * COMINT_CapabilityCommandType on the right hand side (rhs) of the assignment operator.COMINT_CapabilityCommandType
           * [only available to derived classes]
           *
           * @param rhs The COMINT_CapabilityCommandType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::COMINT_CapabilityCommandType
           * @return A constant reference to this COMINT_CapabilityCommandType.
           */
         const COMINT_CapabilityCommandType& operator=(const COMINT_CapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_CapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_CapabilityCommandType_h

