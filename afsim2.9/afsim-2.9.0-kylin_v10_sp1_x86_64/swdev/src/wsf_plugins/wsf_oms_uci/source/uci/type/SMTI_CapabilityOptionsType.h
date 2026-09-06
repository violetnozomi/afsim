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
#ifndef Uci__Type__SMTI_CapabilityOptionsType_h
#define Uci__Type__SMTI_CapabilityOptionsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ElectronicProtectionOptionsSupportedType_h)
# include "uci/type/ElectronicProtectionOptionsSupportedType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SMTI_CapabilityOptionsType sequence accessor class */
      class SMTI_CapabilityOptionsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SMTI_CapabilityOptionsType()
         { }

         /** Returns this accessor's type constant, i.e. SMTI_CapabilityOptionsType
           *
           * @return This accessor's type constant, i.e. SMTI_CapabilityOptionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sMTI_CapabilityOptionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SMTI_CapabilityOptionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the list of EP capabilities that the current HW / SW allows an external actor to control for this
           * capability. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ElectronicProtectionOptionsSupportedType, uci::type::accessorType::electronicProtectionOptionsSupportedType> ElectronicProtectionOptionsSupported;

         /** Returns the value of the SimplePrimitive data type that is identified by the InterruptOtherActivities.
           *
           * @return The value of the simple primitive data type identified by InterruptOtherActivities.
           */
         virtual xs::Boolean getInterruptOtherActivities() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InterruptOtherActivities.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInterruptOtherActivities(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CollectionPolicy.
           *
           * @return The value of the simple primitive data type identified by CollectionPolicy.
           */
         virtual xs::Boolean getCollectionPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CollectionPolicy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCollectionPolicy(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HRR_Supported.
           *
           * @return The value of the simple primitive data type identified by HRR_Supported.
           */
         virtual xs::Boolean getHRR_Supported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HRR_Supported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHRR_Supported(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FTI_Supported.
           *
           * @return The value of the simple primitive data type identified by FTI_Supported.
           */
         virtual xs::Boolean getFTI_Supported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FTI_Supported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFTI_Supported(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TrackingSupported.
           *
           * @return The value of the simple primitive data type identified by TrackingSupported.
           */
         virtual xs::Boolean getTrackingSupported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TrackingSupported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTrackingSupported(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PackingPlan.
           *
           * @return The value of the simple primitive data type identified by PackingPlan.
           */
         virtual xs::Boolean getPackingPlan() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PackingPlan.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPackingPlan(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ConcurrentOperationSupported.
           *
           * @return The value of the simple primitive data type identified by ConcurrentOperationSupported.
           */
         virtual xs::Boolean getConcurrentOperationSupported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ConcurrentOperationSupported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setConcurrentOperationSupported(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsSupported.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsSupported.
           */
         virtual const uci::type::SMTI_CapabilityOptionsType::ElectronicProtectionOptionsSupported& getElectronicProtectionOptionsSupported() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ElectronicProtectionOptionsSupported.
           *
           * @return The bounded list identified by ElectronicProtectionOptionsSupported.
           */
         virtual uci::type::SMTI_CapabilityOptionsType::ElectronicProtectionOptionsSupported& getElectronicProtectionOptionsSupported()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ElectronicProtectionOptionsSupported.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setElectronicProtectionOptionsSupported(const uci::type::SMTI_CapabilityOptionsType::ElectronicProtectionOptionsSupported& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SMTI_CapabilityOptionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SMTI_CapabilityOptionsType to copy from
           */
         SMTI_CapabilityOptionsType(const SMTI_CapabilityOptionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SMTI_CapabilityOptionsType to the contents of the
           * SMTI_CapabilityOptionsType on the right hand side (rhs) of the assignment operator.SMTI_CapabilityOptionsType [only
           * available to derived classes]
           *
           * @param rhs The SMTI_CapabilityOptionsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SMTI_CapabilityOptionsType
           * @return A constant reference to this SMTI_CapabilityOptionsType.
           */
         const SMTI_CapabilityOptionsType& operator=(const SMTI_CapabilityOptionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SMTI_CapabilityOptionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SMTI_CapabilityOptionsType_h

