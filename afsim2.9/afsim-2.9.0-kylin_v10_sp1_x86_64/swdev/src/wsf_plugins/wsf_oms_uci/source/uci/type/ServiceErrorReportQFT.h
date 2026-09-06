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
#ifndef Uci__Type__ServiceErrorReportQFT_h
#define Uci__Type__ServiceErrorReportQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ServiceErrorReportID_Type_h)
# include "uci/type/ServiceErrorReportID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ServiceErrorReportQFT sequence accessor class */
      class ServiceErrorReportQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~ServiceErrorReportQFT()
         { }

         /** Returns this accessor's type constant, i.e. ServiceErrorReportQFT
           *
           * @return This accessor's type constant, i.e. ServiceErrorReportQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::serviceErrorReportQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ServiceErrorReportQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ServiceErrorReportID_Type, uci::type::accessorType::serviceErrorReportID_Type> ServiceErrorReportID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> ServiceID;

         /** Returns the bounded list that is identified by the ServiceErrorReportID.
           *
           * @return The bounded list identified by ServiceErrorReportID.
           */
         virtual const uci::type::ServiceErrorReportQFT::ServiceErrorReportID& getServiceErrorReportID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceErrorReportID.
           *
           * @return The bounded list identified by ServiceErrorReportID.
           */
         virtual uci::type::ServiceErrorReportQFT::ServiceErrorReportID& getServiceErrorReportID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ServiceErrorReportID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setServiceErrorReportID(const uci::type::ServiceErrorReportQFT::ServiceErrorReportID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual const uci::type::ServiceErrorReportQFT::ServiceID& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ServiceID.
           *
           * @return The bounded list identified by ServiceID.
           */
         virtual uci::type::ServiceErrorReportQFT::ServiceID& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setServiceID(const uci::type::ServiceErrorReportQFT::ServiceID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ServiceErrorReportQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ServiceErrorReportQFT to copy from
           */
         ServiceErrorReportQFT(const ServiceErrorReportQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ServiceErrorReportQFT to the contents of the ServiceErrorReportQFT
           * on the right hand side (rhs) of the assignment operator.ServiceErrorReportQFT [only available to derived classes]
           *
           * @param rhs The ServiceErrorReportQFT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ServiceErrorReportQFT
           * @return A constant reference to this ServiceErrorReportQFT.
           */
         const ServiceErrorReportQFT& operator=(const ServiceErrorReportQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ServiceErrorReportQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ServiceErrorReportQFT_h

