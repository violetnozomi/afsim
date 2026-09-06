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
#ifndef Uci__Type__CommResourceType_h
#define Uci__Type__CommResourceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommSystemID_Type_h)
# include "uci/type/CommSystemID_Type.h"
#endif

#if !defined(Uci__Type__CommAssetID_Type_h)
# include "uci/type/CommAssetID_Type.h"
#endif

#if !defined(Uci__Type__AntennaID_Type_h)
# include "uci/type/AntennaID_Type.h"
#endif

#if !defined(Uci__Type__CommAntennaModeType_h)
# include "uci/type/CommAntennaModeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommResourceType sequence accessor class */
      class CommResourceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommResourceType()
         { }

         /** Returns this accessor's type constant, i.e. CommResourceType
           *
           * @return This accessor's type constant, i.e. CommResourceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commResourceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommResourceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystemID.
           */
         virtual const uci::type::CommSystemID_Type& getCommSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSystemID.
           */
         virtual uci::type::CommSystemID_Type& getCommSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommSystemID
           */
         virtual void setCommSystemID(const uci::type::CommSystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommAssetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommAssetID.
           */
         virtual const uci::type::CommAssetID_Type& getCommAssetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommAssetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommAssetID.
           */
         virtual uci::type::CommAssetID_Type& getCommAssetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommAssetID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommAssetID
           */
         virtual void setCommAssetID(const uci::type::CommAssetID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommAssetID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommAssetID is emabled or not
           */
         virtual bool hasCommAssetID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommAssetID
           *
           * @param type = uci::type::accessorType::commAssetID_Type This Accessor's accessor type
           */
         virtual void enableCommAssetID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commAssetID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommAssetID */
         virtual void clearCommAssetID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaID.
           */
         virtual const uci::type::AntennaID_Type& getAntennaID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaID.
           */
         virtual uci::type::AntennaID_Type& getAntennaID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaID
           */
         virtual void setAntennaID(const uci::type::AntennaID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AntennaID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AntennaID is emabled or not
           */
         virtual bool hasAntennaID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AntennaID
           *
           * @param type = uci::type::accessorType::antennaID_Type This Accessor's accessor type
           */
         virtual void enableAntennaID(uci::base::accessorType::AccessorType type = uci::type::accessorType::antennaID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AntennaID */
         virtual void clearAntennaID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaModeID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaModeID.
           */
         virtual const uci::type::CommAntennaModeType& getAntennaModeID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AntennaModeID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaModeID.
           */
         virtual uci::type::CommAntennaModeType& getAntennaModeID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaModeID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaModeID
           */
         virtual void setAntennaModeID(const uci::type::CommAntennaModeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AntennaModeID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AntennaModeID is emabled or not
           */
         virtual bool hasAntennaModeID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AntennaModeID
           *
           * @param type = uci::type::accessorType::commAntennaModeType This Accessor's accessor type
           */
         virtual void enableAntennaModeID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commAntennaModeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AntennaModeID */
         virtual void clearAntennaModeID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommResourceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommResourceType to copy from
           */
         CommResourceType(const CommResourceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommResourceType to the contents of the CommResourceType on the
           * right hand side (rhs) of the assignment operator.CommResourceType [only available to derived classes]
           *
           * @param rhs The CommResourceType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CommResourceType
           * @return A constant reference to this CommResourceType.
           */
         const CommResourceType& operator=(const CommResourceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommResourceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommResourceType_h

