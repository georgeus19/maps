-- CREATE TABLE czedges_green AS SELECT
-- 	edges.uid,
--     sum(
--         ST_Area(ST_Intersection(ST_Buffer(edges.geom, 30), p.way))
--         /
--         ST_Area(ST_Buffer(edges.geom, 30))
--     ) as green_fraction
-- FROM czedges_geom AS edges
-- INNER JOIN planet_osm_polygon AS p ON ST_Intersects(edges.geom, ST_Buffer(p.way,30))
-- WHERE p.natural is not null or p.landuse in ('farmland', 'forest', 'meadow', 'orchard', 'vineyard', 'basin', 'grass', 'village_green')
-- GROUP BY edges.uid
-- select count(*)
-- from czedges
-- ALTER SYSTEM SET work_mem = '256MB';
-- SELECT pg_reload_conf();
-- SHOW work_mem;
-- select *
-- from czedges_green_index
-- limit 100
-- EXPLAIN ANALYZE
-- DROP TABLE IF EXISTS czedges_green_index;
-- DROP TABLE IF EXISTS cz_green_geom;
-- CREATE TEMPORARY TABLE cz_green_geom AS 
-- SELECT p.way
-- FROM planet_osm_polygon AS p
-- WHERE p.natural IN ('wood', 'tree_row', 'tree', 'scrub', 'heath', 'moor', 'grassland', 'fell', 'tundra',
-- 	'bare_rock', 'scree', 'shingle', 'sand', 'mud', 'water', 'wetland', 'glacier', 'bay', 'cape', 'strait', 'beach', 'coastline',
-- 	'reef', 'spring', 'hot_spring', 'geyser', 'peak', 'dune', 'hill', 'volcano', 'valley', 'ridge', 'arete', 'cliff', 'saddle',
-- 	'isthmus', 'peninsula', 'rock', 'stone', 'sinkhole', 'cave_entrance')
-- 	OR p.landuse IN ('farmland', 'forest', 'meadow', 'orchard', 'vineyard', 'basin', 'grass', 'village_green');
-- CREATE INDEX cz_green_geom_gix on cz_green_geom USING GIST (way);
-- CREATE TABLE czedges_green_index AS 
-- SELECT  
-- 	edges.uid,
--     SUM(
--         ST_Area(ST_Intersection(edges.geom, green.way)) / ST_Area(edges.geom)
--     ) AS green_fraction
-- FROM 
-- (
-- 	SELECT uid, ST_Buffer(ST_Transform(geog::geometry, 3857), 30) as geom
-- 	FROM czedges
-- ) AS edges LEFT JOIN cz_green_geom as green ON edges.geom && green.way
-- GROUP BY edges.uid;
-- DROP INDEX IF EXISTS cz_green_geom_gix;
-- CREATE UNIQUE INDEX czedges_green_index_ix ON czedges_green_index (uid);
-- DROP INDEX czedges_green_index_ix;
-- (
-- 	SELECT p.way
-- 	FROM planet_osm_polygon AS p
-- 	WHERE p.natural IN ('wood', 'tree_row', 'tree', 'scrub', 'heath', 'moor', 'grassland', 'fell', 'tundra',
-- 		'bare_rock', 'scree', 'shingle', 'sand', 'mud', 'water', 'wetland', 'glacier', 'bay', 'cape', 'strait', 'beach', 'coastline',
-- 		'reef', 'spring', 'hot_spring', 'geyser', 'peak', 'dune', 'hill', 'volcano', 'valley', 'ridge', 'arete', 'cliff', 'saddle',
-- 		'isthmus', 'peninsula', 'rock', 'stone', 'sinkhole', 'cave_entrance')
-- 		OR p.landuse IN ('farmland', 'forest', 'meadow', 'orchard', 'vineyard', 'basin', 'grass', 'village_green')
-- ) AS green 




-- create index  green_places_gix on green_places using GIST (way);
-- drop table czedges_geom;
-- create table czedges_geom as
-- 	SELECT uid, ST_Buffer(ST_Transform(geog::geometry, 3857), 30) as geom
-- 	FROM czedges
-- create index  czedges_geom_gix on czedges_geom using GIST (geom);

-- SELECT  
-- 	edges.uid,
--     SUM(
--         ST_Area(ST_Intersection(edges.geom, green.way)) / ST_Area(edges.geom)
--     ) AS green_fraction
-- FROM czedges_10000 as edges LEFT JOIN green_places as green ON edges.geom && green.way
-- GROUP BY edges.uid

-- Create table czedges_10000 as
-- SELECT uid, ST_Buffer(ST_Transform(geog::geometry, 3857), 30) as geom
-- FROM czedges
-- limit 10000


-- CREATE TABLE green_places AS
-- SELECT p.way
-- 	FROM planet_osm_polygon AS p
-- 	WHERE p.natural IN ('wood', 'tree_row', 'tree', 'scrub', 'heath', 'moor', 'grassland', 'fell', 'tundra',
-- 		'bare_rock', 'scree', 'shingle', 'sand', 'mud', 'water', 'wetland', 'glacier', 'bay', 'cape', 'strait', 'beach', 'coastline',
-- 		'reef', 'spring', 'hot_spring', 'geyser', 'peak', 'dune', 'hill', 'volcano', 'valley', 'ridge', 'arete', 'cliff', 'saddle',
-- 		'isthmus', 'peninsula', 'rock', 'stone', 'sinkhole', 'cave_entrance')
-- 		OR p.landuse IN ('farmland', 'forest', 'meadow', 'orchard', 'vineyard', 'basin', 'grass', 'village_green')





-- SELECT * 
-- FROM (
-- 	SELECT ST_Buffer(ST_Transform(edges.geog::geometry, 3857), 30)
-- 	FROM czedges
-- ) AS edges


-- create table czedges_geom as
-- select edges.uid, ST_Transform(edges.geog::geometry, 3857) as geom
-- from czedges as edges
-- select count(*) from czwholeways where undirected = true
--  SELECT Find_SRID('public', 'planet_osm_polygon', 'way');






-- WITH edges AS (
-- 	SELECT uid, ST_Buffer(ST_Transform(geog::geometry, 3857), 30) as geom
-- 	FROM czedges
-- 	LIMIT 10000
-- ), green AS (
-- 	SELECT p.way
-- 	FROM planet_osm_polygon AS p
-- 	WHERE p.natural IN ('wood', 'tree_row', 'tree', 'scrub', 'heath', 'moor', 'grassland', 'fell', 'tundra',
-- 		'bare_rock', 'scree', 'shingle', 'sand', 'mud', 'water', 'wetland', 'glacier', 'bay', 'cape', 'strait', 'beach', 'coastline',
-- 		'reef', 'spring', 'hot_spring', 'geyser', 'peak', 'dune', 'hill', 'volcano', 'valley', 'ridge', 'arete', 'cliff', 'saddle',
-- 		'isthmus', 'peninsula', 'rock', 'stone', 'sinkhole', 'cave_entrance')
-- 		OR p.landuse IN ('farmland', 'forest', 'meadow', 'orchard', 'vineyard', 'basin', 'grass', 'village_green')
-- )
-- SELECT  
-- 	edges.uid,
--     SUM(
--         ST_Area(ST_Intersection(edges.geom, green.way)) / ST_Area(edges.geom)
--     ) AS green_fraction
-- FROM edges INNER JOIN green ON ST_Intersects(edges.geom, green.way)
-- GROUP BY edges.uid
