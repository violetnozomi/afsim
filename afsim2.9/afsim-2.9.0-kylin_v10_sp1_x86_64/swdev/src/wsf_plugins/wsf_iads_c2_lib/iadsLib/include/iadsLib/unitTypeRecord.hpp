// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   unitTypeRecord.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <iostream>
#include <string>
#include <iadsLib/enum.hpp>

#ifndef IADSLIB_UNITTYPERECORD_HPP
#define IADSLIB_UNITTYPERECORD_HPP

/**
 *  @brief Encapsulate all IADSLIB functions and classes.
 */
namespace il
{
    /**
       @brief Class to represent IDs.
    */
    class IADSLIB_EXPORT unitTypeRecord
    {
      friend std::ostream &operator<<(std::ostream &os, const unitTypeRecord &record)
      {
         os << record.m_TypeStr << ":" << record.m_SubTypeStr;
        return os;
      }

    public:
      unitTypeRecord ();
      unitTypeRecord (const int &Type,
                      const std::string &type_str,
                      const int &SubType,
                      const std::string &subtype_str);
      ~unitTypeRecord() = default;

      bool operator== (const unitTypeRecord &rhs) const;
      bool operator< (const unitTypeRecord &rhs) const;

      bool isValid() const;

      void set (const int &Type, const std::string &type_str, const int &SubType, const std::string &subtype_str);
      int  getType () const;
      int  getSubType () const;

      const std::string &getTypeStr () const;
      const std::string &getSubTypeStr () const;

    protected:
      int m_Type;
      int m_SubType;

      std::string m_TypeStr;
      std::string m_SubTypeStr;

    };
}
#endif
