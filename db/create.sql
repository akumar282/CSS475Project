-- build test database
CREATE DATABASE airporttest;

\c airporttest; 

CREATE TABLE test
(
    id      SERIAL          NOT NULL,
    name    VARCHAR(30)     NOT NULL 
    PRIMARY KEY(id)
)   
;

-- permissions
CREATE USER test WITH LOGIN PASSWORD 'testpassword';
GRANT ALL ON DATABASE airporttest TO test;
GRANT ALL ON ALL TABLES IN SCHEMA public TO test;

-- inserts
COPY test FROM stdin;
1 test1
2 test2
3 test3
4 test4
5 test5
\.