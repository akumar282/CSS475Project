-- Initialize the new airport database
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

-- Domain Tables

-- Airline Domain
CREATE DOMAIN airline_name_domain AS VARCHAR(40) 
	CHECK (
		VALUE IN ('Alaskan Airlines', 'Spirit Airlines', 'United Airlines', 'Delta Airlines', 'Turkish Airlines', 'Qatar Airways', 'Korean Air')
	);

-- LocationType Domain
CREATE DOMAIN icao_domain AS CHAR(4) 
	CHECK (
		VALUE IN ('KLAX', 'KSEA', 'KJFK', 'KDTW')
	);

-- MealCategory Domain
CREATE DOMAIN meal_category_domain AS VARCHAR(20) 
	CHECK (
		VALUE IN ('Halal', 'Vegan', 'Pescitarian', 'Kosher', 'Non-Veggie', 'Gluten Free')
	);

-- MealType Domain
CREATE DOMAIN meal_name_domain AS TEXT 
	CHECK (
		VALUE IN ('Steak Burger', 'Veggie Burger', 'Fish Tacos', 'Strip Steak', 'Pasta')
	);

-- Status Domain
CREATE DOMAIN status_info_domain AS VARCHAR(40) 
	CHECK (
		VALUE IN ('Delayed', 'In Transit', 'Cancelled', 'Ready')
	);

-- Terminal Domain
CREATE DOMAIN terminal_letter_domain AS CHAR(1) 
	CHECK (
		VALUE IN ('A', 'B', 'C')
	);

-- Database Schema 

-- AirlineType Table
CREATE TABLE AirlineType (
	id				SERIAL NOT NULL,
	name			airline_name_domain NOT NULL UNIQUE,

	PRIMARY KEY		(id)
);

-- AirplaneType Table
CREATE TABLE AirplaneType (
	id				SERIAL NOT NULL,
	make			VARCHAR(40) NOT NULL,
	model			VARCHAR(40) NOT NULL,
	max_cargo 		NUMERIC NOT NULL,
	max_passengers	INTEGER NOT NULL,
	num_rows		INTEGER NOT NULL,

	PRIMARY KEY		(id)
);

--	Cargo Table
CREATE TABLE Cargo (
	id				INTEGER NOT NULL,
	flight_id		INTEGER NOT NULL,
	weight_lb		NUMERIC NOT NULL,				
	
	PRIMARY KEY		(id),
	FOREIGN KEY 	(flight_id)		REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED
);

--	Flight Table
CREATE TABLE Flight (
	id				SERIAL NOT NULL,
	flight_number	INTEGER NOT NULL,
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

--	Gate Table
CREATE TABLE GateType (
	id				SERIAL NOT NULL,
	terminal_id		INTEGER NOT NULL UNIQUE,
	gate_number		INTEGER NOT NULL,

	PRIMARY KEY		(id), 
	FOREIGN KEY 	(terminal_id) REFERENCES TerminalType(id) DEFERRABLE INITIALLY DEFERRED
);

-- LocationType Table 
CREATE TABLE LocationType (
	id				SERIAL NOT NULL,
	country			VARCHAR(40) NOT NULL,
	state			VARCHAR(40) NOT NULL,
	city			VARCHAR(40) NOT NULL,
	icao 			icao_domain NOT NULL UNIQUE,

	PRIMARY KEY		(id)
);

-- MealType Table
CREATE TABLE MealType (
	id			SERIAL NOT NULL,
	name 	meal_name_domain NOT NULL,

	PRIMARY KEY	(id)
);

-- MealCategoryType Table
CREATE TABLE MealCategoryType (
	id				SERIAL NOT NULL,
	category		meal_category_domain NOT NULL,
		
	PRIMARY KEY		(id)
);

--	MealToAirline Table
CREATE TABLE MealToAirline (
	flight_id		INTEGER NOT NULL,
	meal_id 		INTEGER NOT NULL,
	
	PRIMARY KEY		(flight_id, meal_id), 
	FOREIGN KEY 	(flight_id) REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(meal_id) 	REFERENCES MealType(id) DEFERRABLE INITIALLY DEFERRED
);

--	MealToCategory Table
CREATE TABLE MealToCategory (
	meal_id			INTEGER NOT NULL,
	category_id		INTEGER NOT NULL,
		
	PRIMARY KEY		(meal_id, category_id), 
	FOREIGN KEY 	(meal_id)		REFERENCES MealType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY		(category_id)	REFERENCES MealCategoryType(id) DEFERRABLE INITIALLY DEFERRED
);

-- StatusType Table
CREATE TABLE StatusType (
	id			SERIAL NOT NULL,
	name		status_info_domain NOT NULL UNIQUE,

	PRIMARY KEY		(id)
);

--	Terminal Table
CREATE TABLE TerminalType (
	id			SERIAL NOT NULL,
	letter		terminal_letter_domain NOT NULL,

	PRIMARY KEY		(id)
);


-- After the schema has been created, test using the following commands

\d 
\d Flight
\d Cargo
\d MealToAirline
\d MealType
\d MealToCategory
\d MealToCategoryType
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
1	'Alaskan Airlines'
2	'Spirit Airlines'
3	'United Airlines'
4	'Delta Airlines'
5	'Turkish Airlines'
6	'Qatar Airways'
7	'Korean Air'
\.
SELECT setval('airlinetype_id_seq', 8);

-- not neccessarially correct data
COPY AirplaneType(id, make, model, max_cargo, max_passengers, num_rows)
1 'Boeing'				'787'	10000	192
2 'Boeing'				'777'	10000	192
3 'Boeing'				'737'	8000	144
4 'Airbus'				'A320'	10000	192
5 'McDonnell Douglas'	'MD-80'	7500	168
SELECT setval('airplanetype_id_seq', 6);

COPY Cargo(id, flight_id, weight_lb) FROM stdin;
./
SELECT setval('cargo_id_seq', 1);

COPY Flight(id, flight_number, departure_time, arrival_time, num_passengers, gate_id, status_id, airplane_id, destination_id, origin_id, airline_id) FROM stdin;
./
SELECT setval('flight_id_seq', 1);

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
./
SELECT setval('cargo_id_seq', 15);

COPY LocationType(id, country, state, city, icao)
1	'United States'	'CA'	'Los Angeles'	'KLAX'
2	'United States'	'WA'	'Seattle'		'KSEA'
3	'United States'	'NY'	'New York City'	'KJFK'
4	'United States'	'MI'	'Detroit'		'KDTW'
\.
SELECT setval('locationtype_id_seq', 5);

COPY MealCategoryType(id, category) FROM stdin;
1	'Steak Burger'
2	'Veggie Burger'
3	'Fish Tacos'
4	'Strip Steak' 
5	'Pasta'
SELECT setval('mealcategorytype_id_seq', 6);

COPY StatusType(id, name) FROM stdin;

1	'Delayed'
2	'In Transit'
3	'Cancelled'	
4 	'Ready'
\.
SELECT setval('statustype_id_seq', 5);

COPY TerminalType(id, letter) FROM stdin;
1	A
2	B 
3	C
\.
SELECT setval('terminaltype_id_seq', 4);
