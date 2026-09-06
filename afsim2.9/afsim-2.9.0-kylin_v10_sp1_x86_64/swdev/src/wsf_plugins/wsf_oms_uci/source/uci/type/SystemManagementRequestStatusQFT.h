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
#ifndef Uci__Type__SystemManagementRequestStatusQFT_h
#define Uci__Type__SystemManagementRequestStatusQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemManagementRequestID_Type_h)
# include "uci/type/SystemManagementRequestID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ProcessingStatusEnum_h)
# include "uci/type/ProcessingStatusEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SystemManagementRequestStatusQFT sequence accessor class */
      class SystemManagementRequestStatusQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~SystemManagementRequestStatusQFT()
         { }

         /** Returns this accessor's type constant, i.e. SystemManagementRequestStatusQFT
           *
           * @return This accessor's type constant, i.e. SystemManagementRequestStatusQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemManagementRequestStatusQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemManagementRequestStatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filter on the SystemManagementRequest ID. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemManagementRequestID_Type, uci::type::accessorType::systemManagementRequestID_Type> SystemManagementRequestID;

         /** Filter on the System ID representing the targeted system of the request. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** The state of the messages. [Minimum occurrences: 0] [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::ProcessingStatusEnum, uci::type::accessorType::processingStatusEnum> SystemManagementRequestState;

         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @return The value of the simple primitive data type identified by IncludeStatus.
           */
         virtual xs::Boolean getIncludeStatus() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeStatus.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeStatus(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemManagementRequestID.
           *
           * @return The bounded list identified by SystemManagementRequestID.
           */
         virtual const uci::type::SystemManagementRequestStatusQFT::SystemManagementRequestID& getSystemManagementRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemManagementRequestID.
           *
           * @return The bounded list identified by SystemManagementRequestID.
           */
         virtual uci::type::SystemManagementRequestStatusQFT::SystemManagementRequestID& getSystemManagementRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemManagementRequestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemManagementRequestID(const uci::type::SystemManagementRequestStatusQFT::SystemManagementRequestID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::SystemManagementRequestStatusQFT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::SystemManagementRequestStatusQFT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::SystemManagementRequestStatusQFT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemManagementRequestState.
           *
           * @return The bounded list identified by SystemManagementRequestState.
           */
         virtual const uci::type::SystemManagementRequestStatusQFT::SystemManagementRequestState& getSystemManagementRequestState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemManagementRequestState.
           *
           * @return The bounded list identified by SystemManagementRequestState.
           */
         virtual uci::type::SystemManagementRequestStatusQFT::SystemManagementRequestState& getSystemManagementRequestState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemManagementRequestState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemManagementRequestState(const uci::type::SystemManagementRequestStatusQFT::SystemManagementRequestState& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemManagementRequestStatusQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemManagementRequestStatusQFT to copy from
           */
         SystemManagementRequestStatusQFT(const SystemManagementRequestStatusQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemManagementRequestStatusQFT to the contents of the
           * SystemManagementRequestStatusQFT on the right hand side (rhs) of the assignment
           * operator.SystemManagementRequestStatusQFT [only available to derived classes]
           *
           * @param rhs The SystemManagementRequestStatusQFT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SystemManagementRequestStatusQFT
           * @return A constant reference to this SystemManagementRequestStatusQFT.
           */
         const SystemManagementRequestStatusQFT& operator=(const SystemManagementRequestStatusQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemManagementRequestStatusQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemManagementRequestStatusQFT_h

