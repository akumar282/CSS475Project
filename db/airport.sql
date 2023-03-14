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
	CHECK 	(origin_id != destination_id)
	CHECK 	(departure_time < arrival_time)
);

--	Cargo Table
CREATE TABLE Cargo (
	id				SERIAL NOT NULL,
	flight_id		INTEGER NOT NULL,
	weight_lb		NUMERIC NOT NULL,
	barcode			CHAR(12) NOT NULL,				
	
	PRIMARY KEY		(id),
	FOREIGN KEY 	(flight_id)		REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED

	CHECK (weight_lb > 0)
);


--	MealToFlight Table
CREATE TABLE MealToFlight (
	flight_id		INTEGER NOT NULL,
	meal_id 		INTEGER NOT NULL,
	
	PRIMARY KEY		(flight_id, meal_id), 
	FOREIGN KEY 	(flight_id) REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED,
	FOREIGN KEY 	(meal_id) 	REFERENCES MealType(id) DEFERRABLE INITIALLY DEFERRED

);

CREATE TABLE Passenger (
	id				SERIAL NOT NULL,
	flight_id		INTEGER NOT NULL,
	barcode			CHAR(12) NOT NULL UNIQUE,
	
	PRIMARY KEY		(id),
	FOREIGN KEY 	(flight_id) REFERENCES Flight(id) DEFERRABLE INITIALLY DEFERRED
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

COPY CountryType(id, name) FROM stdin;
1	United States
\.
SELECT setval('countrytype_id_seq', 2);

COPY StateType(id, name, country_id) FROM stdin;
1	Michigan	1
2	Washington	1
3	New York	1
4	California	1
5	Florida	1
6	Ohio	1
7	Arizona	1
8	Texas	1
9	Nevada	1
10	New Jersey	1
\.
SELECT setval('statetype_id_seq', 11);

COPY CityType(id, name, state_id) FROM stdin;
1	Detroit	1
2	Seattle	2
3	New York City	3
4	Los Angeles	4
5	Miami	5
6	Columbus	6
7	Phoenix	7
8	Dallas	8
9	Las Vegas	9
10	Newark	10
\.
SELECT setval('locationtype_id_seq', 11);

-- KDTW is our airport
COPY LocationType(id, city_id, icao) FROM stdin;
1	1	KDTW
2	2	KSEA
3	3	KJFK
4	4	KLAX
5	5	KMIA
6	6	KCMH
7	7	KPHX
8	8	KDFW
9	9	KLAS
10	10	KEWR
\.
SELECT setval('locationtype_id_seq', 11);

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
10 	Pescatarian
\.
SELECT setval('mealcategorytype_id_seq', 11);

COPY MealToCategory(meal_id, category_id) FROM stdin;
1	5
1	1
2	3
2	4
3	5
3	1
3	10
4	5
5	3
5	4
5	9
6	5
7	5
7	1
8	5
9	5
10	5
10	10
11	5
11	10
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

COPY Flight(id, flight_number, departure_time, arrival_time, gate_id, status_id, airplane_id, destination_id, origin_id, airline_id) FROM stdin;	
1	AL001	2023-03-09 09:00:00	2023-03-09 16:00:00	8	2	2	1	2	1
2	EK230	2023-03-09 09:00:00	2023-03-09 16:00:00	8	2	2	4	1	5
3	SQ028	2023-03-09 09:00:00	2023-03-09 16:00:00	8	2	2	1	9	4
4	CD024	2023-03-11 11:00:00	2023-03-11 18:00:00	7	4	3	3	1	3
5	AA028	2023-03-11 11:00:00	2023-03-11 18:00:00	10	4	1	5	1	2
6	BB022	2023-03-11 11:00:00	2023-03-11 18:00:00	14	3	1	7	1	6
7	CC055	2023-03-11 11:00:00	2023-03-11 18:00:00	12	2	4	1	8	6
\.
SELECT setval('flight_id_seq', 8);

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
2	3
2	5
2	7
2	16
2	17
2	21
3	3
3	7
3	9
3	18
3	19
3	21
3	23
4	1
4	7
4	8
4	12
4	13
4	20
4	21
5	12
5	14
5	21
6	1
6	2
6 	9
6	17
6	18
6	21
7	1
7	3
7	7
7	11
7	12
7	16
7 	21
\.

COPY Cargo(id, flight_id, weight_lb, barcode) FROM stdin;
1	1	48.4	QV5H0D5R5O5W
2	1	204.3	17F3J6E8K6MA
3	1	175.9	G3F8W8Y9TXE7
4	2	269.8	34W2F2Q2T2G2
5	2	49.9	9Y6T7T6T1K8T
6	2	288.2	85N1G2F8W8X9
7	3	99.6	0N8W8T8T8T8T
8	3	411.5	VX9E9Z7R7Z6R
9	3	164.2	5V5R5D5R5K5W
10	4	62.3	6J3E6K3J6C3H
11	4	231.7	4W9Y9Y7V5L1D
12	4	289.6	V4R2R6R2R6RD
13	5	251.3	3V5G5L5B5V5F
14	5	332.0	Z4Z4H4G4X9ZD
15	5	401.4	7E5F5G5C5V2C
16	6	78.9	9T7T6T1T8T8T
17	6	445.3	L3R3G3W8G3JC
18	6	348.2	98W8E8W8T1KT
19	7	96.8	D1J1K1N1H1M1
20	7	463.1	9X9J9K9L9M9N
21	7	171.7	7J8Y8T8T8T8T
22	7	273.8	3R3G3W3G3H3J
23	7	219.6	1J3H3J3K3L3M
\.
SELECT setval('cargo_id_seq', 24);

COPY Passenger(id, flight_id, barcode) from stdin;
1	1	jK2vH8bZ7wMr
2	1	QV5H0D5R5O5W
3	1	lE2cQ5aZ8nGk
4	1	pI8yK5eV9zCx
5	2	pX9qH3nZ4jWp
6	2	9Y6T7T6T1K8T
7	2	oV9fH1kZ8eRr
8	3	zK2nG6qH8rJy
9	3	5V5R5D5R5K5W
10	3	qT7eP2yB8lWz
11	4	pM9gC5nY8rNk
12	4	4W9Y9Y7V5L1D
13	4	vK9nX1pH6tTb
14	4	vH7mT2zJ6yQs
15	5	Z4Z4H4G4X9ZD
16	5	lU5zF1jM6gDn
17	5	lX9fP3qY6jWq
18	6	pU5hV1kS6fCz
19	6	98W8E8W8T1KT
20	6	iG4fH5vR1xJg
21	7	oQ9xL5vY2mKf
22	7	cZ1wN5vT8yGf
23	7	1J3H3J3K3L3M
\.
SELECT setval('passenger_id_seq', 24);
-- permissions
CREATE USER admin WITH LOGIN PASSWORD 'password';
GRANT ALL ON DATABASE airport TO admin;
GRANT ALL ON ALL TABLES IN SCHEMA public TO admin;
GRANT ALL ON ALL SEQUENCES IN SCHEMA public TO admin;
