#!/bin/sh

CC=${CC:-cc}
OUTPUT=${1:-errno_data.h}
TESTPROG="dump-econf.c"

cat << 'EOF' > "${OUTPUT}"
static const struct entry econf_data[] = {
EOF

echo '#include <libeconf.h>' > "${TESTPROG}"
$CC -E -dD  "${TESTPROG}" | awk '/ECONF_/ { printf "  {\"%s\", %s},\n", $1, $1 }' >> "${OUTPUT}"

cat << 'EOF' >> "${OUTPUT}"
  { NULL, -1 }
};
EOF

rm -f "${TESTPROG}"
