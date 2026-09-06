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
#ifndef Uci__Type__EW_ConfigurationMDT_h
#define Uci__Type__EW_ConfigurationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RF_BandType_h)
# include "uci/type/RF_BandType.h"
#endif

#if !defined(Uci__Type__ChannelDescriptionType_h)
# include "uci/type/ChannelDescriptionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EW_ConfigurationMDT sequence accessor class */
      class EW_ConfigurationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EW_ConfigurationMDT()
         { }

         /** Returns this accessor's type constant, i.e. EW_ConfigurationMDT
           *
           * @return This accessor's type constant, i.e. EW_ConfigurationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eW_ConfigurationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EW_ConfigurationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This represesnts the RF Band(s) which the subsystem will operate over. RF bands represent the frequency range that a
           * subsystem can operate over with specific combinations of tuners, antennas, and RF distribution components. A
           * subsystem typically can operate over several selectable bands, but has to reconfigure to switch bands. Certain
           * capabilities may be restricted to specific bands. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RF_BandType, uci::type::accessorType::rF_BandType> RFBands;

         /** This element defines the RF channel(s) which are used by a subsystem. The channels are typically defined by tuner
           * characteristics and may be constrained by hardware, firmware, and/or software. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ChannelDescriptionType, uci::type::accessorType::channelDescriptionType> ChannelDescription;

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


         /** Returns the bounded list that is identified by the RFBands.
           *
           * @return The bounded list identified by RFBands.
           */
         virtual const uci::type::EW_ConfigurationMDT::RFBands& getRFBands() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RFBands.
           *
           * @return The bounded list identified by RFBands.
           */
         virtual uci::type::EW_ConfigurationMDT::RFBands& getRFBands()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RFBands.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRFBands(const uci::type::EW_ConfigurationMDT::RFBands& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChannelDescription.
           *
           * @return The bounded list identified by ChannelDescription.
           */
         virtual const uci::type::EW_ConfigurationMDT::ChannelDescription& getChannelDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChannelDescription.
           *
           * @return The bounded list identified by ChannelDescription.
           */
         virtual uci::type::EW_ConfigurationMDT::ChannelDescription& getChannelDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ChannelDescription.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setChannelDescription(const uci::type::EW_ConfigurationMDT::ChannelDescription& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EW_ConfigurationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EW_ConfigurationMDT to copy from
           */
         EW_ConfigurationMDT(const EW_ConfigurationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EW_ConfigurationMDT to the contents of the EW_ConfigurationMDT on
           * the right hand side (rhs) of the assignment operator.EW_ConfigurationMDT [only available to derived classes]
           *
           * @param rhs The EW_ConfigurationMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EW_ConfigurationMDT
           * @return A constant reference to this EW_ConfigurationMDT.
           */
         const EW_ConfigurationMDT& operator=(const EW_ConfigurationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EW_ConfigurationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EW_ConfigurationMDT_h

