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
#ifndef Uci__Type__SubsystemStatusDetailedNameValuePairType_h
#define Uci__Type__SubsystemStatusDetailedNameValuePairType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__SubsystemStatusDetailedValueType_h)
# include "uci/type/SubsystemStatusDetailedValueType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The SubsystemStatusDetailedNameValuePair is used to report the value of an individual Subsystem's status attribute.
        * This type is used to provide status for subsystem unique attributes that cannot be reported with other types or
        * structures.
        */
      class SubsystemStatusDetailedNameValuePairType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemStatusDetailedNameValuePairType()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemStatusDetailedNameValuePairType
           *
           * @return This accessor's type constant, i.e. SubsystemStatusDetailedNameValuePairType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemStatusDetailedNameValuePairType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemStatusDetailedNameValuePairType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual const uci::type::VisibleString256Type& getName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual uci::type::VisibleString256Type& getName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setName(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Value.
           *
           * @return The acecssor that provides access to the complex content that is identified by Value.
           */
         virtual const uci::type::SubsystemStatusDetailedValueType& getValue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Value.
           *
           * @return The acecssor that provides access to the complex content that is identified by Value.
           */
         virtual uci::type::SubsystemStatusDetailedValueType& getValue()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Value to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Value
           */
         virtual void setValue(const uci::type::SubsystemStatusDetailedValueType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemStatusDetailedNameValuePairType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemStatusDetailedNameValuePairType to copy from
           */
         SubsystemStatusDetailedNameValuePairType(const SubsystemStatusDetailedNameValuePairType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemStatusDetailedNameValuePairType to the contents of the
           * SubsystemStatusDetailedNameValuePairType on the right hand side (rhs) of the assignment
           * operator.SubsystemStatusDetailedNameValuePairType [only available to derived classes]
           *
           * @param rhs The SubsystemStatusDetailedNameValuePairType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::SubsystemStatusDetailedNameValuePairType
           * @return A constant reference to this SubsystemStatusDetailedNameValuePairType.
           */
         const SubsystemStatusDetailedNameValuePairType& operator=(const SubsystemStatusDetailedNameValuePairType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemStatusDetailedNameValuePairType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemStatusDetailedNameValuePairType_h

