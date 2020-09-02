import React, { useState, useEffect } from 'react';
import logo from './logo.svg';
import './primaryPanel.css';
import SERVER_ADDRESS from './serverConsts';
import Button from 'react-bootstrap/Button';
import FormControl from 'react-bootstrap/FormControl'
import 'bootstrap/dist/js/bootstrap.bundle.min.js'
import 'bootstrap/dist/css/bootstrap.min.css';
import Select from "react-select";
import AsyncSelect from "react-select/async"
import Dropdown from 'react-bootstrap/Dropdown'
const TabEnum = Object.freeze({"searchTab":1, "routeTab":2, "exportTab":3})

function PrimaryPanel(props) {
    
    const [currentTab, setCurrentTab] = useState(TabEnum.routeTab);

    let tab;

    if (currentTab === TabEnum.searchTab) {
        tab = <SearchTab
                currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
        ></SearchTab>;
    } else if (currentTab === TabEnum.routeTab) {
        tab = <RoutingTab currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint} pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}></RoutingTab>;
    } else if (currentTab === TabEnum.exportTab) {
        tab = <ExportTab setCurrentPoint={props.setCurrentPoint}></ExportTab>;
    }

    return (    
        <div className="PrimaryPanel">
            <Header setCurrentPoint={props.setCurrentPoint} setTab={(tab) => setCurrentTab(tab)}/>
            {tab} 
            <Footer/>
        </div>
    );
}

function Header(props) {
    return (
        <div className="Header" onClick={() => props.setCurrentPoint(-1)}>
            <Button className="HeaderOption" onClick={() => props.setTab(TabEnum.searchTab)}>Search</Button>
            <Button className="HeaderOption" onClick={() => props.setTab(TabEnum.routeTab)}>Route</Button>
            <Button className="HeaderOption" onClick={() => props.setTab(TabEnum.exportTab)}>Import/ Export</Button>
        </div>
    )
}

function RoutingTab(props) {

    return (
        <div className="Tab">
            <PointContainer currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint} pathPoints={props.pathPoints} dispatchPoints={props.dispatchPoints}></PointContainer>
            <ConstraintContainer currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint}></ConstraintContainer>
        </div>
    );
}

function PointContainer(props) {

    let points=[];

    props.pathPoints.forEach((point, index) => {
        points.push(<PathPoint key={index} currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint} 
        dispatchPoints={props.dispatchPoints} index={index} pointName={point.name} pointCount={props.pathPoints.length}></PathPoint>);
        if (index !== props.pathPoints.length - 1) {
            points.push(<AddPoint key={"plus" + index} currentPoint={props.currentPoint} setCurrentPoint={props.setCurrentPoint} 
            dispatchPoints={props.dispatchPoints} previousPointIndex={index}></AddPoint>);
        }
    })

    return (
        <div className="PointContainer" >
            <div onClick={() => props.setCurrentPoint(-1)}><p onFocus={() => props.setCurrentPoint(-1)}>Select path points:</p></div>
            {points}
        </div>
            
        
    );
}

function PathPoint(props) {

    function handleInputChange(e) {
        props.setCurrentPoint(props.index);
        props.dispatchPoints({type:'update', value:{name:e.target.value, latLon:[null, null]}, index:props.index})
    }

    return(
        <div className="PathPoint">
            <FormControl type="text" placeholder="Select a path point..." value={props.pointName}
             onChange={handleInputChange} onFocus={() => {console.log("input focused!! " + props.index); return props.setCurrentPoint(props.index)}
             }
             ></FormControl>
             
            {props.pointCount > 2 && <Button onFocus={() => props.setCurrentPoint(-1)} onClick={(e) => props.dispatchPoints({type:'delete', index:props.index})}>-</Button>}
            <Button onFocus={() => props.setCurrentPoint(-1)} onClick={(e) => props.dispatchPoints({type:'clear', index:props.index})}>x</Button>
        </div>
    );
}


function AddPoint(props) {

    return(
        <div className="AddPoint" onClick={() => props.setCurrentPoint(-1)}>
            <Button  onClick={() => props.dispatchPoints({type:'insert', value:{name:'', latLon:[null, null]}, index:props.previousPointIndex})}>+</Button>
        </div>  

    );
}

function ConstraintContainer(props) {
    return (
        <div className="ConstraintContainer" onFocus={() => props.setCurrentPoint(-1)}>
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
    return (
        <div className="SearchContainer">
            <p>Search map:</p>
            <Search
                searchPoint={props.searchPoint} dispatchSearchPoint={props.dispatchSearchPoint}
            ></Search>
        </div>
    );
}

function Search(props) {

    const [text, setText] = useState('');
    //const [predictedObjects, setPredictedObjects] = useState([]);
    const [selectedPlace, setSelectedPlace] = useState(null);
    const [buttonClicked, setButtonClicked] = useState(false);
    const [key, setKey] = useState(1);
//
    //useEffect(() => {
    //    if (text === '') return;
    //    const options = {
    //        method: 'GET'
    //    };
    //    fetch('https://nominatim.openstreetmap.org/search?q=' + text + '&format=json', options)
    //        .then((response) => { console.log("data received"); return response.json();})
    //        .then((data) => {setPredictedObjects(data); console.log(predictedObjects)});
    //}, [text]);
    //let placeOptions = [];
    //predictedObjects.forEach((obj, index) => {
    //    placeOptions.push({value: 'index', label:obj.display_name})
    //});
    //const options = [
    //    { value: 'chocolate', label: 'Chocolate' },
    //    { value: 'strawberry', label: 'Strawberry' },
    //    { value: 'vanilla', label: 'Vanilla' },
    //  ];
//
    //  const [selectedOption, setSelectedOption] = useState(null);

    function fetchPlaces(inputValue) {
        if (buttonClicked === false) {
            console.log("NO fetch.");
            return [];
        }
        setButtonClicked(false);
        console.log("fetch start");
        if (!inputValue) {
            return [];
        }
        const options = {
            method: 'GET'
        };
        return fetch('https://nominatim.openstreetmap.org/search?q=' + inputValue + '&format=json', options)
            .then((response) => { console.log("DATA FETCHED"); return response.json();})
            .then((data) => {
                console.log(data); 
                return data.map((obj) => { return { value:obj, label:obj.display_name} });
            });
    }

    function showOnMap(place) {
        //console.log("Selected place ", place);
        props.dispatchSearchPoint({type:'render', place:place.value});
    }

//<FormControl type='select' value={text} onChange={(e) => setText(e.target.value)}></FormControl>
//            <Button onClick={() => props.dispatchSearchPoint({type:'render'})}>Go</Button>
//<Select
//                defaultValue={selectedOption}
//                onChange={setSelectedOption}
//                options={placeOptions}
//            />
    return (
        <div>
            <AsyncSelect
                cacheOptions   
                defaultOptions
                loadOptions={fetchPlaces}
                onChange={(place) => { setSelectedPlace(place); showOnMap(place) }}
                inputValue={text}
                onInputChange={(t) => {if (t !== '') setText(t);} }
            />
            <Button onClick={() => { setText(''); }}>X</Button>
            <Button onClick={() => { setButtonClicked(true); }}>Search</Button>
            <Button onClick={() => { if (!selectedPlace) { showOnMap(selectedPlace) } }}>Show</Button>
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

export default PrimaryPanel;


//    function GetData() {
//        
//        const options = {
//            method: 'GET'
//        };
//        fetch('/maps/12/2197/1392.png', options).then((response) => { console.log("data received"); return response.json()}).then((data) => setTemp(data.hello));
//    }
//<FormControl value={props.searchPoint.address} onChange={(e) => props.dispatchSearchPoint({type:'set', value:e.target.value})}></FormControl>
  