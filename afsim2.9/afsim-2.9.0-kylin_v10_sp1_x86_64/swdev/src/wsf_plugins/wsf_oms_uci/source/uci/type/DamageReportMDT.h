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
#ifndef Uci__Type__DamageReportMDT_h
#define Uci__Type__DamageReportMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DamageReportID_Type_h)
# include "uci/type/DamageReportID_Type.h"
#endif

#if !defined(Uci__Type__DamageSubjectType_h)
# include "uci/type/DamageSubjectType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DamageReportDetailType_h)
# include "uci/type/DamageReportDetailType.h"
#endif

#if !defined(Uci__Type__DamageInformationSourceType_h)
# include "uci/type/DamageInformationSourceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageReportMDT sequence accessor class */
      class DamageReportMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageReportMDT()
         { }

         /** Returns this accessor's type constant, i.e. DamageReportMDT
           *
           * @return This accessor's type constant, i.e. DamageReportMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageReportMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageReportMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The collection of various types of information affected by damage. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DamageReportDetailType, uci::type::accessorType::damageReportDetailType> DamageDetails;

         /** Returns the accessor that provides access to the complex content that is identified by the DamageReportID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageReportID.
           */
         virtual const uci::type::DamageReportID_Type& getDamageReportID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageReportID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageReportID.
           */
         virtual uci::type::DamageReportID_Type& getDamageReportID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamageReportID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamageReportID
           */
         virtual void setDamageReportID(const uci::type::DamageReportID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Subject.
           *
           * @return The acecssor that provides access to the complex content that is identified by Subject.
           */
         virtual const uci::type::DamageSubjectType& getSubject() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Subject.
           *
           * @return The acecssor that provides access to the complex content that is identified by Subject.
           */
         virtual uci::type::DamageSubjectType& getSubject()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Subject to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Subject
           */
         virtual void setSubject(const uci::type::DamageSubjectType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReportCompletionTime.
           *
           * @return The value of the simple primitive data type identified by ReportCompletionTime.
           */
         virtual uci::type::DateTimeTypeValue getReportCompletionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReportCompletionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReportCompletionTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReportZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReportZone.
           */
         virtual const uci::type::ZoneType& getReportZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReportZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReportZone.
           */
         virtual uci::type::ZoneType& getReportZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReportZone to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReportZone
           */
         virtual void setReportZone(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReportZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReportZone is emabled or not
           */
         virtual bool hasReportZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReportZone
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableReportZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReportZone */
         virtual void clearReportZone()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageDetails.
           *
           * @return The bounded list identified by DamageDetails.
           */
         virtual const uci::type::DamageReportMDT::DamageDetails& getDamageDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DamageDetails.
           *
           * @return The bounded list identified by DamageDetails.
           */
         virtual uci::type::DamageReportMDT::DamageDetails& getDamageDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DamageDetails.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDamageDetails(const uci::type::DamageReportMDT::DamageDetails& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InformationSource.
           *
           * @return The acecssor that provides access to the complex content that is identified by InformationSource.
           */
         virtual const uci::type::DamageInformationSourceType& getInformationSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InformationSource.
           *
           * @return The acecssor that provides access to the complex content that is identified by InformationSource.
           */
         virtual uci::type::DamageInformationSourceType& getInformationSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InformationSource to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InformationSource
           */
         virtual void setInformationSource(const uci::type::DamageInformationSourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InformationSource exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InformationSource is emabled or not
           */
         virtual bool hasInformationSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InformationSource
           *
           * @param type = uci::type::accessorType::damageInformationSourceType This Accessor's accessor type
           */
         virtual void enableInformationSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageInformationSourceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InformationSource */
         virtual void clearInformationSource()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageReportMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageReportMDT to copy from
           */
         DamageReportMDT(const DamageReportMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageReportMDT to the contents of the DamageReportMDT on the
           * right hand side (rhs) of the assignment operator.DamageReportMDT [only available to derived classes]
           *
           * @param rhs The DamageReportMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::DamageReportMDT
           * @return A constant reference to this DamageReportMDT.
           */
         const DamageReportMDT& operator=(const DamageReportMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageReportMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageReportMDT_h

