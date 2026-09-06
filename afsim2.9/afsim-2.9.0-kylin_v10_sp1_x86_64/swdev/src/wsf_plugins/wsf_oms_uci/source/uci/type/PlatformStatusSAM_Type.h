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
#ifndef Uci__Type__PlatformStatusSAM_Type_h
#define Uci__Type__PlatformStatusSAM_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__SAM_ModeEnum_h)
# include "uci/type/SAM_ModeEnum.h"
#endif

#if !defined(Uci__Type__OperationalImpairmentEnum_h)
# include "uci/type/OperationalImpairmentEnum.h"
#endif

#if !defined(Uci__Type__CommunicationsImpairmentEnum_h)
# include "uci/type/CommunicationsImpairmentEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Platform status details for SAM launch sites. */
      class PlatformStatusSAM_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PlatformStatusSAM_Type()
         { }

         /** Returns this accessor's type constant, i.e. PlatformStatusSAM_Type
           *
           * @return This accessor's type constant, i.e. PlatformStatusSAM_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::platformStatusSAM_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PlatformStatusSAM_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HotInventory.
           *
           * @return The value of the simple primitive data type identified by HotInventory.
           */
         virtual xs::UnsignedInt getHotInventory() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HotInventory.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHotInventory(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HotInventory exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HotInventory is emabled or not
           */
         virtual bool hasHotInventory() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HotInventory
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableHotInventory(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HotInventory */
         virtual void clearHotInventory()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ColdInventory.
           *
           * @return The value of the simple primitive data type identified by ColdInventory.
           */
         virtual xs::UnsignedInt getColdInventory() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ColdInventory.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setColdInventory(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ColdInventory exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ColdInventory is emabled or not
           */
         virtual bool hasColdInventory() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ColdInventory
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableColdInventory(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ColdInventory */
         virtual void clearColdInventory()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SAM_Mode.
           *
           * @return The value of the enumeration identified by SAM_Mode.
           */
         virtual const uci::type::SAM_ModeEnum& getSAM_Mode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SAM_Mode.
           *
           * @return The value of the enumeration identified by SAM_Mode.
           */
         virtual uci::type::SAM_ModeEnum& getSAM_Mode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SAM_Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSAM_Mode(const uci::type::SAM_ModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SAM_Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSAM_Mode(uci::type::SAM_ModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SAM_Modeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSAM_Mode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSAM_Mode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::sAM_ModeEnum This Accessor's accessor type
           */
         virtual void enableSAM_Mode(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAM_ModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSAM_Mode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalImpairment.
           *
           * @return The value of the enumeration identified by OperationalImpairment.
           */
         virtual const uci::type::OperationalImpairmentEnum& getOperationalImpairment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalImpairment.
           *
           * @return The value of the enumeration identified by OperationalImpairment.
           */
         virtual uci::type::OperationalImpairmentEnum& getOperationalImpairment()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalImpairment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalImpairment(const uci::type::OperationalImpairmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalImpairment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalImpairment(uci::type::OperationalImpairmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield OperationalImpairmentis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasOperationalImpairment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getOperationalImpairment will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::operationalImpairmentEnum This Accessor's accessor type
           */
         virtual void enableOperationalImpairment(uci::base::accessorType::AccessorType type = uci::type::accessorType::operationalImpairmentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearOperationalImpairment()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommunicationsImpairment.
           *
           * @return The value of the enumeration identified by CommunicationsImpairment.
           */
         virtual const uci::type::CommunicationsImpairmentEnum& getCommunicationsImpairment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommunicationsImpairment.
           *
           * @return The value of the enumeration identified by CommunicationsImpairment.
           */
         virtual uci::type::CommunicationsImpairmentEnum& getCommunicationsImpairment()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommunicationsImpairment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommunicationsImpairment(const uci::type::CommunicationsImpairmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommunicationsImpairment.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommunicationsImpairment(uci::type::CommunicationsImpairmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield CommunicationsImpairmentis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasCommunicationsImpairment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getCommunicationsImpairment will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::communicationsImpairmentEnum This Accessor's accessor type
           */
         virtual void enableCommunicationsImpairment(uci::base::accessorType::AccessorType type = uci::type::accessorType::communicationsImpairmentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCommunicationsImpairment()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ControlPositions.
           *
           * @return The value of the simple primitive data type identified by ControlPositions.
           */
         virtual xs::UnsignedInt getControlPositions() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ControlPositions.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setControlPositions(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ControlPositions exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ControlPositions is emabled or not
           */
         virtual bool hasControlPositions() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ControlPositions
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableControlPositions(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ControlPositions */
         virtual void clearControlPositions()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PlatformStatusSAM_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PlatformStatusSAM_Type to copy from
           */
         PlatformStatusSAM_Type(const PlatformStatusSAM_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PlatformStatusSAM_Type to the contents of the
           * PlatformStatusSAM_Type on the right hand side (rhs) of the assignment operator.PlatformStatusSAM_Type [only available
           * to derived classes]
           *
           * @param rhs The PlatformStatusSAM_Type on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::PlatformStatusSAM_Type
           * @return A constant reference to this PlatformStatusSAM_Type.
           */
         const PlatformStatusSAM_Type& operator=(const PlatformStatusSAM_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PlatformStatusSAM_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PlatformStatusSAM_Type_h

