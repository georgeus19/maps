CREATE TABLE ll(osm_id BIGINT NOT NULL, geog geography(LINESTRING) NOT NULL, from_node BIGINT NOT NULL, to_node BIGINT NOT NULL);
COPY ll FROM 'll.csv' DELIMITER ';' CSV HEADER;
ALTER TABLE ll ADD COLUMN length double precision;
UPDATE ll set length = st_length(geog);
CREATE INDEX ll_gix ON ll USING GIST (geog);
