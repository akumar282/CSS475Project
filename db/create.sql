CREATE DATABASE airporttest;

\c airporttest; 

CREATE TABLE test
(
    id      INTEGER PRIMARY KEY     NOT NULL,
    name    VARCHAR(30)             NOT NULL 
)   
;

CREATE USER test WITH LOGIN AND PASSWORD 'testpassword';