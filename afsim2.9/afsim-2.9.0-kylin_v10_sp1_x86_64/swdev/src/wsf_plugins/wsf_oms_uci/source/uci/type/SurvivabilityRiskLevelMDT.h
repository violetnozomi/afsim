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
#ifndef Uci__Type__SurvivabilityRiskLevelMDT_h
#define Uci__Type__SurvivabilityRiskLevelMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SurvivabilityRiskLevelID_Type_h)
# include "uci/type/SurvivabilityRiskLevelID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__SurvivabilityRiskSettingType_h)
# include "uci/type/SurvivabilityRiskSettingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SurvivabilityRiskLevelMDT sequence accessor class */
      class SurvivabilityRiskLevelMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SurvivabilityRiskLevelMDT()
         { }

         /** Returns this accessor's type constant, i.e. SurvivabilityRiskLevelMDT
           *
           * @return This accessor's type constant, i.e. SurvivabilityRiskLevelMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::survivabilityRiskLevelMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SurvivabilityRiskLevelMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Identifies which vehicle this parameter applies to. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> AppliesToSystemID;

         /** A list of the Risk Settings that govern this particular mission [Maximum occurrences: 4] */
         typedef uci::base::BoundedList<uci::type::SurvivabilityRiskSettingType, uci::type::accessorType::survivabilityRiskSettingType> RiskSetting;

         /** Returns the accessor that provides access to the complex content that is identified by the SurvivabilityRiskLevelID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SurvivabilityRiskLevelID.
           */
         virtual const uci::type::SurvivabilityRiskLevelID_Type& getSurvivabilityRiskLevelID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SurvivabilityRiskLevelID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SurvivabilityRiskLevelID.
           */
         virtual uci::type::SurvivabilityRiskLevelID_Type& getSurvivabilityRiskLevelID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SurvivabilityRiskLevelID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SurvivabilityRiskLevelID
           */
         virtual void setSurvivabilityRiskLevelID(const uci::type::SurvivabilityRiskLevelID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AppliesToSystemID.
           *
           * @return The bounded list identified by AppliesToSystemID.
           */
         virtual const uci::type::SurvivabilityRiskLevelMDT::AppliesToSystemID& getAppliesToSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AppliesToSystemID.
           *
           * @return The bounded list identified by AppliesToSystemID.
           */
         virtual uci::type::SurvivabilityRiskLevelMDT::AppliesToSystemID& getAppliesToSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AppliesToSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAppliesToSystemID(const uci::type::SurvivabilityRiskLevelMDT::AppliesToSystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RiskSetting.
           *
           * @return The bounded list identified by RiskSetting.
           */
         virtual const uci::type::SurvivabilityRiskLevelMDT::RiskSetting& getRiskSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RiskSetting.
           *
           * @return The bounded list identified by RiskSetting.
           */
         virtual uci::type::SurvivabilityRiskLevelMDT::RiskSetting& getRiskSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RiskSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRiskSetting(const uci::type::SurvivabilityRiskLevelMDT::RiskSetting& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SurvivabilityRiskLevelMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SurvivabilityRiskLevelMDT to copy from
           */
         SurvivabilityRiskLevelMDT(const SurvivabilityRiskLevelMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SurvivabilityRiskLevelMDT to the contents of the
           * SurvivabilityRiskLevelMDT on the right hand side (rhs) of the assignment operator.SurvivabilityRiskLevelMDT [only
           * available to derived classes]
           *
           * @param rhs The SurvivabilityRiskLevelMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SurvivabilityRiskLevelMDT
           * @return A constant reference to this SurvivabilityRiskLevelMDT.
           */
         const SurvivabilityRiskLevelMDT& operator=(const SurvivabilityRiskLevelMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SurvivabilityRiskLevelMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SurvivabilityRiskLevelMDT_h

