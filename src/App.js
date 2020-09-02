import React, { useState, useReducer, useEffect } from 'react';
import logo from './logo.svg';
import './App.css';
import PrimaryPanel from './primaryPanel.js';
import MapSection from './mapSection.js';


function App() {

	const [currentPoint, setCurrentPoint] = useState(-1);
	const [pathPoints, dispatchPoints] = useReducer(pathPointsReducer, [{name:'', latLon:[null, null]}, {name:'', latLon:[null, null]}])
	const [searchPoint, dispatchSearchPoint] = useReducer(searchPointReducer, {address:'', latLon:[49.7315809334801,13.384550088168409], zoom:13, render:false})

	return (
    	<div className="App">
        	<MapSection 
				currentPoint={currentPoint} setCurrentPoint={setCurrentPoint}
				pathPoints={pathPoints} dispatchPoints={dispatchPoints}
				searchPoint={searchPoint} dispatchSearchPoint={dispatchSearchPoint}
			/>
        	<PrimaryPanel 
				currentPoint={currentPoint} setCurrentPoint={setCurrentPoint}
				pathPoints={pathPoints} dispatchPoints={dispatchPoints}
				searchPoint={searchPoint} dispatchSearchPoint={dispatchSearchPoint}
			/>
    {/*
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>
          Edit <code>src/App.js</code> and save to reload.
        </p>
        <a
          className="App-link"
          href="https://reactjs.org"
          target="_blank"
          rel="noopener noreferrer"
        >
          Learn React
        </a>
      </header>*/}
		</div>
	);	
}

function searchPointReducer(searchPoint, action) {
	switch (action.type) {
		case 'set':
			return {address:action.value, latLon:searchPoint.latLon, zoom:searchPoint.zoom, render:false}
		case 'render':
			// TODO: nominatim find latlong
			// right now we get latlong !
			// var obj = findObject(searchPoint.address);
			// console.log(obj);
			return {address:action.place.display_text, latLon:[action.place.lat, action.place.lon], zoom:16, render:true}
			//var latlon = searchPoint.address.split(',');
			//var tmp = {address:searchPoint.address, latLon:[parseFloat(latlon[0]), parseFloat(latlon[1])], zoom:13, render:true};
			////console.log(tmp);
			//return tmp;
		case 'off':
			return {address:searchPoint.address, latLon:searchPoint.latLon, zoom:searchPoint.zoom, render:false};

	}
}


async function findObject(address) {
	const options = {
		method: 'GET'
	};
	return await fetch('https://nominatim.openstreetmap.org/search?q=' + address + '&format=json', options)
		.then((response) => { console.log("data received"); return response.json();})
		.then((data) =>{console.log ("DD", data); return data;});
}

function pathPointsReducer(pathPoints, action) {
	console.log('state');
	console.log(pathPoints);
	console.log(action);
	let points;
	switch (action.type) {
		case 'insert':
			points = pathPoints.slice();
			points.splice(action.index, 0, action.value);
			return points;
		case 'push':
			return pathPoints.concat([action.value]);
		case 'delete':
			points = pathPoints.slice();
			points.splice(action.index, 1);
			return points;
		case 'update':
			points = pathPoints.slice();
			points[action.index] = action.value;
			return points;
		case 'clear':
			points = pathPoints.slice();
			points[action.index] = {name:'', latLon:[null, null]};
			return points;
	}
}


export default App;
