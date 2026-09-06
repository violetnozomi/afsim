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
#ifndef Uci__Type__SAR_AutomaticTargetDetectionParametersType_h
#define Uci__Type__SAR_AutomaticTargetDetectionParametersType_h 1

#if !defined(Uci__Type__SAR_ProcessingParametersType_h)
# include "uci/type/SAR_ProcessingParametersType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type extends SAR_ProcessingParametersType to provide processing parameters needed for Automatic Target Detection
        * products from a SAR image product.
        */
      class SAR_AutomaticTargetDetectionParametersType : public virtual uci::type::SAR_ProcessingParametersType {
      public:

         /** The destructor */
         virtual ~SAR_AutomaticTargetDetectionParametersType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_AutomaticTargetDetectionParametersType
           *
           * @return This accessor's type constant, i.e. SAR_AutomaticTargetDetectionParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_AutomaticTargetDetectionParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_AutomaticTargetDetectionParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumSize.
           *
           * @return The value of the simple primitive data type identified by MinimumSize.
           */
         virtual uci::type::DistanceTypeValue getMinimumSize() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumSize.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumSize(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumSize.
           *
           * @return The value of the simple primitive data type identified by MaximumSize.
           */
         virtual uci::type::DistanceTypeValue getMaximumSize() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumSize.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumSize(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_AutomaticTargetDetectionParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_AutomaticTargetDetectionParametersType to copy from
           */
         SAR_AutomaticTargetDetectionParametersType(const SAR_AutomaticTargetDetectionParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_AutomaticTargetDetectionParametersType to the contents of the
           * SAR_AutomaticTargetDetectionParametersType on the right hand side (rhs) of the assignment
           * operator.SAR_AutomaticTargetDetectionParametersType [only available to derived classes]
           *
           * @param rhs The SAR_AutomaticTargetDetectionParametersType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::SAR_AutomaticTargetDetectionParametersType
           * @return A constant reference to this SAR_AutomaticTargetDetectionParametersType.
           */
         const SAR_AutomaticTargetDetectionParametersType& operator=(const SAR_AutomaticTargetDetectionParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_AutomaticTargetDetectionParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_AutomaticTargetDetectionParametersType_h

