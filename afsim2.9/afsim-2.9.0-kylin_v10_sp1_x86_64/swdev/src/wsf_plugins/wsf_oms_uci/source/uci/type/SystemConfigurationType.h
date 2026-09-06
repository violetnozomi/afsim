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
#ifndef Uci__Type__SystemConfigurationType_h
#define Uci__Type__SystemConfigurationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__VehicleConfigurationType_h)
# include "uci/type/VehicleConfigurationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SystemConfigurationType sequence accessor class */
      class SystemConfigurationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SystemConfigurationType()
         { }

         /** Returns this accessor's type constant, i.e. SystemConfigurationType
           *
           * @return This accessor's type constant, i.e. SystemConfigurationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::systemConfigurationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SystemConfigurationType& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the Configuration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Configuration.
           */
         virtual const uci::type::VehicleConfigurationType& getConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Configuration.
           *
           * @return The acecssor that provides access to the complex content that is identified by Configuration.
           */
         virtual uci::type::VehicleConfigurationType& getConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Configuration to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Configuration
           */
         virtual void setConfiguration(const uci::type::VehicleConfigurationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Configuration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Configuration is emabled or not
           */
         virtual bool hasConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Configuration
           *
           * @param type = uci::type::accessorType::vehicleConfigurationType This Accessor's accessor type
           */
         virtual void enableConfiguration(uci::base::accessorType::AccessorType type = uci::type::accessorType::vehicleConfigurationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Configuration */
         virtual void clearConfiguration()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemConfigurationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemConfigurationType to copy from
           */
         SystemConfigurationType(const SystemConfigurationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemConfigurationType to the contents of the
           * SystemConfigurationType on the right hand side (rhs) of the assignment operator.SystemConfigurationType [only
           * available to derived classes]
           *
           * @param rhs The SystemConfigurationType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SystemConfigurationType
           * @return A constant reference to this SystemConfigurationType.
           */
         const SystemConfigurationType& operator=(const SystemConfigurationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemConfigurationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SystemConfigurationType_h

