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
#ifndef Uci__Type__ESM_ConsentRequestMDT_h
#define Uci__Type__ESM_ConsentRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ESM_ConsentRequestType_h)
# include "uci/type/ESM_ConsentRequestType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ESM_ConsentRequestMDT sequence accessor class */
      class ESM_ConsentRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ESM_ConsentRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. ESM_ConsentRequestMDT
           *
           * @return This accessor's type constant, i.e. ESM_ConsentRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eSM_ConsentRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ESM_ConsentRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a consent request instance. An ESM_ConsentRequest can have multiple consent request instances to allow a
           * sequence of related consent requests to be described in a single coherent message. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ESM_ConsentRequestType, uci::type::accessorType::eSM_ConsentRequestType> ConsentRequest;

         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConsentRequest.
           *
           * @return The bounded list identified by ConsentRequest.
           */
         virtual const uci::type::ESM_ConsentRequestMDT::ConsentRequest& getConsentRequest() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConsentRequest.
           *
           * @return The bounded list identified by ConsentRequest.
           */
         virtual uci::type::ESM_ConsentRequestMDT::ConsentRequest& getConsentRequest()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConsentRequest.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConsentRequest(const uci::type::ESM_ConsentRequestMDT::ConsentRequest& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ESM_ConsentRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ESM_ConsentRequestMDT to copy from
           */
         ESM_ConsentRequestMDT(const ESM_ConsentRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ESM_ConsentRequestMDT to the contents of the ESM_ConsentRequestMDT
           * on the right hand side (rhs) of the assignment operator.ESM_ConsentRequestMDT [only available to derived classes]
           *
           * @param rhs The ESM_ConsentRequestMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ESM_ConsentRequestMDT
           * @return A constant reference to this ESM_ConsentRequestMDT.
           */
         const ESM_ConsentRequestMDT& operator=(const ESM_ConsentRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ESM_ConsentRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ESM_ConsentRequestMDT_h

