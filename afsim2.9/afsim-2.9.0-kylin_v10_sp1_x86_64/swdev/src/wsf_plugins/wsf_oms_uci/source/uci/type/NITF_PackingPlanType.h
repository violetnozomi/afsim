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
#ifndef Uci__Type__NITF_PackingPlanType_h
#define Uci__Type__NITF_PackingPlanType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FileHeaderType_h)
# include "uci/type/FileHeaderType.h"
#endif

#if !defined(Uci__Type__ImageSubheaderType_h)
# include "uci/type/ImageSubheaderType.h"
#endif

#if !defined(Uci__Type__ACFTB_Type_h)
# include "uci/type/ACFTB_Type.h"
#endif

#if !defined(Uci__Type__AIMIDB_Type_h)
# include "uci/type/AIMIDB_Type.h"
#endif

#if !defined(Uci__Type__EXPLTB_Type_h)
# include "uci/type/EXPLTB_Type.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__PATCHB_Type_h)
# include "uci/type/PATCHB_Type.h"
#endif

#if !defined(Uci__Type__MSTGTA_Type_h)
# include "uci/type/MSTGTA_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SECTGA_Type_h)
# include "uci/type/SECTGA_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type is a tailored reproduction of data definitions included in NITF as described in MIL-STD-2500C and companion
        * Appendices. This data must flow from Task or Command source to recipient so that the recipient can populate the
        * resulting Products with this data to enable PED and other functions. Under most circumstances, UCI doesn't reproduce
        * other standards "in-band", in UCI messages. An exception is made in this case because to do otherwise would disrupt
        * C2 and PED.
        */
      class NITF_PackingPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~NITF_PackingPlanType()
         { }

         /** Returns this accessor's type constant, i.e. NITF_PackingPlanType
           *
           * @return This accessor's type constant, i.e. NITF_PackingPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::nITF_PackingPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const NITF_PackingPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Information to be used for populating the Secondary Target Revision A (SECTGA) tagged record extension (TRE) as
           * defined by the Compendium of Controlled Extensions (CE) [Minimum occurrences: 0] [Maximum occurrences: 250]
           */
         typedef uci::base::BoundedList<uci::type::SECTGA_Type, uci::type::accessorType::sECTGA_Type> SECTGA;

         /** Returns the accessor that provides access to the complex content that is identified by the FileHeader.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileHeader.
           */
         virtual const uci::type::FileHeaderType& getFileHeader() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileHeader.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileHeader.
           */
         virtual uci::type::FileHeaderType& getFileHeader()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileHeader to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileHeader
           */
         virtual void setFileHeader(const uci::type::FileHeaderType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileHeader exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileHeader is emabled or not
           */
         virtual bool hasFileHeader() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileHeader
           *
           * @param type = uci::type::accessorType::fileHeaderType This Accessor's accessor type
           */
         virtual void enableFileHeader(uci::base::accessorType::AccessorType type = uci::type::accessorType::fileHeaderType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileHeader */
         virtual void clearFileHeader()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ImageSubheader.
           *
           * @return The acecssor that provides access to the complex content that is identified by ImageSubheader.
           */
         virtual const uci::type::ImageSubheaderType& getImageSubheader() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ImageSubheader.
           *
           * @return The acecssor that provides access to the complex content that is identified by ImageSubheader.
           */
         virtual uci::type::ImageSubheaderType& getImageSubheader()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ImageSubheader to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ImageSubheader
           */
         virtual void setImageSubheader(const uci::type::ImageSubheaderType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ImageSubheader exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ImageSubheader is emabled or not
           */
         virtual bool hasImageSubheader() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ImageSubheader
           *
           * @param type = uci::type::accessorType::imageSubheaderType This Accessor's accessor type
           */
         virtual void enableImageSubheader(uci::base::accessorType::AccessorType type = uci::type::accessorType::imageSubheaderType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ImageSubheader */
         virtual void clearImageSubheader()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ACFTB.
           *
           * @return The acecssor that provides access to the complex content that is identified by ACFTB.
           */
         virtual const uci::type::ACFTB_Type& getACFTB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ACFTB.
           *
           * @return The acecssor that provides access to the complex content that is identified by ACFTB.
           */
         virtual uci::type::ACFTB_Type& getACFTB()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ACFTB to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ACFTB
           */
         virtual void setACFTB(const uci::type::ACFTB_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ACFTB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ACFTB is emabled or not
           */
         virtual bool hasACFTB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ACFTB
           *
           * @param type = uci::type::accessorType::aCFTB_Type This Accessor's accessor type
           */
         virtual void enableACFTB(uci::base::accessorType::AccessorType type = uci::type::accessorType::aCFTB_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ACFTB */
         virtual void clearACFTB()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AIMIDB.
           *
           * @return The acecssor that provides access to the complex content that is identified by AIMIDB.
           */
         virtual const uci::type::AIMIDB_Type& getAIMIDB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AIMIDB.
           *
           * @return The acecssor that provides access to the complex content that is identified by AIMIDB.
           */
         virtual uci::type::AIMIDB_Type& getAIMIDB()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AIMIDB to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AIMIDB
           */
         virtual void setAIMIDB(const uci::type::AIMIDB_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AIMIDB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AIMIDB is emabled or not
           */
         virtual bool hasAIMIDB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AIMIDB
           *
           * @param type = uci::type::accessorType::aIMIDB_Type This Accessor's accessor type
           */
         virtual void enableAIMIDB(uci::base::accessorType::AccessorType type = uci::type::accessorType::aIMIDB_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AIMIDB */
         virtual void clearAIMIDB()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EXPLTB.
           *
           * @return The acecssor that provides access to the complex content that is identified by EXPLTB.
           */
         virtual const uci::type::EXPLTB_Type& getEXPLTB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EXPLTB.
           *
           * @return The acecssor that provides access to the complex content that is identified by EXPLTB.
           */
         virtual uci::type::EXPLTB_Type& getEXPLTB()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EXPLTB to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EXPLTB
           */
         virtual void setEXPLTB(const uci::type::EXPLTB_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EXPLTB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EXPLTB is emabled or not
           */
         virtual bool hasEXPLTB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EXPLTB
           *
           * @param type = uci::type::accessorType::eXPLTB_Type This Accessor's accessor type
           */
         virtual void enableEXPLTB(uci::base::accessorType::AccessorType type = uci::type::accessorType::eXPLTB_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EXPLTB */
         virtual void clearEXPLTB()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MPDSRA.
           *
           * @return The value of the string data type identified by MPDSRA.
           */
         virtual const uci::type::EmptyType& getMPDSRA() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MPDSRA.
           *
           * @return The value of the string data type identified by MPDSRA.
           */
         virtual uci::type::EmptyType& getMPDSRA()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MPDSRA to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMPDSRA(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MPDSRA to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMPDSRA(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MPDSRA to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMPDSRA(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MPDSRA exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MPDSRA is emabled or not
           */
         virtual bool hasMPDSRA() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MPDSRA
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableMPDSRA(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MPDSRA */
         virtual void clearMPDSRA()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PATCHB.
           *
           * @return The acecssor that provides access to the complex content that is identified by PATCHB.
           */
         virtual const uci::type::PATCHB_Type& getPATCHB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PATCHB.
           *
           * @return The acecssor that provides access to the complex content that is identified by PATCHB.
           */
         virtual uci::type::PATCHB_Type& getPATCHB()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PATCHB to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PATCHB
           */
         virtual void setPATCHB(const uci::type::PATCHB_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PATCHB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PATCHB is emabled or not
           */
         virtual bool hasPATCHB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PATCHB
           *
           * @param type = uci::type::accessorType::pATCHB_Type This Accessor's accessor type
           */
         virtual void enablePATCHB(uci::base::accessorType::AccessorType type = uci::type::accessorType::pATCHB_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PATCHB */
         virtual void clearPATCHB()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MSTGTA.
           *
           * @return The acecssor that provides access to the complex content that is identified by MSTGTA.
           */
         virtual const uci::type::MSTGTA_Type& getMSTGTA() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MSTGTA.
           *
           * @return The acecssor that provides access to the complex content that is identified by MSTGTA.
           */
         virtual uci::type::MSTGTA_Type& getMSTGTA()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MSTGTA to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MSTGTA
           */
         virtual void setMSTGTA(const uci::type::MSTGTA_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MSTGTA exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MSTGTA is emabled or not
           */
         virtual bool hasMSTGTA() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MSTGTA
           *
           * @param type = uci::type::accessorType::mSTGTA_Type This Accessor's accessor type
           */
         virtual void enableMSTGTA(uci::base::accessorType::AccessorType type = uci::type::accessorType::mSTGTA_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MSTGTA */
         virtual void clearMSTGTA()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SECTGA.
           *
           * @return The bounded list identified by SECTGA.
           */
         virtual const uci::type::NITF_PackingPlanType::SECTGA& getSECTGA() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SECTGA.
           *
           * @return The bounded list identified by SECTGA.
           */
         virtual uci::type::NITF_PackingPlanType::SECTGA& getSECTGA()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SECTGA.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSECTGA(const uci::type::NITF_PackingPlanType::SECTGA& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         NITF_PackingPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The NITF_PackingPlanType to copy from
           */
         NITF_PackingPlanType(const NITF_PackingPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this NITF_PackingPlanType to the contents of the NITF_PackingPlanType
           * on the right hand side (rhs) of the assignment operator.NITF_PackingPlanType [only available to derived classes]
           *
           * @param rhs The NITF_PackingPlanType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::NITF_PackingPlanType
           * @return A constant reference to this NITF_PackingPlanType.
           */
         const NITF_PackingPlanType& operator=(const NITF_PackingPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // NITF_PackingPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__NITF_PackingPlanType_h

