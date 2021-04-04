#!/bin/bash

sysbench \
--db-driver=pgsql \
--report-interval=2 \
--oltp-table-size=100000 \
--oltp-tables-count=24 \
--threads=64 \
--time=300 \
--pgsql-host=127.0.0.1 \
--pgsql-port=8080 \
--pgsql-user=logger_test3 \
--pgsql-password=password \
--pgsql-db=logger_test3 \
/usr/share/sysbench/tests/include/oltp_legacy/select.lua \
run
