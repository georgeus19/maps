import routing_module
import json
"""
start = content[0]
end = content[1]
res = routing_module.CalculateShortestRoute("cz_edges", float(start["lon"]), float(start["lat"]), float(end["lon"]), float(end["lat"]))
"""


res = routing_module.CalculateShortestRoute("cz_edges", 13.3998825, 49.7230553, 13.3868150, 49.7282850)
res2 = routing_module.CalculateShortestRoute("cz_edges", 13.3998825, 49.7230553, 13.3868150, 49.7282850)
print (json.loads(res).extend(json.loads(res2)))