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
#ifndef Uci__Type__ModulationParameterType_h
#define Uci__Type__ModulationParameterType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VisibleString64Type_h)
# include "uci/type/VisibleString64Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a custom modulation parameter that is not covered by other elements in the Signal data structure. This will
        * typically be used to fully cover the fields defined in other interoperability standards such as the JICD.
        */
      class ModulationParameterType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ModulationParameterType()
         { }

         /** Returns this accessor's type constant, i.e. ModulationParameterType
           *
           * @return This accessor's type constant, i.e. ModulationParameterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::modulationParameterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ModulationParameterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ModulationParameterIndex.
           *
           * @return The value of the string data type identified by ModulationParameterIndex.
           */
         virtual const uci::type::VisibleString64Type& getModulationParameterIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ModulationParameterIndex.
           *
           * @return The value of the string data type identified by ModulationParameterIndex.
           */
         virtual uci::type::VisibleString64Type& getModulationParameterIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ModulationParameterIndex to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setModulationParameterIndex(const uci::type::VisibleString64Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ModulationParameterIndex to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setModulationParameterIndex(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ModulationParameterIndex to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setModulationParameterIndex(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ModulationParameterValue.
           *
           * @return The value of the string data type identified by ModulationParameterValue.
           */
         virtual const uci::type::VisibleString64Type& getModulationParameterValue() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ModulationParameterValue.
           *
           * @return The value of the string data type identified by ModulationParameterValue.
           */
         virtual uci::type::VisibleString64Type& getModulationParameterValue()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ModulationParameterValue to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setModulationParameterValue(const uci::type::VisibleString64Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ModulationParameterValue to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setModulationParameterValue(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ModulationParameterValue to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setModulationParameterValue(const char* value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ModulationParameterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ModulationParameterType to copy from
           */
         ModulationParameterType(const ModulationParameterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ModulationParameterType to the contents of the
           * ModulationParameterType on the right hand side (rhs) of the assignment operator.ModulationParameterType [only
           * available to derived classes]
           *
           * @param rhs The ModulationParameterType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ModulationParameterType
           * @return A constant reference to this ModulationParameterType.
           */
         const ModulationParameterType& operator=(const ModulationParameterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ModulationParameterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ModulationParameterType_h

