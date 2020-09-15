import routing_module
import json
"""
start = content[0]
end = content[1]
res = routing_module.CalculateShortestRoute("czedges", float(start["lon"]), float(start["lat"]), float(end["lon"]), float(end["lat"]))

if (len(content) == 0):
    print([])
route = []
for(i in range(0, len(content) - 1)):
    s = routing_module.CalculateShortestRoute("czedges", float(start["lon"]), float(start["lat"]), float(end["lon"]), float(end["lat"]))
    route.extend(json.loads(s))
"""

"""
res = routing_module.CalculateShortestRoute("czedges", 13.3998825, 49.7230553, 13.3868150, 49.7282850)
res2 = routing_module.CalculateShortestRoute("czedges", 13.3998825, 49.7230553, 13.3868150, 49.7282850)
"""
res = routing_module.CalculateShortestRoute("czedges", 13.391427565774665, 49.72625472231029, 13.393126622053574, 49.725377367144915)
print (json.loads(res))
"""print (json.loads(res).extend(json.loads(res2)))"""