# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
Installation of CME                                                 15 Aug 2014
===================

1. Loading the software
---------------------------

a) Select a directory where you wish to install the software.  If it does not
   exist, create it using 'mkdir'.

   For the remainder of the instructions, we'll assume '/ids1c/rel'.  You can
   select any value you want - just replace '/ids1c/rel' with whatever you have
   selected.

   The loading process will create a subdirectory called CME.  It is recommended
   that you rename any old installations which you wish to keep separate before
   extracting the new version.

b) Change your working directory to the directory selected in step a).

     cd /ids1c/rel

c) Load the data.

     gunzip -dc <distribution_file> | tar xvf -    (If the file ends in .gz)
     zcat -dc <distribution_file> | tar xvf -      (If the file ends in .Z)

   The name of the file will be of the form 'cme_unclass_vX.YY_ZZZ.tar.gz'.

   Look for errors that may have occurred in extracting the files.
   A common one is:  "- cannot create -- Permission denied"
   If this happens change the permissions of the file, and extract it again.
   Previous to version 2.40 many files in the data and models directories were
   marked as read only.  If the error is coming from either of those two
   directories, the quick solutions are to move the directory giving it a
   ".old" suffix, or to "chmod u+w data models" from the CME directory.

d) Release note for the current version are in the file: 

     CME/AAA_RELEASE_NOTES.txt


2. Configuring the software
---------------------------

Once you load the software, you may want to set up your path to execute from
anywhere.

a) Take note of the version that you loaded.  This is typically looks like
   'vX.Y.Z'.  If you don't know what it is, simply look in the CME directory
   and you will see directories of the form 'bin.vX.Y.Z'.  The highest 'X.Y.Z'
   is probably what you want.

b) $ export PATH=<path_to_cme_install>/bin.vX.Y.Z:$PATH
     (where 'vX.Y.Z' is the version noted in step a))

c) This may be something you want set up in your start-up script (such as
   .bashrc in your home directory), so that CME is available all the time.


3.  Using the previous version
------------------------------

Using an old version require resetting your path.

a) $ export PATH=<path_to_old_cme_install>/bin.vX.Y.Z:$PATH
     (where 'vX.Y.Z' is the old version))

b) If you set up the path in your start-up script, be sure to modify that as
   well if you wish to revert perminantly.

4.  Deleting old versions
-------------------------

Obsolete versions of CME can be deleted at the users convenience.  In the CME
directory there are two directories and four files for each CME version.
Symbolic links are used to point to the actual current version.  The actual
version numbers are at the end of the directory/file name.  Here is an example:

drwxrwsr-x    2 johnson  swdev       4096 Mar  8 05:03 bin.v3.4.0
drwxrwsr-x    2 johnson  swdev       4096 Mar  8 05:03 bin.v3.4.1
drwxrwsr-x    2 johnson  swdev       4096 Jul 14 15:58 bin.v3.5.0
drwxrwsr-x    2 johnson  swdev       4096 Aug 14 14:00 bin.v3.5.1
-rw-rw-r--    1 johnson  swdev        717 Jan 28  2003 cshcmeremove.v3.4.0
-rw-rw-r--    1 johnson  swdev        717 Feb 26 12:17 cshcmeremove.v3.4.1
-rw-rw-r--    1 johnson  swdev        717 Jun 19 11:53 cshcmeremove.v3.5.0
-rw-rw-r--    1 johnson  swdev        717 Aug 13 15:12 cshcmeremove.v3.5.1
-rw-rw-r--    1 johnson  swdev       2278 Jan 28  2003 cshcmesetup.v3.4.0
-rw-rw-r--    1 johnson  swdev       2278 Feb 26 12:17 cshcmesetup.v3.4.1
-rw-rw-r--    1 johnson  swdev       2278 Jun 20 12:51 cshcmesetup.v3.5.0
-rw-rw-r--    1 johnson  swdev       2278 Aug 13 15:12 cshcmesetup.v3.5.1
drwxrwsr-x    4 johnson  swdev         65 Mar 26 10:01 doc.v3.4.0
drwxrwsr-x    4 johnson  swdev         65 Mar 26 10:04 doc.v3.4.1
drwxrwsr-x    4 johnson  swdev         65 Jun 19 14:43 doc.v3.5.0
drwxrwsr-x    4 johnson  swdev         65 Aug 13 15:12 doc.v3.5.1
drwxrwsr-x    4 johnson  swdev       4096 Feb  3  2003 etc.v3.4.0
drwxrwsr-x    4 johnson  swdev       4096 Apr  9 09:05 etc.v3.4.1
drwxrwsr-x    4 johnson  swdev       4096 Jun 19 11:53 etc.v3.5.0
drwxrwsr-x    4 johnson  swdev       4096 Aug 14 13:48 etc.v3.5.1
-rw-rw-r--    1 johnson  swdev        931 Jan 28  2003 kshcmeremove.v3.4.0
-rw-rw-r--    1 johnson  swdev        931 Feb 26 12:17 kshcmeremove.v3.4.1
-rw-rw-r--    1 johnson  swdev        931 Jun 19 11:53 kshcmeremove.v3.5.0
-rw-rw-r--    1 johnson  swdev        931 Aug 13 15:12 kshcmeremove.v3.5.1
-rw-rw-r--    1 johnson  swdev       2093 Jan 28  2003 kshcmesetup.v3.4.0
-rw-rw-r--    1 johnson  swdev       2093 Feb 26 12:17 kshcmesetup.v3.4.1
-rw-rw-r--    1 johnson  swdev       2093 Jun 20 12:51 kshcmesetup.v3.5.0
-rw-rw-r--    1 johnson  swdev       2093 Aug 13 15:12 kshcmesetup.v3.5.1
drwxrwsr-x    2 johnson  swdev       4096 Jan 29  2003 lib.v3.4.0
drwxrwsr-x    2 johnson  swdev       4096 Feb 26 12:38 lib.v3.4.1
drwxrwsr-x    2 johnson  swdev       4096 Jun 19 11:53 lib.v3.5.0
drwxrwsr-x    2 johnson  swdev       4096 Aug 14 14:00 lib.v3.5.1

From this, it can be determined, that version 3.5.1 is the most current
version, and if desired, the other versions can be removed by issuing
the following command:

rm -rf *.vX.Y.Z
