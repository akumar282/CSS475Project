-- Initialize the new airport database
SELECT current_database();
CREATE DATABASE airport;

\c airport
SELECT current_database();

-- MealCategoryType Table
CREATE TABLE MealToCategoryType (
	id				INTEGER not null,
	category		VARCHAR(20) not null,
		
	Primary Key		(id)
);

-- MealType Table
CREATE TABLE MealType (
	id			INTEGER not null,
	meal_name 	TEXT not null,

	Primary Key	(id)
);


-- StatusType Table
CREATE TABLE StatusType (
	id				INTEGER not null,
	status_info		VARCHAR(40) not null,

	Primary Key		(id)
);

--	Terminal Table
CREATE TABLE TerminalType (
	id			INTEGER not null,
	letter		CHAR(1) not null,

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
	icao 			CHAR(4)	not null,

	Primary Key		(id)
);

-- AirlineType Table
CREATE TABLE AirlineType (
	id				INTEGER not null,
	airline_name	VARCHAR(40) not null,

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
	flight_id		INTEGER not null unique,
	weight_lb		NUMERIC not null,				
	
	Primary Key		(id),
	Foreign Key 	(flight_id)		references Flight(id) DEFERRABLE INITIALLY DEFERRED
);

--	MealToAirline Table
CREATE TABLE MealToAirline (
	flight_id		INTEGER not null,
	meal_id 		INTEGER not null,
	
	Primary Key		(flight_id, meal_id), 
	Foreign Key 	(flight_id) 	references Flight(id) DEFERRABLE INITIALLY DEFERRED,
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

-- Delete Database 
-- \c postgres
-- SELECT current_database();
-- DROP DATABASE airport;
