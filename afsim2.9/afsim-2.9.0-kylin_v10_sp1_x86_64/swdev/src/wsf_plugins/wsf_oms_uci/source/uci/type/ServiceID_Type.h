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
#ifndef Uci__Type__ServiceID_Type_h
#define Uci__Type__ServiceID_Type_h 1

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The ID type for UCI IDs that correspond to services that communicate using the UCI protocol. Services have a digital
        * network location which is represented by this ID.
        */
      class ServiceID_Type : public virtual uci::type::ID_Type {
      public:

         /** The destructor */
         virtual ~ServiceID_Type()
         { }

         /** Returns this accessor's type constant, i.e. ServiceID_Type
           *
           * @return This accessor's type constant, i.e. ServiceID_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::serviceID_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ServiceID_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ServiceVersion.
           *
           * @return The value of the string data type identified by ServiceVersion.
           */
         virtual const uci::type::VisibleString256Type& getServiceVersion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ServiceVersion.
           *
           * @return The value of the string data type identified by ServiceVersion.
           */
         virtual uci::type::VisibleString256Type& getServiceVersion()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ServiceVersion to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setServiceVersion(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ServiceVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setServiceVersion(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ServiceVersion to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setServiceVersion(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ServiceVersion exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ServiceVersion is emabled or not
           */
         virtual bool hasServiceVersion() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ServiceVersion
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableServiceVersion(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ServiceVersion */
         virtual void clearServiceVersion()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ServiceID_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ServiceID_Type to copy from
           */
         ServiceID_Type(const ServiceID_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ServiceID_Type to the contents of the ServiceID_Type on the right
           * hand side (rhs) of the assignment operator.ServiceID_Type [only available to derived classes]
           *
           * @param rhs The ServiceID_Type on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::ServiceID_Type
           * @return A constant reference to this ServiceID_Type.
           */
         const ServiceID_Type& operator=(const ServiceID_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ServiceID_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ServiceID_Type_h

