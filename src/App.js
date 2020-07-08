import React, { useState, useReducer, useEffect } from 'react';
import logo from './logo.svg';
import './App.css';
import PrimaryPanel from './primaryPanel.js';
import MapSection from './mapSection.js';

function App() {

	const [currentPoint, setCurrentPoint] = useState(-1);
	const [pathPoints, dispatchPoints] = useReducer(pathPointsReducer, [{name:'', latLon:[null, null]}, {name:'', latLon:[null, null]}])


	return (
    	<div className="App">
        	<MapSection currentPoint={currentPoint} setCurrentPoint={setCurrentPoint} pathPoints={pathPoints} dispatchPoints={dispatchPoints}/>
        	<PrimaryPanel currentPoint={currentPoint} setCurrentPoint={setCurrentPoint} pathPoints={pathPoints} dispatchPoints={dispatchPoints}/>
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
