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
#ifndef Uci__Type__BDI_CollectionType_h
#define Uci__Type__BDI_CollectionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the BDI_CollectionType sequence accessor class */
      class BDI_CollectionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~BDI_CollectionType()
         { }

         /** Returns this accessor's type constant, i.e. BDI_CollectionType
           *
           * @return This accessor's type constant, i.e. BDI_CollectionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::bDI_CollectionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const BDI_CollectionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Quality.
           *
           * @return The value of the simple primitive data type identified by Quality.
           */
         virtual uci::type::PercentTypeValue getQuality() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Quality.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setQuality(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AccuracyConfidence.
           *
           * @return The value of the simple primitive data type identified by AccuracyConfidence.
           */
         virtual uci::type::PercentTypeValue getAccuracyConfidence() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AccuracyConfidence.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAccuracyConfidence(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         BDI_CollectionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The BDI_CollectionType to copy from
           */
         BDI_CollectionType(const BDI_CollectionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this BDI_CollectionType to the contents of the BDI_CollectionType on
           * the right hand side (rhs) of the assignment operator.BDI_CollectionType [only available to derived classes]
           *
           * @param rhs The BDI_CollectionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::BDI_CollectionType
           * @return A constant reference to this BDI_CollectionType.
           */
         const BDI_CollectionType& operator=(const BDI_CollectionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // BDI_CollectionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__BDI_CollectionType_h

