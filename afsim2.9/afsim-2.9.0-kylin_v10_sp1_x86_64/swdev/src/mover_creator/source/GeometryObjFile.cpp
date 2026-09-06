// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GeometryObjFile.hpp"

#include <QDate>
#include <QDir>
#include <QFile>
#include <QString>
#include <QThread>
#include <sstream>

#include "GeometryBody.hpp"
#include "GeometryDish.hpp"
#include "GeometryEngine.hpp"
#include "GeometryFuselage.hpp"
#include "GeometryNacelle.hpp"
#include "GeometryPropulsionData.hpp"
#include "GeometrySurface.hpp"
#include "GeometryWing.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "ObjPointCollection.hpp"
#include "P6DofUtils.hpp"
#include "ScriptGeneratorGM.hpp"
#include "ScriptGeneratorP6DOF.hpp"
#include "ScriptGeneratorPointMassSixDOF.hpp"
#include "ScriptGeneratorRigidBodySixDOF.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtPath.hpp"
#include "Vehicle.hpp"
#include "VehicleGeometry.hpp"
#include "VehiclePropulsion.hpp"

namespace Designer
{

double GeometryObjFile::mAirfoilPtsX[7] = { 0.0, 0.05, 0.150, 0.25, 0.500, 0.750, 1.0 };
double GeometryObjFile::mAirfoilPtsY[7] = { 0.0, 0.50, 0.875, 1.00, 0.875, 0.625, 0.0 };
double GeometryObjFile::mOgivePtsX[5]   = { 0.0, 0.4375, 0.6875, 0.875, 1.0 };
double GeometryObjFile::mOgivePtsY[5]   = { 1.0, 0.7500, 0.5000, 0.250, 0.0 };

namespace
{
   QString GetResourcesDir()
   {
      QString returnPath;

      // Determine if this is an installed build or if this is running from Visual Studio
      UtPath path = UtPath::WorkingDirectory();
      path.Up();
      std::string pathStr = path.GetSystemPath() + "/resources";
      
      // Path should exist in an installed build
      if (UtPath{pathStr}.Exists())
      {
         returnPath = QString::fromStdString(pathStr);
      }
      else
      {
         // Use the developer path to the resources directory
         const char* resourcePath = ::getenv("RESOURCE_PATH");
         if (resourcePath)
         {
            returnPath = resourcePath;
         }
         else
         {
            ut::log::warning() << "Resource path not found.";
         }
      }

      return returnPath;
   }
}

GeometryObjFile::GeometryObjFile()
{
   // Initialize the matrix
   mActiveMatrix.Set(0.0, 0.0, 0.0);
   ObjPushMatrix();

   // Initialize the translation
   mActiveTranslation.Set(0.0, 0.0, 0.0);
}

GeometryObjFile::~GeometryObjFile()
{
}

bool GeometryObjFile::OutputObjFiles(const std::string& aFileDir, const std::string& aFilename)
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();

   DrawEngines(vehicle);
   DrawWingsAndSurfaces(vehicle);
   DrawDishes(vehicle);
   DrawBodiesAndNacelles(vehicle);
   DrawCanopy(vehicle);

   std::string coreFilename = aFileDir + aFilename; // "Test3dObject";
   std::string mtlSuffix = ".mtl";
   std::string objSuffix = ".obj";

   QDate Today = QDate::currentDate();

   // Output the MTL file
   std::string mtlFilePath = coreFilename + mtlSuffix;
   std::string mtlFileName = aFilename + mtlSuffix;

   QFile mtlFile(mtlFilePath.c_str());

   if (!mtlFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      MoverCreatorMainWindow().CreateErrorBox("The material (.mtl) file for this model can't be opened for writing!");
      return false;
   }

   QTextStream mtlOutStream(&mtlFile);

   mtlOutStream << "# ##################################################################################################\n";
   mtlOutStream << "# \n";
   VersionInfo::OutputVersionStringInOutputStream(mtlOutStream);
   mtlOutStream << "# \n";
   mtlOutStream << "# Creation Date:  " << Today.toString(Qt::RFC2822Date) << "\n";
   mtlOutStream << "# \n";
   mtlOutStream << "# ##################################################################################################\n";
   mtlOutStream << "\n";
   mtlOutStream << "\n";
   mtlOutStream << "newmtl material0\n";
   mtlOutStream << "Kd 0.4 0.4 0.4\n";
   mtlOutStream << "Ka 0.2 0.2 0.2\n";
   mtlOutStream << "Ks 1.0 1.0 1.0\n";
   mtlOutStream << "Ns 128\n";
   mtlOutStream << "\n";

   // Note (above) that we do not use Tr (transparency) or map_Kd (texture)

   mtlFile.close();

   // Output the OBJ file
   std::string objFilePath = coreFilename + objSuffix;

   QFile objFile(objFilePath.c_str());
   if (!objFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      MoverCreatorMainWindow().CreateErrorBox("The object (.obj) file for this model can't be opened for writing!");
      return false;
   }

   QTextStream objOutStream(&objFile);

   objOutStream.setRealNumberNotation(QTextStream::FixedNotation);

   objOutStream << "# ##################################################################################################\n";
   objOutStream << "# \n";
   VersionInfo::OutputVersionStringInOutputStream(objOutStream);
   objOutStream << "# \n";
   objOutStream << "# Creation Date:  " << Today.toString(Qt::RFC2822Date) << "\n";
   objOutStream << "# \n";
   objOutStream << "# ##################################################################################################\n";
   objOutStream << "\n";

   objOutStream << "\n";
   objOutStream << "# 3D object contains " << (int)mVertexList.size() << " vertices, " <<
                (int)mNormalList.size() << " normals, and " << (int)mFaceVertexList.size() << " faces/polygons";
   objOutStream << "\n";

   objOutStream << "mtllib " << mtlFileName.c_str() << "\n";
   objOutStream << "g g0\n";

   OutputVertices(objOutStream);
   OutputNormals(objOutStream);
   OutputTextureCoords(objOutStream);
   OutputFaces(objOutStream);

   objFile.close();

   return true;
}

void GeometryObjFile::Output3dModelSet()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();

   SupportedFileType aFileType;
   if (mw.GetOsgConvFileType() == QString::fromStdString(".osgb"))
   {
      aFileType = SupportedFileType::cOSGB;
   }
   else
   {
      // Output cIVE (default)
      aFileType = SupportedFileType::cIVE;
   }

   OutputTemporary3dModel();

   OutputFinal3dModel(aFileType);
}

void GeometryObjFile::OutputTemporary3dModel()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();

   if (vehicle == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   // We need to ensure the file structure exists to add the 3d model
   if (mw.GetMoverCreatorWidget()->GenerateP6DOFMover())
   {
      ScriptGeneratorP6DOF scriptGenerator;
      scriptGenerator.CreateFolderStructure();
   }
   if (mw.GetMoverCreatorWidget()->GenerateRB6DOFMover())
   {
      ScriptGeneratorRigidBodySixDOF scriptGenerator;
      scriptGenerator.CreateFolderStructure();
   }
   if (mw.GetMoverCreatorWidget()->GeneratePM6DOFMover())
   {
      ScriptGeneratorPointMassSixDOF scriptGenerator;
      scriptGenerator.CreateFolderStructure();
   }
   if (mw.GetMoverCreatorWidget()->GenerateGuidedMover())
   {
      ScriptGeneratorGM scriptGenerator;
      scriptGenerator.CreateFolderStructure();
   }

   // Get the vehicle name and convert to lowercase
   QString name = vehicle->GetVehicleName().c_str();
   name = name.toLower();

   OutputObjFiles(GetLocalModelPath().toStdString(), name.toStdString());
}

void GeometryObjFile::OutputSiteInitialModelsFile(const QString& aFilename)
{
   QFile modelsFile(aFilename);

   if (!modelsFile.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      MoverCreatorMainWindow().CreateErrorBox("Can't create a models.txt file at " + aFilename + "!");
      return;
   }

   QDate Today = QDate::currentDate();

   QTextStream outStream(&modelsFile);

   outStream << "# ##################################################################################################\n";
   outStream << "# \n";
   VersionInfo::OutputVersionStringInOutputStream(outStream);
   outStream << "# \n";
   outStream << "# Creation Date:  " << Today.toString(Qt::RFC2822Date) << "\n";
   outStream << "# \n";
   outStream << "# ##################################################################################################\n";
   outStream << "# \n";
   outStream << "# \n";
   outStream << "# Model Definition File\n";
   outStream << "#\n";
   outStream << "# Model definitions:\n";
   outStream << "# \"model name\n";
   outStream << "#    filename address_of_modelfile\n";
   outStream << "# end_model\"\n";
   outStream << "#\n";
   outStream << "# Modelfiles may be ive, osgb, flt, obj, or any model natively supported by OSG\n";
   outStream << "#\n";
   outStream << "# The model definition may include a pre_xform block which can contain transformations:\n";
   outStream << "# \"scale value\"\n";
   outStream << "# \"translate valueX valueY valueZ\"\n";
   outStream << "# \"rotate axisChar(x,y,or z) valueAngleDeg\"\n";
   outStream << "#\n";
   outStream << "# The transformations will be applied in the order they appear.\n";
   outStream << "# It is preferable to transform the models themselves.\n";
   outStream << "#\n";
   outStream << "# The model definition may include:\n";
   outStream << "# \"screen_scale value\"\n";
   outStream << "# \"wing_tip valueX valueY valueZ\" (denotes right_tip after all transformations, left_tip should be reflection over x-z plane)\n";
   outStream << "#\n";
   outStream << "# Aliases\n";
   outStream << "# \"alias alias_name model_name\"\n";
   outStream << "#\n";
   outStream << "# Applications may reference the model name or alias directly\n";
   outStream << "\n";
   outStream << "\n";

   modelsFile.close();
}

void GeometryObjFile::OutputFinal3dModel(SupportedFileType aFileType)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();

   if (vehicle == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   QString name = vehicle->GetVehicleName().c_str();
   name = name.toLower();

   // Setup the path
   QString localObjFilePathWithoutExtension = GetLocalModelPath() + name;

   // Setup the site files/folders -- we need these regardless of whether we use OBJ or any other format

   // Check for a site folder
   QString sitePath = mw.GetSiteDir();

   // If the user hasn't set a site directory, attempt to use the default
   if (sitePath.isEmpty())
   {
      sitePath = GetResourcesDir() + "/site";

      // If the directory does not exist, create it
      if (!UtPath(sitePath.toStdString()).Exists())
      {
         UtPath(sitePath.toStdString()).Mkdir();

         // Since the site folder was empty, we need to create an initial models.txt file
         QString siteModelsPath = sitePath + "/models.txt";
         OutputSiteInitialModelsFile(siteModelsPath);
      }
   }

   // Check for a models.txt file in the site folder
   QString siteModelsTxtFilePath = sitePath + "/models.txt";

   // If it doesn't exist yet, initialize it
   if (!QFile(siteModelsTxtFilePath).exists())
   {
      OutputSiteInitialModelsFile(siteModelsTxtFilePath);
   }

   // Check for a site/models folder
   QString siteModelsPath = sitePath + "/models/";
   // If the directory does not exist, create it
   if (!UtPath(siteModelsPath.toStdString()).Exists())
   {
      UtPath(siteModelsPath.toStdString()).Mkdir();
   }

   // Convert the obj file to a new file in the same directory
   // If successful, continue
   if (ConvertModelTo(localObjFilePathWithoutExtension, aFileType))
   {
      // Determine the target path the final model to site/models
      QString finalPath = siteModelsPath + name + GetFileExtension(aFileType);

      // Determine the local model path
      QString modelPath = GetLocalModelPath() + name + GetFileExtension(aFileType);

      // Copy the model from local to site/models
      QFile finalModel(modelPath);

      // Remove the previous file (if it exists)
      if (QFile(finalPath).exists())
      {
         QFile(finalPath).remove();
      }

      finalModel.copy(finalPath);

      // Add the vehicle to models.txt using a relative path
      QString relativeFinalPath = "models/" + name + GetFileExtension(aFileType);
      AddVehicleToModelsList(siteModelsTxtFilePath, relativeFinalPath, vehicle);

      return;
   }
   else
   {
      // We were unable to convert the model, so we will use the OBJ files instead

      // Determine the target path the final model to site/models
      QString finalPathObj = siteModelsPath + name + ".obj";
      QString finalPathMtl = siteModelsPath + name + ".mtl";

      // Determine the local model path
      QString modelPathObj = GetLocalModelPath() + name + ".obj";
      QString modelPathMtl = GetLocalModelPath() + name + ".mtl";

      // Copy the model obj from local to site/models
      QFile finalModelObj(modelPathObj);

      // Remove the previous file (if it exists)
      if (QFile(finalPathObj).exists())
      {
         QFile(finalPathObj).remove();
      }

      finalModelObj.copy(finalPathObj);

      // Copy the model mtl from local to site/models
      QFile finalModelMtl(modelPathMtl);

      // Remove the previous file (if it exists)
      if (QFile(finalPathMtl).exists())
      {
         QFile(finalPathMtl).remove();
      }

      finalModelMtl.copy(finalPathMtl);

      // Add the vehicle to models.txt using a relative path
      QString relativeFinalPath = "models/" + name + ".obj";
      AddVehicleToModelsList(siteModelsTxtFilePath, relativeFinalPath, vehicle);
   }
}

void GeometryObjFile::OutputVertices(QTextStream& aOutputStream)
{
   aOutputStream << "\n";
   aOutputStream << "# " << (int)mVertexList.size() << " vertices\n";

   for (auto vertex : mVertexList)
   {
      aOutputStream << "v  ";
      OutputData(aOutputStream, vertex.X(), 8, 3, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " ";
      OutputData(aOutputStream, vertex.Y(), 8, 3, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " ";
      OutputData(aOutputStream, vertex.Z(), 8, 3, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
   }
   aOutputStream << "\n";
}

void GeometryObjFile::OutputNormals(QTextStream& aOutputStream)
{
   aOutputStream << "\n";
   aOutputStream << "# " << (int)mNormalList.size() << " normals\n";

   for (auto normal : mNormalList)
   {
      aOutputStream << "vn  ";
      OutputData(aOutputStream, normal.X(), 8, 5, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " ";
      OutputData(aOutputStream, normal.Y(), 8, 5, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << " ";
      OutputData(aOutputStream, normal.Z(), 8, 5, QTextStream::FieldAlignment::AlignRight);
      aOutputStream << "\n";
   }
   aOutputStream << "\n";
}

void GeometryObjFile::OutputTextureCoords(QTextStream& aOutputStream)
{
   aOutputStream << "\n";
   aOutputStream << "# Texture coords\n";
   aOutputStream << "vt 0.0 0.0\n";
   aOutputStream << "vt 0.5 0.5\n";
   aOutputStream << "vt 1.0 1.0\n";
   aOutputStream << "\n";
}

void GeometryObjFile::OutputFaces(QTextStream& aOutputStream)
{
   aOutputStream << "\n";
   aOutputStream << "# " << (int)mFaceVertexList.size() << " faces/polygons\n";

   // Set material (do this once)
   aOutputStream << "# Set the material\n";
   aOutputStream << "usemtl material0\n";
   aOutputStream << "\n";

   // Output faces (with texture) ........... aOutputStream << "f 1/1/1  2/2/2  3/3/3" << "\n";
   // Output faces (without texture) ........ aOutputStream << "f 1//1  2//2  3//3" << "\n";

   for (auto face : mFaceVertexList)
   {
      aOutputStream << "f ";
      for (auto& vertex : face)
      {
         aOutputStream << " " << vertex + 1 << "/1/" << vertex + 1;
      }
      aOutputStream << "\n";
   }
   aOutputStream << "\n";
}

void GeometryObjFile::DrawEngines(Designer::Vehicle* aVehicle)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();

      if (geometryType == "GeometryEngine")
      {
         GeometryEngine* engine = dynamic_cast<GeometryEngine*>(geometryObject);
         if (engine != nullptr)
         {
            double length_ft = engine->GetLength_ft();
            double diam_ft = engine->GetDiameter_ft();
            double offset_ft = engine->GetThrustOffset_ft();

            UtVec3dX refPt_ft = engine->GetRefPoint_ft();

            double symmetryHorizontalY = engine->GetSymmetryHorizontalY();

            DrawEngine(refPt_ft.X(), refPt_ft.Y(), refPt_ft.Z(), diam_ft, length_ft, offset_ft,
                       engine->GetYaw_deg(), engine->GetPitch_deg(), engine->GetRoll_deg());

            if (engine->IsSymmetrical())
            {
               // Now, determine location of other engine
               double currentEngineY = refPt_ft.Y();
               double delta = currentEngineY - symmetryHorizontalY;
               double otherEngineY = currentEngineY - 2.0 * delta;

               DrawEngine(refPt_ft.X(), otherEngineY, refPt_ft.Z(), diam_ft, length_ft, offset_ft,
                          -engine->GetYaw_deg(), engine->GetPitch_deg(), -engine->GetRoll_deg());
            }
         }
      }
   }
}

void GeometryObjFile::DrawWingsAndSurfaces(Designer::Vehicle* aVehicle)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();

      if (geometryType == "GeometryWing")
      {
         Designer::GeometryWing* wing = (Designer::GeometryWing*)geometryObject;

         if (wing != nullptr)
         {
            UtVec3dX refPt = wing->GetRefPoint_ft();
            double aPosX = refPt.X();
            double aPosY = refPt.Y();
            double aPosZ = refPt.Z();
            double aSpan = wing->GetSpan_ft() * 0.5;       // Note that a wingspan is different from other surfaces span values
            double aSweep_deg = wing->GetSweepAngle_deg();
            double aRootChord = wing->GetRootChord_ft();
            double aTipChord = wing->GetTipChord_ft();
            double aThicknessRatio = wing->GetThicknessRatio();
            double aDihedral_deg = wing->GetDihedralAngle_deg();
            double aIncidence_deg = wing->GetIncidenceAngle_deg();
            double symmetryHorizontalY = wing->GetSymmetryHorizontalY();

            {
               DrawSurface(aPosX, aPosY, aPosZ, aSpan, aSweep_deg, aRootChord, aTipChord,
                           aThicknessRatio, aDihedral_deg, aIncidence_deg, 20);
               if (wing->IsSymmetrical())
               {
                  // Now, determine location of other surface
                  double currentSurfaceY = refPt.Y();
                  double delta = currentSurfaceY - symmetryHorizontalY;
                  double otherSurfaceY = currentSurfaceY - 2.0 * delta;

                  DrawSurface(aPosX, otherSurfaceY, aPosZ, aSpan, aSweep_deg, aRootChord,
                              aTipChord, aThicknessRatio, -180.0 - aDihedral_deg, -aIncidence_deg, 20);
               }
            }
         }
      }
      else if (geometryType == "GeometrySurface")
      {
         Designer::GeometrySurface* surface = (Designer::GeometrySurface*)geometryObject;

         if (surface != nullptr)
         {
            UtVec3dX refPt = surface->GetRefPoint_ft();
            double aPosX = refPt.X();
            double aPosY = refPt.Y();
            double aPosZ = refPt.Z();
            double aSpan = surface->GetSpan_ft();
            double aSweep_deg = surface->GetSweepAngle_deg();
            double aRootChord = surface->GetRootChord_ft();
            double aTipChord = surface->GetTipChord_ft();
            double aThicknessRatio = surface->GetThicknessRatio();
            double aDihedral_deg = surface->GetDihedralAngle_deg();
            double aIncidence_deg = surface->GetIncidenceAngle_deg();
            double aAltPosRefRadius = surface->GetFinRefRadius_ft();
            double symmetryHorizontalY = surface->GetSymmetryHorizontalY();
            double symmetryVerticalZ = surface->GetSymmetryVerticalZ();

            // Get data for the control angle(s)
            double controlAngle1_deg = 0.0;
            double controlAngle2_deg = 0.0;
            double controlAngle3_deg = 0.0;
            double controlAngle4_deg = 0.0;

            // Check for the control angle
            VehicleAero* vehicleAero = aVehicle->GetAero();
            if (vehicleAero != nullptr)
            {
               const std::vector<AeroObject*> list =
                  vehicleAero->GetAeroObjectsGeneratedFrom(surface->GetName());
               size_t numControls = list.size();
               if (numControls > 0)
               {
                  controlAngle1_deg = list.at(0)->GetControlAngle_deg();
                  if (numControls > 1)
                  {
                     controlAngle2_deg = list.at(1)->GetControlAngle_deg();
                  }
                  if (numControls > 2)
                  {
                     controlAngle3_deg = list.at(2)->GetControlAngle_deg();
                  }
                  if (numControls > 3)
                  {
                     controlAngle4_deg = list.at(3)->GetControlAngle_deg();
                  }
               }
            }
            else
            {
               MoverCreatorMainWindow().CreateErrorBox("Vehicle does not include a valid aero component!");
            }

            std::string symmetryString = surface->GetSymmetryString().toStdString();

            // Default pattern is + pattern
            double finAngle_rad = 0.0;
            if (symmetryString == "X Pattern") { finAngle_rad = UtMath::cPI_OVER_4; }

            double finPos1X = refPt.X();
            double finPos1Y = refPt.Y() + aAltPosRefRadius * cos(finAngle_rad);
            double finPos1Z = refPt.Z() - aAltPosRefRadius * sin(finAngle_rad);
            double finDihedral1_deg = finAngle_rad * UtMath::cDEG_PER_RAD;

            finAngle_rad += UtMath::cPI_OVER_2;
            double finPos2X = refPt.X();
            double finPos2Y = refPt.Y() + aAltPosRefRadius * cos(finAngle_rad);
            double finPos2Z = refPt.Z() - aAltPosRefRadius * sin(finAngle_rad);
            double finDihedral2_deg = finAngle_rad * UtMath::cDEG_PER_RAD;

            finAngle_rad += UtMath::cPI_OVER_2;
            double finPos3X = refPt.X();
            double finPos3Y = refPt.Y() + aAltPosRefRadius * cos(finAngle_rad);
            double finPos3Z = refPt.Z() - aAltPosRefRadius * sin(finAngle_rad);
            double finDihedral3_deg = finAngle_rad * UtMath::cDEG_PER_RAD;

            finAngle_rad += UtMath::cPI_OVER_2;
            double finPos4X = refPt.X();
            double finPos4Y = refPt.Y() + aAltPosRefRadius * cos(finAngle_rad);
            double finPos4Z = refPt.Z() - aAltPosRefRadius * sin(finAngle_rad);
            double finDihedral4_deg = finAngle_rad * UtMath::cDEG_PER_RAD;

            {
               if ((symmetryString == "Single") ||
                   (symmetryString == "Horizontal") ||
                   (symmetryString == "Vertical"))
               {
                  double incidence_deg = aIncidence_deg + controlAngle1_deg;
                  DrawSurface(aPosX, aPosY, aPosZ, aSpan, aSweep_deg, aRootChord, aTipChord,
                              aThicknessRatio, aDihedral_deg, incidence_deg, 5);
               }
               else if ((symmetryString == "+ Pattern") || (symmetryString == "X Pattern"))
               {
                  double incidence_deg = aIncidence_deg + controlAngle1_deg;
                  DrawSurface(finPos1X, finPos1Y, finPos1Z, aSpan, aSweep_deg, aRootChord,
                              aTipChord, aThicknessRatio, finDihedral1_deg, incidence_deg, 5);
               }

               if (surface->IsSymmetrical())
               {
                  if (symmetryString == "Horizontal")
                  {
                     // Now, determine location of other surface
                     double currentSurfaceY = refPt.Y();
                     double delta = currentSurfaceY - symmetryHorizontalY;
                     double otherSurfaceY = currentSurfaceY - 2.0 * delta;

                     double incidence_deg = -aIncidence_deg - controlAngle2_deg;
                     DrawSurface(aPosX, otherSurfaceY, aPosZ, aSpan, aSweep_deg, aRootChord,
                                 aTipChord, aThicknessRatio, -180.0 - aDihedral_deg, incidence_deg, 5);
                  }
                  else if (symmetryString == "Vertical")
                  {
                     // Now, determine location of other surface
                     double currentSurfaceZ = refPt.Z();
                     double delta = currentSurfaceZ - symmetryVerticalZ;
                     double otherSurfaceZ = currentSurfaceZ - 2.0 * delta;

                     double incidence_deg = -aIncidence_deg - controlAngle2_deg;
                     DrawSurface(aPosX, aPosY, otherSurfaceZ, aSpan, aSweep_deg, aRootChord,
                                 aTipChord, aThicknessRatio, -aDihedral_deg, incidence_deg, 5);
                  }
                  else if ((symmetryString == "+ Pattern") || (symmetryString == "X Pattern"))
                  {
                     double incidence_deg = aIncidence_deg + controlAngle2_deg;
                     DrawSurface(finPos2X, finPos2Y, finPos2Z, aSpan, aSweep_deg, aRootChord,
                                 aTipChord, aThicknessRatio, finDihedral2_deg, incidence_deg, 5);

                     incidence_deg = aIncidence_deg + controlAngle3_deg;
                     DrawSurface(finPos3X, finPos3Y, finPos3Z, aSpan, aSweep_deg, aRootChord,
                                 aTipChord, aThicknessRatio, finDihedral3_deg, incidence_deg, 5);

                     incidence_deg = aIncidence_deg + controlAngle4_deg;
                     DrawSurface(finPos4X, finPos4Y, finPos4Z, aSpan, aSweep_deg, aRootChord,
                                 aTipChord, aThicknessRatio, finDihedral4_deg, incidence_deg, 5);
                  }
               }
            }
         }
      }
   }
}

void GeometryObjFile::DrawDishes(Designer::Vehicle* aVehicle)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryDish")
      {
         Designer::GeometryDish* dish = (Designer::GeometryDish*)geometryObject;

         if (dish != nullptr)
         {
            UtVec3dX refPt = dish->GetRefPoint_ft();
            double aPosX = refPt.X();
            double aPosY = refPt.Y();
            double aPosZ = refPt.Z();

            double aDiameter_ft  = dish->GetDiameter_ft();
            double aThickness_ft = dish->GetThickness_ft();

            DrawDish(aPosX, aPosY, aPosZ, aDiameter_ft, aThickness_ft);
         }
      }
   }
}

void GeometryObjFile::DrawBodiesAndNacelles(Designer::Vehicle* aVehicle)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if ((geometryType == "GeometryFuselage") || (geometryType == "GeometryBody"))
      {
         Designer::GeometryBody* body = (Designer::GeometryBody*)geometryObject;
         if (body != nullptr)
         {
            UtVec3dX refPt               = body->GetRefPoint_ft();
            double posX                  = refPt.X();
            double posY                  = refPt.Y();
            double posZ                  = refPt.Z();
            double length                = body->GetLength_ft();
            double height                = body->GetHeight_ft();
            double width                 = body->GetWidth_ft();
            double frontLength           = body->GetForwardShapeLength_ft();
            double rearLength            = body->GetAftShapeLength_ft();
            double rearDiam              = body->GetAftShapeDiam_ft();
            double yaw_deg               = body->GetYawAngle_deg();
            double pitch_deg             = body->GetPitchAngle_deg();
            double roll_deg              = body->GetRollAngle_deg();
            double symmetryHorizontalY   = body->GetSymmetryHorizontalY();
            GeometryBody::ForwardShapeType frontShapeString = body->GetForwardShapeType();
            GeometryBody::AftShapeType aftShapeString = body->GetAftShapeType();

            if (aftShapeString == GeometryBody::AftShapeType::cBLUNT)
            {
               rearLength = 0;
            }

            {
               DrawBody(posX, posY, posZ, length, height, width, frontShapeString, frontLength,
                        aftShapeString, rearLength, rearDiam, yaw_deg, pitch_deg, roll_deg);

               if (body->IsSymmetrical())
               {
                  // Consider the other fuselage/body
                  double currentBodyY = refPt.Y();
                  double delta = currentBodyY - symmetryHorizontalY;
                  double otherBodyY = currentBodyY - 2.0 * delta;

                  DrawBody(posX, otherBodyY, posZ, length, height, width, frontShapeString, frontLength,
                           aftShapeString, rearLength, rearDiam, -yaw_deg, pitch_deg, -roll_deg);
               }
            }
         }
      }
      else if (geometryType == "GeometryNacelle")
      {
         Designer::GeometryNacelle* nacelle = (Designer::GeometryNacelle*)geometryObject;
         if (nacelle != nullptr)
         {
            UtVec3dX refPt_ft = nacelle->GetRefPoint_ft();
            double posX = refPt_ft.X();
            double posY = refPt_ft.Y();
            double posZ = refPt_ft.Z();
            double length = nacelle->GetLength_ft();
            double height = nacelle->GetHeight_ft();

            double heightInner = nacelle->GetHeightInner_ft();
            double heightOuter = nacelle->GetHeightOuter_ft();
            double forwardSweepLength_ft = nacelle->GetForwardSweepLength_ft();

            double width = nacelle->GetWidth_ft();
            double thickness = nacelle->GetThickness_ft();
            double yaw_deg = nacelle->GetYawAngle_deg();
            double pitch_deg = nacelle->GetPitchAngle_deg();
            double roll_deg = nacelle->GetRollAngle_deg();

            double symmetryHorizontalY = nacelle->GetSymmetryHorizontalY();

            std::string overallShapeString = nacelle->GetOverallShapeString().toStdString();
            std::string aftSectionString = nacelle->GetAftSectionString().toStdString();
            double aftSectionLength_ft = nacelle->GetAftSectionLength_ft();

            {
               DrawNacelle(posX, posY, posZ, length, height, heightInner, heightOuter, width, thickness, forwardSweepLength_ft,
                           overallShapeString, aftSectionString, aftSectionLength_ft, yaw_deg, pitch_deg, roll_deg);
               if (nacelle->IsSymmetrical())
               {
                  // "Rounded" << "Half-Round-Right" << "Half-Round-Left" << "Half-Round-Top" << "Half-Round-Bottom"
                  // << "Flat-Sided" << "Flat-Swept-Right" << "Flat-Swept-Left"

                  // Consider the other nacelle/body
                  double currentBodyY = refPt_ft.Y();
                  double delta = currentBodyY - symmetryHorizontalY;
                  double otherBodyY = currentBodyY - 2.0 * delta;

                  if ((overallShapeString == "Half-Round-Right") || (overallShapeString == "Half-Round-Left") ||
                      (overallShapeString == "Flat-Swept-Right") || (overallShapeString == "Flat-Swept-Left"))
                  {
                     yaw_deg  = -yaw_deg;
                     roll_deg = 180.0 - roll_deg;
                  }
                  else
                  {
                     yaw_deg  = -yaw_deg;
                     roll_deg = -roll_deg;
                  }

                  DrawNacelle(posX, otherBodyY, posZ, length, height, heightInner, heightOuter, width, thickness, forwardSweepLength_ft,
                              overallShapeString, aftSectionString, aftSectionLength_ft, yaw_deg, pitch_deg, roll_deg);
               }
            }
         }
      }
   }
}

void GeometryObjFile::DrawCanopy(Designer::Vehicle* aVehicle)
{
   VehicleGeometry& vehicleGeometry = aVehicle->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryFuselage")
      {
         Designer::GeometryFuselage* fuselage = (Designer::GeometryFuselage*)geometryObject;
         if (fuselage != nullptr)
         {
            double posX = fuselage->GetRefPoint_ft().X();
            double posY = fuselage->GetRefPoint_ft().Y();
            double posZ = fuselage->GetRefPoint_ft().Z();
            double length = fuselage->CanopyTotalLength();
            double height = fuselage->CanopyHeight();
            double width = fuselage->CanopyWidth();
            double frontLength = fuselage->CanopyForwardLength();
            double rearLength = fuselage->CanopyAftLength();
            double yaw_deg = fuselage->GetYawAngle_deg();
            double pitch_deg = fuselage->GetPitchAngle_deg();
            double roll_deg = fuselage->GetRollAngle_deg();

            // Create position vector for fuselage
            UtVec3dX pos(posX, posY, posZ);

            // Create dcm for fuselage
            double yaw_rad   = yaw_deg * UtMath::cRAD_PER_DEG;
            double pitch_rad = pitch_deg * UtMath::cRAD_PER_DEG;
            double roll_rad  = roll_deg * UtMath::cRAD_PER_DEG;
            UtDCM dcm(yaw_rad, pitch_rad, roll_rad);

            // Set relative location vector
            UtVec3dX vec(fuselage->CanopyRefX(), fuselage->CanopyRefY(), fuselage->CanopyRefZ());

            // Get location in world coords
            vec = dcm.InverseTransform(vec);
            UtVec3dX posVec = vec + pos;

            if (fuselage->CanopyPresent())
            {
               DrawBody(posVec.X(), posVec.Y(), posVec.Z(), length, height, width,
                        GeometryBody::ForwardShapeType::cOGIVE, frontLength, GeometryBody::AftShapeType::cOGIVE,
                        rearLength, 0.0, yaw_deg, pitch_deg, roll_deg);
            }
         }
      }
   }
}

void GeometryObjFile::DrawDish(double aPosX,
                               double aPosY,
                               double aPosZ,
                               double aDiameter_ft,
                               double aThickness_ft)
{
   // Check data
   if ((aDiameter_ft <= 0.0) || (aThickness_ft <= 0.0)) { return; }

   // Data points
   double r[19];
   double h[19];
   double normal_r[19];
   double normal_h[19];

   double radius = 0.5 * aDiameter_ft;
   double halfThickness = 0.5 * aThickness_ft;

   h[0] = -halfThickness;
   r[0] = 0.0;
   h[18] = halfThickness;
   r[18] = 0.0;

   normal_r[0] = 0;
   normal_h[0] = -1.0;
   normal_r[18] = 0.0;
   normal_h[18] = 1.0;

   int count = 1;
   for (int iang_deg = -80; iang_deg <= 80; iang_deg += 10)
   {
      double ang_r = (double)iang_deg * UtMath::cRAD_PER_DEG;
      r[count] = radius * cos(ang_r);
      h[count] = halfThickness * sin(ang_r);
      ++count;
   }

   for (int i = 1; i < 18; ++i)
   {
      UtVec3dX lastPt(r[i - 1], h[i - 1], 0.0);
      UtVec3dX currentPt(r[i], h[i], 0.0);
      UtVec3dX nextPt(r[i + 1], h[i + 1], 0.0);
      UtVec3dX lastVec = lastPt - currentPt;
      UtVec3dX nextVec = nextPt - currentPt;
      UtVec3dX posZVec(0.0, 0.0, 1.0);
      UtVec3dX negZVec(0.0, 0.0, -1.0);

      UtVec3dX lastNormVec = lastVec.Cross(negZVec);
      UtVec3dX nextNormVec = nextVec.Cross(posZVec);
      UtVec3dX normVec = lastNormVec + nextNormVec;
      normVec.Normalize();
      normal_r[i] = normVec.X();
      normal_h[i] = normVec.Y();
   }

   ObjPushMatrix();
   ObjTranslated(aPosX, -aPosZ, aPosY);

   int delta_iang = 10;
   for (int iang = delta_iang; iang <= 360; iang += delta_iang)
   {
      double ang0_rad = (double)(iang - delta_iang) * UtMath::cRAD_PER_DEG;
      double ang1_rad = (double)iang * UtMath::cRAD_PER_DEG;


      {
         double pt0x = r[0];
         double pt0y = h[0];
         double pt0z = r[0];

         double norm0x = normal_r[0];
         double norm0y = normal_h[0];
         double norm0z = normal_r[0];

         double pt1x = r[1] * sin(ang0_rad);
         double pt1y = h[1];
         double pt1z = r[1] * cos(ang0_rad);

         double norm1x = normal_r[1] * sin(ang0_rad);
         double norm1y = normal_h[1];
         double norm1z = normal_r[1] * cos(ang0_rad);

         double pt2x = r[1] * sin(ang1_rad);
         double pt2y = h[1];
         double pt2z = r[1] * cos(ang1_rad);

         double norm2x = normal_r[1] * sin(ang1_rad);
         double norm2y = normal_h[1];
         double norm2z = normal_r[1] * cos(ang1_rad);

         ObjBegin();
         ObjNormal3d(norm2x, norm2y, norm2z);
         ObjVertex3d(pt2x, pt2y, pt2z);
         ObjNormal3d(norm1x, norm1y, norm1z);
         ObjVertex3d(pt1x, pt1y, pt1z);
         ObjNormal3d(norm0x, norm0y, norm0z);
         ObjVertex3d(pt0x, pt0y, pt0z);
         ObjEnd();
      }

      for (int j = 1; j < 17; ++j)
      {
         double pt0x = r[j] * sin(ang0_rad);
         double pt0y = h[j];
         double pt0z = r[j] * cos(ang0_rad);

         double norm0x = normal_r[j] * sin(ang0_rad);
         double norm0y = normal_h[j];
         double norm0z = normal_r[j] * cos(ang0_rad);

         double pt1x = r[j + 1] * sin(ang0_rad);
         double pt1y = h[j + 1];
         double pt1z = r[j + 1] * cos(ang0_rad);

         double norm1x = normal_r[j + 1] * sin(ang0_rad);
         double norm1y = normal_h[j + 1];
         double norm1z = normal_r[j + 1] * cos(ang0_rad);

         double pt2x = r[j + 1] * sin(ang1_rad);
         double pt2y = h[j + 1];
         double pt2z = r[j + 1] * cos(ang1_rad);

         double norm2x = normal_r[j + 1] * sin(ang1_rad);
         double norm2y = normal_h[j + 1];
         double norm2z = normal_r[j + 1] * cos(ang1_rad);

         double pt3x = r[j] * sin(ang1_rad);
         double pt3y = h[j];
         double pt3z = r[j] * cos(ang1_rad);

         double norm3x = normal_r[j] * sin(ang1_rad);
         double norm3y = normal_h[j];
         double norm3z = normal_r[j] * cos(ang1_rad);

         ObjBegin();
         ObjNormal3d(norm3x, norm3y, norm3z);
         ObjVertex3d(pt3x, pt3y, pt3z);
         ObjNormal3d(norm2x, norm2y, norm2z);
         ObjVertex3d(pt2x, pt2y, pt2z);
         ObjNormal3d(norm1x, norm1y, norm1z);
         ObjVertex3d(pt1x, pt1y, pt1z);
         ObjNormal3d(norm0x, norm0y, norm0z);
         ObjVertex3d(pt0x, pt0y, pt0z);
         ObjEnd();
      }

      {
         double pt0x = r[18];
         double pt0y = h[18];
         double pt0z = r[18];

         double norm0x = normal_r[18];
         double norm0y = normal_h[18];
         double norm0z = normal_r[18];

         double pt1x = r[17] * sin(ang0_rad);
         double pt1y = h[17];
         double pt1z = r[17] * cos(ang0_rad);

         double norm1x = normal_r[17] * sin(ang0_rad);
         double norm1y = normal_h[17];
         double norm1z = normal_r[17] * cos(ang0_rad);

         double pt2x = r[17] * sin(ang1_rad);
         double pt2y = h[17];
         double pt2z = r[17] * cos(ang1_rad);

         double norm2x = normal_r[17] * sin(ang1_rad);
         double norm2y = normal_h[17];
         double norm2z = normal_r[17] * cos(ang1_rad);

         ObjBegin();
         ObjNormal3d(norm0x, norm0y, norm0z);
         ObjVertex3d(pt0x, pt0y, pt0z);
         ObjNormal3d(norm1x, norm1y, norm1z);
         ObjVertex3d(pt1x, pt1y, pt1z);
         ObjNormal3d(norm2x, norm2y, norm2z);
         ObjVertex3d(pt2x, pt2y, pt2z);
         ObjEnd();
      }
   }

   ObjPopMatrix();
}

void GeometryObjFile::DrawBodyCylinder(double aPosX,
                                       double aPosY,
                                       double aPosZ,
                                       double aLength,
                                       double aHeight,
                                       double aWidth,
                                       bool   aAft)
{
   ObjPointCollection pointCollection(true, *this);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;

   int numSteps = 10;

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();

   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }

   // Add far end ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      ObjPushMatrix();
      ObjRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw();
      ObjPopMatrix();
   }
   else
   {
      pointCollection.Draw();
   }
}

void GeometryObjFile::DrawBodyOgive(double aPosX,
                                    double aPosY,
                                    double aPosZ,
                                    double aLength,
                                    double aHeight,
                                    double aWidth,
                                    bool   aAft)
{
   ObjPointCollection pointCollection(true, *this);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;
   double lengthFactor = 0.0;
   double radiusFactor = 1.0;

   int numSteps = 10;

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add second ring
   lengthFactor = 0.3;
   radiusFactor = 0.9;
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add third ring
   lengthFactor = 0.55;
   radiusFactor = 0.7;
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add fourth ring
   lengthFactor = 0.8;
   radiusFactor = 0.4;
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add fifth (last) ring
   lengthFactor = 1.0;
   radiusFactor = 0.001;
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      ObjPushMatrix();
      ObjRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw();
      ObjPopMatrix();
   }
   else
   {
      pointCollection.Draw();
   }
}

void GeometryObjFile::DrawBodyCone(double aPosX,
                                   double aPosY,
                                   double aPosZ,
                                   double aLength,
                                   double aHeight,
                                   double aWidth,
                                   bool   aAft)
{
   ObjPointCollection pointCollection(true, *this);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;

   int numSteps = 10;

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }

   // Add nose/tip ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = -aPosZ;
      double z = aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = -aPosZ;
      double z = aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = -aPosZ;
      double z = aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = -aPosZ;
      double z = aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      ObjPushMatrix();
      ObjRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw();
      ObjPopMatrix();
   }
   else
   {
      pointCollection.Draw();
   }
}

void GeometryObjFile::DrawBodySphere(double aPosX,
                                     double aPosY,
                                     double aPosZ,
                                     double aLength,
                                     double aHeight,
                                     double aWidth,
                                     bool   aAft)
{
   ObjPointCollection pointCollection(true, *this);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;
   double lengthFactor = 0.0;
   double radiusFactor = 1.0;

   int numSteps = 10;

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add second ring
   lengthFactor = 0.2;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add third ring
   lengthFactor = 0.4;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add fourth ring
   lengthFactor = 0.6;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add fifth ring
   lengthFactor = 0.8;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add sixth ring
   lengthFactor = 0.9;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Add seventh (and last) ring
   lengthFactor = 0.999;
   radiusFactor = sqrt(1.0 - lengthFactor * lengthFactor);
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = -EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight * radiusFactor;
      double z = EllipticalX_GivenY(y, halfWidth * radiusFactor, halfHeight * radiusFactor);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength * lengthFactor;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      ObjPushMatrix();
      ObjRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw();
      ObjPopMatrix();
   }
   else
   {
      pointCollection.Draw();
   }
}

void GeometryObjFile::DrawBodyBoatTail(double aPosX,
                                       double aPosY,
                                       double aPosZ,
                                       double aLength,
                                       double aHeight,
                                       double aWidth,
                                       double aDiam,
                                       bool   aAft)
{
   ObjPointCollection pointCollection(true, *this);

   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;
   double radius = 0.5 * aDiam;

   int numSteps = 10;

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   // Add main/base ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX;
      pointCollection.AddPoint(x, y, z);
   }

   // Add boat tail ring
   pointCollection.AddRing();
   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = -EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = -EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals
   pointCollection.CalculatePointCollection();

   // Draw
   if (aAft)
   {
      ObjPushMatrix();
      ObjRotated(180.0, 0.0, -1.0, 0.0);
      pointCollection.Draw();
   }
   else
   {
      pointCollection.Draw();
   }

   // Now, handle the end disk

   std::vector<UtVec3dX> vertices;

   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = -EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = -EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * radius;
      double z = EllipticalX_GivenY(y, radius, radius);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }

   // Draw disk

   ObjBegin();
   // Need to run through points in reverse to get proper winding order
   for (auto revIter = vertices.rbegin(); revIter != vertices.rend(); ++revIter)
   {
      ObjNormal3d(1.0, 0.0, 0.0);
      UtVec3dX& vec = *revIter;
      ObjVertex3d(vec.X(), vec.Y(), vec.Z());
   }
   ObjEnd();

   if (aAft)
   {
      ObjPopMatrix();
   }
}

void GeometryObjFile::DrawBodyBlunt(double aPosX,
                                    double aPosY,
                                    double aPosZ,
                                    double aLength,
                                    double aHeight,
                                    double aWidth,
                                    bool   aAft)
{
   double halfHeight = 0.5 * aHeight;
   double halfWidth = 0.5 * aWidth;

   int numSteps = 10;

   double stepFactor = 1.0 / static_cast<double>(numSteps);

   std::vector<UtVec3dX> vertices;

   for (int hgt = 0; hgt < numSteps; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = numSteps; hgt > 0; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = 0; hgt > -numSteps; --hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = -EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }
   for (int hgt = -numSteps; hgt < 0; ++hgt)
   {
      double y = stepFactor * (double)hgt * halfHeight;
      double z = EllipticalX_GivenY(y, halfWidth, halfHeight);
      y += -aPosZ;
      z += aPosY;
      double x = aPosX + aLength;
      UtVec3dX vert(x, y, z);
      vertices.push_back(vert);
   }

   // Draw disk
   if (aAft)
   {
      ObjPushMatrix();
      ObjRotated(180.0, 0.0, -1.0, 0.0);
   }

   ObjBegin();

   // Need to run through points in reverse to get proper winding order
   for (std::vector<UtVec3dX>::reverse_iterator revIter = vertices.rbegin();
        revIter != vertices.rend(); ++revIter)
   {
      ObjNormal3d(1.0, 0.0, 0.0);
      UtVec3dX& vec = *revIter;
      ObjVertex3d(vec.X(), vec.Y(), vec.Z());
   }

   ObjEnd();

   if (aAft)
   {
      ObjPopMatrix();
   }
}

void GeometryObjFile::DrawBody(double                         aPosX,
                               double                         aPosY,
                               double                         aPosZ,
                               double                         aLength,
                               double                         aHeight,
                               double                         aWidth,
                               GeometryBody::ForwardShapeType aFrontShape,
                               double                         aFrontLength,
                               GeometryBody::AftShapeType     aAftShape,
                               double                         aRearLength,
                               double                         aBoatTailDiam,
                               double                         aYaw_deg,
                               double                         aPitch_deg,
                               double                         aRoll_deg)
{
   // Verify correct values
   if ((aFrontLength + aRearLength) >= aLength) { return; }

   ObjPushMatrix();
   ObjTranslated(aPosX, -aPosZ, aPosY);
   ObjRotated(aYaw_deg, 0.0, -1.0, 0.0);
   ObjRotated(aPitch_deg, 0.0, 0.0, 1.0);
   ObjRotated(aRoll_deg, 1.0, 0.0, 0.0);

   // Draw forward shape

   switch (aFrontShape)
   {
      case Designer::GeometryBody::ForwardShapeType::cCONE:
         DrawBodyCone((aLength * 0.5 - aFrontLength), 0.0, 0.0, aFrontLength, aHeight, aWidth);
         break;
      case Designer::GeometryBody::ForwardShapeType::cOGIVE:
         DrawBodyOgive((aLength * 0.5 - aFrontLength), 0.0, 0.0, aFrontLength, aHeight, aWidth);
         break;
      case Designer::GeometryBody::ForwardShapeType::cROUND:
         DrawBodySphere((aLength * 0.5 - aFrontLength), 0.0, 0.0, aFrontLength, aHeight, aWidth);
         break;
      case Designer::GeometryBody::ForwardShapeType::cBLUNT:
         aFrontLength = 0.0; // Ensure front length is zero if blunt
         DrawBodyBlunt((aLength * 0.5 - aFrontLength), 0.0, 0.0, aFrontLength, aHeight, aWidth);
         break;
      default:
         break;
   }

   // Draw main/center shape (cylinder)
   DrawBodyCylinder((-aLength * 0.5 + aRearLength), 0.0, 0.0, aLength - aFrontLength - aRearLength, aHeight, aWidth);

   switch (aAftShape)
   {
      case Designer::GeometryBody::AftShapeType::cCONE:
         DrawBodyCone((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, true);
         break;
      case Designer::GeometryBody::AftShapeType::cOGIVE:
         DrawBodyOgive((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, true);
         break;
      case Designer::GeometryBody::AftShapeType::cROUND:
         DrawBodySphere((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, true);
         break;
      case Designer::GeometryBody::AftShapeType::cBOATTAIL:
         DrawBodyBoatTail((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, aBoatTailDiam, true);
         break;
      case Designer::GeometryBody::AftShapeType::cBLUNT:
         aRearLength = 0.0; // Ensure aft length is zero if blunt
         DrawBodyBlunt((aLength * 0.5 - aRearLength), 0.0, 0.0, aRearLength, aHeight, aWidth, true);
         break;
      default:
         break;
   }

   ObjPopMatrix();
}

void GeometryObjFile::DrawBoxWithHole(double aLength, double aHeight, double aWidth, double aThickness)
{
   double xFront  = aLength *  0.5;
   double xBack   = aLength * -0.5;
   double yTop    = aHeight *  0.5;
   double yBottom = aHeight * -0.5;
   double zRight  = aWidth  *  0.5;
   double zLeft   = aWidth  * -0.5;

   // Outside Right
   ObjBegin();
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(xBack, yBottom, zRight);
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(xFront, yBottom, zRight);
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(xFront, yTop, zRight);
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(xBack, yTop, zRight);
   ObjEnd();

   // Outside Left
   ObjBegin();
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(xFront, yBottom, zLeft);
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(xBack, yBottom, zLeft);
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(xBack, yTop, zLeft);
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(xFront, yTop, zLeft);
   ObjEnd();

   // Outside Top
   ObjBegin();
   ObjNormal3d(0.0, 1.0, 0.0);
   ObjVertex3d(xFront, yTop, zLeft);
   ObjNormal3d(0.0, 1.0, 0.0);
   ObjVertex3d(xBack, yTop, zLeft);
   ObjNormal3d(0.0, 1.0, 0.0);
   ObjVertex3d(xBack, yTop, zRight);
   ObjNormal3d(0.0, 1.0, 0.0);
   ObjVertex3d(xFront, yTop, zRight);
   ObjEnd();

   // Outside Bottom
   ObjBegin();
   ObjNormal3d(0.0, -1.0, 0.0);
   ObjVertex3d(xFront, yBottom, zLeft);
   ObjNormal3d(0.0, -1.0, 0.0);
   ObjVertex3d(xFront, yBottom, zRight);
   ObjNormal3d(0.0, -1.0, 0.0);
   ObjVertex3d(xBack, yBottom, zRight);
   ObjNormal3d(0.0, -1.0, 0.0);
   ObjVertex3d(xBack, yBottom, zLeft);
   ObjEnd();

   // Inside Right
   ObjBegin();
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(xBack, yTop - aThickness, zRight - aThickness);
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(xFront, yTop - aThickness, zRight - aThickness);
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
   ObjEnd();

   // Inside Left
   ObjBegin();
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
   ObjEnd();

   // Inside Top
   ObjBegin();
   ObjNormal3d(0.0, -1.0, 0.0);
   ObjVertex3d(xFront, yTop - aThickness, zRight - aThickness);
   ObjNormal3d(0.0, -1.0, 0.0);
   ObjVertex3d(xBack, yTop - aThickness, zRight - aThickness);
   ObjNormal3d(0.0, -1.0, 0.0);
   ObjVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
   ObjNormal3d(0.0, -1.0, 0.0);
   ObjVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
   ObjEnd();

   // Inside Bottom
   ObjBegin();
   ObjNormal3d(0.0, 1.0, 0.0);
   ObjVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
   ObjNormal3d(0.0, 1.0, 0.0);
   ObjVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
   ObjNormal3d(0.0, 1.0, 0.0);
   ObjVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
   ObjNormal3d(0.0, 1.0, 0.0);
   ObjVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
   ObjEnd();

   // Front
   ObjBegin();
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yBottom, zRight);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yBottom, zLeft);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
   ObjEnd();

   ObjBegin();
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yBottom, zLeft);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yTop, zLeft);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yBottom + aThickness, zLeft + aThickness);
   ObjEnd();

   ObjBegin();
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yTop, zRight);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yBottom, zRight);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yBottom + aThickness, zRight - aThickness);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yTop - aThickness, zRight - aThickness);
   ObjEnd();

   ObjBegin();
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yTop, zLeft);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yTop, zRight);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yTop - aThickness, zRight - aThickness);
   ObjNormal3d(1.0, 0.0, 0.0);
   ObjVertex3d(xFront, yTop - aThickness, zLeft + aThickness);
   ObjEnd();

   // Back
   ObjBegin();
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yBottom, zLeft);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yBottom, zRight);
   ObjEnd();

   ObjBegin();
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yBottom + aThickness, zLeft + aThickness);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yTop, zLeft);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yBottom, zLeft);
   ObjEnd();

   ObjBegin();
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yTop - aThickness, zRight - aThickness);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yBottom + aThickness, zRight - aThickness);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yBottom, zRight);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yTop, zRight);
   ObjEnd();

   ObjBegin();
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yTop - aThickness, zLeft + aThickness);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yTop - aThickness, zRight - aThickness);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yTop, zRight);
   ObjNormal3d(-1.0, 0.0, 0.0);
   ObjVertex3d(xBack, yTop, zLeft);
   ObjEnd();
}

void GeometryObjFile::DrawCylinder(double aLength, double aDiameter)
{
   double r = aDiameter * 0.5;

   double ang_rad[20];
   for (int i = 0; i < 20; ++i)
   {
      ang_rad[i] = (double)i * 18.0 * UtMath::cRAD_PER_DEG;
   }
   UtVec3dX ptVec[20];
   for (int i = 0; i < 20; ++i)
   {
      double x = 0.0;
      double y = r * sin(ang_rad[i]);
      double z = r * cos(ang_rad[i]);
      ptVec[i].Set(x, y, z);
   }

   UtVec3dX normVec[20];
   for (int i = 0; i < 20; ++i)
   {
      double x = 0.0;
      double y = sin(ang_rad[i]);
      double z = cos(ang_rad[i]);
      normVec[i].Set(x, y, z);
      normVec[i].Normalize();
   }

   for (int i = 0; i < 20; ++i)
   {
      ObjBegin();

      int pt1 = i;
      int pt2 = i + 1;
      if (pt2 > 19) { pt2 = 0; }

      ObjNormal3d(normVec[pt2].X(), normVec[pt2].Y(), normVec[pt2].Z());
      ObjVertex3d(0.0, ptVec[pt2].Y(), ptVec[pt2].Z());
      ObjNormal3d(normVec[pt1].X(), normVec[pt1].Y(), normVec[pt1].Z());
      ObjVertex3d(0.0, ptVec[pt1].Y(), ptVec[pt1].Z());
      ObjNormal3d(normVec[pt1].X(), normVec[pt1].Y(), normVec[pt1].Z());
      ObjVertex3d(aLength, ptVec[pt1].Y(), ptVec[pt1].Z());
      ObjNormal3d(normVec[pt2].X(), normVec[pt2].Y(), normVec[pt2].Z());
      ObjVertex3d(aLength, ptVec[pt2].Y(), ptVec[pt2].Z());

      ObjEnd();
   }

   ObjBegin();
   for (int i = 0; i < 20; ++i)
   {
      ObjNormal3d(-1.0, 0.0, 0.0);
      ObjVertex3d(0.0, ptVec[i].Y(), ptVec[i].Z());
   }
   ObjEnd();

   ObjBegin();
   for (int i = 19; i >= 0; --i)
   {
      ObjNormal3d(1.0, 0.0, 0.0);
      ObjVertex3d(aLength, ptVec[i].Y(), ptVec[i].Z());
   }
   ObjEnd();
}

void GeometryObjFile::DrawEngine(double aPosX,
                                 double aPosY,
                                 double aPosZ,
                                 double aDiameter_ft,
                                 double aLength_ft,
                                 double aOffset_ft,
                                 double aYaw_deg,
                                 double aPitch_deg,
                                 double aRoll_deg)
{
   UtVec3dX zeroPt(0.0, 0.0, 0.0);

   UtDCM dcm(aYaw_deg * UtMath::cRAD_PER_DEG, aPitch_deg * UtMath::cRAD_PER_DEG, aRoll_deg * UtMath::cRAD_PER_DEG);
   UtVec3dX pushBack(-aLength_ft * 0.5, 0.0, 0.0);
   pushBack = dcm.InverseTransform(pushBack);
   UtVec3dX refPt(aPosX, aPosY, aPosZ);
   UtVec3dX adjustedRefPt = refPt + pushBack;

   ObjPushMatrix();
   ObjTranslated(adjustedRefPt.X(), -adjustedRefPt.Z(), adjustedRefPt.Y());
   ObjRotated(aYaw_deg, 0.0, -1.0, 0.0);
   ObjRotated(aPitch_deg, 0.0, 0.0, 1.0);
   ObjRotated(aRoll_deg, 1.0, 0.0, 0.0);
   DrawCylinder(aLength_ft, aDiameter_ft);
   ObjPopMatrix();

   if ((aLength_ft * 0.5 + aOffset_ft) < 0.0)
   {
      double burnerLength = -aLength_ft * 0.5 - aOffset_ft;

      pushBack.Set(-aLength_ft * 0.5 - burnerLength, 0.0, 0.0);
      pushBack = dcm.InverseTransform(pushBack);
      refPt.Set(aPosX, aPosY, aPosZ);
      adjustedRefPt = refPt + pushBack;

      ObjPushMatrix();
      ObjTranslated(adjustedRefPt.X(), -adjustedRefPt.Z(), adjustedRefPt.Y());
      ObjRotated(aYaw_deg, 0.0, -1.0, 0.0);
      ObjRotated(aPitch_deg, 0.0, 0.0, 1.0);
      ObjRotated(aRoll_deg, 1.0, 0.0, 0.0);
      DrawCylinder(burnerLength, aDiameter_ft);
      ObjPopMatrix();
   }
}

void GeometryObjFile::DrawSurface(double aPosX,
                                  double aPosY,
                                  double aPosZ,
                                  double aSpan,
                                  double aSweep_deg,
                                  double aRootChord,
                                  double aTipChord,
                                  double aThicknessRatio,
                                  double aDihedral_deg,
                                  double aIncidence_deg,
                                  int    aNumSpanElements)
{
   ObjPushMatrix();

   ObjTranslated(aPosX, -aPosZ, aPosY);
   ObjRotated(aDihedral_deg, -1.0, 0.0, 0.0);
   ObjRotated(aIncidence_deg, 0.0, 0.0, 1.0);

   // Root Airfoil loop
   double rootLeadingEdgeX = aRootChord * 0.25;  // LE is 25% root chord forward of ref pt

   // Tip Airfoil loop
   double tipLeadingEdgeX = rootLeadingEdgeX - aSpan * tan(aSweep_deg * UtMath::cRAD_PER_DEG);

   // Root top surface
   ObjBegin();
   for (int i = 6; i >= 0; --i)
   {
      ObjNormal3d(0.0, 0.0, -1.0);
      ObjVertex3d(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
   }
   ObjEnd();

   // Root bottom surface
   ObjBegin();
   for (int i = 0; i < 7; ++i)
   {
      ObjNormal3d(0.0, 0.0, -1.0);
      // Negative height
      ObjVertex3d(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
   }
   ObjEnd();

   // Tip top surface
   ObjBegin();
   for (int i = 0; i < 7; ++i)
   {
      ObjNormal3d(0.0, 0.0, 1.0);
      ObjVertex3d(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
   }
   ObjEnd();

   // Tip bottom surface
   ObjBegin();
   for (int i = 6; i >= 0; --i)
   {
      ObjNormal3d(0.0, 0.0, 1.0);
      // Negative height
      ObjVertex3d(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
   }
   ObjEnd();

   // Next, we calculate the normals for the root/tip top surfaces. Keep in mind that lower values are near the LE.
   UtVec3dX TopRootNormals[7];
   UtVec3dX TopTipNormals[7];
   UtVec3dX spanVec(0.0, 0.0, 0.0);
   UtVec3dX chordVec(0.0, 0.0, 0.0);
   for (int i = 0; i < 7; ++i)
   {
      if (i == 0)
      {
         // Special case - no fore
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootAft - rootCenter;
         TopRootNormals[i] = chordVec.Cross(spanVec);
         TopRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipAft - tipCenter;
         TopTipNormals[i] = spanVec.Cross(chordVec);
         TopTipNormals[i].Normalize();
      }
      else if (i == 6)
      {
         // Special case - no aft
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i - 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i - 1], aSpan);
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i - 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i - 1], 0.0);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootFore - rootCenter;
         TopRootNormals[i] = spanVec.Cross(chordVec);
         TopRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipFore - tipCenter;
         TopTipNormals[i] = chordVec.Cross(spanVec);
         TopTipNormals[i].Normalize();
      }
      else
      {
         // Nominal case
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i - 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i - 1], aSpan);
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i - 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i - 1], 0.0);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootAft - rootCenter;
         UtVec3dX rootAftTempNormal = chordVec.Cross(spanVec);
         chordVec = rootFore - rootCenter;
         UtVec3dX rootForeTempNormal = spanVec.Cross(chordVec);
         TopRootNormals[i] = rootAftTempNormal + rootForeTempNormal;
         TopRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipAft - tipCenter;
         UtVec3dX tipAftTempNormal = spanVec.Cross(chordVec);
         chordVec = tipFore - tipCenter;
         UtVec3dX tipForeTempNormal = chordVec.Cross(spanVec);
         TopTipNormals[i] = tipAftTempNormal + tipForeTempNormal;
         TopTipNormals[i].Normalize();
      }
   }

   // Next, we calculate the normals for the root/tip bottom surfaces. Keep in mind that lower values are near the LE.
   UtVec3dX BottomRootNormals[7];
   UtVec3dX BottomTipNormals[7];
   for (int i = 0; i < 7; ++i)
   {
      if (i == 0)
      {
         // Special case - no fore (be sure to negate the Y term, since we're doing the bottom)
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootAft - rootCenter;
         BottomRootNormals[i] = spanVec.Cross(chordVec);
         BottomRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipAft - tipCenter;
         BottomTipNormals[i] = chordVec.Cross(spanVec);
         BottomTipNormals[i].Normalize();
      }
      else if (i == 6)
      {
         // Special case - no aft (be sure to negate the Y term, since we're doing the bottom)
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i - 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i - 1], aSpan);
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i - 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i - 1], 0.0);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootFore - rootCenter;
         BottomRootNormals[i] = chordVec.Cross(spanVec);
         BottomRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipFore - tipCenter;
         BottomTipNormals[i] = spanVec.Cross(chordVec);
         BottomTipNormals[i].Normalize();
      }
      else
      {
         // Nominal case (be sure to negate the Y term, since we're doing the bottom)
         UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i - 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i - 1], aSpan);
         UtVec3dX tipCenter(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
         UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);
         UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i - 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i - 1], 0.0);
         UtVec3dX rootCenter(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
         UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);

         spanVec = tipCenter - rootCenter;
         chordVec = rootAft - rootCenter;
         UtVec3dX rootAftTempNormal = spanVec.Cross(chordVec);
         chordVec = rootFore - rootCenter;
         UtVec3dX rootForeTempNormal = chordVec.Cross(spanVec);
         BottomRootNormals[i] = rootAftTempNormal + rootForeTempNormal;
         BottomRootNormals[i].Normalize();

         spanVec = rootCenter - tipCenter;
         chordVec = tipAft - tipCenter;
         UtVec3dX tipAftTempNormal = chordVec.Cross(spanVec);
         chordVec = tipFore - tipCenter;
         UtVec3dX tipForeTempNormal = spanVec.Cross(chordVec);
         BottomTipNormals[i] = tipAftTempNormal + tipForeTempNormal;
         BottomTipNormals[i].Normalize();
      }
   }

   // Top surface
   for (int i = 0; i < 6; ++i)
   {
      UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
      UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
      UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);
      UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], 0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);

      ObjBegin();
      ObjNormal3d(TopTipNormals[i].X(), TopTipNormals[i].Y(), TopTipNormals[i].Z());
      ObjVertex3d(tipFore.X(), tipFore.Y(), tipFore.Z());

      ObjNormal3d(TopRootNormals[i].X(), TopRootNormals[i].Y(), TopRootNormals[i].Z());
      ObjVertex3d(rootFore.X(), rootFore.Y(), rootFore.Z());

      ObjNormal3d(TopRootNormals[i + 1].X(), TopRootNormals[i + 1].Y(), TopRootNormals[i + 1].Z());
      ObjVertex3d(rootAft.X(), rootAft.Y(), rootAft.Z());

      ObjNormal3d(TopTipNormals[i + 1].X(), TopTipNormals[i + 1].Y(), TopTipNormals[i + 1].Z());
      ObjVertex3d(tipAft.X(), tipAft.Y(), tipAft.Z());
      ObjEnd();
   }

   // Bottom surface
   for (int i = 0; i < 6; ++i)
   {
      UtVec3dX tipFore(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i], aSpan);
      UtVec3dX rootFore(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i], 0.0);
      UtVec3dX rootAft(rootLeadingEdgeX - aRootChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aRootChord * mAirfoilPtsY[i + 1], 0.0);
      UtVec3dX tipAft(tipLeadingEdgeX - aTipChord * mAirfoilPtsX[i + 1], -0.5 * aThicknessRatio * aTipChord * mAirfoilPtsY[i + 1], aSpan);

      ObjBegin();
      ObjNormal3d(BottomRootNormals[i].X(), BottomRootNormals[i].Y(), BottomRootNormals[i].Z());
      ObjVertex3d(rootFore.X(), rootFore.Y(), rootFore.Z());

      ObjNormal3d(BottomTipNormals[i].X(), BottomTipNormals[i].Y(), BottomTipNormals[i].Z());
      ObjVertex3d(tipFore.X(), tipFore.Y(), tipFore.Z());

      ObjNormal3d(BottomTipNormals[i + 1].X(), BottomTipNormals[i + 1].Y(), BottomTipNormals[i + 1].Z());
      ObjVertex3d(tipAft.X(), tipAft.Y(), tipAft.Z());

      ObjNormal3d(BottomRootNormals[i + 1].X(), BottomRootNormals[i + 1].Y(), BottomRootNormals[i + 1].Z());
      ObjVertex3d(rootAft.X(), rootAft.Y(), rootAft.Z());
      ObjEnd();
   }

   ObjPopMatrix();
}

void GeometryObjFile::DrawRoundedNacelle(double             aLength_ft,
                                         double             aHeight_ft,
                                         double             aWidth_ft,
                                         double             aThickness_ft,
                                         const std::string& aOverallShapeString,
                                         bool               aAftSectionIsTapered,
                                         double             aAftSectionLength_ft)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth  = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   ObjPointCollection inletPointCollection(true, *this);

   // Draw rings in increasing x

   double inletX0          = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0  = halfWidth - aThickness_ft;
   double inletX1          = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1  = halfWidth * 0.001;

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw();

   // Main portion of nacelle ......................................

   double exitLipLength  = aLength_ft * 0.15;
   double inletLipLength = aLength_ft * 0.075;
   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   ObjPointCollection pointCollection(true, *this);

   double x3          = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3  = halfWidth - aThickness_ft;

   double x2          = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2  = halfWidth;

   double x1          = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1  = 0.0;

   double x0          = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0  = 0.0;

   if (aAftSectionIsTapered)
   {
      x1          = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1  = halfWidth;

      x0          = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0  = halfWidth * 0.001;
   }
   else
   {
      x1          = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1  = halfWidth;

      x0          = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0  = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw();

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      ObjPointCollection exitPointCollection(true, *this);

      // Draw rings in increasing x

      inletX0          = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0  = halfWidth * 0.001;

      inletX1          = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1  = halfWidth - aThickness_ft;

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt < 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 10; hgt > 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt > -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = -10; hgt < 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt < 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 10; hgt > 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt > -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = -10; hgt < 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw();
   }
}

void GeometryObjFile::DrawRoundedRightNacelle(double             aLength_ft,
                                              double             aHeight_ft,
                                              double             aWidth_ft,
                                              double             aThickness_ft,
                                              const std::string& aOverallShapeString,
                                              bool               aAftSectionIsTapered,
                                              double             aAftSectionLength_ft)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   ObjPointCollection inletPointCollection(true, *this);

   // Draw rings in increasing x

   double inletX0          = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0  = halfWidth - aThickness_ft;

   double inletX1          = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1  = halfWidth * 0.001;

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw();

   // Main portion of nacelle ......................................

   double exitLipLength  = aLength_ft * 0.15;
   double inletLipLength = aLength_ft * 0.075;

   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   ObjPointCollection pointCollection(true, *this);

   double x3          = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3  = halfWidth - aThickness_ft;

   double x2          = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2  = halfWidth;

   double x1          = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1  = 0.0;

   double x0          = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0  = 0.0;

   if (aAftSectionIsTapered)
   {
      x1 = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0 = halfWidth * 0.001;
   }
   else
   {
      x1 = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1 = halfWidth;

      x0 = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0 = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = -10; hgt < 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt <= 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw();

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      ObjPointCollection exitPointCollection(true, *this);

      // Draw rings in increasing x

      inletX0          = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0  = halfWidth * 0.001;

      inletX1          = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1  = halfWidth - aThickness_ft;

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = -10; hgt < 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt <= 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = -10; hgt < 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt <= 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw();
   }
}

void GeometryObjFile::DrawRoundedLeftNacelle(double             aLength_ft,
                                             double             aHeight_ft,
                                             double             aWidth_ft,
                                             double             aThickness_ft,
                                             const std::string& aOverallShapeString,
                                             bool               aAftSectionIsTapered,
                                             double             aAftSectionLength_ft)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth  = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   ObjPointCollection inletPointCollection(true, *this);

   // Draw rings in increasing x

   double inletX0          = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0  = halfWidth - aThickness_ft;

   double inletX1          = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1  = halfWidth * 0.001;

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw();

   // Main portion of nacelle ......................................

   double exitLipLength  = aLength_ft * 0.15;
   double inletLipLength = aLength_ft * 0.075;

   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   ObjPointCollection pointCollection(true, *this);

   double x3          = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3  = halfWidth - aThickness_ft;

   double x2          = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2  = halfWidth;

   double x1          = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1  = 0.0;

   double x0          = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0  = 0.0;

   if (aAftSectionIsTapered)
   {
      x1          = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1  = halfWidth;

      x0          = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0  = halfWidth * 0.001;
   }
   else
   {
      x1          = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1  = halfWidth;

      x0          = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0  = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 10; hgt > 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 0; hgt >= -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw();

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      ObjPointCollection exitPointCollection(true, *this);

      // Draw rings in increasing x

      inletX0          = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0  = halfWidth * 0.001;

      inletX1          = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1  = halfWidth - aThickness_ft;

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = 10; hgt > 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt >= -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = 10; hgt > 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 0; hgt >= -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw();
   }
}

void GeometryObjFile::DrawRoundedTopNacelle(double             aLength_ft,
                                            double             aHeight_ft,
                                            double             aWidth_ft,
                                            double             aThickness_ft,
                                            const std::string& aOverallShapeString,
                                            bool               aAftSectionIsTapered,
                                            double             aAftSectionLength_ft)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth  = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   ObjPointCollection inletPointCollection(true, *this);

   // Draw rings in increasing x

   double inletX0          = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0  = halfWidth - aThickness_ft;

   double inletX1          = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1  = halfWidth * 0.001;

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw();

   // Main portion of nacelle ......................................

   double exitLipLength  = aLength_ft * 0.15;
   double inletLipLength = aLength_ft * 0.075;

   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength       = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   ObjPointCollection pointCollection(true, *this);

   double x3          = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3  = halfWidth - aThickness_ft;

   double x2          = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2  = halfWidth;

   double x1          = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1  = 0.0;

   double x0          = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0  = 0.0;

   if (aAftSectionIsTapered)
   {
      x1          = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1  = halfWidth;

      x0          = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0  = halfWidth * 0.001;
   }
   else
   {
      x1          = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1  = halfWidth;

      x0          = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0  = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt < 10; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = 10; hgt >= 0; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw();

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      ObjPointCollection exitPointCollection(true, *this);

      // Draw rings in increasing x

      inletX0          = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0  = halfWidth * 0.001;

      inletX1          = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1  = halfWidth - aThickness_ft;

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt < 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 10; hgt >= 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt < 10; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = 10; hgt >= 0; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw();
   }
}

void GeometryObjFile::DrawRoundedBottomNacelle(double             aLength_ft,
                                               double             aHeight_ft,
                                               double             aWidth_ft,
                                               double             aThickness_ft,
                                               const std::string& aOverallShapeString,
                                               bool               aAftSectionIsTapered,
                                               double             aAftSectionLength_ft)
{
   double halfHeight = 0.5 * aHeight_ft;
   double halfWidth  = 0.5 * aWidth_ft;
   double offsetPosX = 0.0;
   double offsetPosY = 0.0;
   double offsetPosZ = 0.0;

   // Ensure zero aft length, if appropriate
   if (!aAftSectionIsTapered)
   {
      aAftSectionLength_ft = 0.0;
   }

   // Inlet ........................................................

   ObjPointCollection inletPointCollection(true, *this);

   // Draw rings in increasing x

   double inletX0          = offsetPosX + aLength_ft * 0.5;
   double inletHalfHeight0 = halfHeight - aThickness_ft;
   double inletHalfWidth0  = halfWidth - aThickness_ft;

   double inletX1          = offsetPosX + aLength_ft * 0.5 + 0.001 * aLength_ft;
   double inletHalfHeight1 = halfHeight * 0.001;
   double inletHalfWidth1  = halfWidth * 0.001;

   // Add center ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight0;
      double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX0;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Add inlet outer radius ring
   inletPointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * inletHalfHeight1;
      double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = inletX1;
      inletPointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   inletPointCollection.CalculatePointCollection();
   inletPointCollection.Draw();

   // Main portion of nacelle ......................................

   double exitLipLength  = aLength_ft * 0.15;
   double inletLipLength = aLength_ft * 0.075;

   if (aAftSectionIsTapered)
   {
      double maxLipLength = aLength_ft - aAftSectionLength_ft;
      if (inletLipLength > maxLipLength)
      {
         inletLipLength = maxLipLength - aLength_ft * 0.01;
      }

      // Modify values if necessary
      if (inletLipLength < aLength_ft * 0.01)
      {
         inletLipLength       = aLength_ft * 0.01;
         aAftSectionLength_ft = aLength_ft * 0.98;
      }
   }

   ObjPointCollection pointCollection(true, *this);

   double x3          = offsetPosX + aLength_ft * 0.5;
   double halfHeight3 = halfHeight - aThickness_ft;
   double halfWidth3  = halfWidth - aThickness_ft;

   double x2          = offsetPosX + aLength_ft * 0.5 - inletLipLength;
   double halfHeight2 = halfHeight;
   double halfWidth2  = halfWidth;

   double x1          = 0.0;
   double halfHeight1 = 0.0;
   double halfWidth1  = 0.0;

   double x0          = 0.0;
   double halfHeight0 = 0.0;
   double halfWidth0  = 0.0;

   if (aAftSectionIsTapered)
   {
      x1          = offsetPosX - aLength_ft * 0.5 + aAftSectionLength_ft;
      halfHeight1 = halfHeight;
      halfWidth1  = halfWidth;

      x0          = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight * 0.001;
      halfWidth0  = halfWidth * 0.001;
   }
   else
   {
      x1          = offsetPosX - aLength_ft * 0.5 + exitLipLength;
      halfHeight1 = halfHeight;
      halfWidth1  = halfWidth;

      x0          = offsetPosX - aLength_ft * 0.5;
      halfHeight0 = halfHeight - aThickness_ft;
      halfWidth0  = halfWidth - aThickness_ft;
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = -EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight0;
      double z = EllipticalX_GivenY(y, halfWidth0, halfHeight0);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x0;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = -EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight1;
      double z = EllipticalX_GivenY(y, halfWidth1, halfHeight1);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x1;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = -EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight2;
      double z = EllipticalX_GivenY(y, halfWidth2, halfHeight2);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x2;
      pointCollection.AddPoint(x, y, z);
   }

   pointCollection.AddRing();
   for (int hgt = 0; hgt > -10; --hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = -EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }
   for (int hgt = -10; hgt <= 0; ++hgt)
   {
      double y = 0.1 * (double)hgt * halfHeight3;
      double z = EllipticalX_GivenY(y, halfWidth3, halfHeight3);
      y += -offsetPosZ;
      z += offsetPosY;
      double x = x3;
      pointCollection.AddPoint(x, y, z);
   }

   // Calculate normals and draw
   pointCollection.CalculatePointCollection();
   pointCollection.Draw();

   // Exit .........................................................

   if (!aAftSectionIsTapered)
   {
      ObjPointCollection exitPointCollection(true, *this);

      // Draw rings in increasing x

      inletX0          = offsetPosX - aLength_ft * 0.5 - 0.001 * aLength_ft;
      inletHalfHeight0 = halfHeight * 0.001;
      inletHalfWidth0  = halfWidth * 0.001;

      inletX1          = offsetPosX - aLength_ft * 0.5;
      inletHalfHeight1 = halfHeight - aThickness_ft;
      inletHalfWidth1  = halfWidth - aThickness_ft;

      // Add center ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt > -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = -EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = -10; hgt <= 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight0;
         double z = EllipticalX_GivenY(y, inletHalfWidth0, inletHalfHeight0);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX0;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Add inlet outer radius ring
      exitPointCollection.AddRing();
      for (int hgt = 0; hgt > -10; --hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = -EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }
      for (int hgt = -10; hgt <= 0; ++hgt)
      {
         double y = 0.1 * (double)hgt * inletHalfHeight1;
         double z = EllipticalX_GivenY(y, inletHalfWidth1, inletHalfHeight1);
         y += -offsetPosZ;
         z += offsetPosY;
         double x = inletX1;
         exitPointCollection.AddPoint(x, y, z);
      }

      // Calculate normals and draw
      exitPointCollection.CalculatePointCollection();
      exitPointCollection.Draw();
   }
}

void GeometryObjFile::DrawFlatSidedNacelle(double             aLength_ft,
                                           double             aHeight_ft,
                                           double             aWidth_ft,
                                           double             aThickness_ft,
                                           double             aForwardSweepLength_ft,
                                           const std::string& aOverallShapeString,
                                           bool               aAftSectionIsTapered,
                                           double             aAftSectionLength_ft)
{
   // F = front, A = aft, T = top, B = bottom, R = right, L = right
   UtVec3dX ptFTR;
   UtVec3dX ptFTL;
   UtVec3dX ptFBR;
   UtVec3dX ptFBL;
   UtVec3dX ptATR;
   UtVec3dX ptATL;
   UtVec3dX ptABR;
   UtVec3dX ptABL;

   DrawBoxWithHole(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft);
}

void GeometryObjFile::DrawFlatSweptRightNacelle(double             aLength_ft,
                                                double             aHeightInner_ft,
                                                double             aHeightOuter_ft,
                                                double             aWidth_ft,
                                                double             aThickness_ft,
                                                double             aForwardSweepLength_ft,
                                                const std::string& aOverallShapeString,
                                                bool               aAftSectionIsTapered,
                                                double             aAftSectionLength_ft)
{
   // Check value for aAftSectionLength_ft
   if (!aAftSectionIsTapered) { aAftSectionLength_ft = 0.0; }

   // F = front, A = aft, T = top, B = bottom, R = right, L = right

   // Incremental length for offset for inlets
   double del = aLength_ft * 0.0001;
   double dx  = aForwardSweepLength_ft;
   double dy  = aHeightInner_ft * 0.5 - aHeightOuter_ft * 0.5;
   double dz  = aWidth_ft;

   double dxOverDz = dx / dz;
   double xOffset  = aThickness_ft * dxOverDz;
   double dyOverDz = dy / dz;
   double yOffset  = aThickness_ft * dyOverDz;

   UtVec3dX inletFTR(xOffset + del + aLength_ft * 0.5 - aForwardSweepLength_ft,
                     aHeightOuter_ft * 0.5 - aThickness_ft + yOffset, aWidth_ft - aThickness_ft);
   UtVec3dX inletFTL(-xOffset + del + aLength_ft * 0.5,
                     aHeightInner_ft * 0.5 - aThickness_ft - yOffset, aThickness_ft);
   UtVec3dX inletFBR(xOffset + del + aLength_ft * 0.5 - aForwardSweepLength_ft,
                     -aHeightOuter_ft * 0.5 + aThickness_ft - yOffset, aWidth_ft - aThickness_ft);
   UtVec3dX inletFBL(-xOffset + del + aLength_ft * 0.5,
                     -aHeightInner_ft * 0.5 + aThickness_ft + yOffset, aThickness_ft);

   UtVec3dX ptFTR(aLength_ft * 0.5 - aForwardSweepLength_ft,  aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptFTL(aLength_ft * 0.5,                           aHeightInner_ft * 0.5, 0.0);
   UtVec3dX ptFBR(aLength_ft * 0.5 - aForwardSweepLength_ft, -aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptFBL(aLength_ft * 0.5,                          -aHeightInner_ft * 0.5, 0.0);

   UtVec3dX ptATR(-aLength_ft * 0.5 + aAftSectionLength_ft, aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptATL(-aLength_ft * 0.5, aHeightInner_ft * 0.5, 0.0);
   UtVec3dX ptABR(-aLength_ft * 0.5 + aAftSectionLength_ft, -aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptABL(-aLength_ft * 0.5, -aHeightInner_ft * 0.5, 0.0);

   UtVec3dX vec1;
   UtVec3dX vec2;
   UtVec3dX norm;

   // Front
   ObjBegin();
   vec1 = ptFTL - ptFBL;
   vec2 = ptFBR - ptFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   ObjEnd();

   // Aft
   ObjBegin();
   vec1 = ptABR - ptABL;
   vec2 = ptATL - ptABL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   ObjEnd();

   // Top
   ObjBegin();
   vec1 = ptATL - ptFTL;
   vec2 = ptATR - ptFTL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   ObjEnd();

   // Bottom
   ObjBegin();
   vec1 = ptFBR - ptFBL;
   vec2 = ptABL - ptFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   ObjEnd();

   // Right
   ObjBegin();
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   ObjEnd();

   // Left
   ObjBegin();
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   ObjEnd();
}

void GeometryObjFile::DrawFlatSweptLeftNacelle(double             aLength_ft,
                                               double             aHeightInner_ft,
                                               double             aHeightOuter_ft,
                                               double             aWidth_ft,
                                               double             aThickness_ft,
                                               double             aForwardSweepLength_ft,
                                               const std::string& aOverallShapeString,
                                               bool               aAftSectionIsTapered,
                                               double             aAftSectionLength_ft)
{
   // Check value for aAftSectionLength_ft
   if (!aAftSectionIsTapered) { aAftSectionLength_ft = 0.0; }

   // F = front, A = aft, T = top, B = bottom, R = right, L = right

   // Incremental length for offset for inlets
   double del = aLength_ft * 0.0001;
   double dx = aForwardSweepLength_ft;
   double dy = aHeightInner_ft * 0.5 - aHeightOuter_ft * 0.5;
   double dz = aWidth_ft;

   double dxOverDz = dx / dz;
   double xOffset = aThickness_ft * dxOverDz;
   double dyOverDz = dy / dz;
   double yOffset = aThickness_ft * dyOverDz;

   UtVec3dX inletFTR(xOffset + del + aLength_ft * 0.5,                            aHeightOuter_ft * 0.5 - aThickness_ft + yOffset, aWidth_ft - aThickness_ft);
   UtVec3dX inletFTL(-xOffset + del + aLength_ft * 0.5 - aForwardSweepLength_ft,  aHeightInner_ft * 0.5 - aThickness_ft - yOffset, aThickness_ft);
   UtVec3dX inletFBR(xOffset + del + aLength_ft * 0.5,                           -aHeightOuter_ft * 0.5 + aThickness_ft - yOffset, aWidth_ft - aThickness_ft);
   UtVec3dX inletFBL(-xOffset + del + aLength_ft * 0.5 - aForwardSweepLength_ft, -aHeightInner_ft * 0.5 + aThickness_ft + yOffset, aThickness_ft);

   UtVec3dX ptFTR(aLength_ft * 0.5,                           aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptFTL(aLength_ft * 0.5 - aForwardSweepLength_ft,  aHeightInner_ft * 0.5, 0.0);
   UtVec3dX ptFBR(aLength_ft * 0.5,                          -aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptFBL(aLength_ft * 0.5 - aForwardSweepLength_ft, -aHeightInner_ft * 0.5, 0.0);

   UtVec3dX ptATR(-aLength_ft * 0.5 + aAftSectionLength_ft, aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptATL(-aLength_ft * 0.5, aHeightInner_ft * 0.5, 0.0);
   UtVec3dX ptABR(-aLength_ft * 0.5 + aAftSectionLength_ft, -aHeightOuter_ft * 0.5, aWidth_ft);
   UtVec3dX ptABL(-aLength_ft * 0.5, -aHeightInner_ft * 0.5, 0.0);

   UtVec3dX vec1;
   UtVec3dX vec2;
   UtVec3dX norm;

   // Front
   ObjBegin();
   vec1 = ptFTL - ptFBL;
   vec2 = ptFBR - ptFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   ObjEnd();

   // Aft
   ObjBegin();
   vec1 = ptABR - ptABL;
   vec2 = ptATL - ptABL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   ObjEnd();

   // Top
   ObjBegin();
   vec1 = ptATL - ptFTL;
   vec2 = ptATR - ptFTL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   ObjEnd();

   // Bottom
   ObjBegin();
   vec1 = ptFBR - ptFBL;
   vec2 = ptABL - ptFBL;
   norm = vec1.Cross(vec2);
   norm.Normalize();
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   ObjNormal3d(norm.X(), norm.Y(), norm.Z());
   ObjVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   ObjEnd();

   // Right
   ObjBegin();
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(ptABR.X(), ptABR.Y(), ptABR.Z());
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(ptFBR.X(), ptFBR.Y(), ptFBR.Z());
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(ptFTR.X(), ptFTR.Y(), ptFTR.Z());
   ObjNormal3d(0.0, 0.0, 1.0);
   ObjVertex3d(ptATR.X(), ptATR.Y(), ptATR.Z());
   ObjEnd();

   // Left
   ObjBegin();
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(ptFTL.X(), ptFTL.Y(), ptFTL.Z());
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(ptFBL.X(), ptFBL.Y(), ptFBL.Z());
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(ptABL.X(), ptABL.Y(), ptABL.Z());
   ObjNormal3d(0.0, 0.0, -1.0);
   ObjVertex3d(ptATL.X(), ptATL.Y(), ptATL.Z());
   ObjEnd();
}

void GeometryObjFile::DrawNacelle(double             aPosX,
                                  double             aPosY,
                                  double             aPosZ,
                                  double             aLength_ft,
                                  double             aHeight_ft,
                                  double             aHeightInner_ft,
                                  double             aHeightOuter_ft,
                                  double             aWidth_ft,
                                  double             aThickness_ft,
                                  double             aForwardSweepLength_ft,
                                  const std::string& aOverallShapeString,
                                  const std::string& aAftSectionString,
                                  double             aAftSectionLength_ft,
                                  double             aYaw_deg,
                                  double             aPitch_deg,
                                  double             aRoll_deg)
{
   ObjPushMatrix();
   ObjTranslated(aPosX, -aPosZ, aPosY);
   ObjRotated(aYaw_deg, 0.0, -1.0, 0.0);
   ObjRotated(aPitch_deg, 0.0, 0.0, 1.0);
   ObjRotated(aRoll_deg, 1.0, 0.0, 0.0);

   // Tapered flag
   bool aftNacelleIsTapered = false;
   if (aAftSectionString == "Blunt") { aftNacelleIsTapered = false; }
   else if (aAftSectionString == "Tapered") { aftNacelleIsTapered = true; }

   if (aftNacelleIsTapered)
   {
      // Verify length values - return if aft length is too long
      if (aAftSectionLength_ft > aLength_ft) { return; }
   }

   if (aOverallShapeString == "Rounded")
   {
      DrawRoundedNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString,
                         aftNacelleIsTapered, aAftSectionLength_ft);
   }
   else if (aOverallShapeString == "Half-Round-Right")
   {
      DrawRoundedRightNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString,
                              aftNacelleIsTapered, aAftSectionLength_ft);
   }
   else if (aOverallShapeString == "Half-Round-Left")
   {
      DrawRoundedLeftNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString,
                             aftNacelleIsTapered, aAftSectionLength_ft);
   }
   else if (aOverallShapeString == "Half-Round-Top")
   {
      DrawRoundedTopNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString,
                            aftNacelleIsTapered, aAftSectionLength_ft);
   }
   else if (aOverallShapeString == "Half-Round-Bottom")
   {
      DrawRoundedBottomNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aOverallShapeString,
                               aftNacelleIsTapered, aAftSectionLength_ft);
   }
   else if (aOverallShapeString == "Flat-Sided")
   {
      DrawFlatSidedNacelle(aLength_ft, aHeight_ft, aWidth_ft, aThickness_ft, aForwardSweepLength_ft,
                           aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft);
   }
   else if (aOverallShapeString == "Flat-Swept-Right")
   {
      DrawFlatSweptRightNacelle(aLength_ft, aHeightInner_ft, aHeightOuter_ft, aWidth_ft, aThickness_ft,
                                aForwardSweepLength_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft);
   }
   else if (aOverallShapeString == "Flat-Swept-Left")
   {
      DrawFlatSweptLeftNacelle(aLength_ft, aHeightInner_ft, aHeightOuter_ft, aWidth_ft, aThickness_ft,
                               aForwardSweepLength_ft, aOverallShapeString, aftNacelleIsTapered, aAftSectionLength_ft);
   }

   ObjPopMatrix();
}

double GeometryObjFile::EllipticalY_GivenX(double x, double aHalfWidth, double aHalfHeight)
{
   return sqrt((1 - ((x * x) / (aHalfWidth * aHalfWidth))) * (aHalfHeight * aHalfHeight));
}

double GeometryObjFile::EllipticalX_GivenY(double y, double aHalfWidth, double aHalfHeight)
{
   return sqrt((1 - ((y * y) / (aHalfHeight * aHalfHeight))) * (aHalfWidth * aHalfWidth));
}

void GeometryObjFile::ObjPushMatrix()
{
   mMatrixStack.push(mActiveMatrix);
}

void GeometryObjFile::ObjTranslated(double aPosX, double aPosY, double aPosZ)
{
   mActiveTranslation.Set(aPosX, aPosY, aPosZ);
   ++mNumTranslations;
}

void GeometryObjFile::ObjRotated(double aAngle_deg, double aX, double aY, double aZ)
{
   bool zeroX = false;
   bool zeroY = false;
   bool zeroZ = false;

   // Check for zero X
   if (std::abs(aX) < mEpsilon)
   {
      zeroX = true;
   }

   // Check for zero Y
   if (std::abs(aY) < mEpsilon)
   {
      zeroY = true;
   }

   // Check for zero Z
   if (std::abs(aZ) < mEpsilon)
   {
      zeroZ = true;
   }

   bool positiveOneX = false;
   bool positiveOneY = false;
   bool positiveOneZ = false;

   // Check for positive 1
   if (std::abs(aX - 1.0) < mEpsilon)
   {
      positiveOneX = true;
   }

   // Check for positive 1
   if (std::abs(aY - 1.0) < mEpsilon)
   {
      positiveOneY = true;
   }

   // Check for positive 1
   if (std::abs(aZ - 1.0) < mEpsilon)
   {
      positiveOneZ = true;
   }

   bool negativeOneX = false;
   bool negativeOneY = false;
   bool negativeOneZ = false;

   // Check for negative 1
   if (std::abs(aX + 1.0) < mEpsilon)
   {
      negativeOneX = true;
   }

   // Check for negative 1
   if (std::abs(aY + 1.0) < mEpsilon)
   {
      negativeOneY = true;
   }

   // Check for negative 1
   if (std::abs(aZ + 1.0) < mEpsilon)
   {
      negativeOneZ = true;
   }

   // Test for a valid condition

   if (positiveOneX || negativeOneX)
   {
      if (positiveOneY || negativeOneY || positiveOneZ || negativeOneZ)
      {
         ut::log::error() << "Invalid Y or Z values in in GeometryObjFile::ObjRotated().";
         return;
      }
   }
   if (positiveOneY || negativeOneY)
   {
      if (positiveOneX || negativeOneX || positiveOneZ || negativeOneZ)
      {
         ut::log::error() << "Invalid X or Z values in in GeometryObjFile::ObjRotated().";
         return;
      }
   }
   if (positiveOneZ || negativeOneZ)
   {
      if (positiveOneY || negativeOneY || positiveOneX || negativeOneX)
      {
         ut::log::error() << "Invalid X or Y values in in GeometryObjFile::ObjRotated().";
         return;
      }
   }

   // Note:  Rotate X --> RotZ  Rotate Y --> RotX  Rotate Z --> RotY

   if (positiveOneX && zeroY && zeroZ)
   {
      double angle_rad = aAngle_deg * UtMath::cRAD_PER_DEG;
      UtDCM matrix(0.0, 0.0, angle_rad);
      mActiveMatrix = matrix * mActiveMatrix;
   }
   else if (negativeOneX && zeroY && zeroZ)
   {
      double angle_rad = -aAngle_deg * UtMath::cRAD_PER_DEG;
      UtDCM matrix(0.0, 0.0, angle_rad);
      mActiveMatrix = matrix * mActiveMatrix;
   }
   else if (positiveOneY && zeroX && zeroZ)
   {
      double angle_rad = aAngle_deg * UtMath::cRAD_PER_DEG;
      UtDCM matrix(0.0, angle_rad, 0.0);
      mActiveMatrix = matrix * mActiveMatrix;
   }
   else if (negativeOneY && zeroX && zeroZ)
   {
      double angle_rad = -aAngle_deg * UtMath::cRAD_PER_DEG;
      UtDCM matrix(0.0, angle_rad, 0.0);
      mActiveMatrix = matrix * mActiveMatrix;
   }
   else if (positiveOneZ && zeroX && zeroY)
   {
      double angle_rad = aAngle_deg * UtMath::cRAD_PER_DEG;
      UtDCM matrix(angle_rad, 0.0, 0.0);
      mActiveMatrix = matrix * mActiveMatrix;
   }
   else if (negativeOneZ && zeroX && zeroY)
   {
      double angle_rad = -aAngle_deg * UtMath::cRAD_PER_DEG;
      UtDCM matrix(angle_rad, 0.0, 0.0);
      mActiveMatrix = matrix * mActiveMatrix;
   }
   else
   {
      ut::log::error() << "Invalid values in in GeometryObjFile::ObjRotated().";
   }
}

void GeometryObjFile::ObjPopMatrix()
{
   if (mMatrixStack.empty())
   {
      ut::log::error() << "Cannot pop, since mMatrixStack is empty in GeometryObjFile::ObjPopMatrix().";
      return;
   }

   // Get the top item and make it active
   mActiveMatrix = mMatrixStack.top();

   // Pop the top item off stack
   mMatrixStack.pop();

   // Decrement the number of translations
   --mNumTranslations;

   // Reset translation
   if (mNumTranslations == 0)
   {
      mActiveTranslation.Set(0.0, 0.0, 0.0);
   }
   else
   {
      mActiveTranslation.Set(0.0, 0.0, 0.0);
      mNumTranslations = 0;
   }
}

void GeometryObjFile::ObjBegin()
{
   mStartingVertexIndex = (int)mVertexList.size();
   mStartingNormalIndex = (int)mNormalList.size();

   mNumVertexInPolygon = 0;
   mNumNormalInPolygon = 0;
}

void GeometryObjFile::ObjVertex3d(double aX, double aY, double aZ)
{
   // Setup a vector
   UtVec3dX tempVector(aX, aY, aZ);

   // Transform the vector
   tempVector = mActiveMatrix.InverseTransform(tempVector);

   // Apply the active translation
   tempVector += mActiveTranslation;

   // Add the vector to the list
   mVertexList.push_back(tempVector);

   // Increment the count
   ++mNumVertexInPolygon;
}

void GeometryObjFile::ObjNormal3d(double aX, double aY, double aZ)
{
   // Setup a normal
   UtVec3dX tempNormal(aX, aY, aZ);

   // Transform the normal
   tempNormal = mActiveMatrix.InverseTransform(tempNormal);

   // Normalize the normal
   tempNormal.Normalize();

   // Add the normal to the list
   mNormalList.push_back(tempNormal);

   // Increment the count
   ++mNumNormalInPolygon;
}

void GeometryObjFile::ObjEnd()
{
   // Ensure that the number of vertices and normals match
   if (mNumVertexInPolygon != mNumNormalInPolygon)
   {
      auto outStream = ut::log::error() << "Vertex/Normals do not match in GeometryObjFile::ObjEnd().";
      outStream.AddNote() << "mNumVertexInPolygon: " << mNumVertexInPolygon;
      outStream.AddNote() << "mNumNormalInPolygon: " << mNumNormalInPolygon;
      return;
   }

   // Confirm that we have zero or one translation
   if (mNumTranslations > 1 || mNumTranslations < 0)
   {
      auto outStream = ut::log::error() << "Invalid number of translations in GeometryObjFile::ObjEnd().";
      if (mNumTranslations > 1)
      {
         outStream.AddNote() << "Too many translations: " << mNumTranslations;
         return;
      }
      else if (mNumTranslations < 0)
      {
         outStream.AddNote() << "Negative translation: " << mNumTranslations;
         return;
      }
   }

   // Vertices
   std::vector<int> tempVertexVector;
   int vertexIndex = mStartingVertexIndex;
   for (int i = 0; i < mNumVertexInPolygon; ++i)
   {
      tempVertexVector.push_back(vertexIndex);
      ++vertexIndex;
   }
   mFaceVertexList.push_back(tempVertexVector);

   // Normals
   std::vector<int> tempNormalVector;
   int normalIndex = mStartingNormalIndex;
   for (int i = 0; i < mNumNormalInPolygon; ++i)
   {
      tempNormalVector.push_back(normalIndex);
      ++normalIndex;
   }
   mFaceNormalList.push_back(tempNormalVector);
}

void GeometryObjFile::OutputData(QTextStream&                aOutputStream,
                                 double                      aData,
                                 int                         aWidth,
                                 int                         aPrecision,
                                 QTextStream::FieldAlignment aAlignment)
{
   int currentPrecision = aOutputStream.realNumberPrecision();
   int currentWidth = aOutputStream.fieldWidth();
   QTextStream::FieldAlignment currentAlignment = aOutputStream.fieldAlignment();

   aOutputStream.setFieldWidth(aWidth);
   aOutputStream.setRealNumberPrecision(aPrecision);
   aOutputStream.setFieldAlignment(aAlignment);

   aOutputStream << aData;

   aOutputStream.setFieldWidth(currentWidth);
   aOutputStream.setRealNumberPrecision(currentPrecision);
   aOutputStream.setFieldAlignment(currentAlignment);
}

QString GeometryObjFile::GetLocalModelPath()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();

   if (vehicle == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return QString();
   }

   // Get the vehicle name and convert to lowercase
   QString name = vehicle->GetVehicleName().c_str();
   name = name.toLower();

   return MoverCreatorMainWindow::GetDataPath() + "/AFSIM_Scripts/" + name + "/model_3d/";
}

bool GeometryObjFile::AddVehicleToModelsList(const QString&     aFilePath,
                                             const QString&     aRelativePathToModel,
                                             Designer::Vehicle* aVehicle)
{
   QString name = aVehicle->GetVehicleName().c_str();
   name = name.toLower();

   QFile modelFile(aFilePath);

   if (modelFile.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      // Create a temporary models file in the current directory
      QFile newModelFile("./models.txt");

      QTextStream modelOutStream(&modelFile);
      QTextStream modelInStream(&newModelFile);

      newModelFile.open(QIODevice::WriteOnly | QIODevice::Text);

      QString currentLine = modelOutStream.readLine();
      modelInStream << currentLine << "\n";

      // Read and copy until we reach the models section and/or the end of the file
      bool addedModel = false;
      while (!modelOutStream.atEnd())
      {
         currentLine = modelOutStream.readLine();

         // The models section is the first section after the initial comment block
         // and the only separator between the two is a single newline
         // (which gets trimmed when QTextStream reads a line).
         // Since we want to add the new model to the top of the models section,
         // we look for the first blank line
         if (currentLine == "" && !addedModel)
         {
            modelInStream << "\n";
            modelInStream << "model " << name << "\n";
            modelInStream << "   filename " << aRelativePathToModel << "\n";  // Add the path to the model file
            modelInStream << "   pre_xform\n";
            modelInStream << "      rotate x 180\n";  // Make sure the model is right-side-up

            QString ftToM = QString::number(UtMath::cM_PER_FT);
            modelInStream << "      scale " + ftToM + "\n";  // Convert from feet to meters
            modelInStream << "   end_pre_xform\n";

            // Add wing_tip data
            // We only need the right wing tip location

            GeometryWing* wing = aVehicle->GetGeometry().GetWing();
            if (wing != nullptr)
            {
               UtVec3dX refPt;
               double halfSpan_ft = 10.0;
               double dihedral_rad = 0.0;
               double sweep_rad = 0.0;

               refPt = wing->GetRefPoint_ft();
               halfSpan_ft = 0.5 * wing->GetSpan_ft();
               dihedral_rad = wing->GetDihedralAngle_deg() * UtMath::cRAD_PER_DEG;
               sweep_rad = wing->GetSweepAngle_deg() * UtMath::cRAD_PER_DEG;

               double x = refPt.X() - halfSpan_ft * sin(sweep_rad);
               double y = halfSpan_ft * cos(dihedral_rad);
               double z = refPt.Z() - halfSpan_ft * sin(dihedral_rad);

               // Convert from ft to meters
               x *= UtMath::cM_PER_FT;
               y *= UtMath::cM_PER_FT;
               z *= UtMath::cM_PER_FT;

               modelInStream << "   wing_tip " << x << " " << y << " " << z << "\n";
            }
            else
            {
               // This vehicle lacks a wing, so check for a body
               GeometryBody* body = aVehicle->GetGeometry().GetBody();
               if (body != nullptr)
               {
                  UtVec3dX refPt;
                  double radius_ft = 0.5 * body->GetWidth_ft();
                  double aftX_ft   = 0.5 * body->GetLength_ft();

                  refPt = body->GetRefPoint_ft();

                  double x = refPt.X() - aftX_ft;
                  double y = radius_ft;
                  double z = refPt.Z();

                  // Convert from ft to meters
                  x *= UtMath::cM_PER_FT;
                  z *= UtMath::cM_PER_FT;

                  modelInStream << "   wing_tip " << x << " " << y << " " << z << "\n";
               }
               else
               {
                  // Use a default of 1 meter span
                  double x = 0.0;
                  double y = 0.5;
                  double z = 0.0;
                  modelInStream << "   wing_tip " << x << " " << y << " " << z << "\n";
               }
            }

            modelInStream << "end_model" << "\n";
            modelInStream << "\n";

            addedModel = true;
         }
         else if (currentLine == QString::fromStdString("model ") + name)
         {
            // Skip adding the previous entry by reading lines until we hit the next model
            while (currentLine != "")
            {
               currentLine = modelOutStream.readLine();
            }
         }
         else  // Model isn't being added here, just copy the original line
         {
            modelInStream << currentLine << "\n";
         }
      }

      modelFile.close();
      newModelFile.close();

      // Delete original models.txt and replace it with the new models.txt
      modelFile.remove();
      newModelFile.rename(aFilePath);
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Can't open models.txt at " + aFilePath + "!");
      return false;
   }
   return true;
}

bool GeometryObjFile::ConvertModelTo(QString aFilePathWithoutExtension, SupportedFileType aFileType)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();

   QString objFilePath = aFilePathWithoutExtension + ".obj";

   if (QFile(objFilePath).exists())
   {
      // Check for osgconv -- If osgconv exists, convert the file
      QString osgPath = mw.GetOsgConvPath();

      if (osgPath.isEmpty())
      {
         ut::log::info() << "Using OBJ files instead of IVE, since unable to locate osgconv.";
         return false;
      }

      QProcess* osgConvProcess = new QProcess();
      QStringList osgArguments;

      // osgconv [source file] [destination file]
      osgArguments.append(objFilePath);

      QString newExtension = GetFileExtension(aFileType);
      QString newFilePath = aFilePathWithoutExtension + newExtension;
      osgArguments.append(newFilePath);

      osgConvProcess->start(osgPath, osgArguments);

      // Make sure the conversion process finishes before returning
      osgConvProcess->waitForFinished();

      return true;
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Can't find " + objFilePath + " for model conversion!");
   }

   return false;
}

QString GeometryObjFile::GetFileExtension(SupportedFileType aFileType)
{
   QString newExtension;

   switch (aFileType)
   {
      case SupportedFileType::cOBJ:
         newExtension = ".obj";
         break;
      case SupportedFileType::cIVE:
         newExtension = ".ive";
         break;
      case SupportedFileType::cOSGB:
         newExtension = ".osgb";
         break;
      default:
         break;
   }

   return newExtension;
}

} // namespace Designer
