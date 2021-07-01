DROP INDEX IF EXISTS czedges_green_index_idx;
DROP TABLE IF EXISTS czedges_green_index;
CREATE TABLE czedges_green_index AS
SELECT 	edges.uid,
	SUM(         ST_Area(ST_Intersection(edges.geom, green.way)) / ST_Area(edges.geom)     ) AS green_value
FROM  ( 	
	SELECT uid, ST_Buffer(ST_Transform(geog::geometry, 3857), 30) as geom 	
	FROM czedges
	limit 100 ) AS edges LEFT JOIN 
( 	SELECT p.way 	
	FROM planet_osm_polygon AS p 	
	WHERE p.natural IN ('wood', 'tree_row', 'tree', 'scrub', 'heath', 'moor', 'grassland', 'fell', 'tundra',
						'bare_rock', 'scree', 'shingle', 'sand', 'mud', 'water', 'wetland', 'glacier', 'bay', 'cape', 'strait', 'beach', 'coastline',
						'reef', 'spring', 'hot_spring', 'geyser', 'peak', 'dune', 'hill', 'volcano', 'valley', 'ridge', 'arete', 'cliff', 'saddle',
						'isthmus', 'peninsula', 'rock', 'stone', 'sinkhole', 'cave_entrance')
 		OR p.landuse IN ('farmland', 'forest', 'meadow', 'orchard', 'vineyard', 'basin', 'grass', 'plant_nursery', 'village_green',
				  'recreation_ground', 'allotments', 'cemetery')
 		OR p.leisure in ('disc_golf_course', 'dog_park', 'golf_course', 'garden', 'park', 'pitch', 'beach_resort')
) as green ON edges.geom && green.way GROUP BY edges.uid;
CREATE UNIQUE INDEX czedges_green_index_idx ON czedges_green_index (uid); 
