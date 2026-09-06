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
#ifndef Uci__Type__CommPointingCommandMDT_h
#define Uci__Type__CommPointingCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommPointingCommandID_Type_h)
# include "uci/type/CommPointingCommandID_Type.h"
#endif

#if !defined(Uci__Type__CommPointingID_Type_h)
# include "uci/type/CommPointingID_Type.h"
#endif

#if !defined(Uci__Type__PointingVectorCommandTypeEnum_h)
# include "uci/type/PointingVectorCommandTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommPointingCommandMDT sequence accessor class */
      class CommPointingCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommPointingCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommPointingCommandMDT
           *
           * @return This accessor's type constant, i.e. CommPointingCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commPointingCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommPointingCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingCommandID.
           */
         virtual const uci::type::CommPointingCommandID_Type& getCommPointingCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingCommandID.
           */
         virtual uci::type::CommPointingCommandID_Type& getCommPointingCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommPointingCommandID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommPointingCommandID
           */
         virtual void setCommPointingCommandID(const uci::type::CommPointingCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingID.
           */
         virtual const uci::type::CommPointingID_Type& getCommPointingID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommPointingID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommPointingID.
           */
         virtual uci::type::CommPointingID_Type& getCommPointingID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommPointingID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommPointingID
           */
         virtual void setCommPointingID(const uci::type::CommPointingID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PointingVectorCommand.
           *
           * @return The value of the enumeration identified by PointingVectorCommand.
           */
         virtual const uci::type::PointingVectorCommandTypeEnum& getPointingVectorCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PointingVectorCommand.
           *
           * @return The value of the enumeration identified by PointingVectorCommand.
           */
         virtual uci::type::PointingVectorCommandTypeEnum& getPointingVectorCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PointingVectorCommand.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPointingVectorCommand(const uci::type::PointingVectorCommandTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PointingVectorCommand.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPointingVectorCommand(uci::type::PointingVectorCommandTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommPointingCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommPointingCommandMDT to copy from
           */
         CommPointingCommandMDT(const CommPointingCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommPointingCommandMDT to the contents of the
           * CommPointingCommandMDT on the right hand side (rhs) of the assignment operator.CommPointingCommandMDT [only available
           * to derived classes]
           *
           * @param rhs The CommPointingCommandMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CommPointingCommandMDT
           * @return A constant reference to this CommPointingCommandMDT.
           */
         const CommPointingCommandMDT& operator=(const CommPointingCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommPointingCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommPointingCommandMDT_h

