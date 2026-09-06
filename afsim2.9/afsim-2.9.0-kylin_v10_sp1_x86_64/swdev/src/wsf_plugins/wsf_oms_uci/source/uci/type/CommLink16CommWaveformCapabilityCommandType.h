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
#ifndef Uci__Type__CommLink16CommWaveformCapabilityCommandType_h
#define Uci__Type__CommLink16CommWaveformCapabilityCommandType_h 1

#if !defined(Uci__Type__CommWaveformCapabilityCommandPET_h)
# include "uci/type/CommWaveformCapabilityCommandPET.h"
#endif

#if !defined(Uci__Type__CommLink16ControlType_h)
# include "uci/type/CommLink16ControlType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base CommWaveformCapabilityCommandPET for
        * communication Link16 parameters.
        */
      class CommLink16CommWaveformCapabilityCommandType : public virtual uci::type::CommWaveformCapabilityCommandPET {
      public:

         /** The destructor */
         virtual ~CommLink16CommWaveformCapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16CommWaveformCapabilityCommandType
           *
           * @return This accessor's type constant, i.e. CommLink16CommWaveformCapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16CommWaveformCapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16CommWaveformCapabilityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by Settings.
           */
         virtual const uci::type::CommLink16ControlType& getSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by Settings.
           */
         virtual uci::type::CommLink16ControlType& getSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Settings to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Settings
           */
         virtual void setSettings(const uci::type::CommLink16ControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Settings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Settings is emabled or not
           */
         virtual bool hasSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Settings
           *
           * @param type = uci::type::accessorType::commLink16ControlType This Accessor's accessor type
           */
         virtual void enableSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::commLink16ControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Settings */
         virtual void clearSettings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16CommWaveformCapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16CommWaveformCapabilityCommandType to copy from
           */
         CommLink16CommWaveformCapabilityCommandType(const CommLink16CommWaveformCapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16CommWaveformCapabilityCommandType to the contents of the
           * CommLink16CommWaveformCapabilityCommandType on the right hand side (rhs) of the assignment
           * operator.CommLink16CommWaveformCapabilityCommandType [only available to derived classes]
           *
           * @param rhs The CommLink16CommWaveformCapabilityCommandType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::CommLink16CommWaveformCapabilityCommandType
           * @return A constant reference to this CommLink16CommWaveformCapabilityCommandType.
           */
         const CommLink16CommWaveformCapabilityCommandType& operator=(const CommLink16CommWaveformCapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16CommWaveformCapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16CommWaveformCapabilityCommandType_h

