//
// Created by hrubyk on 10.08.20.
//

#ifndef BACKEND_DATABASEHELPER_H
#define BACKEND_DATABASEHELPER_H
#include <string>
#include <string_view>
#include <pqxx/pqxx>
#include <iostream>

class IDatabaseHelper {

};

class DatabaseHelper {
private:
    std::string db_name_;
    std::string user_;
    std::string password_;
    std::string host_address_;
    std::string port_;

    pqxx::connection connection_;

public:
    DatabaseHelper(const std::string & db_name, const std::string & user, const std::string & password,
                   const std::string & host_address, const std::string & port);

    ~DatabaseHelper();

    bool IsDbOpen();
    void CreateGraphTable(const std::string & sql);

    void SearchFor(const std::string & s);

    void FindClosestEdge(double lon, double lat, const std::string & table_name);

    void LoadGraph(double lon, double lat, double radius, const std::string & table_name);

    std::string MakeSTPoint(double lon, double lat);

};

/*
-- "Closest" 100 streets to Broad Street station are?long 13.391480 lat 49.726250   49.7262000N, 13.3915000E
WITH closest_candidates AS (
  SELECT e.osm_id, e.geog
  FROM cz_edges as e
  ORDER BY
    e.geog <-> 'SRID=4326;POINT(13.3915000 49.7262000)'::geography
  LIMIT 100
)
SELECT osm_id, geog
FROM closest_candidates
ORDER BY
  ST_Distance(geog, 'SRID=4326;POINT(13.3915000 49.7262000)'::geography)
LIMIT 1;
 */

/*
select * from cz_edges as e where ST_DWithin('SRID=4326;POINT(13.3915000 49.7262000)'::geography, e.geog, 2000);
*/
#endif //BACKEND_DATABASEHELPER_H