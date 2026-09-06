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
#ifndef Uci__Type__POST_ComponentSettingsFocalPlaneArrayType_h
#define Uci__Type__POST_ComponentSettingsFocalPlaneArrayType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_FPA_ID_Type_h)
# include "uci/type/PO_FPA_ID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__POST_WindowID_Type_h)
# include "uci/type/POST_WindowID_Type.h"
#endif

#if !defined(Uci__Type__ComponentControlsA_Type_h)
# include "uci/type/ComponentControlsA_Type.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocalPlaneArrayChannelType_h)
# include "uci/type/PO_ComponentSettingsFocalPlaneArrayChannelType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentSettingsFocalPlaneArrayType sequence accessor class */
      class POST_ComponentSettingsFocalPlaneArrayType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentSettingsFocalPlaneArrayType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentSettingsFocalPlaneArrayType
           *
           * @return This accessor's type constant, i.e. POST_ComponentSettingsFocalPlaneArrayType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentSettingsFocalPlaneArrayType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentSettingsFocalPlaneArrayType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the id of a given subset or window of an FPA. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_WindowID_Type, uci::type::accessorType::pOST_WindowID_Type> WindowSelectionID;

         /** A focal plane array (FPA) is composed of one or more channels that measure the number of photons of a particular
           * range of frequencies that hit the FPA. Depending on the type of FPA, the operational characteristics of its channels
           * may be configurable. This field contains the current state of the channel [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_ComponentSettingsFocalPlaneArrayChannelType, uci::type::accessorType::pO_ComponentSettingsFocalPlaneArrayChannelType> Channel;

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


         /** Returns the bounded list that is identified by the WindowSelectionID.
           *
           * @return The bounded list identified by WindowSelectionID.
           */
         virtual const uci::type::POST_ComponentSettingsFocalPlaneArrayType::WindowSelectionID& getWindowSelectionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WindowSelectionID.
           *
           * @return The bounded list identified by WindowSelectionID.
           */
         virtual uci::type::POST_ComponentSettingsFocalPlaneArrayType::WindowSelectionID& getWindowSelectionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the WindowSelectionID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWindowSelectionID(const uci::type::POST_ComponentSettingsFocalPlaneArrayType::WindowSelectionID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FPA_Controls.
           *
           * @return The acecssor that provides access to the complex content that is identified by FPA_Controls.
           */
         virtual const uci::type::ComponentControlsA_Type& getFPA_Controls() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FPA_Controls.
           *
           * @return The acecssor that provides access to the complex content that is identified by FPA_Controls.
           */
         virtual uci::type::ComponentControlsA_Type& getFPA_Controls()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FPA_Controls to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FPA_Controls
           */
         virtual void setFPA_Controls(const uci::type::ComponentControlsA_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FPA_Controls exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FPA_Controls is emabled or not
           */
         virtual bool hasFPA_Controls() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FPA_Controls
           *
           * @param type = uci::type::accessorType::componentControlsA_Type This Accessor's accessor type
           */
         virtual void enableFPA_Controls(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentControlsA_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FPA_Controls */
         virtual void clearFPA_Controls()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Channel.
           *
           * @return The bounded list identified by Channel.
           */
         virtual const uci::type::POST_ComponentSettingsFocalPlaneArrayType::Channel& getChannel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Channel.
           *
           * @return The bounded list identified by Channel.
           */
         virtual uci::type::POST_ComponentSettingsFocalPlaneArrayType::Channel& getChannel()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Channel.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setChannel(const uci::type::POST_ComponentSettingsFocalPlaneArrayType::Channel& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentSettingsFocalPlaneArrayType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentSettingsFocalPlaneArrayType to copy from
           */
         POST_ComponentSettingsFocalPlaneArrayType(const POST_ComponentSettingsFocalPlaneArrayType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentSettingsFocalPlaneArrayType to the contents of the
           * POST_ComponentSettingsFocalPlaneArrayType on the right hand side (rhs) of the assignment
           * operator.POST_ComponentSettingsFocalPlaneArrayType [only available to derived classes]
           *
           * @param rhs The POST_ComponentSettingsFocalPlaneArrayType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::POST_ComponentSettingsFocalPlaneArrayType
           * @return A constant reference to this POST_ComponentSettingsFocalPlaneArrayType.
           */
         const POST_ComponentSettingsFocalPlaneArrayType& operator=(const POST_ComponentSettingsFocalPlaneArrayType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentSettingsFocalPlaneArrayType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentSettingsFocalPlaneArrayType_h

