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
#ifndef Uci__Type__CommPortSettingsType_h
#define Uci__Type__CommPortSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__IO_PortOpenCloseStateEnum_h)
# include "uci/type/IO_PortOpenCloseStateEnum.h"
#endif

#if !defined(Uci__Type__DataRateType_h)
# include "uci/type/DataRateType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__ParityTypeEnum_h)
# include "uci/type/ParityTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommPortSettingsType sequence accessor class */
      class CommPortSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommPortSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. CommPortSettingsType
           *
           * @return This accessor's type constant, i.e. CommPortSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commPortSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommPortSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComPortID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComPortID.
           */
         virtual const uci::type::ForeignKeyType& getComPortID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComPortID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComPortID.
           */
         virtual uci::type::ForeignKeyType& getComPortID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComPortID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComPortID
           */
         virtual void setComPortID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ComPortID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ComPortID is emabled or not
           */
         virtual bool hasComPortID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ComPortID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableComPortID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ComPortID */
         virtual void clearComPortID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OpenCloseState.
           *
           * @return The value of the enumeration identified by OpenCloseState.
           */
         virtual const uci::type::IO_PortOpenCloseStateEnum& getOpenCloseState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OpenCloseState.
           *
           * @return The value of the enumeration identified by OpenCloseState.
           */
         virtual uci::type::IO_PortOpenCloseStateEnum& getOpenCloseState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OpenCloseState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOpenCloseState(const uci::type::IO_PortOpenCloseStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OpenCloseState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOpenCloseState(uci::type::IO_PortOpenCloseStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BaudRate.
           *
           * @return The value of the simple primitive data type identified by BaudRate.
           */
         virtual uci::type::DataRateTypeValue getBaudRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BaudRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBaudRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CharWidth.
           *
           * @return The value of the simple primitive data type identified by CharWidth.
           */
         virtual xs::Int getCharWidth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CharWidth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCharWidth(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Parity.
           *
           * @return The value of the enumeration identified by Parity.
           */
         virtual const uci::type::ParityTypeEnum& getParity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Parity.
           *
           * @return The value of the enumeration identified by Parity.
           */
         virtual uci::type::ParityTypeEnum& getParity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Parity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setParity(const uci::type::ParityTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Parity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setParity(uci::type::ParityTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StopBits.
           *
           * @return The value of the simple primitive data type identified by StopBits.
           */
         virtual xs::Int getStopBits() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StopBits.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStopBits(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StopBits exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StopBits is emabled or not
           */
         virtual bool hasStopBits() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StopBits
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableStopBits(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StopBits */
         virtual void clearStopBits()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommPortSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommPortSettingsType to copy from
           */
         CommPortSettingsType(const CommPortSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommPortSettingsType to the contents of the CommPortSettingsType
           * on the right hand side (rhs) of the assignment operator.CommPortSettingsType [only available to derived classes]
           *
           * @param rhs The CommPortSettingsType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CommPortSettingsType
           * @return A constant reference to this CommPortSettingsType.
           */
         const CommPortSettingsType& operator=(const CommPortSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommPortSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommPortSettingsType_h

