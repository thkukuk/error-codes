#!/bin/sh

CC=${CC:-cc}
OUTPUT=${1:-curl_data.h}
TESTPROG="dump-curl.c"

cat << 'EOF' > "${OUTPUT}"
static const struct entry curl_data[] = {
EOF

echo '#include <curl/curl.h>' > "${TESTPROG}"
$CC -E -dD  "${TESTPROG}" | sed -e 's|,||g' | awk '/^  CURLE_/ { printf "  {\"%s\", %s},\n", $1, $1 }' >> "${OUTPUT}"

cat << 'EOF' >> "${OUTPUT}"
  { NULL, -1 }
};
EOF

rm -f "${TESTPROG}"
