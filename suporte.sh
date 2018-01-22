#!/bin/bash
OUTPUT=$(mktemp /tmp/mail.suporte.XXXXXXXXXXX)

cat > ${OUTPUT}

(exec /opt/mailbot/mailbot ${OUTPUT})

exit 0
