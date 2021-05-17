import React, { useState, useEffect, useReducer } from 'react';
import './primaryPanel.css';
import SearchInput from './searchInput.js'
import Button from 'react-bootstrap/Button';
import FormControl from 'react-bootstrap/FormControl'
import Form from 'react-bootstrap/Form'
import 'bootstrap/dist/js/bootstrap.bundle.min.js'
import 'bootstrap/dist/css/bootstrap.min.css';
import { Camera, Search, PlusSquare, PlusCircle, Trash2 } from 'react-feather';
import { point } from 'leaflet';
import { Range } from 'react-range';

const TabEnum = Object.freeze({"searchTab":1, "routeTab":2, "exportTab":3})

function profileReducer(profile, action) {
    const actions = new Map([
        ['set', (p) => {
            return p;
        }],
        ['update', (new_property) => {
            return profile.map((prop) => {
                if (prop.name === new_property.name) {
                    return new_property;
                } else {
                    return prop;
                }
            })
        }]    
    ]);

    if (actions.has(action.type)) {
        return actions.get(action.type)(action.value);
    } else {
        console.error("Action for profile not recognized.", action);
        return profile;
    }
}



/**
 * Component `PrimaryPanel` contains a header, a body (=tab) and a footer.
 * Only one tab can be visible at a time. Tabs contain main logic.
 * `Header` only provides functionality for switching between tabs.
 * @param {*} props 
 */
function PrimaryPanel(props) {
    let tab;

    const [profile, dispatchProfile] = useReducer(profileReducer, []);

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
            profile={profile} dispatchProfile={dispatchProfile}
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
 * Latter is `Profile` that lets user define special ProfilePropertys for the route (e.g. max altitude).
 * @param {*} props 
 */
function RoutingTab(props) {
    return (
        <div className="Tab">
            <PointContainer 
                currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
                pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}
                route={props.route} setRoute={props.setRoute}
                profile={props.profile}
            ></PointContainer>
            <Profile
                profile={props.profile} dispatchProfile={props.dispatchProfile}
                currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
            ></Profile>
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
    function findRoute(coordinates, profile) {
        let coordinatesPairs = [];
        for (let i = 0; i < coordinates.length - 1; i++) {
            coordinatesPairs.push([coordinates[i], coordinates[i + 1]]);
        }
        Promise.all(coordinatesPairs.map((pair) => {
            return fetch('/route?coordinates=' + JSON.stringify(pair) + '&profile=' + JSON.stringify(profile), {
                    method: 'GET',
                    headers: {
                        'Content-type': 'application/json; charset=UTF-8'
                    }
                })
                .then((response) => {
                    if (response.ok) {
                        return response.json();
                    }
                    return Promise.reject(response);
                })
                .then((data) => {
                    // console.log(data);
                    if (data.ok) {
                        const route = JSON.parse(data.route);
                        if (route.length === 0) {
                            return Promise.reject("No route found");
                        } else {
                            console.log("Fetched route: ", route);
                            return route;
                        }
                    } else {
                        return Promise.reject(data.error);
                    }
                });
        }))
        .then((routes) => {
            const joinedRoute = routes.flat(1);
            props.setRoute({data:joinedRoute, key:props.route.key < 0 ? 1 : -1});
            console.log("routes ", routes);
            console.log("joinedRoute ", joinedRoute);
        })
        .catch((error) => {
            alert("Invalid path.");
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
            findRoute(coordinates, props.profile);
        } else {
            // Clear route.
            props.setRoute({data:[], key:props.route.key < 0 ? 1 : -1});
        }

    }, [props.pathPoints]);

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
    points.push(<AddPoint key={"plus" + (props.pathPoints.length - 1)} currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint} 
    dispatchPoints={props.dispatchPoints} nextPointIndex={(props.pathPoints.length - 1) + 1}></AddPoint>);
    console.log("points: ", points);
    console.log(props.pathPoints.length);
    return (
        <div className="PointContainer" >
            <p>Select path points:</p>
            {points}
        </div>
            
        
    );
}

/**
 * Component `PathPoint` lets user to type a name of a location
 * and add that location as a route point.
 * Requests are sent to the server and it tries to find suitable
 * location whose addresses match the text provided by users.
 * @param {*} props 
 */
function PathPoint(props) {
    /**
     * Represents text value of select/ input.
     */
    const [text, setText] = useState('');

    /**
     * Printing to map is handled in `MapSection` so nothing is done here.
     * @param {*} place 
     */
    function showOnMap(place) {}

    /**
     * Callback to `SearchInput`. Is called when user selects option from dropdown menu
     * to update `pathPoints`(array of points that make a route.).
     * @param {*} place {label:..., value:...}
     */
    function handleSelect(place) {
        console.log('PLACE: ', place);
        props.setCurrentPoint(props.index);
        props.dispatchPoints({type:'update', value:{name:place.label, latLon:[place.value.lat, place.value.lon]}, index:props.index})
    }

    /**
     * Update `text` when name of point changes.
     */
    useEffect(() => {
        setText(props.pointName);
    }, [props.pointName]);

    return(
        <div className="PathPoint">
            <SearchInput 
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
                selectedPlace={props.pointName} setSelectedPlace={(place) => handleSelect(place)}
                showOnMap={(place) => showOnMap(place)}
                text={text} setText={setText}
                handleClick={() => props.setCurrentPoint(props.index)}
            >
                  {props.pointCount > 2 && <Button onClick={(e) => props.dispatchPoints({type:'delete', index:props.index})}>
                  <Trash2/>
                  </Button>}
            
            </SearchInput>
        </div>
    );
}

/**
 * Component `AddPoint` lets user add more points to better define route locations.
 * @param {*} props 
 */
function AddPoint(props) {
    return(
        <div className="AddPoint" onClick={() => {} /* props.setCurrentPoint(-1) */}>
            <Button  onClick={() => {
                props.dispatchPoints({type:'insert', value:{name:'', latLon:[null, null]}, index:props.nextPointIndex});
                props.setCurrentPoint(props.nextPointIndex);
             }}><PlusSquare/></Button>
        </div>  

    );
}

/**
 * @param {*} props 
 */
function Profile(props) {

    const [allProperties, setAllProperties] = useState([]);

    const getProfileProperties = () => {
        fetch('/profile_properties', {
            method: 'GET',
            headers: {
                'Content-type': 'application/json; charset=UTF-8'
            }
        })
        .then((response) => {
            if (response.ok) {
                return response.json();
            }
            return Promise.reject(response);
        })
        .then((data) => {
            console.log(data);
            if (data.ok) {
                console.log('properties', data.profile_properties);
                props.dispatchProfile({type:'set', value:data.profile_properties.map((property) => {
                    return {name:property.name, importance:property.importance_options[0]};
                })});
                setAllProperties(data.profile_properties.map((property) => {
                    return {name:property.name, importanceOptions:property.importance_options};
                }));
                console.log('profile', props.profile);
            } else {
                return Promise.reject(data.error);
            }
        })
        .catch((error) => {
            alert('Profile properties load failed!');
            console.warn('Profile properties load failed!', error);
        });
    }

    useEffect(() => {
        getProfileProperties();
    }, []);

    

    const properties = allProperties.map((property, index) => {
        let importanceLabel;
        let importance;
        if (index < props.profile.length) {
            importance = property.importanceOptions.indexOf(props.profile[index].importance);
            importanceLabel = props.profile[index].importance;
        } else {
            importance = property.importanceOptions[0];
            importanceLabel = 0;
        }
        return {name:property.name, importance:importance, importanceLabel:importanceLabel, importanceOptions:property.importanceOptions}
    }).filter((property) => {
        return property.importanceOptions.length > 1;
    }).map((property) => {
        return <ProfileProperty name={property.name} importance={property.importance} importanceLabel={property.importanceLabel}
            importanceOptions={property.importanceOptions} dispatchProfile={props.dispatchProfile} ></ProfileProperty>
    });

    return (
        <div className="Profile" onFocus={() => {} /* props.setCurrentPoint(-1) */}>
            {properties}
        </div>
    );
}

/**
 * @param {*} props 
 */
function ProfileProperty(props) {
    const [initial, ...rest] = props.name;
    const name = [initial.toUpperCase(), ...rest].join('');

    const onChange = (e) => {
        props.dispatchProfile({type:'update', value:{name:props.name, importance:props.importanceOptions[e.target.value]}});
    }
    return (
        <div className="ProfileProperty">
            <Form>
            <Form.Group controlId={name}>
                <Form.Label>{name} importance {props.importanceLabel}%</Form.Label>
                <Form.Control type='range' min='0' max={props.importanceOptions.length - 1} value={props.importance} onChange={onChange} step='1' />
            </Form.Group>
            </Form>
        </div>
    );
}

/**
 * Not yet implemented.
 * @param {*} props 
 */
function Footer(props) {
    return null;
}

/**
 * Component `SearchTab` just forwards all `props` to `SearchContainer` which it contains.
 * @param {*} props 
 */
function SearchTab(props) {
    return (
        <div className="Tab">
            <SearchContainer
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
            ></SearchContainer>
        </div>
    );
}

/**
 * Component `SearchContainer` provides a way for user to search 
 * in the map by having an select/input with dropdown menu.
 * Requests are sent to the server and it tries to find suitable
 * location whose addresses match the text provided by users. 
 * @param {*} props 
 */
function SearchContainer(props) {
    /**
     * Represents text value of select/ input.
     */
    const [text, setText] = useState(props.searchPoint.address);
    
    /**
     * Update `text` when address of search point is changed.
     */
    useEffect(() => {
        setText(props.searchPoint.address);
    }, [props.searchPoint.address]);

    /**
     * Trigger `MapSection` to render a marker for this place.
     * @param {*} place {label:..., value:...}
     */
    function showOnMap(place) {
        props.dispatchSearchPoint({type:'render', place:place.value});
    }
    return (
        <div className="SearchContainer">
            <p>Search map:</p>
            <SearchInput
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
                selectedPlace={props.searchPoint.address} setSelectedPlace={(t) => {}}
                showOnMap={(place) => showOnMap(place)}
                text={text} setText={setText}
                handleClick={() => {}}
            ></SearchInput>
        </div>
    );
}

/**
 * Not yet implemented.
 * @param {*} props 
 */
function ExportTab(props) {

    return (
        <div className="Tab">
            <ExportContainer/>
            <ImportContainer/>
        </div>
    );
}

/**
 * Not yet implemented.
 * @param {*} props 
 */
function ImportContainer(props) {
    return (
        <div className="ExportContainer">
            <p>Import path:</p>
            <Import></Import>

        </div>
    );
}

/**
 * Not yet implemented.
 * @param {*} props 
 */
function Import(props) {
    return (
        <div>
            <FormControl></FormControl>
        </div>
    );
}

/**
 * Not yet implemented.
 * @param {*} props 
 */
function ExportContainer(props) {
    return (
        <div className="ExportContainer"> 
            <p>Export path:</p>
            <Export></Export>
        </div>
    );
}

/**
 * Not yet implemented.
 * @param {*} props 
 */
function Export(props) {
    return (
        <div>
            <Button>Export</Button>
        </div>
    );
}

export default PrimaryPanel;


