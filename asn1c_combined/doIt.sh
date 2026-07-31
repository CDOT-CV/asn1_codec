#!/bin/bash
set -euo pipefail

# This script assumes that the J2735 files have already been generated.
# To generate the J2735 files, run generate-files.sh with asn1c installed.

export LD_LIBRARY_PATH=/usr/local/lib
export CC=gcc

# if J2735_YEAR is not set, default to 2024
if [ -z "${J2735_YEAR:-}" ]; then
    year="2024"
else
    year=$J2735_YEAR
fi

# Copy generated files for specified year to asn1c_combined & extract
echo "Extracting & copying generated files for $year"
tar -xzf ./generated-files/$year.tar.gz
cp ./generated-files/$year/* .

# ISO 17573 generates a Time type in Time.h/time.h, which can shadow libc's
# <time.h> when the ASN.1 runtime sources are compiled from this directory.
# Rewrite those includes to a shim that always pulls in the real system header.
cat > system_time.h << 'EOF'
#ifndef ASN1_CODEC_SYSTEM_TIME_H_
#define ASN1_CODEC_SYSTEM_TIME_H_
/* Resolve to the real libc time.h, skipping any local ASN.1 time.h. */
#include_next <time.h>
#endif
EOF

sed -i 's|#include <time.h>|#include "system_time.h"|' \
  GeneralizedTime.c GeneralizedTime_ber.c GeneralizedTime_print.c GeneralizedTime_xer.c \
  generated-files/$year/GeneralizedTime.c \
  generated-files/$year/GeneralizedTime_ber.c \
  generated-files/$year/GeneralizedTime_print.c \
  generated-files/$year/GeneralizedTime_xer.c

# Compile example
echo "Compiling example"

# The HAVE_TM_GMTOFF flag needs to be set to compile GeneralizedTime.c on platforms including Alpine.
# Add it to ASN_MODULE_CFLAGS which gets added to CFLAGS in converter-example.mk
make ASN_MODULE_CFLAGS="-DHAVE_TM_GMTOFF" -f ./converter-example.mk

# `time.h` is a generated ISO 17573 type header.  It is not referenced by
# the compiled runtime (the type is exposed through Time.h), but leaving it
# beside the C++ sources makes standard <ctime> resolve to this ASN.1 header.
rm -f ./time.h

# Verify that the converter example executable compiled and can run
./converter-example -p list


# Clean up
echo "Cleaning up"
rm -rf ./generated-files/$year
