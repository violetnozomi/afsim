// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTQTCOMPRESSDIRECTORY_H
#define UTQTCOMPRESSDIRECTORY_H

#include "utilqt_export.h"

#include <QDir>

class QDataStream;
class QString;

//! Compresses a directory into a file and back again.
class UTILQT_EXPORT UtQtCompressDirectory
{
public:
   UtQtCompressDirectory();
   virtual ~UtQtCompressDirectory();

   bool Compress(const QString& aSourceDirPath, const QString& aDestFilePath);
   bool Decompress(const QString& aSourceFilePath, const QString& aDestDirPath);
   void SetIncludeEmptyDirectories(bool aInclude);

protected:
   virtual QFileInfoList FileList(const QDir& aDir) const;
   virtual QFileInfoList SubdirectoryList(const QDir& aDir) const;

private:
   bool CompressDir(QDataStream& aDataStream, const QDir& aSourceDir, const QDir& aPrefixDir = QDir());

   bool mIncludeEmptyDirectories;
};

#endif // UTQTCOMPRESSDIRECTORY_H
