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
t { lon: 15.568861848533453, lat: 50.66747073900733 }
​
1: Object { lon: 15.581821585773252, lat: 50.67989977147217 }
"""
res = routing_module.CalculateShortestRoute("cznoloops", 15.568861848533453, 50.66747073900733, 15.581821585773252,  50.67989977147217)
print(res)
print (json.loads(res))
"""print (json.loads(res).extend(json.loads(res2)))"""
