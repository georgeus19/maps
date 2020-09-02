import React, { useState, useEffect } from 'react';
import logo from './logo.svg';
import './mapSection.css';
import { Map, Marker, Popup, TileLayer } from 'react-leaflet' 
import L from 'leaflet'   
import 'leaflet/dist/leaflet.css';
import StateManager from 'react-select';
import marker from './marker.svg'


var markerIcon   = L.icon({
    iconUrl: marker,
    iconSize:     [30, 30], // size of the icon
    iconAnchor:   [15, 30], // point of the icon which will correspond to marker's location
});

function MapSection(props) {
    return <div className="MapSection">
        <MapContainer currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
         pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}
         searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
         />
    </div>
  }
  
  function MapContainer(props) {
    const [viewport, setViewport] = useState({center:[49.7315809334801,13.384550088168409], zoom:13});
    const [searchMarker, setSearchMarker] = useState(null);
    
    useEffect(() => {
        if (props.searchPoint.render === true) {
            setViewport({center:props.searchPoint.latLon, zoom:props.searchPoint.zoom});
            setSearchMarker(props.searchPoint.latLon);
            props.dispatchSearchPoint({type:'off'});
        }
    });
 
    function printMarker(latLon) {
        const icon = L.icon({
            iconUrl: markerIcon,
        
            iconSize:     [38, 95], // size of the icon
            shadowSize:   [50, 64], // size of the shadow
            iconAnchor:   [22, 94], // point of the icon which will correspond to marker's location
            shadowAnchor: [4, 62],  // the same for the shadow
            popupAnchor:  [-3, -76] // point from which the popup should open relative to the iconAnchor
        });
        return (
            <Marker position={latLon} icon={markerIcon}>

            </Marker>
        );
    }

    function handleClick(e) {
        console.log(props.currentPoint);
        if (props.currentPoint !== -1) {
            // fetch data about the point and put them in value: {name:'...'}
            props.dispatchPoints({type:'update', value:{name:'FromMap', latLon:e.latlng}, index:props.currentPoint})
            console.log(props.pathPoints);
        }
    }
   /*
        let markersJsx = [];
        for(let i = 0; i < markers.length; i < 0) {
            markersJsx.push(<Marker position={markers[i]}></Marker>)
        }
        console.log(markersJsx);
*/
    return (
       /*/  
        <Map className="Map" center={[49.7315809334801,13.384550088168409]} zoom={13} onclick={(e) => handleClick(e)}>
            <TileLayer
                url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
                attribution="&copy; <a href=&quot;http://osm.org/copyright&quot;>OpenStreetMap</a> contributors"
            />
        </Map>
        /*/
        <Map className="Map" onViewportChanged={(v) => {setViewport(v)}} viewport={viewport} onclick={(e) => handleClick(e)}>
            <TileLayer
                url="http://127.0.0.1/hot/{z}/{x}/{y}.png"
                attribution="&copy; <a href=&quot;http://osm.org/copyright&quot;>OpenStreetMap</a> contributors"
            />
            {searchMarker && printMarker(searchMarker)}
        </Map>
/**/
    );  
  }

  export default MapSection;