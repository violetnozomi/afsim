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
#ifndef Uci__Type__OperatorActionCommandMDT_h
#define Uci__Type__OperatorActionCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OperatorActionCommandID_Type_h)
# include "uci/type/OperatorActionCommandID_Type.h"
#endif

#if !defined(Uci__Type__OperatorNotificationID_Type_h)
# include "uci/type/OperatorNotificationID_Type.h"
#endif

#if !defined(Uci__Type__OperatorActionType_h)
# include "uci/type/OperatorActionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorActionCommandMDT sequence accessor class */
      class OperatorActionCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperatorActionCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. OperatorActionCommandMDT
           *
           * @return This accessor's type constant, i.e. OperatorActionCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorActionCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorActionCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorActionCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorActionCommandID.
           */
         virtual const uci::type::OperatorActionCommandID_Type& getOperatorActionCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OperatorActionCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OperatorActionCommandID.
           */
         virtual uci::type::OperatorActionCommandID_Type& getOperatorActionCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OperatorActionCommandID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OperatorActionCommandID
           */
         virtual void setOperatorActionCommandID(const uci::type::OperatorActionCommandID_Type& value)
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


         /** Returns the accessor that provides access to the complex content that is identified by the CommandedAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandedAction.
           */
         virtual const uci::type::OperatorActionType& getCommandedAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandedAction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandedAction.
           */
         virtual uci::type::OperatorActionType& getCommandedAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandedAction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandedAction
           */
         virtual void setCommandedAction(const uci::type::OperatorActionType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorActionCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorActionCommandMDT to copy from
           */
         OperatorActionCommandMDT(const OperatorActionCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorActionCommandMDT to the contents of the
           * OperatorActionCommandMDT on the right hand side (rhs) of the assignment operator.OperatorActionCommandMDT [only
           * available to derived classes]
           *
           * @param rhs The OperatorActionCommandMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::OperatorActionCommandMDT
           * @return A constant reference to this OperatorActionCommandMDT.
           */
         const OperatorActionCommandMDT& operator=(const OperatorActionCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorActionCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorActionCommandMDT_h

