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
#ifndef Uci__Type__StoreInformationType_h
#define Uci__Type__StoreInformationType_h 1

#if !defined(Uci__Type__StoreInformationBaseType_h)
# include "uci/type/StoreInformationBaseType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FuzeType_h)
# include "uci/type/FuzeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Mission store representation. */
      class StoreInformationType : public virtual uci::type::StoreInformationBaseType {
      public:

         /** The destructor */
         virtual ~StoreInformationType()
         { }

         /** Returns this accessor's type constant, i.e. StoreInformationType
           *
           * @return This accessor's type constant, i.e. StoreInformationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::storeInformationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StoreInformationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the fuzes in the weapon. Typically a weapon with a fuze will have them located in the nose, tail or both.
           * [Minimum occurrences: 0] [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::FuzeType, uci::type::accessorType::fuzeType> Fuzes;

         /** Returns the value of the SimplePrimitive data type that is identified by the Ammo.
           *
           * @return The value of the simple primitive data type identified by Ammo.
           */
         virtual xs::Int getAmmo() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Ammo.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAmmo(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Fuzes.
           *
           * @return The bounded list identified by Fuzes.
           */
         virtual const uci::type::StoreInformationType::Fuzes& getFuzes() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Fuzes.
           *
           * @return The bounded list identified by Fuzes.
           */
         virtual uci::type::StoreInformationType::Fuzes& getFuzes()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Fuzes.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFuzes(const uci::type::StoreInformationType::Fuzes& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StoreInformationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StoreInformationType to copy from
           */
         StoreInformationType(const StoreInformationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StoreInformationType to the contents of the StoreInformationType
           * on the right hand side (rhs) of the assignment operator.StoreInformationType [only available to derived classes]
           *
           * @param rhs The StoreInformationType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::StoreInformationType
           * @return A constant reference to this StoreInformationType.
           */
         const StoreInformationType& operator=(const StoreInformationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StoreInformationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StoreInformationType_h

