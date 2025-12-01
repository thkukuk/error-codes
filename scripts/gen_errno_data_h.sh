#!/bin/sh

CC=${CC:-cc}
OUTPUT=${1:-errno_data.h}
TESTPROG="dump-errno.c"

cat << 'EOF' > "${OUTPUT}"
static const struct entry errno_data[] = {
EOF

echo '#include <errno.h>' > "${TESTPROG}"
$CC -E -dD "${TESTPROG}" | awk '/^#define E/ { printf "  {\"%s\", %s},\n", $2, $2 }' >> "${OUTPUT}"

cat << 'EOF' >> "${OUTPUT}"
    { NULL, -1 }
};
EOF

rm -f "${TESTPROG}"
