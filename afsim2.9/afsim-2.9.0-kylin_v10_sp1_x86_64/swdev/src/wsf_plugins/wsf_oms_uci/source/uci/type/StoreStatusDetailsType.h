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
#ifndef Uci__Type__StoreStatusDetailsType_h
#define Uci__Type__StoreStatusDetailsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Possible store settings that could be used to give more detail information about the state or extra data related to
        * the attached Mission store
        */
      class StoreStatusDetailsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StoreStatusDetailsType()
         { }

         /** Returns this accessor's type constant, i.e. StoreStatusDetailsType
           *
           * @return This accessor's type constant, i.e. StoreStatusDetailsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::storeStatusDetailsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StoreStatusDetailsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the INS_ENABLE.
           *
           * @return The value of the simple primitive data type identified by INS_ENABLE.
           */
         virtual xs::Boolean getINS_ENABLE() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the INS_ENABLE.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setINS_ENABLE(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GPS_ENABLE.
           *
           * @return The value of the simple primitive data type identified by GPS_ENABLE.
           */
         virtual xs::Boolean getGPS_ENABLE() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GPS_ENABLE.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGPS_ENABLE(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TARGETED.
           *
           * @return The value of the simple primitive data type identified by TARGETED.
           */
         virtual xs::Boolean getTARGETED() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TARGETED.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTARGETED(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SELECTED_JETTISON.
           *
           * @return The value of the simple primitive data type identified by SELECTED_JETTISON.
           */
         virtual xs::Boolean getSELECTED_JETTISON() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SELECTED_JETTISON.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSELECTED_JETTISON(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StoreStatusDetailsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StoreStatusDetailsType to copy from
           */
         StoreStatusDetailsType(const StoreStatusDetailsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StoreStatusDetailsType to the contents of the
           * StoreStatusDetailsType on the right hand side (rhs) of the assignment operator.StoreStatusDetailsType [only available
           * to derived classes]
           *
           * @param rhs The StoreStatusDetailsType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::StoreStatusDetailsType
           * @return A constant reference to this StoreStatusDetailsType.
           */
         const StoreStatusDetailsType& operator=(const StoreStatusDetailsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StoreStatusDetailsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StoreStatusDetailsType_h

