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
#ifndef Uci__Type__CryptoCommandStatusQFT_h
#define Uci__Type__CryptoCommandStatusQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SupportCapabilityCommandID_Type_h)
# include "uci/type/SupportCapabilityCommandID_Type.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CryptoCommandStatusQFT sequence accessor class */
      class CryptoCommandStatusQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~CryptoCommandStatusQFT()
         { }

         /** Returns this accessor's type constant, i.e. CryptoCommandStatusQFT
           *
           * @return This accessor's type constant, i.e. CryptoCommandStatusQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cryptoCommandStatusQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CryptoCommandStatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityCommandID_Type, uci::type::accessorType::supportCapabilityCommandID_Type> CommandID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SupportCapabilityID_Type, uci::type::accessorType::supportCapabilityID_Type> SupportCapabilityID;

         /** Returns the bounded list that is identified by the CommandID.
           *
           * @return The bounded list identified by CommandID.
           */
         virtual const uci::type::CryptoCommandStatusQFT::CommandID& getCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommandID.
           *
           * @return The bounded list identified by CommandID.
           */
         virtual uci::type::CryptoCommandStatusQFT::CommandID& getCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommandID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommandID(const uci::type::CryptoCommandStatusQFT::CommandID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual const uci::type::CryptoCommandStatusQFT::SupportCapabilityID& getSupportCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SupportCapabilityID.
           *
           * @return The bounded list identified by SupportCapabilityID.
           */
         virtual uci::type::CryptoCommandStatusQFT::SupportCapabilityID& getSupportCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SupportCapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSupportCapabilityID(const uci::type::CryptoCommandStatusQFT::SupportCapabilityID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CryptoCommandStatusQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CryptoCommandStatusQFT to copy from
           */
         CryptoCommandStatusQFT(const CryptoCommandStatusQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CryptoCommandStatusQFT to the contents of the
           * CryptoCommandStatusQFT on the right hand side (rhs) of the assignment operator.CryptoCommandStatusQFT [only available
           * to derived classes]
           *
           * @param rhs The CryptoCommandStatusQFT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CryptoCommandStatusQFT
           * @return A constant reference to this CryptoCommandStatusQFT.
           */
         const CryptoCommandStatusQFT& operator=(const CryptoCommandStatusQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CryptoCommandStatusQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CryptoCommandStatusQFT_h

