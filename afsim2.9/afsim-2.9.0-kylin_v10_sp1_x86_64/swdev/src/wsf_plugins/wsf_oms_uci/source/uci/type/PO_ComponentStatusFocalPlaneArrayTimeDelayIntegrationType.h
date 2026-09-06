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
#ifndef Uci__Type__PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType_h
#define Uci__Type__PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType sequence accessor class */
      class PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType
           *
           * @return This accessor's type constant, i.e. PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TDI_Setting.
           *
           * @return The value of the simple primitive data type identified by TDI_Setting.
           */
         virtual xs::Double getTDI_Setting() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TDI_Setting.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTDI_Setting(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AutoTDI.
           *
           * @return The value of the simple primitive data type identified by AutoTDI.
           */
         virtual xs::Boolean getAutoTDI() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AutoTDI.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAutoTDI(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AutoTDI exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AutoTDI is emabled or not
           */
         virtual bool hasAutoTDI() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AutoTDI
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAutoTDI(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AutoTDI */
         virtual void clearAutoTDI()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType to copy from
           */
         PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType(const PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType to the
           * contents of the PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType on the right hand side (rhs) of the
           * assignment operator.PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType on the right hand side (rhs) of the
           *      assignment operator whose contents are used to set the contents of this
           *      uci::type::PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType
           * @return A constant reference to this PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType.
           */
         const PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType& operator=(const PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentStatusFocalPlaneArrayTimeDelayIntegrationType_h

