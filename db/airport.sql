-- Initialize the new airport database
DROP DATABASE airport;
SELECT current_database();
CREATE DATABASE airport;

\c airport
SELECT current_database();

-- Table List
/*
Airline
Airplane
Cargo
Flight
Gate
Location
Meal
MealCategory
MealToAirline
MealToCategory
Status
Terminal
*/

-- Database Schema 

-- AirlineType Table
CREATE TABLE AirlineType (
	id				SERIAL NOT NULL,
	name			VARCHAR(40) NOT NULL UNIQUE,

	PRIMARY KEY		(id)
);

-- AirplaneType Table
CREATE TABLE AirplaneType (
	id				SERIAL NOT NULL,
	name			VARCHAR(40) NOT NULL,
	max_cargo 		NUMERIC NOT NULL,
	max_passengers	INTEGER NOT NULL,
	num_rows		INTEGER NOT NULL,

	PRIMARY KEY		(id)
);

-- LocationType Table 
CREATE TABLE LocationType (
	id				SERIAL NOT NULL,
	country			VARCHAR(40) NOT NULL,
	state			VARCHAR(40) NOT NULL,
	city			VARCHAR(40) NOT NULL,
	icao 			VARCHAR(4) NOT NULL UNIQUE,

	PRIMARY KEY		(id)
);

-- MealType Table
CREATE TABLE MealType (
	id		SERIAL NOT NULL,
	name 	VARCHAR(20) NOT NULL,

	PRIMARY KEY	(id)
);

-- MealCategoryType Table
CREATE TABLE MealCategoryType (
	id				SERIAL NOT NULL,
	category		VARCHAR(20) NOT NULL,
		
	PRIMARY KEY		(id)
);

-- StatusType Table
CREATE TABLE StatusType (
	id			SERIAL NOT NULL,
	name		VARCHAR(20) NOT NULL UNIQUE,

	PRIMARY KEY		(id)
);

--	Terminal Table
CREATE TABLE TerminalType (
	id			SERIAL NOT NULL,
	letter		CHAR(1) NOT NULL,

	PRIMARY KEY		(id)
);

--	Gate Table
CREATE TABLE GateType (
	id				SERIAL NOT NULL,
	terminal_id		INTEGER NOT NULL,
	gate_number		INTEGER NOT NULL,

	PRIMARY KEY		(id), 
	FOREIGN KEY 	(terminal_id) REFERENCES TerminalType(id) DEFERRABLE INITIALLY DEFERRED
);

--	MealToCategory Table
CREATE TABLE MealToCategory (
	meal_id			INTEGER NOT NULL,
	category_id		INTEGER NOT NULL,
		
	PRIMARY KEY		(meal_id, category_id), 
	FOREIGN KEY 	(meal_id)		REFERENCES MealType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY		(category_id)	REFERENCES MealCategoryType(id) DEFERRABLE INITIALLY DEFERRED
);

--	Flight Table
CREATE TABLE Flight (
	id				SERIAL NOT NULL,
	flight_number	VARCHAR(7) NOT NULL,
	departure_time	TIMESTAMP NOT NULL,
	arrival_time	TIMESTAMP NOT NULL,
	num_passengers	INTEGER NOT NULL,
	gate_id 		INTEGER NOT NULL,
	status_id		INTEGER NOT NULL,
	airplane_id 	INTEGER NOT NULL,
	destination_id	INTEGER NOT NULL,
	origin_id		INTEGER NOT NULL,
	airline_id		INTEGER NOT NULL,
	
	PRIMARY KEY		(id),
	FOREIGN KEY 	(gate_id) 			REFERENCES GateType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(status_id) 		REFERENCES StatusType(id) DEFERRABLE INITIALLY DEFERRED,  -- "status" is a keyword in psql
	FOREIGN KEY 	(airplane_id) 		REFERENCES AirplaneType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(destination_id)	REFERENCES LocationType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(origin_id) 		REFERENCES LocationType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(airline_id) 		REFERENCES AirlineType(id) DEFERRABLE INITIALLY DEFERRED

);

--	MealToAirline Table
CREATE TABLE MealToAirline (
	flight_id		INTEGER NOT NULL,
	meal_id 		INTEGER NOT NULL,
	
	PRIMARY KEY		(flight_id, meal_id), 
	FOREIGN KEY 	(flight_id) REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(meal_id) 	REFERENCES MealType(id) DEFERRABLE INITIALLY DEFERRED
);

--	Cargo Table
CREATE TABLE Cargo (
	id				SERIAL NOT NULL,
	flight_id		INTEGER NOT NULL,
	weight_lb		NUMERIC NOT NULL,				
	
	PRIMARY KEY		(id),
	FOREIGN KEY 	(flight_id)		REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED
);


-- After the schema has been created, test using the following commands

\d 
\d Flight
\d Cargo
\d MealType
\d MealCategoryType
\d MealToAirline
\d MealToCategory
\d StatusType
\d GateType
\d TerminalType
\d AirplaneType
\d LocationType
\d AirlineType

-- Delete Database 
--\c postgres
--SELECT current_database();
--DROP DATABASE airport;

-- Frontloaded Data

COPY AirlineType(id, name) FROM stdin;
1	Alaskan Airlines
2	Spirit Airlines
3	United Airlines
4	Delta Airlines
5	Turkish Airlines
6	Qatar Airways
7	Korean Air
\.
SELECT setval('airlinetype_id_seq', 8);

-- not neccessarially correct data
COPY AirplaneType(id, name, max_cargo, max_passengers, num_rows) FROM stdin;
1	Boeing 787	10000.0	296	6
2	Boeing 777	10000.0	312	6
3 	Boeing 737	8000.0	144	6
4	Airbus A320	10000.0	192	6
5	McDonnell Douglas MD-80	7500.0	168	6
\.
SELECT setval('airplanetype_id_seq', 6);

COPY Cargo(id, flight_id, weight_lb) FROM stdin;
\.
SELECT setval('cargo_id_seq', 1);

COPY LocationType(id, country, state, city, icao) FROM stdin;
1	United States	CA	Los Angeles	KLAX
2	United States	WA	Seattle	KSEA
3	United States	NY	New York City	KJFK
4	United States	MI	Detroit	KDTW
\.
SELECT setval('locationtype_id_seq', 5);

COPY MealType(id, name) FROM stdin;
\.
SELECT setval('mealtype_id_seq', 1);

COPY MealCategoryType(id, category) FROM stdin;
1	Steak Burger
2	Veggie Burger
3	Fish Tacos
4	Strip Steak
5	Pasta
\.
SELECT setval('mealcategorytype_id_seq', 6);

-- COPY MealToAirline(flight_id, meal_id) FROM stdin;
-- \.

-- COPY MealToCategory(meal_id, category_id) FROM stdin;
-- \.

COPY StatusType(id, name) FROM stdin;
1	Delayed
2	In Transit
3	Cancelled
4	Ready
\.
SELECT setval('statustype_id_seq', 5);

COPY TerminalType(id, letter) FROM stdin;
1	A
2	B 
3	C
\.
SELECT setval('terminaltype_id_seq', 4);

COPY GateType(id, terminal_id, gate_number) FROM stdin;
1	1	1
2	1	2
3	1	3
4	1	4
5	1	5
6	2	1
7	2	2
8	2	3
9	2	4
10	2	5
11	2	6
12	3	1
13	3	2
14	3	3
\.
SELECT setval('cargo_id_seq', 15);

COPY Flight(id, flight_number, departure_time, arrival_time, num_passengers, gate_id, status_id, airplane_id, destination_id, origin_id, airline_id) FROM stdin;
1	AL001	2023-03-09 09:00:00	2023-03-09 16:00:00	124	8	2	2	1	2	1
\.
SELECT setval('flight_id_seq', 1);

-- permissions
CREATE USER admin WITH LOGIN PASSWORD 'password';
GRANT ALL ON DATABASE airport TO admin;
GRANT ALL ON ALL TABLES IN SCHEMA public TO admin;
