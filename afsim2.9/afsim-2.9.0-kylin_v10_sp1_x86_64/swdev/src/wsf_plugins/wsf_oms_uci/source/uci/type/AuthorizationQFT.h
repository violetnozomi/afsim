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
#ifndef Uci__Type__AuthorizationQFT_h
#define Uci__Type__AuthorizationQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AuthorizationID_Type_h)
# include "uci/type/AuthorizationID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AuthorizationQFT sequence accessor class */
      class AuthorizationQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~AuthorizationQFT()
         { }

         /** Returns this accessor's type constant, i.e. AuthorizationQFT
           *
           * @return This accessor's type constant, i.e. AuthorizationQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::authorizationQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AuthorizationQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::AuthorizationID_Type, uci::type::accessorType::authorizationID_Type> AuthorizationID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Returns the bounded list that is identified by the AuthorizationID.
           *
           * @return The bounded list identified by AuthorizationID.
           */
         virtual const uci::type::AuthorizationQFT::AuthorizationID& getAuthorizationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AuthorizationID.
           *
           * @return The bounded list identified by AuthorizationID.
           */
         virtual uci::type::AuthorizationQFT::AuthorizationID& getAuthorizationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AuthorizationID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAuthorizationID(const uci::type::AuthorizationQFT::AuthorizationID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::AuthorizationQFT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::AuthorizationQFT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::AuthorizationQFT::SystemID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AuthorizationQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AuthorizationQFT to copy from
           */
         AuthorizationQFT(const AuthorizationQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AuthorizationQFT to the contents of the AuthorizationQFT on the
           * right hand side (rhs) of the assignment operator.AuthorizationQFT [only available to derived classes]
           *
           * @param rhs The AuthorizationQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AuthorizationQFT
           * @return A constant reference to this AuthorizationQFT.
           */
         const AuthorizationQFT& operator=(const AuthorizationQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AuthorizationQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AuthorizationQFT_h

