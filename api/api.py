from flask import Flask   
from flask_restplus import Api, Resource
import renderModule
from werkzeug.routing import BaseConverter
from flask import send_file
import os.path

app = Flask(__name__)
api = Api(app)

@api.route('/test')
class Test(Resource):
    def get(self):
        return {'hello': "wrolds"}

@api.route('/maps/<int:z>/<int:x>/<int:y>.png')
class MapFeeder(Resource):
    def get(self, z, x, y):
        tile_path = f"/home/hrubyk/projects/maps/tiles/t{x}_{y}_{z}.png"
        print(x, y, z, tile_path)
        if (os.path.isfile(tile_path) == False):
            renderModule.Render(x, y, z, tile_path) 
        return send_file(tile_path)
        
if __name__ == "__main__" :
    app.run(debug=True)
