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
#ifndef Uci__Type__EmitterFunctionType_h
#define Uci__Type__EmitterFunctionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EmitterFunctionType sequence accessor class */
      class EmitterFunctionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EmitterFunctionType()
         { }

         /** Returns this accessor's type constant, i.e. EmitterFunctionType
           *
           * @return This accessor's type constant, i.e. EmitterFunctionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::emitterFunctionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EmitterFunctionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EmitterFunction.
           *
           * @return The value of the simple primitive data type identified by EmitterFunction.
           */
         virtual xs::UnsignedInt getEmitterFunction() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EmitterFunction.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEmitterFunction(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CustomEmitterFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CustomEmitterFunction.
           */
         virtual const uci::type::ForeignKeyType& getCustomEmitterFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CustomEmitterFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by CustomEmitterFunction.
           */
         virtual uci::type::ForeignKeyType& getCustomEmitterFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CustomEmitterFunction to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CustomEmitterFunction
           */
         virtual void setCustomEmitterFunction(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CustomEmitterFunction exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CustomEmitterFunction is emabled or not
           */
         virtual bool hasCustomEmitterFunction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CustomEmitterFunction
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableCustomEmitterFunction(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CustomEmitterFunction */
         virtual void clearCustomEmitterFunction()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CustomEmitterFunctionDescription.
           *
           * @return The value of the string data type identified by CustomEmitterFunctionDescription.
           */
         virtual const uci::type::VisibleString256Type& getCustomEmitterFunctionDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CustomEmitterFunctionDescription.
           *
           * @return The value of the string data type identified by CustomEmitterFunctionDescription.
           */
         virtual uci::type::VisibleString256Type& getCustomEmitterFunctionDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CustomEmitterFunctionDescription to the string
           * accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCustomEmitterFunctionDescription(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CustomEmitterFunctionDescription to the specified
           * string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCustomEmitterFunctionDescription(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CustomEmitterFunctionDescription to the specified
           * string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCustomEmitterFunctionDescription(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CustomEmitterFunctionDescription exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CustomEmitterFunctionDescription is emabled or not
           */
         virtual bool hasCustomEmitterFunctionDescription() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CustomEmitterFunctionDescription
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableCustomEmitterFunctionDescription(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CustomEmitterFunctionDescription */
         virtual void clearCustomEmitterFunctionDescription()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EmitterFunctionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EmitterFunctionType to copy from
           */
         EmitterFunctionType(const EmitterFunctionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EmitterFunctionType to the contents of the EmitterFunctionType on
           * the right hand side (rhs) of the assignment operator.EmitterFunctionType [only available to derived classes]
           *
           * @param rhs The EmitterFunctionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EmitterFunctionType
           * @return A constant reference to this EmitterFunctionType.
           */
         const EmitterFunctionType& operator=(const EmitterFunctionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EmitterFunctionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EmitterFunctionType_h

