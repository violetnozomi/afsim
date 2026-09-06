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
#ifndef Uci__Type__OperatorActionAutonomousType_h
#define Uci__Type__OperatorActionAutonomousType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VisibleString32Type_h)
# include "uci/type/VisibleString32Type.h"
#endif

#if !defined(Uci__Type__VisibleString64Type_h)
# include "uci/type/VisibleString64Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorActionAutonomousType sequence accessor class */
      class OperatorActionAutonomousType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperatorActionAutonomousType()
         { }

         /** Returns this accessor's type constant, i.e. OperatorActionAutonomousType
           *
           * @return This accessor's type constant, i.e. OperatorActionAutonomousType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorActionAutonomousType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorActionAutonomousType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Title.
           *
           * @return The value of the string data type identified by Title.
           */
         virtual const uci::type::VisibleString32Type& getTitle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Title.
           *
           * @return The value of the string data type identified by Title.
           */
         virtual uci::type::VisibleString32Type& getTitle()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Title to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setTitle(const uci::type::VisibleString32Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Title to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTitle(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Title to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setTitle(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Description.
           *
           * @return The value of the string data type identified by Description.
           */
         virtual const uci::type::VisibleString64Type& getDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Description.
           *
           * @return The value of the string data type identified by Description.
           */
         virtual uci::type::VisibleString64Type& getDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Description to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setDescription(const uci::type::VisibleString64Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Description to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDescription(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Description to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setDescription(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Description exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Description is emabled or not
           */
         virtual bool hasDescription() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Description
           *
           * @param type = uci::type::accessorType::visibleString64Type This Accessor's accessor type
           */
         virtual void enableDescription(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString64Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Description */
         virtual void clearDescription()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ExecutionTime.
           *
           * @return The value of the simple primitive data type identified by ExecutionTime.
           */
         virtual uci::type::DateTimeTypeValue getExecutionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ExecutionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExecutionTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ExecutionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ExecutionTime is emabled or not
           */
         virtual bool hasExecutionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ExecutionTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableExecutionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ExecutionTime */
         virtual void clearExecutionTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorActionAutonomousType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorActionAutonomousType to copy from
           */
         OperatorActionAutonomousType(const OperatorActionAutonomousType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorActionAutonomousType to the contents of the
           * OperatorActionAutonomousType on the right hand side (rhs) of the assignment operator.OperatorActionAutonomousType
           * [only available to derived classes]
           *
           * @param rhs The OperatorActionAutonomousType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::OperatorActionAutonomousType
           * @return A constant reference to this OperatorActionAutonomousType.
           */
         const OperatorActionAutonomousType& operator=(const OperatorActionAutonomousType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorActionAutonomousType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorActionAutonomousType_h

