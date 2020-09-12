import React, { useState, useEffect, useCallback } from 'react';
import './primaryPanel.css';
import SearchInput from './searchInput.js'
import Button from 'react-bootstrap/Button';
import FormControl from 'react-bootstrap/FormControl'
import 'bootstrap/dist/js/bootstrap.bundle.min.js'
import 'bootstrap/dist/css/bootstrap.min.css';
import { Camera, Search, PlusSquare, PlusCircle, Trash2 } from 'react-feather';

const TabEnum = Object.freeze({"searchTab":1, "routeTab":2, "exportTab":3})

/**
 * Component `PrimaryPanel` contains a header, a body (=tab) and a footer.
 * Only one tab can be visible at a time. Tabs contain main logic.
 * `Header` only provides functionality for switching between tabs.
 * @param {*} props 
 */
function PrimaryPanel(props) {
    let tab;

    if (props.currentTab === TabEnum.searchTab) {
        tab = <SearchTab
                currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
        ></SearchTab>;
    } else if (props.currentTab === TabEnum.routeTab) {
        tab = <RoutingTab 
            currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
            pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}
            route={props.route} setRoute={props.setRoute}
        ></RoutingTab>;
    } else if (props.currentTab === TabEnum.exportTab) {
        tab = <ExportTab setCurrentPoint={props.setCurrentPoint}></ExportTab>;
    }

    return (    
        <div className="PrimaryPanel">
            <Header setCurrentPoint={props.setCurrentPoint} setTab={(tab) => props.setCurrentTab(tab)}/>
            {tab} 
            <Footer/>
        </div>
    );
}

/**
 * Component `Header` provides functionality for switching between tabs by having
 * a button for each tab.
 * @param {*} props 
 */
function Header(props) {
    return (
        <div className="Header" onClick={() => {} /* props.setCurrentPoint(-1) */ }>
            <Button className="HeaderOption" onClick={() => props.setTab(TabEnum.searchTab)}>Search</Button>
            <Button className="HeaderOption" onClick={() => props.setTab(TabEnum.routeTab)}>Route</Button>
            <Button className="HeaderOption" onClick={() => props.setTab(TabEnum.exportTab)}>Import/ Export</Button>
        </div>
    )
}

/**
 * Component `RoutingTab` consists of two containers. 
 * Former is `PointContainer` that lets user define a route.
 * Latter is `ConstraintContainer` that lets user define special constraints for the route (e.g. max altitude).
 * @param {*} props 
 */
function RoutingTab(props) {
    return (
        <div className="Tab">
            <PointContainer 
                currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
                pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}
                route={props.route} setRoute={props.setRoute}
            ></PointContainer>
            <ConstraintContainer
                currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
            ></ConstraintContainer>
        </div>
    );
}

/**
 * Component `PointContainer` provides functionality for defining a route.
 * User can fill in selects/ inputs to search for a point (startpoint, endpoint).
 * User can add more points that make a route.
 * @param {*} props 
 */
function PointContainer(props) {
    /**
     * Sends a request to server to calculate the best route based on `points`.
     * @param {Array of pairs} points [[lon, lat], ...] 
     */
    function findRoute(points) {
        fetch('/test', {
            method: 'POST',
            body: JSON.stringify(points),
            headers: {
                'Content-type': 'application/json; charset=UTF-8'
            }
        })
        .then( (response) => {
            if (response.ok) {
                return response.json();
            }
            return Promise.reject(response);
        })
        .then((route) => {
            props.setRoute(route);
            console.log("fetched route: ", route);
        })
        .catch((error) => {
            console.warn('Error occured with respect to routing.', error);
        });

    }

    /**
     * If there are atleast two path nodes selected
     * then calculate the best route.
     */
    useEffect(() => {
        const coordinates = props.pathPoints.filter((point) => {
            if (point === null) {
                return false;
            }    
            if (point.latLon.includes(null)) {
                return false;
            }
            return true;
        }).map((point) => {
            // Server expects lon, lat format.
            return {lon:point.latLon[1], lat:point.latLon[0]};
        });
        console.log("coordinates: ", coordinates);
        if (coordinates.length > 1) {
            // Fetch data from server and set route.
            findRoute(coordinates);
        } else {
            // Clear route.
            props.setRoute([]);
        }

    }, [props.pathPoints])

    // Create an array of `PathPoint` where between all adjacent ones is placed AddPoint.
    // -> `PathPoint`,`AddPoint`,`PathPoint`, ...
    let points=[];  
    props.pathPoints.forEach((point, index) => {
        points.push(<PathPoint key={index} currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint} 
        dispatchPoints={props.dispatchPoints} index={index} pointName={point.name} pointCount={props.pathPoints.length}></PathPoint>);
        if (index !== props.pathPoints.length - 1) {
            points.push(<AddPoint key={"plus" + index} currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint} 
            dispatchPoints={props.dispatchPoints} nextPointIndex={index + 1}></AddPoint>);
        }
    })
    return (
        <div className="PointContainer" >
            <p>Select path points:</p>
            {points}
        </div>
            
        
    );
}

/**
 * 
 * @param {*} props 
 */
function PathPoint(props) {
    const [selectedPlace, setSelectedPlace] = useState(null);
    const [text, setText] = useState('');
    function showOnMap(place) {}

    function handleInputChange(place) {
        console.log('PLACE: ', place);
        //setText(place.label);   
        props.setCurrentPoint(props.index);
        props.dispatchPoints({type:'update', value:{name:place.label, latLon:[place.value.lat, place.value.lon]}, index:props.index})
    }

    useEffect(() => {
        setText(props.pointName);
    }, [props.pointName])

// <FormControl 
//              type="text" placeholder="Select a path point..." value={props.pointName}
//              onChange={handleInputChange} onFocus={() => {console.log("input focused!! " + props.index); return props.setCurrentPoint(props.index)}
//              }
//              ></FormControl>
// <Button onFocus={() => props.setCurrentPoint(-1)} onClick={(e) => props.dispatchPoints({type:'clear', index:props.index})}>x</Button>
    return(
        <div className="PathPoint">
            <SearchInput 
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
                selectedPlace={props.pointName} setSelectedPlace={(place) => handleInputChange(place)}
                showOnMap={(place) => showOnMap(place)}
                text={text} setText={setText}
                handleClick={() => props.setCurrentPoint(props.index)}
            >
                  {props.pointCount > 2 && <Button onFocus={() => {} /*props.setCurrentPoint(-1) */} onClick={(e) => props.dispatchPoints({type:'delete', index:props.index})}>
                  <Trash2/>
                  </Button>}
            
            </SearchInput>
        </div>
    );
}


function AddPoint(props) {

    return(
        <div className="AddPoint" onClick={() => {} /* props.setCurrentPoint(-1) */}>
            <Button  onClick={() => props.dispatchPoints({type:'insert', value:{name:'', latLon:[null, null]}, index:props.nextPointIndex})}><PlusSquare/></Button>
        </div>  

    );
}

function ConstraintContainer(props) {
    return (
        <div className="ConstraintContainer" onFocus={() => {} /* props.setCurrentPoint(-1) */}>
            <Constraint></Constraint>
            <Constraint></Constraint>
        </div>
    );
}

function Constraint(props) {
    return (
        <div className="Constraint">
            <Button>constraint placeholder...</Button>
        </div>
    );
}

function Footer(props) {
    return null;
}

function SearchTab(props) {
    return (
        <div className="Tab">
            <SearchContainer
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
            ></SearchContainer>
        </div>
    );
}

function SearchContainer(props) {
    const [selectedPlace, setSelectedPlace] = useState(null);
    const [text, setText] = useState(props.searchPoint.address);
    
    useEffect(() => {
        console.log("saerchpoint address: ", props.searchPoint.address);
        setText(props.searchPoint.address);
    }, [props.searchPoint.address])

    function showOnMap(place) {
        //console.log("Selected place ", place);
        props.dispatchSearchPoint({type:'render', place:place.value});
    }
    return (
        <div className="SearchContainer">
            <p>Search map:</p>
            <SearchInput
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
                selectedPlace={selectedPlace} setSelectedPlace={setSelectedPlace}
                showOnMap={(place) => showOnMap(place)}
                text={text} setText={setText}
                handleClick={() => {}}
            ></SearchInput>
  
        </div>
    );
}


function ExportTab(props) {

    return (
        <div className="Tab">
            <ExportContainer/>
            <ImportContainer/>
        </div>
    );
}

function ImportContainer(props) {
    return (
        <div className="ExportContainer">
            <p>Import path:</p>
            <Import></Import>

        </div>
    );
}

function Import(props) {
    return (
        <div>
            <FormControl></FormControl>
        </div>
    );
}

function ExportContainer(props) {
    return (
        <div className="ExportContainer"> 
            <p>Export path:</p>
            <Export></Export>
        </div>
    );
}

function Export(props) {
    return (
        <div>
            <Button>Export</Button>
        </div>
    );
}
//    function GetData() {
//        
//        const options = {
//            method: 'GET'
//        };
//        fetch('/maps/12/2197/1392.png', options).then((response) => { console.log("data received"); return response.json()}).then((data) => setTemp(data.hello));
//    }
//<FormControl value={props.searchPoint.address} onChange={(e) => props.dispatchSearchPoint({type:'set', value:e.target.value})}></FormControl>
function useRefresh() {
    const [state, refresh] = useState(0);
    return [
      state,
      useCallback(() => {
        refresh(state + 1);
      }, [state]),
    ];
  } 

export default PrimaryPanel;


