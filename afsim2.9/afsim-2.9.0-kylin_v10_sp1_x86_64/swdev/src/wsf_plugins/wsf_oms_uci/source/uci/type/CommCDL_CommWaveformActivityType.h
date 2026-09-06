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
#ifndef Uci__Type__CommCDL_CommWaveformActivityType_h
#define Uci__Type__CommCDL_CommWaveformActivityType_h 1

#if !defined(Uci__Type__CommWaveformActivityPET_h)
# include "uci/type/CommWaveformActivityPET.h"
#endif

#if !defined(Uci__Type__CommCDL_ControlType_h)
# include "uci/type/CommCDL_ControlType.h"
#endif

#if !defined(Uci__Type__CommCDL_LinkFallbackStateEnum_h)
# include "uci/type/CommCDL_LinkFallbackStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base CommWaveformActivityPET for communication
        * CDL parameters.
        */
      class CommCDL_CommWaveformActivityType : public virtual uci::type::CommWaveformActivityPET {
      public:

         /** The destructor */
         virtual ~CommCDL_CommWaveformActivityType()
         { }

         /** Returns this accessor's type constant, i.e. CommCDL_CommWaveformActivityType
           *
           * @return This accessor's type constant, i.e. CommCDL_CommWaveformActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commCDL_CommWaveformActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommCDL_CommWaveformActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SettingsStatus.
           */
         virtual const uci::type::CommCDL_ControlType& getSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SettingsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by SettingsStatus.
           */
         virtual uci::type::CommCDL_ControlType& getSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SettingsStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SettingsStatus
           */
         virtual void setSettingsStatus(const uci::type::CommCDL_ControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SettingsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SettingsStatus is emabled or not
           */
         virtual bool hasSettingsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SettingsStatus
           *
           * @param type = uci::type::accessorType::commCDL_ControlType This Accessor's accessor type
           */
         virtual void enableSettingsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::commCDL_ControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SettingsStatus */
         virtual void clearSettingsStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LinkFallbackState.
           *
           * @return The value of the enumeration identified by LinkFallbackState.
           */
         virtual const uci::type::CommCDL_LinkFallbackStateEnum& getLinkFallbackState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LinkFallbackState.
           *
           * @return The value of the enumeration identified by LinkFallbackState.
           */
         virtual uci::type::CommCDL_LinkFallbackStateEnum& getLinkFallbackState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LinkFallbackState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLinkFallbackState(const uci::type::CommCDL_LinkFallbackStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LinkFallbackState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLinkFallbackState(uci::type::CommCDL_LinkFallbackStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield LinkFallbackStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLinkFallbackState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLinkFallbackState will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::commCDL_LinkFallbackStateEnum This Accessor's accessor type
           */
         virtual void enableLinkFallbackState(uci::base::accessorType::AccessorType type = uci::type::accessorType::commCDL_LinkFallbackStateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLinkFallbackState()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommCDL_CommWaveformActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommCDL_CommWaveformActivityType to copy from
           */
         CommCDL_CommWaveformActivityType(const CommCDL_CommWaveformActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommCDL_CommWaveformActivityType to the contents of the
           * CommCDL_CommWaveformActivityType on the right hand side (rhs) of the assignment
           * operator.CommCDL_CommWaveformActivityType [only available to derived classes]
           *
           * @param rhs The CommCDL_CommWaveformActivityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommCDL_CommWaveformActivityType
           * @return A constant reference to this CommCDL_CommWaveformActivityType.
           */
         const CommCDL_CommWaveformActivityType& operator=(const CommCDL_CommWaveformActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommCDL_CommWaveformActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommCDL_CommWaveformActivityType_h

