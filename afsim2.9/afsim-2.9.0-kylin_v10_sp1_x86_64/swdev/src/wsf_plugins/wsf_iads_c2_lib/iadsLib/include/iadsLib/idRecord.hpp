// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
*  @file   idRecord.hpp
*  @author Kenneth R. Sewell III
*          Kenneth.Sewell@wpafb.af.mil
*          AFRL/RYWF
*          2241 Avionics Circle
*          WPAFB, Ohio 45433-7303
*          937-656-4899 x3004
*
*  Classification: UNCLASSIFIED
*/



#ifndef IADSLIB_IDRECORD_HPP
#define IADSLIB_IDRECORD_HPP

#include <iostream>
#include <stdint.h>
#include <string>
#include "iadslib_export.h"

/**
*  @brief Encapsulate all IADSLIB functions and classes.
*/
namespace il
{
   /**
   @brief Class to represent IDs.
   */
   class IADSLIB_EXPORT idRecord
   {
         friend std::ostream &operator<<(std::ostream &os, const idRecord &record)
         {
            if (record.m_IDString.length() > 0)
            { os << record.m_IDString; }
            else
            { os << record.m_ID; }

            os << ":";

            if (record.m_SubIDString.length() > 0)
            { os << record.m_SubIDString; }
            else
            { os << record.m_SubID; }

            return os;
         }

      public:
         idRecord(const int32_t &ID = 0, const int32_t &SubID = 0, const std::string &IDString = "", const std::string &SubIDString = "");
         ~idRecord() = default;  
         bool operator==(const idRecord &record) const;
         bool operator==(const int32_t &id) const;
         bool operator!=(const idRecord &record) const;
         bool operator<(const idRecord &record) const;

         bool isValid() const;

         void set(const int32_t &ID, const int32_t &SubID, const std::string &IDString = "", const std::string &SubIDString = "");

         // get/set when used as generic ID.
         void setID(const int32_t &ID, const std::string &IDString = "");
         const int32_t &getID() const;
         std::string getIDString() const;

         void setSubID(const int32_t &SubID, const std::string &SubIDString = "");
         const int32_t &getSubID() const;
         std::string getSubIDString() const;

         // get/set when used as a track ID.
         void setSystemUnitID(const int32_t &ID, const std::string &IDString = "");
         const int32_t &getSystemUnitID() const;
         const std::string &getSystemUnitIDString() const;

         void setSystemTrackID(const int32_t &SubID, const std::string &SubIDString = "");
         const int32_t &getSystemTrackID() const;


      protected:
         int32_t m_ID;
         int32_t m_SubID;

         // these are optional and will only be used for output convenience
         std::string m_IDString;
         std::string m_SubIDString;
   };
}

#endif
