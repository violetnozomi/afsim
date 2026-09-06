// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__LinkRatesType_h
#define Uci__Type__LinkRatesType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DataRateType_h)
# include "uci/type/DataRateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LinkRatesType sequence accessor class */
      class LinkRatesType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LinkRatesType()
         { }

         /** Returns this accessor's type constant, i.e. LinkRatesType
           *
           * @return This accessor's type constant, i.e. LinkRatesType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::linkRatesType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LinkRatesType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UplinkRate.
           *
           * @return The value of the simple primitive data type identified by UplinkRate.
           */
         virtual uci::type::DataRateTypeValue getUplinkRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UplinkRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUplinkRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DownlinkRate.
           *
           * @return The value of the simple primitive data type identified by DownlinkRate.
           */
         virtual uci::type::DataRateTypeValue getDownlinkRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DownlinkRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDownlinkRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LinkRatesType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LinkRatesType to copy from
           */
         LinkRatesType(const LinkRatesType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LinkRatesType to the contents of the LinkRatesType on the right
           * hand side (rhs) of the assignment operator.LinkRatesType [only available to derived classes]
           *
           * @param rhs The LinkRatesType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::LinkRatesType
           * @return A constant reference to this LinkRatesType.
           */
         const LinkRatesType& operator=(const LinkRatesType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LinkRatesType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LinkRatesType_h

