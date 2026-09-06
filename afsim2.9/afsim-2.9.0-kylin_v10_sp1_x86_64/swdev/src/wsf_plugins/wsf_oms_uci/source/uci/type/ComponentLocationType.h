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
#ifndef Uci__Type__ComponentLocationType_h
#define Uci__Type__ComponentLocationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ComponentLocationType sequence accessor class */
      class ComponentLocationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ComponentLocationType()
         { }

         /** Returns this accessor's type constant, i.e. ComponentLocationType
           *
           * @return This accessor's type constant, i.e. ComponentLocationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::componentLocationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ComponentLocationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OffsetX.
           *
           * @return The value of the simple primitive data type identified by OffsetX.
           */
         virtual xs::Float getOffsetX() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OffsetX.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOffsetX(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OffsetY.
           *
           * @return The value of the simple primitive data type identified by OffsetY.
           */
         virtual xs::Float getOffsetY() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OffsetY.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOffsetY(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OffsetZ.
           *
           * @return The value of the simple primitive data type identified by OffsetZ.
           */
         virtual xs::Float getOffsetZ() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OffsetZ.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOffsetZ(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LocationID.
           */
         virtual const uci::type::ForeignKeyType& getLocationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LocationID.
           */
         virtual uci::type::ForeignKeyType& getLocationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LocationID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LocationID
           */
         virtual void setLocationID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LocationID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LocationID is emabled or not
           */
         virtual bool hasLocationID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LocationID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableLocationID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LocationID */
         virtual void clearLocationID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ComponentLocationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ComponentLocationType to copy from
           */
         ComponentLocationType(const ComponentLocationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ComponentLocationType to the contents of the ComponentLocationType
           * on the right hand side (rhs) of the assignment operator.ComponentLocationType [only available to derived classes]
           *
           * @param rhs The ComponentLocationType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ComponentLocationType
           * @return A constant reference to this ComponentLocationType.
           */
         const ComponentLocationType& operator=(const ComponentLocationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ComponentLocationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ComponentLocationType_h

