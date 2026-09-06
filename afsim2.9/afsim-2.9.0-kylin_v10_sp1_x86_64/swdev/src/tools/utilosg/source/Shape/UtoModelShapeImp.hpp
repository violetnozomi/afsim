// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoModelShapeImp.hpp : header file

#if !defined(_UTOMODELSHAPEIMP_HPP_)
#define _UTOMODELSHAPEIMP_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <istream>
#include <map>
#include <vector>

#include "UtoPriv.hpp"
#include "UtoShapeImp.hpp"
#include "UtoTypes.hpp"

class UtoModelPartImp;
class UtoModelSwitchImp;
namespace osg
{
class Material;
}
class ModelFrame;

class UtoModelShapeImp : public UtoShapeImp
{
public:
   typedef std::map<std::string, double> MetricMap;
   typedef std::vector<std::string>      GroupVec;
   typedef void (UtoModelShapeImp::*ReadAttrFnPtr)(const std::string&, const std::string&);
   typedef std::map<std::string, ReadAttrFnPtr>      AttributeMap;
   typedef std::map<std::string, UtoModelPartImp*>   ModelPartMap;
   typedef std::map<std::string, UtoModelSwitchImp*> ModelSwitchMap;

   class Data
   {
   public:
      Data();
      ~Data();

      std::string m_sType;
      MetricMap   m_Metrics;
      GroupVec    m_Groups;

   private:
      Data(const Data&);
      Data& operator=(const Data&);
   };

   struct LoadData
   {
      bool   is_model;
      bool   is_image;
      bool   is_subimage;
      bool   is_loaded;
      bool   bInvertRectY;
      bool   anti_alias;
      bool   mip_mapping;
      double width;
      double height;
      long   nShapeFlags;

      struct Rect
      {
         unsigned short x0, y0, x1, y1;
      } SImgRect;

      osg::Vec3   scale;
      std::string file_name;
   };

   typedef RefCntBase<Data> SharedData;

   // UtoModelShapeImp (const std::string& res_name);
   UtoModelShapeImp();
   UtoModelShapeImp(const UtoModelShapeImp&);
   ~UtoModelShapeImp() override;

   void SetMetric(const std::string& name, double value);
   bool Metric(const std::string& name, double& value) const;
   bool InGroup(const std::string& group) const;

   bool                            HasPart(const std::string& name);
   std::vector<std::string>        GetParts();
   static std::vector<std::string> GetParts(osg::ref_ptr<osg::Node> aModelNode);
   UtoModelPartImp&                FindPart(const std::string& name);

   bool                            HasSwitch(const std::string& name);
   std::vector<std::string>        GetSwitches();
   static std::vector<std::string> GetSwitches(osg::ref_ptr<osg::Node> aModelNode);
   static std::vector<int>         GetSwitchSizes(osg::ref_ptr<osg::Node> aModelNode);
   UtoModelSwitchImp&              FindSwitch(const std::string& name);
   void                            RebuildSwitchMap();

   void SetCopyOptions(int copyFlags);
   void ForceFullCopy()
   {
      if (mIsShallowCopy)
      {
         FullCopy();
      }
   }
   void             FullCopy();
   osg::Matrix*     GetStaticRotation();
   UtoPosition      GetStaticOffset() { return m_StaticOffset; }
   static UtoShape* CreateModel();
   static UtoShape* CreateImage();

   void SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved = 0) override;

   UtoModelShapeImp& operator=(const UtoModelShapeImp&);

   bool PreLoad() override;
   bool PostLoad() override;
   bool ReadAttribute(const std::string& attr, const std::string& buf) override;

   // attribute reading functions.
   virtual void ReadFile(const std::string& attr, const std::string& buf);
   virtual void ReadRotateX(const std::string& attr, const std::string& buf);
   virtual void ReadRotateY(const std::string& attr, const std::string& buf);
   virtual void ReadRotateZ(const std::string& attr, const std::string& buf);
   virtual void ReadRotate3(const std::string& attr, const std::string& buf);
   virtual void ReadScale3(const std::string& attr, const std::string& buf);
   virtual void ReadScale(const std::string& attr, const std::string& buf);
   virtual void ReadLODScale(const std::string& attr, const std::string& buf);
   virtual void ReadTranslate(const std::string& attr, const std::string& buf);
   virtual void ReadWingTip(const std::string& attr, const std::string& buf);
   virtual void ReadLeftTip(const std::string& attr, const std::string& buf);
   virtual void ReadRightTip(const std::string& attr, const std::string& buf);
   virtual void ReadWidth(const std::string& attr, const std::string& buf);
   virtual void ReadHeight(const std::string& attr, const std::string& buf);
   virtual void ReadNoAntiAlias(const std::string& attr, const std::string& buf);
   virtual void ReadNoMipMapping(const std::string& attr, const std::string& buf);
   virtual void ReadAntiAlias(const std::string& attr, const std::string& buf);
   virtual void ReadMipMapping(const std::string& attr, const std::string& buf);
   virtual void ReadBbox(const std::string& attr, const std::string& in);
   virtual void ReadGroup(const std::string& attr, const std::string& in);
   virtual void ReadPilotEye(const std::string& attr, const std::string& buf);
   virtual void ReadOther(const std::string& attr, const std::string& buf);
   virtual void ReadRect(const std::string& attr, const std::string& buf);
   virtual void ReadInvertY(const std::string& attr, const std::string& buf);
   virtual void ReadShapeFlags(const std::string& attr, const std::string& buf);

   // helper functions.
   virtual int ReadModelFile(const std::string& in);
   virtual int ReadImageFile(const std::string& in);

   void SetDepthTesting(bool aState);
   void SetLighting(bool aState);

protected:
   static AttributeMap m_ModelAttributes;
   static AttributeMap m_ImageAttributes;
   static AttributeMap m_SubImageAttributes;

   virtual void SetupColorState();
   virtual void SetupStateSet();

   AttributeMap* m_Attributes;

   SharedData                   m_Data;
   LoadData*                    m_LoadData;
   ModelPartMap                 m_ModelPart;
   ModelSwitchMap               m_ModelSwitch;
   osg::ref_ptr<osg::Node>      m_ModelNode;
   bool                         m_TexturedPolygon;
   bool                         mIsShallowCopy;
   int                          m_CopyFlags;
   osg::ref_ptr<osg::Vec4Array> m_ColorArray;
};

#endif // !defined(_UTOMODELSHAPEIMP_HPP_)
