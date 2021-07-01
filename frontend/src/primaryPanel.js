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
var FileSaver = require('file-saver');
var togpx = require('togpx');

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
        tab = <ExportTab setCurrentPoint={props.setCurrentPoint}
            route={props.route}
        ></ExportTab>;
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
            <Button className={`HeaderOption GreenButton ${props.currentTab === TabEnum.searchTab ? 'selected' : ''}`} onClick={() => props.setTab(TabEnum.searchTab)}>Search</Button>
            <Button className={`HeaderOption GreenButton ${props.currentTab === TabEnum.routeTab ? 'selected' : ''}`} onClick={() => props.setTab(TabEnum.routeTab)}>Route</Button>
            <Button className={`HeaderOption GreenButton ${props.currentTab === TabEnum.exportTab ? 'selected' : ''}`} onClick={() => props.setTab(TabEnum.exportTab)}>Export</Button>
        </div>
    )
}

/**
 * Component `RoutingTab` consists of two containers. 
 * Former is `Route` that lets user define a route.
 * Latter is `Profile` that lets user define special ProfilePropertys for the route (e.g. max altitude).
 * @param {*} props 
 */
function RoutingTab(props) {
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
            console.log("fetch", '/route?coordinates=' + JSON.stringify(pair) + '&profile=' + JSON.stringify(profile));
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
            const joinedRoute = {
                'type': 'LineString',
                'coordinates': mergeRoutes(routes)
            };
            props.setRoute({data:joinedRoute, key:props.route.key < 0 ? 1 : -1});
            // console.log("routes ", routes);
            // console.log("joinedRoute ", joinedRoute);
        })
        .catch((error) => {
            alert("Invalid path.");
            console.warn('Error occured with respect to routing.', error);
        });
    }

    function mergeRoutes(routes) {
        const unordered_coordinates = routes.flat(1).map((linestring) => {
            return linestring.coordinates;
        });
        const equals = (a, b) => { const eps = 0.00001; return (a + eps >= b) && (a - eps <= b)};
        const coordinatesEqual = (former, latter) => {
            return equals(former[0], latter[0]) && equals(former[1], latter[1]); 
        };
        if (unordered_coordinates.length > 1) {
            const former = unordered_coordinates[0];
            const latter = unordered_coordinates[1];
            if (coordinatesEqual(former[0], latter[0])
                || coordinatesEqual(former[0], latter[latter.length - 1])) {
                    unordered_coordinates[0] = former.reverse();
            }
        }
        for (let i = 0; i < unordered_coordinates.length - 1; i++) {
            const former = unordered_coordinates[i]; 
            const latter = unordered_coordinates[i + 1];
            if (!coordinatesEqual(former[former.length - 1], latter[0])) {
                unordered_coordinates[i + 1] = latter.reverse();
            }
        }
        return unordered_coordinates.flat(1);
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

    }, [props.pathPoints, props.profile]);

    return (
        <div className="Tab">
            <Route 
                currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
                pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}
                route={props.route} setRoute={props.setRoute}
                profile={props.profile}
            ></Route>
            <Profile
                profile={props.profile} dispatchProfile={props.dispatchProfile}
                currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
            ></Profile>
        </div>
    );
}

/**
 * Component `Route` provides functionality for defining a route.
 * User can fill in selects/ inputs to search for a point (startpoint, endpoint).
 * User can add more points that make a route.
 * @param {*} props 
 */
function Route(props) {
    
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
        <div className="Route" >
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
     * Callback to `SearchInput`. Is called when user selects option from dropdown menu
     * to update `pathPoints`(array of points that make a route.).
     * @param {*} place {label:..., value:...}
     */
    function handleSelect(place) {
        console.log('PLACE: ', place);
        props.setCurrentPoint(props.index);
        props.dispatchPoints({type:'update', value:{name:place.name, latLon:place.latLon}, index:props.index})
    }

    /**
     * Update `text` when name of point changes.
     */
    useEffect(() => {
        setText(props.pointName);
    }, [props.pointName]);

    let placeholder;
    if (props.index === 0) {
        placeholder = 'Enter route start...';
    } else if (props.index === props.pointCount - 1) {
        placeholder = 'Enter route end...';
    } else {
        placeholder = 'Enter route point...'
    }

    const trashHandleClick = (e) => {
        props.dispatchPoints({type:'delete', index:props.index});
        if (props.pointCount <= 2) {
            props.dispatchPoints({type:'insert', value:{name:'', latLon:[null, null]}, index:props.index});
        } 
    }

    return(
        <div className="PathPoint">
            <SearchInput 
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
                selectedPlace={props.pointName} setSelectedPlace={(place) => handleSelect(place)}
                text={text} setText={setText}
                onClick={() => props.setCurrentPoint(props.index)}
                placeholder={placeholder}
            >
            
            </SearchInput>
            <Button className="GreenButton" onClick={trashHandleClick}>
                <Trash2/>
            </Button>
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
            <Button className="GreenButton" onClick={() => {
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
                <Form.Label className="Label">{name} importance {props.importanceLabel}%</Form.Label>
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
 * Component `SearchTab` just forwards all `props` to `Search` which it contains.
 * @param {*} props 
 */
function SearchTab(props) {
    return (
        <div className="Tab">
            <Search
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
            ></Search>
        </div>
    );
}

/**
 * Component `Search` provides a way for user to search 
 * in the map by having an select/input with dropdown menu.
 * Requests are sent to the server and it tries to find suitable
 * location whose addresses match the text provided by users. 
 * @param {*} props 
 */
function Search(props) {
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

    const placeholder = 'Select place to search...'

    return (
        <div className="Search">
            <SearchInput
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
                selectedPlace={props.searchPoint.address} setSelectedPlace={(t) => {}}
                showOnMap={(place) => showOnMap(place)}
                text={text} setText={setText}
                handleClick={() => {}}
                placeholder={placeholder}
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
            <Export route={props.route} />
        </div>
    );
}

/**
 * Not yet implemented.
 * @param {*} props 
 */
function Export(props) {
    return (
        <div className="Export"> 
            <ExportButton route={props.route} ></ExportButton>
        </div>
    );
}

/**
 * Not yet implemented.
 * @param {*} props 
 */
function ExportButton(props) {
    const exportRoute = () => {
        console.log('route', props.route.data);
        const gpx = togpx(props.route.data);
        console.log('gpx', gpx);
        var blob = new Blob([gpx], {type: "text/plain;charset=utf-8"});
        FileSaver.saveAs(blob, "route.gpx");
    };
    return (
        <div>
            <Button className="GreenButton" onClick={exportRoute} >Export route</Button>
        </div>
    );
}

export default PrimaryPanel;


