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
#ifndef Uci__Type__IFF_Mode4Type_h
#define Uci__Type__IFF_Mode4Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__InterrogationStatusEnum_h)
# include "uci/type/InterrogationStatusEnum.h"
#endif

#if !defined(Uci__Type__Mode4CodeEnum_h)
# include "uci/type/Mode4CodeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the IFF_Mode4Type sequence accessor class */
      class IFF_Mode4Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~IFF_Mode4Type()
         { }

         /** Returns this accessor's type constant, i.e. IFF_Mode4Type
           *
           * @return This accessor's type constant, i.e. IFF_Mode4Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::iFF_Mode4Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const IFF_Mode4Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mode4Indicator.
           *
           * @return The value of the enumeration identified by Mode4Indicator.
           */
         virtual const uci::type::InterrogationStatusEnum& getMode4Indicator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mode4Indicator.
           *
           * @return The value of the enumeration identified by Mode4Indicator.
           */
         virtual uci::type::InterrogationStatusEnum& getMode4Indicator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mode4Indicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMode4Indicator(const uci::type::InterrogationStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mode4Indicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMode4Indicator(uci::type::InterrogationStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Mode4Indicatoris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasMode4Indicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getMode4Indicator will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::interrogationStatusEnum This Accessor's accessor type
           */
         virtual void enableMode4Indicator(uci::base::accessorType::AccessorType type = uci::type::accessorType::interrogationStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearMode4Indicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mode4Code.
           *
           * @return The value of the enumeration identified by Mode4Code.
           */
         virtual const uci::type::Mode4CodeEnum& getMode4Code() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Mode4Code.
           *
           * @return The value of the enumeration identified by Mode4Code.
           */
         virtual uci::type::Mode4CodeEnum& getMode4Code()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mode4Code.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMode4Code(const uci::type::Mode4CodeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Mode4Code.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMode4Code(uci::type::Mode4CodeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Mode4Codeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasMode4Code() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getMode4Code will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::mode4CodeEnum This Accessor's accessor type
           */
         virtual void enableMode4Code(uci::base::accessorType::AccessorType type = uci::type::accessorType::mode4CodeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearMode4Code()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IFF_Mode4Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IFF_Mode4Type to copy from
           */
         IFF_Mode4Type(const IFF_Mode4Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IFF_Mode4Type to the contents of the IFF_Mode4Type on the right
           * hand side (rhs) of the assignment operator.IFF_Mode4Type [only available to derived classes]
           *
           * @param rhs The IFF_Mode4Type on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::IFF_Mode4Type
           * @return A constant reference to this IFF_Mode4Type.
           */
         const IFF_Mode4Type& operator=(const IFF_Mode4Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IFF_Mode4Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__IFF_Mode4Type_h

