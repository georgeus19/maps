import React, { useState, useReducer, useEffect } from 'react';
import './App.css';
import PrimaryPanel from './primaryPanel.js';
import MapSection from './mapSection.js';
const TabEnum = Object.freeze({"searchTab":1, "routeTab":2, "exportTab":3})

/**
 * Component App is an envelope for the whole web application. 
 * It splits the app into two separate parts - Map section and Primary panel.
 * Map section handles map editor and all interaction with the map.
 * Primary panel contains search, routing, export tabs.
 * 
 * App also holds states that are used by both parts. 
 */
function App() {
	/**
	 * Represents current route point. When a select of a route point is focused
	 * this state is set to the index of the route point. It is used for setting
	 * the corresponding route point to a location in map by clicking on the
	 * location in the map.
	 */
	const [currentPoint, setCurrentPoint] = useState(-1);

	/**
	 * Array of route/ path points that are set by user. Each point has its own select/input.
	 * Initial state is 2 null points so that user can see two inputs where to write 
	 * start and end location of his planned route. More points can be added by user.
	 */
	const [pathPoints, dispatchPoints] = useReducer(pathPointsReducer, [{name:'', latLon:[null, null]}, {name:'', latLon:[null, null]}]);

	/**
	 * A point not used in routing section but in a search section.
	 * Represents value that user puts in an input in search tab.
	 */
	const [searchPoint, dispatchSearchPoint] = useReducer(searchPointReducer, {address:'', latLon:[49.7315809334801,13.384550088168409], zoom:13, render:false});

	/**
	 * Array of geoJson features that represent route that goes from selected start point to end point.
	 */
	const [route, setRoute] = useState([]);

	/**
	 * Indicates which tab is currently open in Primary panel.
	 */
	const [currentTab, setCurrentTab] = useState(TabEnum.routeTab); // route, search, export

	return (
    	<div className="App">
        	<MapSection 
				currentPoint={currentPoint} setCurrentPoint={setCurrentPoint}
				pathPoints={pathPoints} dispatchPoints={dispatchPoints}
				searchPoint={searchPoint} dispatchSearchPoint={dispatchSearchPoint}
				route={route}
				currentTab={currentTab}
			/>
        	<PrimaryPanel 
				currentPoint={currentPoint} setCurrentPoint={setCurrentPoint}
				pathPoints={pathPoints} dispatchPoints={dispatchPoints}
				searchPoint={searchPoint} dispatchSearchPoint={dispatchSearchPoint}
				route={route} setRoute={setRoute}
				currentTab={currentTab} setCurrentTab={setCurrentTab}
			/>
		</div>
	);	
}

/**
 * Reducer for state `App.searchPoint` - handles all possible types of modification of the state value.
 * @param {point object} searchPoint Old value of the state `App.searchPoint`. 
 * @param {action object} action Object containing new values for the state and type of action.
 */
function searchPointReducer(searchPoint, action) {
	switch (action.type) {
		// case 'set':
			// Seems useless!
			// return {address:action.value, latLon:searchPoint.latLon, zoom:searchPoint.zoom, render:false}
		case 'render':
			// Set a new value to `App.searchPoint` state and view in the map.
			return {address:action.place.display_text, latLon:[action.place.lat, action.place.lon], zoom:16, render:true};
		case 'off':
			// Stop re-rendering in the map.
			return {address:searchPoint.address, latLon:searchPoint.latLon, zoom:searchPoint.zoom, render:false};
	}
}

/**
 * Reducer for state `App.pathPoints` - handles all possible types of modification of the state value.
 * @param {Array of point object} pathPoints Old value of the state `App.pathPoints`.
 * @param {action object} action Object containing new values for the state and type of action.
 */
function pathPointsReducer(pathPoints, action) {
	let points;
	switch (action.type) {
		case 'insert':
			// Insert a new point(=`action.value`) to the state at index `action.index`.
			points = pathPoints.slice();
			points.splice(action.index, 0, action.value);
			console.log("index: ", action.index);
			console.log ("new points: ", points);
			return points;
		case 'push':
			// Insert a point(=`action.value`) to the end of the state.
			return pathPoints.concat([action.value]);
		case 'delete':
			points = pathPoints.slice();
			points.splice(action.index, 1);
			return points;
		case 'update':
			// Update value of a point at `action.index` in the state.
			points = pathPoints.slice();
			points[action.index] = action.value;
			return points;
		case 'clear':
			// Delete a point at `action.index` in the state.
			points = pathPoints.slice();
			points[action.index] = {name:'', latLon:[null, null]};
			return points;
	}
}

export default App;