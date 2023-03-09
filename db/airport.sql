-- Initialize the new airport database
SELECT current_database();
CREATE DATABASE airport;

\c airport
SELECT current_database();

-- Domain Tables

-- Airline Domain
CREATE DOMAIN airline_name_domain AS VARCHAR(40) 
	CHECK (
		VALUE IN ('Alaskan Airlines', 'Spirit Airlines', 'United Airlines', 'Delta Airlines', 'Turkish Airlines', 'Qatar Airways', 'Korean Air')
	);

-- Terminal Domain
CREATE DOMAIN terminal_letter_domain AS CHAR(1) 
	CHECK (
		VALUE IN ('A', 'B', 'C')
	);

-- MealType Domain
CREATE DOMAIN meal_name_domain AS TEXT 
	CHECK (
		VALUE IN ('Steak Burger', ' Veggie Burger', 'Fish Tacos', 'Strip Steak', 'Pasta')
	);

-- Status Domain
CREATE DOMAIN status_info_domain AS VARCHAR(40) 
	CHECK (
		VALUE IN ('Delayed', 'In Transit', 'Cancelled', 'Ready')
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


-- Databse Schema 

-- MealCategoryType Table
CREATE TABLE MealCategoryType (
	id				INTEGER not null,
	category		meal_category_domain not null,
		
	Primary Key		(id)
);

-- MealType Table
CREATE TABLE MealType (
	id			INTEGER not null,
	meal_name 	meal_name_domain not null,

	Primary Key	(id)
);

--	MealToCategory Table
CREATE TABLE MealToCategory (
	meal_id			INTEGER not null,
	category_id		INTEGER not null,
		
	Primary Key		(meal_id, category_id), 
	Foreign Key 	(meal_id)		references MealType(id) DEFERRABLE INITIALLY DEFERRED,
	Foreign Key		(category_id)	references MealCategoryType(id) DEFERRABLE INITIALLY DEFERRED
);

-- StatusType Table
CREATE TABLE StatusType (
	id				INTEGER not null,
	status_info		status_info_domain not null unique,

	Primary Key		(id)
);

--	Terminal Table
CREATE TABLE TerminalType (
	id			INTEGER not null,
	letter		terminal_letter_domain not null,

	Primary Key		(id)
);

--	Gate Table
CREATE TABLE GateType (
	id				INTEGER not null,
	terminal_id		INTEGER not null unique,
	gate_number		INTEGER not null,

	Primary Key		(id), 
	Foreign Key 	(terminal_id) references TerminalType(id) DEFERRABLE INITIALLY DEFERRED
);


-- AirplaneType Table
CREATE TABLE AirplaneType (
	id				INTEGER not null,
	airplane_name	VARCHAR(40) not null,
	model			VARCHAR(40) not null unique,
	max_cargo 		NUMERIC not null,
	max_passengers	INTEGER not null,
	num_rows		INTEGER not null,

	Primary Key		(id)
);

-- LocationType Table 
CREATE TABLE LocationType (
	id				INTEGER not null,
	country_name	VARCHAR(40) not null,
	state_name		VARCHAR(40) not null,
	city_name 		VARCHAR(40) not null,
	icao 			icao_domain not null,

	Primary Key		(id)
);

-- AirlineType Table
CREATE TABLE AirlineType (
	id				INTEGER not null,
	airline_name	airline_name_domain not null unique,

	Primary Key		(id)
);

--	Flight Table
CREATE TABLE Flight (
	id				INTEGER not null,
	flight_number	INTEGER not null,
	departure_time	TIMESTAMP not null,
	arrival_time	TIMESTAMP not null,
	num_passengers	INTEGER not null,
	gate_id 		INTEGER not null,
	status_id		INTEGER not null,
	airplane_id 	INTEGER not null,
	destination_id	INTEGER not null,
	origin_id		INTEGER not null,
	airline_id		INTEGER not null,
	
	Primary Key		(id),
	Foreign Key 	(gate_id) 			references GateType(id) DEFERRABLE INITIALLY DEFERRED,
	Foreign Key 	(status_id) 		references StatusType(id) DEFERRABLE INITIALLY DEFERRED,  -- "status" is a keyword in psql
	Foreign Key 	(airplane_id) 		references AirplaneType(id) DEFERRABLE INITIALLY DEFERRED,
	Foreign Key 	(destination_id)	references LocationType(id) DEFERRABLE INITIALLY DEFERRED,
	Foreign Key 	(origin_id) 		references LocationType(id) DEFERRABLE INITIALLY DEFERRED,
	Foreign Key 	(airline_id) 		references AirlineType(id) DEFERRABLE INITIALLY DEFERRED

);

--	Cargo Table
CREATE TABLE Cargo (
	id				INTEGER not null,
	flight_id		INTEGER not null,
	weight_lb		NUMERIC not null,				
	
	Primary Key		(id),
	Foreign Key 	(flight_id)		references Flight(id) DEFERRABLE INITIALLY DEFERRED
);

--	MealToAirline Table
CREATE TABLE MealToAirline (
	flight_id		INTEGER not null,
	meal_id 		INTEGER not null,
	
	Primary Key		(flight_id, meal_id), 
	Foreign Key 	(flight_id) references Flight(id) DEFERRABLE INITIALLY DEFERRED,
	Foreign Key 	(meal_id) 	references MealType(id) DEFERRABLE INITIALLY DEFERRED
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



-- CRUD operations for domain tables

-- Delete Database 
--\c postgres
--SELECT current_database();
--DROP DATABASE airport;
