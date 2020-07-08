import React, { useState, useEffect } from 'react';
import logo from './logo.svg';
import './mapSection.css';
import { Map, Marker, Popup, TileLayer } from 'react-leaflet' 
import L from 'leaflet'   


var markerIcon = L.icon({
    iconUrl: require('./marker.svg'),
    iconSize:     [30, 30], // size of the icon
    iconAnchor:   [15, 30], // point of the icon which will correspond to marker's location
});

function MapSection(props) {
    return <div className="MapSection">
        <MapContainer currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
         pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}/>
    </div>
  }
  
  function MapContainer(props) {
    const position = [49.7315809334801, 13.384550088168409];

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
        <Map className="Map" center={position} zoom={13} onclick={(e) => handleClick(e)}>
            <TileLayer
                url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
                attribution="&copy; <a href=&quot;http://osm.org/copyright&quot;>OpenStreetMap</a> contributors"
            />
        </Map>
    );
  }

  export default MapSection;