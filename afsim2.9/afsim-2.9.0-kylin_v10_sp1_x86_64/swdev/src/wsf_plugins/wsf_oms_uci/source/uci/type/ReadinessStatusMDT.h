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
#ifndef Uci__Type__ReadinessStatusMDT_h
#define Uci__Type__ReadinessStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ReadinessItemType_h)
# include "uci/type/ReadinessItemType.h"
#endif

#if !defined(Uci__Type__MaintenanceStatusType_h)
# include "uci/type/MaintenanceStatusType.h"
#endif

#if !defined(Uci__Type__TimedZoneType_h)
# include "uci/type/TimedZoneType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ReadinessStatusMDT sequence accessor class */
      class ReadinessStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ReadinessStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ReadinessStatusMDT
           *
           * @return This accessor's type constant, i.e. ReadinessStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::readinessStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ReadinessStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Item.
           *
           * @return The acecssor that provides access to the complex content that is identified by Item.
           */
         virtual const uci::type::ReadinessItemType& getItem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Item.
           *
           * @return The acecssor that provides access to the complex content that is identified by Item.
           */
         virtual uci::type::ReadinessItemType& getItem()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Item to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Item
           */
         virtual void setItem(const uci::type::ReadinessItemType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MaintenanceStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by MaintenanceStatus.
           */
         virtual const uci::type::MaintenanceStatusType& getMaintenanceStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MaintenanceStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by MaintenanceStatus.
           */
         virtual uci::type::MaintenanceStatusType& getMaintenanceStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MaintenanceStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MaintenanceStatus
           */
         virtual void setMaintenanceStatus(const uci::type::MaintenanceStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimedZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimedZone.
           */
         virtual const uci::type::TimedZoneType& getTimedZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimedZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimedZone.
           */
         virtual uci::type::TimedZoneType& getTimedZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimedZone to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimedZone
           */
         virtual void setTimedZone(const uci::type::TimedZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimedZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimedZone is emabled or not
           */
         virtual bool hasTimedZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimedZone
           *
           * @param type = uci::type::accessorType::timedZoneType This Accessor's accessor type
           */
         virtual void enableTimedZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::timedZoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimedZone */
         virtual void clearTimedZone()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ReadinessStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ReadinessStatusMDT to copy from
           */
         ReadinessStatusMDT(const ReadinessStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ReadinessStatusMDT to the contents of the ReadinessStatusMDT on
           * the right hand side (rhs) of the assignment operator.ReadinessStatusMDT [only available to derived classes]
           *
           * @param rhs The ReadinessStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ReadinessStatusMDT
           * @return A constant reference to this ReadinessStatusMDT.
           */
         const ReadinessStatusMDT& operator=(const ReadinessStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ReadinessStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ReadinessStatusMDT_h

