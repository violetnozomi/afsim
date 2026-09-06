// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__PO_ConstraintControlsType_h
#define Uci__Type__PO_ConstraintControlsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ConstraintControlsType sequence accessor class */
      class PO_ConstraintControlsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ConstraintControlsType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ConstraintControlsType
           *
           * @return This accessor's type constant, i.e. PO_ConstraintControlsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ConstraintControlsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ConstraintControlsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EnableDisable.
           *
           * @return The value of the simple primitive data type identified by EnableDisable.
           */
         virtual xs::Boolean getEnableDisable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EnableDisable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEnableDisable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EnableDisable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EnableDisable is emabled or not
           */
         virtual bool hasEnableDisable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EnableDisable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableEnableDisable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EnableDisable */
         virtual void clearEnableDisable()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AutoEnableDisable.
           *
           * @return The value of the simple primitive data type identified by AutoEnableDisable.
           */
         virtual xs::Boolean getAutoEnableDisable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AutoEnableDisable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAutoEnableDisable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AutoEnableDisable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AutoEnableDisable is emabled or not
           */
         virtual bool hasAutoEnableDisable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AutoEnableDisable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAutoEnableDisable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AutoEnableDisable */
         virtual void clearAutoEnableDisable()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Reset.
           *
           * @return The value of the string data type identified by Reset.
           */
         virtual const uci::type::EmptyType& getReset() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Reset.
           *
           * @return The value of the string data type identified by Reset.
           */
         virtual uci::type::EmptyType& getReset()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reset to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setReset(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reset to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReset(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reset to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReset(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Reset exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Reset is emabled or not
           */
         virtual bool hasReset() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Reset
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableReset(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Reset */
         virtual void clearReset()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ConstraintControlsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ConstraintControlsType to copy from
           */
         PO_ConstraintControlsType(const PO_ConstraintControlsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ConstraintControlsType to the contents of the
           * PO_ConstraintControlsType on the right hand side (rhs) of the assignment operator.PO_ConstraintControlsType [only
           * available to derived classes]
           *
           * @param rhs The PO_ConstraintControlsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::PO_ConstraintControlsType
           * @return A constant reference to this PO_ConstraintControlsType.
           */
         const PO_ConstraintControlsType& operator=(const PO_ConstraintControlsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ConstraintControlsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ConstraintControlsType_h

