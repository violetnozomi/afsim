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
#ifndef Uci__Type__GenerateOptionCommandType_h
#define Uci__Type__GenerateOptionCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommandResponseType_h)
# include "uci/type/CommandResponseType.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Type__CapabilityCommandTemporalConstraintsType_h)
# include "uci/type/CapabilityCommandTemporalConstraintsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the GenerateOptionCommandType sequence accessor class */
      class GenerateOptionCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GenerateOptionCommandType()
         { }

         /** Returns this accessor's type constant, i.e. GenerateOptionCommandType
           *
           * @return This accessor's type constant, i.e. GenerateOptionCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::generateOptionCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GenerateOptionCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandType.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandType.
           */
         virtual const uci::type::CommandResponseType& getCommandType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandType.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandType.
           */
         virtual uci::type::CommandResponseType& getCommandType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandType
           */
         virtual void setCommandType(const uci::type::CommandResponseType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Ranking.
           *
           * @return The acecssor that provides access to the complex content that is identified by Ranking.
           */
         virtual const uci::type::RankingType& getRanking() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Ranking.
           *
           * @return The acecssor that provides access to the complex content that is identified by Ranking.
           */
         virtual uci::type::RankingType& getRanking()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Ranking to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Ranking
           */
         virtual void setRanking(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Ranking exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Ranking is emabled or not
           */
         virtual bool hasRanking() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Ranking
           *
           * @param type = uci::type::accessorType::rankingType This Accessor's accessor type
           */
         virtual void enableRanking(uci::base::accessorType::AccessorType type = uci::type::accessorType::rankingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Ranking */
         virtual void clearRanking()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimingConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimingConstraints.
           */
         virtual const uci::type::CapabilityCommandTemporalConstraintsType& getTimingConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimingConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimingConstraints.
           */
         virtual uci::type::CapabilityCommandTemporalConstraintsType& getTimingConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimingConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimingConstraints
           */
         virtual void setTimingConstraints(const uci::type::CapabilityCommandTemporalConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimingConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimingConstraints is emabled or not
           */
         virtual bool hasTimingConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimingConstraints
           *
           * @param type = uci::type::accessorType::capabilityCommandTemporalConstraintsType This Accessor's accessor type
           */
         virtual void enableTimingConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityCommandTemporalConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimingConstraints */
         virtual void clearTimingConstraints()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GenerateOptionCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GenerateOptionCommandType to copy from
           */
         GenerateOptionCommandType(const GenerateOptionCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GenerateOptionCommandType to the contents of the
           * GenerateOptionCommandType on the right hand side (rhs) of the assignment operator.GenerateOptionCommandType [only
           * available to derived classes]
           *
           * @param rhs The GenerateOptionCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::GenerateOptionCommandType
           * @return A constant reference to this GenerateOptionCommandType.
           */
         const GenerateOptionCommandType& operator=(const GenerateOptionCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GenerateOptionCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GenerateOptionCommandType_h

