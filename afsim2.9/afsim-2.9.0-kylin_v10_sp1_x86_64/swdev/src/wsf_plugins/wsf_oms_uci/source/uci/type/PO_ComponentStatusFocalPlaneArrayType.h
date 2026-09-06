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
#ifndef Uci__Type__PO_ComponentStatusFocalPlaneArrayType_h
#define Uci__Type__PO_ComponentStatusFocalPlaneArrayType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_FPA_ID_Type_h)
# include "uci/type/PO_FPA_ID_Type.h"
#endif

#if !defined(Uci__Type__ComponentControlsStatusType_h)
# include "uci/type/ComponentControlsStatusType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PO_ComponentStatusFocalPlaneArrayChannelType_h)
# include "uci/type/PO_ComponentStatusFocalPlaneArrayChannelType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentStatusFocalPlaneArrayType sequence accessor class */
      class PO_ComponentStatusFocalPlaneArrayType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentStatusFocalPlaneArrayType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentStatusFocalPlaneArrayType
           *
           * @return This accessor's type constant, i.e. PO_ComponentStatusFocalPlaneArrayType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentStatusFocalPlaneArrayType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentStatusFocalPlaneArrayType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A focal plane array (FPA) is composed of one or more channels that measure the number of photons of a particular
           * range of frequencies that hit the FPA. Depending on the type of FPA, the operational characteristics of its channels
           * may be configurable. This field contains the current state of the channel [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_ComponentStatusFocalPlaneArrayChannelType, uci::type::accessorType::pO_ComponentStatusFocalPlaneArrayChannelType> Channel;

         /** Returns the accessor that provides access to the complex content that is identified by the FocalPlaneArrayID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FocalPlaneArrayID.
           */
         virtual const uci::type::PO_FPA_ID_Type& getFocalPlaneArrayID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FocalPlaneArrayID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FocalPlaneArrayID.
           */
         virtual uci::type::PO_FPA_ID_Type& getFocalPlaneArrayID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FocalPlaneArrayID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FocalPlaneArrayID
           */
         virtual void setFocalPlaneArrayID(const uci::type::PO_FPA_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FPA_ControlsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by FPA_ControlsStatus.
           */
         virtual const uci::type::ComponentControlsStatusType& getFPA_ControlsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FPA_ControlsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by FPA_ControlsStatus.
           */
         virtual uci::type::ComponentControlsStatusType& getFPA_ControlsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FPA_ControlsStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FPA_ControlsStatus
           */
         virtual void setFPA_ControlsStatus(const uci::type::ComponentControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Channel.
           *
           * @return The bounded list identified by Channel.
           */
         virtual const uci::type::PO_ComponentStatusFocalPlaneArrayType::Channel& getChannel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Channel.
           *
           * @return The bounded list identified by Channel.
           */
         virtual uci::type::PO_ComponentStatusFocalPlaneArrayType::Channel& getChannel()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Channel.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setChannel(const uci::type::PO_ComponentStatusFocalPlaneArrayType::Channel& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentStatusFocalPlaneArrayType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusFocalPlaneArrayType to copy from
           */
         PO_ComponentStatusFocalPlaneArrayType(const PO_ComponentStatusFocalPlaneArrayType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentStatusFocalPlaneArrayType to the contents of the
           * PO_ComponentStatusFocalPlaneArrayType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentStatusFocalPlaneArrayType [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusFocalPlaneArrayType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::PO_ComponentStatusFocalPlaneArrayType
           * @return A constant reference to this PO_ComponentStatusFocalPlaneArrayType.
           */
         const PO_ComponentStatusFocalPlaneArrayType& operator=(const PO_ComponentStatusFocalPlaneArrayType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentStatusFocalPlaneArrayType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentStatusFocalPlaneArrayType_h

