import React, { useState, useEffect } from 'react';
import './mapSection.css';
import { Map, Marker, Popup, TileLayer, GeoJSON, GeoJSONProps } from 'react-leaflet' 
import L from 'leaflet'   
import 'leaflet/dist/leaflet.css';
import marker from './marker.svg'
const TabEnum = Object.freeze({"searchTab":1, "routeTab":2, "exportTab":3})

/**
 * Icon of point marker that can be printed in the map.
 */
var markerIcon   = L.icon({
    iconUrl: marker,
    iconSize:     [30, 30], // size of the icon.
    iconAnchor:   [15, 30], // point of the icon which will correspond to marker's location.
});

/**
 * `MapSection` contains a container for the map. 
 * Forwards all `props` to `MapContainer`.
 * @param {*} props Properties.
 */
function MapSection(props) {
    return <div className="MapSection">
        <MapContainer currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
            pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}
            searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
            route={props.route} 
            currentTab={props.currentTab}
            />
    </div>
}

/**
 * `MapContainer` contains the map element.
 * It handles all interaction with the map - creating point markers,
 * printing route in the map, changing map views.
 * @param {*} props Properties
 */
function MapContainer(props) {
    /**
     * Represents current view of the map editor. It is used to change the view of the
     * map editor to have newly created point in the center of the map editor.
     */
    const [viewport, setViewport] = useState({center:[49.7315809334801,13.384550088168409], zoom:13});

    /**
     * Represents marker element for search point from the search tab.
     */
    const [searchMarker, setSearchMarker] = useState(null);

    /**
     * Represents all markers for user-defined route points.
     */
    const [routeMarkers, setRouteMarkers] = useState([]);

    /**
     * Represent a rectangle in the map that contains all
     * route points and the whole route. When a route is found
     * it is set and map editor shows the area of the rectangle.
     */
    const [bounds, setBounds] = useState(null);
        
    /**
     * When `props.searchPoint` changes
     * create and set a correspoding marker and 
     * set a current map editor view (so that the point is in the center).
     */
    useEffect(() => {
        if (props.searchPoint.render === true) {
            setViewport({center:props.searchPoint.latLon, zoom:props.searchPoint.zoom});
            setSearchMarker(props.searchPoint.latLon);
            props.dispatchSearchPoint({type:'off'});
        }
    }, [props.searchPoint]);

    /**
     * When `props.pathPoints` are changed
     * create and set new markers and update 
     * map editor view by setting `viewport` or `bounds`.
     */
    useEffect(() => {
        if (props.currentTab !== TabEnum.routeTab) {
            return;
        }
        const validPoints = props.pathPoints.filter((point) => {
            return isValidPoint(point);
        })
        const coordinates = validPoints.map((point) => {
            return point.latLon;
        })

        if (coordinates.length === 0) {

        } else if (coordinates.length === 1) {
            setViewport({center:coordinates[0] , zoom:16})
        } else {
            setBounds(calculateMaxBounds(coordinates));
        }

        setRouteMarkers(validPoints.map((point) => {
            return printMarker(point.latLon, point.name);
        }))        
    }, [props.pathPoints])

    /**
     * Checks in `point` is a valid point - has latitude and longitude.
     * @param {Point object} point 
     */
    function isValidPoint(point) {
        if (point === null) {
            return false;
        }    
        if (point.latLon.includes(null)) {
            return false;
        }
        return true;
    }

    /**
     * Calculate a rectangle that contains all coordinates.
     * @param {Array of pairs} coordinates [[lat, lon], ...]
     */
    function calculateMaxBounds(coordinates) {
        let latmin = 1000;
        let latmax = -1000;
        let lonmin = 1000;
        let lonmax = -1000;
        coordinates.forEach(coordinate => {
            const lat = coordinate[0]; 
            if (lat < latmin) {latmin = lat;}
            if (lat > latmax) {latmax = lat;}
            const lon = coordinate[1]; 
            if (lon < lonmin) {lonmin = lon;}
            if (lon > lonmax) {lonmax = lon;}
        });
        return [[latmin, lonmin], [latmax, lonmax]];

    }

    /**
     * Create a marker element with predefined `markerIcon`. 
     * @param {pair} latLon [lat, lon]
     * @param {*} key 
     */
    function printMarker(latLon, key) {
        const icon = L.icon({
            iconUrl: markerIcon,
        
            iconSize:     [38, 95], // size of the icon
            shadowSize:   [50, 64], // size of the shadow
            iconAnchor:   [22, 94], // point of the icon which will correspond to marker's location
            shadowAnchor: [4, 62],  // the same for the shadow
            popupAnchor:  [-3, -76] // point from which the popup should open relative to the iconAnchor
        });
        return (
            <Marker position={latLon} icon={markerIcon} key={key}>

            </Marker>
        );
    }

    /**
     * If `props.currentPoint` is set
     * find address of location specified by parameters.
     * @param {double} lon 
     * @param {double} lat 
     */
    function setPoint(lon, lat) {
        if (props.currentPoint === -1) {
            return;
        }

        console.log("setPoint: ", props.currentPoint);
        const options = {
            method: 'GET'
        };
        fetch('https://nominatim.openstreetmap.org/reverse?lat=' + lat + '&lon=' + lon + '&format=json', options)
                .then((response) => { console.log("DATA FETCHED"); return response.json();})
                .then((data) => {
                    console.log(data); 
                    let adr;
                    if (data.display_name) {
                        adr = data.display_name;
                    } else if (data.length > 0) {
                        adr = data[0].display_name;
                    } else {
                        adr = lat + 'N, ' + lon + 'E';
                    }
                    props.dispatchPoints({type:'update', value:{name:adr, latLon:[lat, lon]}, index:props.currentPoint})
                    props.setCurrentPoint(-1);
                })
                .catch((error) => {
                    console.warn('Error occured with respect to searching for address based on latlng.', error);
                });
    }

    /**
     * Handle clicking in the map editor.
     * @param {Click event} e 
     */
    function handleClick(e) {
        setPoint(e.latlng.lng, e.latlng.lat);
    }

    return (
       /*/  Map using tiles from osm server.
        <Map className="Map" center={[49.7315809334801,13.384550088168409]} zoom={13} onclick={(e) => handleClick(e)}>
            <TileLayer
                url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
                attribution="&copy; <a href=&quot;http://osm.org/copyright&quot;>OpenStreetMap</a> contributors"
            />
        </Map>
        /*/
        <Map className="Map" bounds={bounds}  onViewportChanged={(v) => {setViewport(v)}} viewport={viewport} onclick={(e) => handleClick(e)}>
            <TileLayer
                url="http://127.0.0.1/hot/{z}/{x}/{y}.png" // Address of tiles on our local server.
                attribution="&copy; <a href=&quot;http://osm.org/copyright&quot;>OpenStreetMap</a> contributors"
            /> 
            {props.currentTab === TabEnum.searchTab && searchMarker && printMarker(searchMarker, -1)}
            {props.currentTab === TabEnum.routeTab && routeMarkers}
            {props.currentTab === TabEnum.routeTab && <GeoJSON data={props.route} key={(props.route)}></GeoJSON>}
        </Map>
    );  
  }

  export default MapSection;