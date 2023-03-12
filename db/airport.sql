-- Initialize the new airport database
DROP DATABASE airport;
DROP ROLE admin;
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
MealToFlight
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


-- Country Table
CREATE TABLE CountryType (
	id				SERIAL NOT NULL,
	name			VARCHAR(40) UNIQUE,

	PRIMARY KEY 	(id)
);

-- State Table
CREATE TABLE StateType (
	id				SERIAL NOT NULL,
	name			VARCHAR(40),
	country_id		INTEGER NOT NULL,

	PRIMARY KEY 	(id),
	FOREIGN KEY		(country_id)	REFERENCES CountryType(id) DEFERRABLE INITIALLY DEFERRED
);

-- City Table
CREATE TABLE CityType (
	id				SERIAL NOT NULL,
	name			VARCHAR(40) NOT NULL,
	state_id		INTEGER NOT NULL,

	PRIMARY KEY 	(id),
	FOREIGN KEY 	(state_id) 		REFERENCES StateType(id) DEFERRABLE INITIALLY DEFERRED
);

-- LocationType Table 
CREATE TABLE LocationType (
	id				SERIAL NOT NULL,
	city_id			INTEGER NOT NULL, 
	icao 			VARCHAR(4) NOT NULL UNIQUE,

	PRIMARY KEY		(id),
	FOREIGN KEY		(city_id)	REFERENCES	CityType(id) DEFERRABLE INITIALLY DEFERRED
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
	airline_id		INTEGER NOT NULL,
	destination_id	INTEGER NOT NULL,
	origin_id		INTEGER NOT NULL,
	
	PRIMARY KEY		(id),
	FOREIGN KEY 	(gate_id) 			REFERENCES GateType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(status_id) 		REFERENCES StatusType(id) DEFERRABLE INITIALLY DEFERRED,  -- "status" is a keyword in psql
	FOREIGN KEY 	(airplane_id) 		REFERENCES AirplaneType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(destination_id)	REFERENCES LocationType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(origin_id) 		REFERENCES LocationType(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(airline_id) 		REFERENCES AirlineType(id) DEFERRABLE INITIALLY DEFERRED,

	CHECK	(origin_id = 1 OR destination_id = 1)

);

--	Cargo Table
CREATE TABLE Cargo (
	id				SERIAL NOT NULL,
	flight_id		INTEGER NOT NULL,
	weight_lb		NUMERIC NOT NULL,				
	
	PRIMARY KEY		(id),
	FOREIGN KEY 	(flight_id)		REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED
);


--	MealToFlight Table
CREATE TABLE MealToFlight (
	flight_id		INTEGER NOT NULL,
	meal_id 		INTEGER NOT NULL,
	
	PRIMARY KEY		(flight_id, meal_id), 
	FOREIGN KEY 	(flight_id) REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(meal_id) 	REFERENCES MealType(id) DEFERRABLE INITIALLY DEFERRED
);

-- After the schema has been created, test using the following commands

-- \d 
-- \d Flight
-- \d Cargo
-- \d MealType
-- \d MealCategoryType
-- \d MealToFlight
-- \d MealToCategory
-- \d StatusType
-- \d GateType
-- \d TerminalType
-- \d AirplaneType
-- \d LocationType
-- \d AirlineType

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
8 	American Airlines
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

COPY CountryType(id, name) FROM stdin;
1	United States
\.
SELECT setval('countrytype_id_seq', 2);

COPY StateType(id, name, country_id) FROM stdin;
1	Michigan	1
2	Washington	1
3	New York	1
4	California	1
\.
SELECT setval('statetype_id_seq', 5);

COPY CityType(id, name, state_id) FROM stdin;
1	Detroit	1
2	Seattle	2
3	New York City	3
4	Los Angeles	4
\.
SELECT setval('locationtype_id_seq', 5);

-- KDTW is our airport
COPY LocationType(id, city_id, icao) FROM stdin;
1	1	KDTW
2	2	KSEA
3	3	KJFK
4	4	KLAX
\.
SELECT setval('locationtype_id_seq', 5);

COPY MealType(id, name) FROM stdin;
1	Steak Burger
2	Veggie Burger
3	Fish Tacos
4	Strip Steak
5	Vegan Pasta
6	Chicken Sandwich
7	Chicken Nuggets
8	Beef Enchiladas
9	Pork Sandwich
10	Seafood Salad
11 	Shrimp Tacos
12 	Mushroom Tacos
13	Caesar Salad
14	Grilled Cheese
15	Fruit Cup
16	Coca-Cola
17	Sprite
18	Fanta
19	Milk
20	Orange Juice
21	Water
22	Yogurt Parfait
23	Fruit Salad
\.
SELECT setval('mealtype_id_seq', 24);

COPY MealCategoryType(id, category) FROM stdin;
1 	Halal
2	Kosher
3 	Vegan
4	Vegetarian
5	Non-Vegetarian
6	Non-Dairy
7	Dairy
8	Nut-Free
9	Gluten-Free
10 	pescatarian
\.
SELECT setval('mealcategorytype_id_seq', 11);


COPY MealToCategory(meal_id, category_id) FROM stdin;
1	5
2	3
2 	4
3	5
4	5
5	3
5	4
5 	9
6	5
7	5
8	5
9	5
10	5
10 	10
11	5
11 	10
12	3
12	4
12 	9
13	4
14	4
15	3
15	4
16	6
17	6
18	6
19	7
20	6
21	6
22	7
23	3
23	4
\.

COPY StatusType(id, name) FROM stdin;
1	Standby
2	Boarding
3	Departed
4	Delayed
5	In Transit
6	Arrived
7	Cancelled
\.
SELECT setval('statustype_id_seq', 8);

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
SELECT setval('gatetype_id_seq', 15);

COPY Flight(id, flight_number, departure_time, arrival_time, num_passengers, gate_id, status_id, airplane_id, destination_id, origin_id, airline_id) FROM stdin;	
1	AL001	2023-03-09 09:00:00	2023-03-09 16:00:00	124	8	2	2	1	2	1
2	EK230	2023-03-09 09:00:00	2023-03-09 16:00:00	124	8	2	2	1	2	1
3	SQ028	2023-03-09 09:00:00	2023-03-09 16:00:00	124	8	2	2	1	2	1
4	CD024	2023-03-11 11:00:00	2023-03-11 18:00:00	230	7	4	3	3	1	1
\.
SELECT setval('flight_id_seq', 5);

COPY MealToFlight(flight_id, meal_id) FROM stdin;
1	1
1	2
1	3
1	4
1	5
1	16
1	19
1	21
1	22
\.


COPY Cargo(id, flight_id, weight_lb) FROM stdin;
1	1	1000.0
2	2	1000.0
3	1	1000.0
4	2	1000.0
5	3	1000.0
\.
SELECT setval('cargo_id_seq', 6);

-- permissions
CREATE USER admin WITH LOGIN PASSWORD 'password';
GRANT ALL ON DATABASE airport TO admin;
GRANT ALL ON ALL TABLES IN SCHEMA public TO admin;
GRANT ALL ON ALL SEQUENCES IN SCHEMA public TO admin;
