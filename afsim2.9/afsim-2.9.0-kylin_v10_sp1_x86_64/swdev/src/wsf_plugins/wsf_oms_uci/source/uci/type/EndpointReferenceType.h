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
#ifndef Uci__Type__EndpointReferenceType_h
#define Uci__Type__EndpointReferenceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AttributedURI_Type_h)
# include "uci/type/AttributedURI_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EndpointReferenceType sequence accessor class */
      class EndpointReferenceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EndpointReferenceType()
         { }

         /** Returns this accessor's type constant, i.e. EndpointReferenceType
           *
           * @return This accessor's type constant, i.e. EndpointReferenceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::endpointReferenceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EndpointReferenceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Address.
           *
           * @return The value of the string data type identified by Address.
           */
         virtual const uci::type::AttributedURI_Type& getAddress() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Address.
           *
           * @return The value of the string data type identified by Address.
           */
         virtual uci::type::AttributedURI_Type& getAddress()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Address to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAddress(const uci::type::AttributedURI_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Address to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAddress(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Address to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAddress(const char* value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EndpointReferenceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EndpointReferenceType to copy from
           */
         EndpointReferenceType(const EndpointReferenceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EndpointReferenceType to the contents of the EndpointReferenceType
           * on the right hand side (rhs) of the assignment operator.EndpointReferenceType [only available to derived classes]
           *
           * @param rhs The EndpointReferenceType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::EndpointReferenceType
           * @return A constant reference to this EndpointReferenceType.
           */
         const EndpointReferenceType& operator=(const EndpointReferenceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EndpointReferenceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EndpointReferenceType_h

