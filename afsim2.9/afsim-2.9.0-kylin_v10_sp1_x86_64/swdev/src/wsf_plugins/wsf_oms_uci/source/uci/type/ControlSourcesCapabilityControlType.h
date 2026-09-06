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
#ifndef Uci__Type__ControlSourcesCapabilityControlType_h
#define Uci__Type__ControlSourcesCapabilityControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__PrimaryControllerType_h)
# include "uci/type/PrimaryControllerType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SecondaryControllerType_h)
# include "uci/type/SecondaryControllerType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ControlSourcesCapabilityControlType sequence accessor class */
      class ControlSourcesCapabilityControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ControlSourcesCapabilityControlType()
         { }

         /** Returns this accessor's type constant, i.e. ControlSourcesCapabilityControlType
           *
           * @return This accessor's type constant, i.e. ControlSourcesCapabilityControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::controlSourcesCapabilityControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ControlSourcesCapabilityControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a secondary controller of the Capability. See ControlStatus message annotations for further details.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SecondaryControllerType, uci::type::accessorType::secondaryControllerType> SecondaryController;

         /** Returns the accessor that provides access to the complex content that is identified by the ControleeCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControleeCapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getControleeCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControleeCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControleeCapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getControleeCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ControleeCapabilityID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ControleeCapabilityID
           */
         virtual void setControleeCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PrimaryController.
           *
           * @return The acecssor that provides access to the complex content that is identified by PrimaryController.
           */
         virtual const uci::type::PrimaryControllerType& getPrimaryController() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PrimaryController.
           *
           * @return The acecssor that provides access to the complex content that is identified by PrimaryController.
           */
         virtual uci::type::PrimaryControllerType& getPrimaryController()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PrimaryController to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PrimaryController
           */
         virtual void setPrimaryController(const uci::type::PrimaryControllerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecondaryController.
           *
           * @return The bounded list identified by SecondaryController.
           */
         virtual const uci::type::ControlSourcesCapabilityControlType::SecondaryController& getSecondaryController() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SecondaryController.
           *
           * @return The bounded list identified by SecondaryController.
           */
         virtual uci::type::ControlSourcesCapabilityControlType::SecondaryController& getSecondaryController()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SecondaryController.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSecondaryController(const uci::type::ControlSourcesCapabilityControlType::SecondaryController& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlSourcesCapabilityControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlSourcesCapabilityControlType to copy from
           */
         ControlSourcesCapabilityControlType(const ControlSourcesCapabilityControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlSourcesCapabilityControlType to the contents of the
           * ControlSourcesCapabilityControlType on the right hand side (rhs) of the assignment
           * operator.ControlSourcesCapabilityControlType [only available to derived classes]
           *
           * @param rhs The ControlSourcesCapabilityControlType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ControlSourcesCapabilityControlType
           * @return A constant reference to this ControlSourcesCapabilityControlType.
           */
         const ControlSourcesCapabilityControlType& operator=(const ControlSourcesCapabilityControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlSourcesCapabilityControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ControlSourcesCapabilityControlType_h

