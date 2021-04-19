from flask import Flask, request, jsonify
from flask_restplus import Api, Resource
import renderModule
import routing_module
from werkzeug.routing import BaseConverter
from flask import render_template
from flask import send_file, send_from_directory
import os.path
import json

app = Flask(__name__, static_folder='../build/', static_url_path='/')
api = Api(app)

@api.route('/route')
class Test(Resource):
    def post(self):
        content = request.json
        route = []
        if len(content) > 1:
            for i in range(0, len(content) - 1):
                start = content[i]
                end = content[i + 1]
                s = routing_module.CalculateShortestRoute("CHcznoloops", float(start["lon"]), float(start["lat"]), float(end["lon"]), float(end["lat"]))
                if s[0] == 'E' and s[1] == 'R':
                    return []
                route.extend(json.loads(s))
        print(route)
        return route


# @app.route('/main')
# def index():
    # return app.send_static_file('index.html')
@api.route('/main')
class Main(Resource):
    def get(self):
        return send_file("/home/hrubyk/projects/maps/build/index.html")
        # return app.send_static_file("index.html")

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
