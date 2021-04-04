# Load tests (tested on Debian/Ubuntu)


## 1. Install sysbench
> $ curl -s https://packagecloud.io/install/repositories/akopytov/sysbench/script.deb.sh | sudo bash

> $ sudo apt -y install sysbench

## 2. Create the database and user inside PostgreSQL
>$ psql
>> CREATE USER sbtest WITH PASSWORD 'password';

>> CREATE DATABASE sbtest;

>> GRANT ALL PRIVILEGES ON DATABASE sbtest TO sbtest;

## 3. Run test_1.sh for table and rows creation.

![alt text](https://raw.githubusercontent.com/AiratZa/Proxy_SQL_Logger/master/load_tests/test_1_results.png)

## 4. Run multi-thread stress test with SELECT-queries

![alt text](https://raw.githubusercontent.com/AiratZa/Proxy_SQL_Logger/master/load_tests/test_2_results.png)


