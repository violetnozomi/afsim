// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__LAR_ReportMDT_h
#define Uci__Type__LAR_ReportMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__LAR_ID_Type_h)
# include "uci/type/LAR_ID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__LAR_AnalysisType_h)
# include "uci/type/LAR_AnalysisType.h"
#endif

#if !defined(Uci__Type__LAR_ReleaseConditionsType_h)
# include "uci/type/LAR_ReleaseConditionsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LAR_ReportMDT sequence accessor class */
      class LAR_ReportMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LAR_ReportMDT()
         { }

         /** Returns this accessor's type constant, i.e. LAR_ReportMDT
           *
           * @return This accessor's type constant, i.e. LAR_ReportMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lAR_ReportMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LAR_ReportMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_ID.
           */
         virtual const uci::type::LAR_ID_Type& getLAR_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_ID.
           */
         virtual uci::type::LAR_ID_Type& getLAR_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LAR_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LAR_ID
           */
         virtual void setLAR_ID(const uci::type::LAR_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InsideLAR.
           *
           * @return The value of the simple primitive data type identified by InsideLAR.
           */
         virtual xs::Boolean getInsideLAR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InsideLAR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInsideLAR(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_Analysis.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_Analysis.
           */
         virtual const uci::type::LAR_AnalysisType& getLAR_Analysis() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_Analysis.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_Analysis.
           */
         virtual uci::type::LAR_AnalysisType& getLAR_Analysis()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LAR_Analysis to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LAR_Analysis
           */
         virtual void setLAR_Analysis(const uci::type::LAR_AnalysisType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleaseConditions.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleaseConditions.
           */
         virtual const uci::type::LAR_ReleaseConditionsType& getReleaseConditions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleaseConditions.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleaseConditions.
           */
         virtual uci::type::LAR_ReleaseConditionsType& getReleaseConditions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReleaseConditions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReleaseConditions
           */
         virtual void setReleaseConditions(const uci::type::LAR_ReleaseConditionsType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LAR_ReportMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LAR_ReportMDT to copy from
           */
         LAR_ReportMDT(const LAR_ReportMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LAR_ReportMDT to the contents of the LAR_ReportMDT on the right
           * hand side (rhs) of the assignment operator.LAR_ReportMDT [only available to derived classes]
           *
           * @param rhs The LAR_ReportMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::LAR_ReportMDT
           * @return A constant reference to this LAR_ReportMDT.
           */
         const LAR_ReportMDT& operator=(const LAR_ReportMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LAR_ReportMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LAR_ReportMDT_h

