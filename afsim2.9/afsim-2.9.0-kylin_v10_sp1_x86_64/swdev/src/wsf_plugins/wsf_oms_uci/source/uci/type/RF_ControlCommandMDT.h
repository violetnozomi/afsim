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
#ifndef Uci__Type__RF_ControlCommandMDT_h
#define Uci__Type__RF_ControlCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RF_ControlCommandID_Type_h)
# include "uci/type/RF_ControlCommandID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RF_ProfileType_h)
# include "uci/type/RF_ProfileType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_ControlCommandMDT sequence accessor class */
      class RF_ControlCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_ControlCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. RF_ControlCommandMDT
           *
           * @return This accessor's type constant, i.e. RF_ControlCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ControlCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ControlCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a RF_Profile to apply to the Capability given by the sibling ApplicableCapabilityID element. When multiple
           * profiles are specified, they should be logically ANDed; the Capability is allowed to operate in the intersection of
           * the profiles. A given RF_ControlCommand must always express the full set of profiles for the target Capability. In
           * other words, RF_ControlCommand is not additive/subtractive; each instance wholly supersedes the previous instance for
           * a given Capability, regardless of whether the RF_ControlCommandID or MessageState changes. If omitted, all existing
           * policies should be removed and the Capability is free to select any frequency within its other constraints. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RF_ProfileType, uci::type::accessorType::rF_ProfileType> Profile;

         /** Returns the accessor that provides access to the complex content that is identified by the RF_ControlCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RF_ControlCommandID.
           */
         virtual const uci::type::RF_ControlCommandID_Type& getRF_ControlCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RF_ControlCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RF_ControlCommandID.
           */
         virtual uci::type::RF_ControlCommandID_Type& getRF_ControlCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RF_ControlCommandID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RF_ControlCommandID
           */
         virtual void setRF_ControlCommandID(const uci::type::RF_ControlCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableCapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getApplicableCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableCapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getApplicableCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApplicableCapabilityID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApplicableCapabilityID
           */
         virtual void setApplicableCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Profile.
           *
           * @return The bounded list identified by Profile.
           */
         virtual const uci::type::RF_ControlCommandMDT::Profile& getProfile() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Profile.
           *
           * @return The bounded list identified by Profile.
           */
         virtual uci::type::RF_ControlCommandMDT::Profile& getProfile()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Profile.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProfile(const uci::type::RF_ControlCommandMDT::Profile& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ControlCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ControlCommandMDT to copy from
           */
         RF_ControlCommandMDT(const RF_ControlCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ControlCommandMDT to the contents of the RF_ControlCommandMDT
           * on the right hand side (rhs) of the assignment operator.RF_ControlCommandMDT [only available to derived classes]
           *
           * @param rhs The RF_ControlCommandMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::RF_ControlCommandMDT
           * @return A constant reference to this RF_ControlCommandMDT.
           */
         const RF_ControlCommandMDT& operator=(const RF_ControlCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ControlCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ControlCommandMDT_h

