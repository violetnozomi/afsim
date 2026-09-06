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
#ifndef Uci__Type__SAR_CapabilityStatusType_h
#define Uci__Type__SAR_CapabilityStatusType_h 1

#if !defined(Uci__Type__CapabilityStatusType_h)
# include "uci/type/CapabilityStatusType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SupportedResolutionAvailabilityType_h)
# include "uci/type/SupportedResolutionAvailabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_CapabilityStatusType sequence accessor class */
      class SAR_CapabilityStatusType : public virtual uci::type::CapabilityStatusType {
      public:

         /** The destructor */
         virtual ~SAR_CapabilityStatusType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_CapabilityStatusType
           *
           * @return This accessor's type constant, i.e. SAR_CapabilityStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_CapabilityStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_CapabilityStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the availability of a supported resolution by ID [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SupportedResolutionAvailabilityType, uci::type::accessorType::supportedResolutionAvailabilityType> SupportedResolutionAvailability;

         /** Returns the bounded list that is identified by the SupportedResolutionAvailability.
           *
           * @return The bounded list identified by SupportedResolutionAvailability.
           */
         virtual const uci::type::SAR_CapabilityStatusType::SupportedResolutionAvailability& getSupportedResolutionAvailability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportedResolutionAvailability.
           *
           * @return The bounded list identified by SupportedResolutionAvailability.
           */
         virtual uci::type::SAR_CapabilityStatusType::SupportedResolutionAvailability& getSupportedResolutionAvailability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportedResolutionAvailability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportedResolutionAvailability(const uci::type::SAR_CapabilityStatusType::SupportedResolutionAvailability& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_CapabilityStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_CapabilityStatusType to copy from
           */
         SAR_CapabilityStatusType(const SAR_CapabilityStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_CapabilityStatusType to the contents of the
           * SAR_CapabilityStatusType on the right hand side (rhs) of the assignment operator.SAR_CapabilityStatusType [only
           * available to derived classes]
           *
           * @param rhs The SAR_CapabilityStatusType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SAR_CapabilityStatusType
           * @return A constant reference to this SAR_CapabilityStatusType.
           */
         const SAR_CapabilityStatusType& operator=(const SAR_CapabilityStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_CapabilityStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_CapabilityStatusType_h

