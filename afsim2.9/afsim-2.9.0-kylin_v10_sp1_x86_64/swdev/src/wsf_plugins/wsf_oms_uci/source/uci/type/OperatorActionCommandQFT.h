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
#ifndef Uci__Type__OperatorActionCommandQFT_h
#define Uci__Type__OperatorActionCommandQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OperatorActionCommandID_Type_h)
# include "uci/type/OperatorActionCommandID_Type.h"
#endif

#if !defined(Uci__Type__OperatorNotificationID_Type_h)
# include "uci/type/OperatorNotificationID_Type.h"
#endif

#if !defined(Uci__Type__OperatorActionID_Type_h)
# include "uci/type/OperatorActionID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorActionCommandQFT sequence accessor class */
      class OperatorActionCommandQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~OperatorActionCommandQFT()
         { }

         /** Returns this accessor's type constant, i.e. OperatorActionCommandQFT
           *
           * @return This accessor's type constant, i.e. OperatorActionCommandQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorActionCommandQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorActionCommandQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::OperatorActionCommandID_Type, uci::type::accessorType::operatorActionCommandID_Type> OperatorActionCommandID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::OperatorNotificationID_Type, uci::type::accessorType::operatorNotificationID_Type> OperatorNotificationID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::OperatorActionID_Type, uci::type::accessorType::operatorActionID_Type> OperatorActionID;

         /** Returns the bounded list that is identified by the OperatorActionCommandID.
           *
           * @return The bounded list identified by OperatorActionCommandID.
           */
         virtual const uci::type::OperatorActionCommandQFT::OperatorActionCommandID& getOperatorActionCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorActionCommandID.
           *
           * @return The bounded list identified by OperatorActionCommandID.
           */
         virtual uci::type::OperatorActionCommandQFT::OperatorActionCommandID& getOperatorActionCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OperatorActionCommandID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOperatorActionCommandID(const uci::type::OperatorActionCommandQFT::OperatorActionCommandID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorNotificationID.
           *
           * @return The bounded list identified by OperatorNotificationID.
           */
         virtual const uci::type::OperatorActionCommandQFT::OperatorNotificationID& getOperatorNotificationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorNotificationID.
           *
           * @return The bounded list identified by OperatorNotificationID.
           */
         virtual uci::type::OperatorActionCommandQFT::OperatorNotificationID& getOperatorNotificationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OperatorNotificationID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOperatorNotificationID(const uci::type::OperatorActionCommandQFT::OperatorNotificationID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorActionID.
           *
           * @return The bounded list identified by OperatorActionID.
           */
         virtual const uci::type::OperatorActionCommandQFT::OperatorActionID& getOperatorActionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorActionID.
           *
           * @return The bounded list identified by OperatorActionID.
           */
         virtual uci::type::OperatorActionCommandQFT::OperatorActionID& getOperatorActionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OperatorActionID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOperatorActionID(const uci::type::OperatorActionCommandQFT::OperatorActionID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorActionCommandQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorActionCommandQFT to copy from
           */
         OperatorActionCommandQFT(const OperatorActionCommandQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorActionCommandQFT to the contents of the
           * OperatorActionCommandQFT on the right hand side (rhs) of the assignment operator.OperatorActionCommandQFT [only
           * available to derived classes]
           *
           * @param rhs The OperatorActionCommandQFT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::OperatorActionCommandQFT
           * @return A constant reference to this OperatorActionCommandQFT.
           */
         const OperatorActionCommandQFT& operator=(const OperatorActionCommandQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorActionCommandQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorActionCommandQFT_h

