from mapnik import PostGIS, Layer
params = dict(dbname='gis',table='osm',user='postgres',password='wtz2trln')
params['estimate_extent'] = False
params['extent'] = '-20037508,-19929239,20037508,19929239'
postgis = PostGIS(**params)
lyr = Layer('PostGIS Layer')
lyr.datasource = postgis


