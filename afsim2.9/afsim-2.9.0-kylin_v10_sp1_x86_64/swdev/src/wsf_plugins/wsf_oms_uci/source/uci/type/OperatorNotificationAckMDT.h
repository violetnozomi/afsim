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
#ifndef Uci__Type__OperatorNotificationAckMDT_h
#define Uci__Type__OperatorNotificationAckMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OperatorNotificationAckID_Type_h)
# include "uci/type/OperatorNotificationAckID_Type.h"
#endif

#if !defined(Uci__Type__OperatorNotificationID_Type_h)
# include "uci/type/OperatorNotificationID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorNotificationAckMDT sequence accessor class */
      class OperatorNotificationAckMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperatorNotificationAckMDT()
         { }

         /** Returns this accessor's type constant, i.e. OperatorNotificationAckMDT
           *
           * @return This accessor's type constant, i.e. OperatorNotificationAckMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorNotificationAckMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorNotificationAckMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorNotificationAckID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorNotificationAckID.
           */
         virtual const uci::type::OperatorNotificationAckID_Type& getOperatorNotificationAckID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorNotificationAckID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorNotificationAckID.
           */
         virtual uci::type::OperatorNotificationAckID_Type& getOperatorNotificationAckID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperatorNotificationAckID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperatorNotificationAckID
           */
         virtual void setOperatorNotificationAckID(const uci::type::OperatorNotificationAckID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorNotificationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorNotificationID.
           */
         virtual const uci::type::OperatorNotificationID_Type& getOperatorNotificationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorNotificationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorNotificationID.
           */
         virtual uci::type::OperatorNotificationID_Type& getOperatorNotificationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperatorNotificationID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperatorNotificationID
           */
         virtual void setOperatorNotificationID(const uci::type::OperatorNotificationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IsAcknowledged.
           *
           * @return The value of the simple primitive data type identified by IsAcknowledged.
           */
         virtual xs::Boolean getIsAcknowledged() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IsAcknowledged.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIsAcknowledged(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorNotificationAckMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorNotificationAckMDT to copy from
           */
         OperatorNotificationAckMDT(const OperatorNotificationAckMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorNotificationAckMDT to the contents of the
           * OperatorNotificationAckMDT on the right hand side (rhs) of the assignment operator.OperatorNotificationAckMDT [only
           * available to derived classes]
           *
           * @param rhs The OperatorNotificationAckMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::OperatorNotificationAckMDT
           * @return A constant reference to this OperatorNotificationAckMDT.
           */
         const OperatorNotificationAckMDT& operator=(const OperatorNotificationAckMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorNotificationAckMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorNotificationAckMDT_h

