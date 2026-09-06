// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfBMAuxDataZoneAttachment.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: A container class for providing zone attachments to weapons or
*    platforms within aux data. This is to get around the fact that container
*    classes aren't supported within AUX data for the AFSIM core currently.
*
*******************************************************************************/

#ifndef WSF_BM_AUX_DATA_ZONE_ATTACHMENT_HPP
#define WSF_BM_AUX_DATA_ZONE_ATTACHMENT_HPP

#include <WsfBMCommon.hpp>
#include <iadsLib/enum.hpp>
#include <vector>
#include <WsfObject.hpp>

#define WSF_BM_AUX_DATA_ZONE_ATTACH_SCRIPT_TYPE_NAME   "WsfBMAuxDataZoneAttachment"

class WSF_IADS_C2_LIB_EXPORT WsfBMAuxDataZoneAttachment : public WsfObject
{
   public:
      enum eAttachmentType { E_PLATFORM, E_GLOBAL };

      WsfBMAuxDataZoneAttachment();
      WsfBMAuxDataZoneAttachment* Clone() const override;
      inline const char* GetScriptClassName() const override { return WSF_BM_AUX_DATA_ZONE_ATTACH_SCRIPT_TYPE_NAME; }

      virtual void AddZoneAttachment(const eAttachmentType attach_type, const double priority, const std::string& zone_name, const il::eZoneType zone_type);
      virtual std::size_t GetZoneAttachmentCount() const;
      virtual void GetNthZoneAttachment(const std::size_t nth_zone, eAttachmentType& attach_type, double& priority, std::string& zone_name, il::eZoneType& zone_type) const;

   protected:
      class tAttachment
      {
         public:
            tAttachment(const double priority, const eAttachmentType attachment, const std::string& zone_name, const il::eZoneType zone_type)
               : m_priority(priority), m_attachment(attachment), m_zone_name(zone_name), m_zone_type(zone_type) { }
            double            m_priority;
            eAttachmentType   m_attachment;
            std::string       m_zone_name;
            il::eZoneType     m_zone_type;
      };

      std::vector <tAttachment> m_zones;
};

#endif
