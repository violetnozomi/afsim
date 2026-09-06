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
#ifndef Uci__Type__StoreStatusType_h
#define Uci__Type__StoreStatusType_h 1

#if !defined(Uci__Type__StoreStatusBaseType_h)
# include "uci/type/StoreStatusBaseType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Possible store settings that could be used to id the state or extra data related to the attached Mission or Carriage
        * Store
        */
      class StoreStatusType : public virtual uci::type::StoreStatusBaseType {
      public:

         /** The destructor */
         virtual ~StoreStatusType()
         { }

         /** Returns this accessor's type constant, i.e. StoreStatusType
           *
           * @return This accessor's type constant, i.e. StoreStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::storeStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StoreStatusType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StoreStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StoreStatusType to copy from
           */
         StoreStatusType(const StoreStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StoreStatusType to the contents of the StoreStatusType on the
           * right hand side (rhs) of the assignment operator.StoreStatusType [only available to derived classes]
           *
           * @param rhs The StoreStatusType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::StoreStatusType
           * @return A constant reference to this StoreStatusType.
           */
         const StoreStatusType& operator=(const StoreStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StoreStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StoreStatusType_h

