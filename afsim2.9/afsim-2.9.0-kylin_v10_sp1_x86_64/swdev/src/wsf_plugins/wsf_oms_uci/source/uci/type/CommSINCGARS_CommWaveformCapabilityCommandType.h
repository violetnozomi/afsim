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
#ifndef Uci__Type__CommSINCGARS_CommWaveformCapabilityCommandType_h
#define Uci__Type__CommSINCGARS_CommWaveformCapabilityCommandType_h 1

#if !defined(Uci__Type__CommWaveformCapabilityCommandPET_h)
# include "uci/type/CommWaveformCapabilityCommandPET.h"
#endif

#if !defined(Uci__Type__CommSINCGARS_ControlType_h)
# include "uci/type/CommSINCGARS_ControlType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base CommWaveformCapabilityCommandPET for
        * communication SINCGARS parameters.
        */
      class CommSINCGARS_CommWaveformCapabilityCommandType : public virtual uci::type::CommWaveformCapabilityCommandPET {
      public:

         /** The destructor */
         virtual ~CommSINCGARS_CommWaveformCapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. CommSINCGARS_CommWaveformCapabilityCommandType
           *
           * @return This accessor's type constant, i.e. CommSINCGARS_CommWaveformCapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commSINCGARS_CommWaveformCapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommSINCGARS_CommWaveformCapabilityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by Settings.
           */
         virtual const uci::type::CommSINCGARS_ControlType& getSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Settings.
           *
           * @return The acecssor that provides access to the complex content that is identified by Settings.
           */
         virtual uci::type::CommSINCGARS_ControlType& getSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Settings to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Settings
           */
         virtual void setSettings(const uci::type::CommSINCGARS_ControlType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommSINCGARS_CommWaveformCapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommSINCGARS_CommWaveformCapabilityCommandType to copy from
           */
         CommSINCGARS_CommWaveformCapabilityCommandType(const CommSINCGARS_CommWaveformCapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommSINCGARS_CommWaveformCapabilityCommandType to the contents of
           * the CommSINCGARS_CommWaveformCapabilityCommandType on the right hand side (rhs) of the assignment
           * operator.CommSINCGARS_CommWaveformCapabilityCommandType [only available to derived classes]
           *
           * @param rhs The CommSINCGARS_CommWaveformCapabilityCommandType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::CommSINCGARS_CommWaveformCapabilityCommandType
           * @return A constant reference to this CommSINCGARS_CommWaveformCapabilityCommandType.
           */
         const CommSINCGARS_CommWaveformCapabilityCommandType& operator=(const CommSINCGARS_CommWaveformCapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommSINCGARS_CommWaveformCapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommSINCGARS_CommWaveformCapabilityCommandType_h

