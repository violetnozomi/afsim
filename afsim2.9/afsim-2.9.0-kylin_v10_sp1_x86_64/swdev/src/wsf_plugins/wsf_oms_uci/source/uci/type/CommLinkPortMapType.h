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
#ifndef Uci__Type__CommLinkPortMapType_h
#define Uci__Type__CommLinkPortMapType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommUserIO_ID_Type_h)
# include "uci/type/CommUserIO_ID_Type.h"
#endif

#if !defined(Uci__Type__CommUserLinkID_Type_h)
# include "uci/type/CommUserLinkID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Identifies assigned ports. */
      class CommLinkPortMapType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLinkPortMapType()
         { }

         /** Returns this accessor's type constant, i.e. CommLinkPortMapType
           *
           * @return This accessor's type constant, i.e. CommLinkPortMapType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLinkPortMapType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLinkPortMapType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Identifies an input/output port used to inject or withdraw data to/from an I/O component. Where the subsystem
           * provides RED/BLACK separation, UserIO components exist on the RED side of the communication subsystem. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommUserIO_ID_Type, uci::type::accessorType::commUserIO_ID_Type> UserIO_ID;

         /** Identifies an input/output RF link used to send or receive payload data. Where the subsystem provides RED/BLACK
           * separation, user link components exist on the BLACK side of the communication subsystem. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommUserLinkID_Type, uci::type::accessorType::commUserLinkID_Type> RF_UserLinkID;

         /** Returns the bounded list that is identified by the UserIO_ID.
           *
           * @return The bounded list identified by UserIO_ID.
           */
         virtual const uci::type::CommLinkPortMapType::UserIO_ID& getUserIO_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UserIO_ID.
           *
           * @return The bounded list identified by UserIO_ID.
           */
         virtual uci::type::CommLinkPortMapType::UserIO_ID& getUserIO_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the UserIO_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setUserIO_ID(const uci::type::CommLinkPortMapType::UserIO_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_UserLinkID.
           *
           * @return The bounded list identified by RF_UserLinkID.
           */
         virtual const uci::type::CommLinkPortMapType::RF_UserLinkID& getRF_UserLinkID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_UserLinkID.
           *
           * @return The bounded list identified by RF_UserLinkID.
           */
         virtual uci::type::CommLinkPortMapType::RF_UserLinkID& getRF_UserLinkID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_UserLinkID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_UserLinkID(const uci::type::CommLinkPortMapType::RF_UserLinkID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLinkPortMapType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLinkPortMapType to copy from
           */
         CommLinkPortMapType(const CommLinkPortMapType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLinkPortMapType to the contents of the CommLinkPortMapType on
           * the right hand side (rhs) of the assignment operator.CommLinkPortMapType [only available to derived classes]
           *
           * @param rhs The CommLinkPortMapType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CommLinkPortMapType
           * @return A constant reference to this CommLinkPortMapType.
           */
         const CommLinkPortMapType& operator=(const CommLinkPortMapType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLinkPortMapType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLinkPortMapType_h

