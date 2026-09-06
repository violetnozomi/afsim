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
#ifndef Uci__Type__POST_ComponentFocalPlaneArrayType_h
#define Uci__Type__POST_ComponentFocalPlaneArrayType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_FPA_ID_Type_h)
# include "uci/type/PO_FPA_ID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_ComponentFocalPlaneArrayChannelType_h)
# include "uci/type/POST_ComponentFocalPlaneArrayChannelType.h"
#endif

#if !defined(Uci__Type__POST_ComponentWindowType_h)
# include "uci/type/POST_ComponentWindowType.h"
#endif

#if !defined(Uci__Type__ComponentControllabilityB_Type_h)
# include "uci/type/ComponentControllabilityB_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentFocalPlaneArrayType sequence accessor class */
      class POST_ComponentFocalPlaneArrayType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentFocalPlaneArrayType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentFocalPlaneArrayType
           *
           * @return This accessor's type constant, i.e. POST_ComponentFocalPlaneArrayType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentFocalPlaneArrayType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentFocalPlaneArrayType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Focal plane arrays (FPAs) divide up the optical spectrum into one or more channels, each channel being used to
           * measure the number of photons that fall within a fixed range of the optical spectrum. This fixed range is known as
           * the channel's optical band. The channel's optical band may or may not overlapped with another channel's optical band
           * and the combination of the FPA's channel may or may not completely cover the optical spectrum, i.e. there may be
           * gaps. This field is used to specify what channels are available. Included in this information is the channel's
           * attributes, e.g. the channel's optical band, and the channel's control settings. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ComponentFocalPlaneArrayChannelType, uci::type::accessorType::pOST_ComponentFocalPlaneArrayChannelType> Channel;

         /** A FPA can be a logical FPA that represents a "window" of a larger physical FPA. This defines a sub-window in the FPA.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ComponentWindowType, uci::type::accessorType::pOST_ComponentWindowType> Window;

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


         /** Returns the bounded list that is identified by the Channel.
           *
           * @return The bounded list identified by Channel.
           */
         virtual const uci::type::POST_ComponentFocalPlaneArrayType::Channel& getChannel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Channel.
           *
           * @return The bounded list identified by Channel.
           */
         virtual uci::type::POST_ComponentFocalPlaneArrayType::Channel& getChannel()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Channel.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setChannel(const uci::type::POST_ComponentFocalPlaneArrayType::Channel& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Window.
           *
           * @return The bounded list identified by Window.
           */
         virtual const uci::type::POST_ComponentFocalPlaneArrayType::Window& getWindow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Window.
           *
           * @return The bounded list identified by Window.
           */
         virtual uci::type::POST_ComponentFocalPlaneArrayType::Window& getWindow()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Window.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWindow(const uci::type::POST_ComponentFocalPlaneArrayType::Window& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Controllability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Controllability.
           */
         virtual const uci::type::ComponentControllabilityB_Type& getControllability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Controllability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Controllability.
           */
         virtual uci::type::ComponentControllabilityB_Type& getControllability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Controllability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Controllability
           */
         virtual void setControllability(const uci::type::ComponentControllabilityB_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Controllability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Controllability is emabled or not
           */
         virtual bool hasControllability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Controllability
           *
           * @param type = uci::type::accessorType::componentControllabilityB_Type This Accessor's accessor type
           */
         virtual void enableControllability(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentControllabilityB_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Controllability */
         virtual void clearControllability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentFocalPlaneArrayType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentFocalPlaneArrayType to copy from
           */
         POST_ComponentFocalPlaneArrayType(const POST_ComponentFocalPlaneArrayType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentFocalPlaneArrayType to the contents of the
           * POST_ComponentFocalPlaneArrayType on the right hand side (rhs) of the assignment
           * operator.POST_ComponentFocalPlaneArrayType [only available to derived classes]
           *
           * @param rhs The POST_ComponentFocalPlaneArrayType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::POST_ComponentFocalPlaneArrayType
           * @return A constant reference to this POST_ComponentFocalPlaneArrayType.
           */
         const POST_ComponentFocalPlaneArrayType& operator=(const POST_ComponentFocalPlaneArrayType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentFocalPlaneArrayType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentFocalPlaneArrayType_h

