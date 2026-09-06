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

#ifndef GEOMETRY_OBJ_FILE_HPP
#define GEOMETRY_OBJ_FILE_HPP

#include <stack>
#include <vector>
#include <QString>

#include "UtVec3dX.hpp"
#include "Vehicle.hpp"

namespace Designer
{
   class ObjFace
   {
      public:
         ObjFace() = default;
         ~ObjFace() = default;

         std::vector<int> mFaceVertexIndexList;
   };

   enum class SupportedFileType
   {
      cOBJ,
      cIVE,
      cOSGB
   };

   class GeometryObjFile
   {
      public:
         GeometryObjFile();
         ~GeometryObjFile();

         // This outputs the OBJ/MTL files for the 3D model to the local mover area,
         // converts the 3D model to a better format (if osgconv is available), copies
         // the appropriate model file(s) to the resources/site folder, and modifies
         // the site/model.txt file to include the new model.
         void Output3dModelSet();

         // Returns the appropriate path to the local mover models_3d folder
         QString GetLocalModelPath();

         // Returns a QString representing the corresponding file extension
         QString GetFileExtension(SupportedFileType aFileType);

         // Converts an OBJ file to a specified format.
         // Returns true on success.
         bool ConvertModelTo(QString           aFilePathWithoutExtension,
                             SupportedFileType aFileType);

         // Creates a local copy of models.txt with the new vehicle added,
         // then overwrites the original copy in resources/site
         bool AddVehicleToModelsList(const QString&     aFilePath,
                                     const QString&     aRelativePathToModel,
                                     Designer::Vehicle* aVehicle);

         void ObjBegin();
         void ObjVertex3d(double aX, double aY, double aZ);
         void ObjNormal3d(double aX, double aY, double aZ);
         void ObjEnd();

      protected:

         // Outputs the OBJ file to the local model folder
         void OutputTemporary3dModel();

         // Converts the OBJ model to a better format (if osgconv is available),
         // copies the appropriate model file(s) to the resources/site/models folder,
         // and modifies the site/model.txt file to include the new model.
         // If osgconv exists, the obj file is converted to the specified type (.ive by default)
         void OutputFinal3dModel(SupportedFileType aFileType = SupportedFileType::cIVE);

         // This outputs the OBJ and MTL files. The common name (without suffix)
         // for the files is specified by aFilename.
         bool OutputObjFiles(const std::string& aFileDir, const std::string& aFilename);

         void OutputSiteInitialModelsFile(const QString& aFilename);

         virtual void DrawEngines(Designer::Vehicle* aVehicle);
         virtual void DrawWingsAndSurfaces(Designer::Vehicle* aVehicle);
         virtual void DrawDishes(Designer::Vehicle* aVehicle);
         virtual void DrawBodiesAndNacelles(Designer::Vehicle* aVehicle);
         virtual void DrawCanopy(Designer::Vehicle* aVehicle);

         void DrawBodyCylinder(double aPosX,
                               double aPosY,
                               double aPosZ,
                               double aLength,
                               double aHeight,
                               double aWidth,
                               bool   aAft = false);

         void DrawBodyOgive(double aPosX,
                            double aPosY,
                            double aPosZ,
                            double aLength,
                            double aHeight,
                            double aWidth,
                            bool   aAft = false);

         void DrawBodyCone(double aPosX,
                           double aPosY,
                           double aPosZ,
                           double aLength,
                           double aHeight,
                           double aWidth,
                           bool   aAft = false);

         void DrawBodySphere(double aPosX,
                             double aPosY,
                             double aPosZ,
                             double aLength,
                             double aHeight,
                             double aWidth,
                             bool   aAft = false);

         void DrawBodyBoatTail(double aPosX,
                               double aPosY,
                               double aPosZ,
                               double aLength,
                               double aHeight,
                               double aWidth,
                               double aDiam,
                               bool   aAft = false);

         void DrawBodyBlunt(double aPosX,
                            double aPosY,
                            double aPosZ,
                            double aLength,
                            double aHeight,
                            double aWidth,
                            bool   aAft = false);

         void DrawRoundedNacelle(double             aLength_ft,
                                 double             aHeight_ft,
                                 double             aWidth_ft,
                                 double             aThickness_ft,
                                 const std::string& aOverallShapeString,
                                 bool               aAftSectionIsTapered,
                                 double             aAftSectionLength_ft);

         void DrawRoundedRightNacelle(double             aLength_ft,
                                      double             aHeight_ft,
                                      double             aWidth_ft,
                                      double             aThickness_ft,
                                      const std::string& aOverallShapeString,
                                      bool               aAftSectionIsTapered,
                                      double             aAftSectionLength_ft);

         void DrawRoundedLeftNacelle(double             aLength_ft,
                                     double             aHeight_ft,
                                     double             aWidth_ft,
                                     double             aThickness_ft,
                                     const std::string& aOverallShapeString,
                                     bool               aAftSectionIsTapered,
                                     double             aAftSectionLength_ft);

         void DrawRoundedTopNacelle(double             aLength_ft,
                                    double             aHeight_ft,
                                    double             aWidth_ft,
                                    double             aThickness_ft,
                                    const std::string& aOverallShapeString,
                                    bool               aAftSectionIsTapered,
                                    double             aAftSectionLength_ft);

         void DrawRoundedBottomNacelle(double             aLength_ft,
                                       double             aHeight_ft,
                                       double             aWidth_ft,
                                       double             aThickness_ft,
                                       const std::string& aOverallShapeString,
                                       bool               aAftSectionIsTapered,
                                       double             aAftSectionLength_ft);

         void DrawFlatSidedNacelle(double             aLength_ft,
                                   double             aHeight_ft,
                                   double             aWidth_ft,
                                   double             aThickness_ft,
                                   double             aForwardSweepLength_ft,
                                   const std::string& aOverallShapeString,
                                   bool               aAftSectionIsTapered,
                                   double             aAftSectionLength_ft);

         void DrawFlatSweptRightNacelle(double             aLength_ft,
                                        double             aHeightInner_ft,
                                        double             aHeightOuter_ft,
                                        double             aWidth_ft,
                                        double             aThickness_ft,
                                        double             aForwardSweepLength_ft,
                                        const std::string& aOverallShapeString,
                                        bool               aAftSectionIsTapered,
                                        double             aAftSectionLength_ft);

         void DrawFlatSweptLeftNacelle(double             aLength_ft,
                                       double             aHeightInner_ft,
                                       double             aHeightOuter_ft,
                                       double             aWidth_ft,
                                       double             aThickness_ft,
                                       double             aForwardSweepLength_ft,
                                       const std::string& aOverallShapeString,
                                       bool               aAftSectionIsTapered,
                                       double             aAftSectionLength_ft);

         void DrawBody(double                         aPosX,
                       double                         aPosY,
                       double                         aPosZ,
                       double                         aLength,
                       double                         aHeight,
                       double                         aWidth,
                       GeometryBody::ForwardShapeType aFrontShapeString,
                       double                         aFrontLength,
                       GeometryBody::AftShapeType     aAftShapeString,
                       double                         aRearLength,
                       double                         aBoatTailDiam,
                       double                         aYaw_deg,
                       double                         aPitch_deg,
                       double                         aRoll_deg);

         void DrawEngine(double aPosX,
                         double aPosY,
                         double aPosZ,
                         double aDiameter_ft,
                         double aLength_ft,
                         double aOffset_ft,
                         double aYaw_deg,
                         double aPitch_deg,
                         double aRoll_deg);

         void DrawSurface(double aPosX,
                          double aPosY,
                          double aPosZ,
                          double aSpan,
                          double aSweep_deg,
                          double aRootChord,
                          double aTipChord,
                          double aThicknessRatio,
                          double aDihedral_deg,
                          double aIncidence_deg,
                          int    aNumSpanElements);

         void DrawNacelle(double             aPosX,
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
                          double             aRoll_deg);

         void DrawDish(double aPosX,
                       double aPosY,
                       double aPosZ,
                       double aDiameter_ft,
                       double aThickness_ft);

         void DrawBoxWithHole(double aLength, double aHeight, double aWidth, double aThickness);

         void DrawCylinder(double aLength, double aDiameter);

         double EllipticalY_GivenX(double x, double aHalfWidth, double aHalfHeight);
         double EllipticalX_GivenY(double y, double aHalfWidth, double aHalfHeight);

         void ObjPushMatrix();
         void ObjTranslated(double aPosX, double aPosY, double aPosZ);
         void ObjRotated(double aAngle_deg, double aX, double aY, double aZ);
         void ObjPopMatrix();

         void OutputVertices(QTextStream& aOutputStream);
         void OutputNormals(QTextStream& aOutputStream);
         void OutputTextureCoords(QTextStream& aOutputStream);
         void OutputFaces(QTextStream& aOutputStream);

         void OutputData(QTextStream&                aOutputStream,
                         double                      aData,
                         int                         aWidth,
                         int                         aPrecision,
                         QTextStream::FieldAlignment aAlignment);

         // Normalized points for top surface of airfoil measured aft of leading edge in X
         static double               mAirfoilPtsX[7];
         static double               mAirfoilPtsY[7];

         // Ogive with normalized X/Y from base
         static double               mOgivePtsX[5];
         static double               mOgivePtsY[5];

         double                      mEpsilon = std::numeric_limits<double>::epsilon();

         UtDCM                       mActiveMatrix;
         std::stack<UtDCM>           mMatrixStack;

         UtVec3dX                    mActiveTranslation;
         int                         mNumTranslations = 0;

         std::vector<UtVec3dX>       mVertexList;
         std::vector<UtVec3dX>       mNormalList;
         std::list<std::vector<int>> mFaceVertexList;
         std::list<std::vector<int>> mFaceNormalList;

         int                         mStartingVertexIndex = 0;
         int                         mNumVertexInPolygon  = 0;
         int                         mStartingNormalIndex = 0;
         int                         mNumNormalInPolygon  = 0;
   };
}

#endif // !GEOMETRY_OBJ_FILE_HPP
