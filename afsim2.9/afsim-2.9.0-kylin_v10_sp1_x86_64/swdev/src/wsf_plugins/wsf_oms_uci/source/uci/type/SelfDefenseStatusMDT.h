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
#ifndef Uci__Type__SelfDefenseStatusMDT_h
#define Uci__Type__SelfDefenseStatusMDT_h 1

#if !defined(Uci__Type__SupportCapabilityStatusBaseType_h)
# include "uci/type/SupportCapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Component of the vehicle to defend against attack. */
      class SelfDefenseStatusMDT : public virtual uci::type::SupportCapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~SelfDefenseStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. SelfDefenseStatusMDT
           *
           * @return This accessor's type constant, i.e. SelfDefenseStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::selfDefenseStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SelfDefenseStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Chaff.
           *
           * @return The value of the simple primitive data type identified by Chaff.
           */
         virtual xs::UnsignedInt getChaff() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Chaff.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setChaff(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Flare.
           *
           * @return The value of the simple primitive data type identified by Flare.
           */
         virtual xs::UnsignedInt getFlare() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Flare.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFlare(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Decoy.
           *
           * @return The value of the simple primitive data type identified by Decoy.
           */
         virtual xs::UnsignedInt getDecoy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Decoy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDecoy(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DirectedEnergy.
           *
           * @return The value of the simple primitive data type identified by DirectedEnergy.
           */
         virtual xs::Boolean getDirectedEnergy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DirectedEnergy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDirectedEnergy(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SelfDefenseStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SelfDefenseStatusMDT to copy from
           */
         SelfDefenseStatusMDT(const SelfDefenseStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SelfDefenseStatusMDT to the contents of the SelfDefenseStatusMDT
           * on the right hand side (rhs) of the assignment operator.SelfDefenseStatusMDT [only available to derived classes]
           *
           * @param rhs The SelfDefenseStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::SelfDefenseStatusMDT
           * @return A constant reference to this SelfDefenseStatusMDT.
           */
         const SelfDefenseStatusMDT& operator=(const SelfDefenseStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SelfDefenseStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SelfDefenseStatusMDT_h

