# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
#!/bin/sh

# $Id: install_setup.sh,v 1.1 2004/09/16 15:45:03 m279366 Exp $
# Copy the [ck]shcmesetup and [ck]shcmeremove scripts to the release area.
# (Invoked by 'make install')

RELROOT=${1}
RELVER=${2}

##### of the CME_REL_SUFFIX environment variable)

sed_input=./install_tmp.sed
set -e              # Bomb on non-zero return code

rm -f ${RELROOT}/cshcmesetup.${RELVER} ${sed_input}
cat <<EOF >${sed_input}
/^ *setenv  *CME_REL_BASE  */c\\
setenv CME_REL_BASE "${RELROOT}"
/^ *setenv  *CME_REL_SUFFIX  */c\\
setenv CME_REL_SUFFIX ".${RELVER}"
EOF
sed -f ${sed_input} cshcmesetup >${RELROOT}/cshcmesetup.${RELVER}
chmod 664 ${RELROOT}/cshcmesetup.${RELVER}

rm -f ${RELROOT}/cshcmeremove.${RELVER}
cp cshcmeremove ${RELROOT}/cshcmeremove.${RELVER}
chmod 664 ${RELROOT}/cshcmeremove.${RELVER}

rm -f ${RELROOT}/kshcmesetup.${RELVER} ${sed_input}
cat <<EOF >${sed_input}
/^ *CME_REL_BASE=/c\\
CME_REL_BASE="${RELROOT}"
/^ *CME_REL_SUFFIX=/c\\
CME_REL_SUFFIX=".${RELVER}"
EOF
sed -f ${sed_input} kshcmesetup >${RELROOT}/kshcmesetup.${RELVER}
chmod 664 ${RELROOT}/kshcmesetup.${RELVER}

rm -f ${RELROOT}/kshcmeremove.${RELVER}
cp kshcmeremove ${RELROOT}/kshcmeremove.${RELVER}
chmod 664 ${RELROOT}/kshcmeremove.${RELVER}

rm -f ${sed_input}
exit 0
