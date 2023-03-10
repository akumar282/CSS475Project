SELECT name
FROM flight
    JOIN airplanestatustype ON (flight.status_id = airplanestatustype.id)
WHERE flight_number = ''
;