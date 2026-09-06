// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SignalReportMDT_h
#define Uci__Type__SignalReportMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SignalReportID_Type_h)
# include "uci/type/SignalReportID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SignalType_h)
# include "uci/type/SignalType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalReportMDT sequence accessor class */
      class SignalReportMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalReportMDT()
         { }

         /** Returns this accessor's type constant, i.e. SignalReportMDT
           *
           * @return This accessor's type constant, i.e. SignalReportMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalReportMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalReportMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a collection of pulses with similar characteristics (frequency, pulsewidth, PRI, etc.) that are grouped
           * together, analyzed, and summarized. Signal characteristics can build up over time; additional characteristics may be
           * added to the initial report of the Signal, such as identity, activity, collection time, collection method, scan data.
           * and location. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SignalType, uci::type::accessorType::signalType> Signal;

         /** Returns the accessor that provides access to the complex content that is identified by the SignalReportID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalReportID.
           */
         virtual const uci::type::SignalReportID_Type& getSignalReportID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalReportID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalReportID.
           */
         virtual uci::type::SignalReportID_Type& getSignalReportID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalReportID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalReportID
           */
         virtual void setSignalReportID(const uci::type::SignalReportID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SystemID is emabled or not
           */
         virtual bool hasSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SystemID */
         virtual void clearSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SubsystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SubsystemID is emabled or not
           */
         virtual bool hasSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SubsystemID
           *
           * @param type = uci::type::accessorType::subsystemID_Type This Accessor's accessor type
           */
         virtual void enableSubsystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::subsystemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SubsystemID */
         virtual void clearSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Signal.
           *
           * @return The bounded list identified by Signal.
           */
         virtual const uci::type::SignalReportMDT::Signal& getSignal() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Signal.
           *
           * @return The bounded list identified by Signal.
           */
         virtual uci::type::SignalReportMDT::Signal& getSignal()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Signal.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSignal(const uci::type::SignalReportMDT::Signal& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalReportMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalReportMDT to copy from
           */
         SignalReportMDT(const SignalReportMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalReportMDT to the contents of the SignalReportMDT on the
           * right hand side (rhs) of the assignment operator.SignalReportMDT [only available to derived classes]
           *
           * @param rhs The SignalReportMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::SignalReportMDT
           * @return A constant reference to this SignalReportMDT.
           */
         const SignalReportMDT& operator=(const SignalReportMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalReportMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalReportMDT_h

