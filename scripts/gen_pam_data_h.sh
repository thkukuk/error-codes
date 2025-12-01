#!/bin/sh

CC=${CC:-cc}
OUTPUT=${1:-errno_data.h}
TESTPROG="dump-pam.c"

cat << 'EOF' > "${OUTPUT}"
static const struct entry pam_data[] = {
EOF

echo '#include <security/_pam_types.h>' > "${TESTPROG}"
NUM_VALUES=$($CC -E -dD "${TESTPROG}" | grep _PAM_RETURN_VALUES |awk '{print $3}')
$CC -E -dD "${TESTPROG}" | awk '/^#define PAM_/ { printf "  {\"%s\", %s},\n", $2, $2 }' | head -n "${NUM_VALUES}" >> "${OUTPUT}"

cat << 'EOF' >> "${OUTPUT}"
  { NULL, -1 }
};
EOF

rm -f "${TESTPROG}"
