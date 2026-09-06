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
#ifndef Uci__Type__FaultID_Type_h
#define Uci__Type__FaultID_Type_h 1

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString64Type_h)
# include "uci/type/VisibleString64Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A fault id should be unique for every fault. DescriptiveLabel should have as much detailedl information as possible
        * about the fault. The name should be something short that fits nicely in a display where as description can be long
        * and help the user really understand the fault. For example, DescriptiveLabel - "Temp Over Heat in Oil pump engine 1
        * inlet", Name - "Temp Over Heat Eng 1".
        */
      class FaultID_Type : public virtual uci::type::ID_Type {
      public:

         /** The destructor */
         virtual ~FaultID_Type()
         { }

         /** Returns this accessor's type constant, i.e. FaultID_Type
           *
           * @return This accessor's type constant, i.e. FaultID_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::faultID_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FaultID_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual const uci::type::VisibleString64Type& getName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Name.
           *
           * @return The value of the string data type identified by Name.
           */
         virtual uci::type::VisibleString64Type& getName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Name to the string accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setName(const uci::type::VisibleString64Type& value)
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


         /** Returns whether the Element that is identified by Name exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Name is emabled or not
           */
         virtual bool hasName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Name
           *
           * @param type = uci::type::accessorType::visibleString64Type This Accessor's accessor type
           */
         virtual void enableName(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString64Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Name */
         virtual void clearName()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FaultID_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FaultID_Type to copy from
           */
         FaultID_Type(const FaultID_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FaultID_Type to the contents of the FaultID_Type on the right hand
           * side (rhs) of the assignment operator.FaultID_Type [only available to derived classes]
           *
           * @param rhs The FaultID_Type on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FaultID_Type
           * @return A constant reference to this FaultID_Type.
           */
         const FaultID_Type& operator=(const FaultID_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FaultID_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FaultID_Type_h

