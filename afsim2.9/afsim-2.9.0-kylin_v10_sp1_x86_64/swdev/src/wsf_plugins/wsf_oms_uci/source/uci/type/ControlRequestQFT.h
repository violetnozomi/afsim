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
#ifndef Uci__Type__ControlRequestQFT_h
#define Uci__Type__ControlRequestQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ControlRequestID_Type_h)
# include "uci/type/ControlRequestID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ControlRequestQFT sequence accessor class */
      class ControlRequestQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ControlRequestQFT()
         { }

         /** Returns this accessor's type constant, i.e. ControlRequestQFT
           *
           * @return This accessor's type constant, i.e. ControlRequestQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::controlRequestQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ControlRequestQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ControlRequestID_Type, uci::type::accessorType::controlRequestID_Type> ControlRequestID;

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


         /** Returns the bounded list that is identified by the ControlRequestID.
           *
           * @return The bounded list identified by ControlRequestID.
           */
         virtual const uci::type::ControlRequestQFT::ControlRequestID& getControlRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ControlRequestID.
           *
           * @return The bounded list identified by ControlRequestID.
           */
         virtual uci::type::ControlRequestQFT::ControlRequestID& getControlRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ControlRequestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setControlRequestID(const uci::type::ControlRequestQFT::ControlRequestID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlRequestQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlRequestQFT to copy from
           */
         ControlRequestQFT(const ControlRequestQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlRequestQFT to the contents of the ControlRequestQFT on the
           * right hand side (rhs) of the assignment operator.ControlRequestQFT [only available to derived classes]
           *
           * @param rhs The ControlRequestQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ControlRequestQFT
           * @return A constant reference to this ControlRequestQFT.
           */
         const ControlRequestQFT& operator=(const ControlRequestQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlRequestQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ControlRequestQFT_h

