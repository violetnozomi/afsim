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

#include "UtQtCompressDirectory.hpp"

#include <cassert>

#include <QDataStream>
#include <QFile>

UtQtCompressDirectory::UtQtCompressDirectory()
   : mIncludeEmptyDirectories(true)
{
}

UtQtCompressDirectory::~UtQtCompressDirectory() {}

//! Compresses a directory into a file.
//! @param aSourceDirPath The path of the directory to compress.
//! @param aDestFilePath The desired path of the resulting compressed file.
bool UtQtCompressDirectory::Compress(const QString& aSourceDirPath, const QString& aDestFilePath)
{
   QFile destFile(aDestFilePath);
   if (destFile.open(QIODevice::WriteOnly))
   {
      QDataStream dataStream(&destFile);
      return CompressDir(dataStream, aSourceDirPath);
   }
   return false;
}

//! Decompresses a file into a directory.
//! @param aSourceFilePath The path of the file to decompress.
//! @param aDestDirPath The directory to decompress the file into.
bool UtQtCompressDirectory::Decompress(const QString& aSourceFilePath, const QString& aDestDirPath)
{
   QFile sourceFile(aSourceFilePath);
   if (QDir().mkpath(aDestDirPath) && sourceFile.open(QIODevice::ReadOnly))
   {
      QDataStream dataStream(&sourceFile);
      while (!dataStream.atEnd())
      {
         QString    filePath;
         QByteArray fileData;

         dataStream >> filePath >> fileData;

         // Create the subdirectories at the destination necessary to write the
         // decompressed file.
         const int lastSeparatorIdx = filePath.lastIndexOf('/');
         assert(lastSeparatorIdx != 0);
         if (lastSeparatorIdx > 0)
         {
            const QString& subdir = filePath.left(lastSeparatorIdx);
            QDir(aDestDirPath).mkpath(subdir);
         }

         // Write the file with decompressed data.
         if (!fileData.isEmpty())
         {
            QFile outFile(QDir(aDestDirPath).filePath(filePath));
            if (outFile.open(QIODevice::WriteOnly))
            {
               outFile.write(qUncompress(fileData));
            }
            else
            {
               return false;
            }
         }
      }

      return true;
   }

   return false;
}

//! Sets whether or not to include empty directories in the compressed directory
//! structure. By default, empty directories are included.
void UtQtCompressDirectory::SetIncludeEmptyDirectories(bool aInclude)
{
   mIncludeEmptyDirectories = aInclude;
}

//! Returns a list of files in the given directory. Subclasses can override this
//! to filter the files.
QFileInfoList UtQtCompressDirectory::FileList(const QDir& aDir) const
{
   return aDir.entryInfoList(QDir::Files | QDir::Hidden);
}

//! Returns a list of subdirectories in the given directory. Subclasses can
//! override this to filter the subdirectories.
QFileInfoList UtQtCompressDirectory::SubdirectoryList(const QDir& aDir) const
{
   return aDir.entryInfoList(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);
}

//! Recursive function for compressing a directory and its subdirectories.
//! @param aDataStream The data stream to output to the compressed file being created.
//! @param aSourceDirPath The path to a [sub]directory to compress.
//! @param aPrefix The relative path (taking parent directories into account) of
//! the directory currently being processed.
bool UtQtCompressDirectory::CompressDir(QDataStream& aDataStream, const QDir& aSourceDir, const QDir& aPrefixDir)
{
   if (aSourceDir.exists())
   {
      // For each subdirectory, call recursively.
      const QFileInfoList& subdirs = SubdirectoryList(aSourceDir);
      for (int i = 0; i < subdirs.count(); ++i)
      {
         const QDir subdir(subdirs[i].absoluteFilePath());
         const QDir newPrefixDir(aPrefixDir.filePath(subdirs[i].fileName()));
         CompressDir(aDataStream, subdir, newPrefixDir);
      }

      // For each file in the directory, output the file path and compressed data.
      const QFileInfoList& files = FileList(aSourceDir);
      for (int i = 0; i < files.count(); ++i)
      {
         QFile file(files[i].absoluteFilePath());
         if (file.open(QIODevice::ReadOnly))
         {
            aDataStream << aPrefixDir.filePath(files[i].fileName());
            aDataStream << qCompress(file.readAll());
         }
         else
         {
            return false;
         }
      }

      // Handle empty directories.
      if (files.isEmpty() && mIncludeEmptyDirectories)
      {
         aDataStream << aPrefixDir.path() + '/';
         aDataStream << QByteArray();
      }

      return true;
   }

   return false;
}
