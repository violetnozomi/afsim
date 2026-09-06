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
#ifndef Uci__Type__ExternalDataRequestMDT_h
#define Uci__Type__ExternalDataRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ExternalDataRequestID_Type_h)
# include "uci/type/ExternalDataRequestID_Type.h"
#endif

#if !defined(Uci__Type__ExternalDataRequestEnum_h)
# include "uci/type/ExternalDataRequestEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ExternalDataRequestMDT sequence accessor class */
      class ExternalDataRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ExternalDataRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. ExternalDataRequestMDT
           *
           * @return This accessor's type constant, i.e. ExternalDataRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::externalDataRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ExternalDataRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ExternalDataRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ExternalDataRequestID.
           */
         virtual const uci::type::ExternalDataRequestID_Type& getExternalDataRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ExternalDataRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ExternalDataRequestID.
           */
         virtual uci::type::ExternalDataRequestID_Type& getExternalDataRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ExternalDataRequestID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ExternalDataRequestID
           */
         virtual void setExternalDataRequestID(const uci::type::ExternalDataRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestedData.
           *
           * @return The value of the enumeration identified by RequestedData.
           */
         virtual const uci::type::ExternalDataRequestEnum& getRequestedData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RequestedData.
           *
           * @return The value of the enumeration identified by RequestedData.
           */
         virtual uci::type::ExternalDataRequestEnum& getRequestedData()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestedData.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestedData(const uci::type::ExternalDataRequestEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RequestedData.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRequestedData(uci::type::ExternalDataRequestEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ExternalDataRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ExternalDataRequestMDT to copy from
           */
         ExternalDataRequestMDT(const ExternalDataRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ExternalDataRequestMDT to the contents of the
           * ExternalDataRequestMDT on the right hand side (rhs) of the assignment operator.ExternalDataRequestMDT [only available
           * to derived classes]
           *
           * @param rhs The ExternalDataRequestMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ExternalDataRequestMDT
           * @return A constant reference to this ExternalDataRequestMDT.
           */
         const ExternalDataRequestMDT& operator=(const ExternalDataRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ExternalDataRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ExternalDataRequestMDT_h

