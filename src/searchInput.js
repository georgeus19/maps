import React, { useState, useEffect, useCallback } from 'react';
import Button from 'react-bootstrap/Button';
import './searchInput.css';
import Select from "react-select";
import AsyncSelect from "react-select/async"
import Dropdown from 'react-bootstrap/Dropdown'
import { Camera, Search, PlusSquare, PlusCircle, Trash2, RefreshCcw } from 'react-feather';
function SearchInput(props) {
    const [search, setSearch] = useState(false);
    const [placeOptions, setPlaceOptions] = useState([]);
    const [menuOpen, setMenuOpen] = useState(false);
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
    useEffect(() => {
        const fetchPlaces = async () => {
            const options = {
                method: 'GET'
            };
            await fetch('https://nominatim.openstreetmap.org/search?q=' + props.text + '&format=json&addressdetails=1', options)
                .then((response) => { console.log("DATA FETCHED"); return response.json();})
                .then((data) => {
                    console.log(data); 
                    const res = data.map((obj) => { 
                        const adr = obj.address;
                        let displayName = '';
                        if (adr.road) {
                            displayName += adr.road + ', ';
                        }
                        if (adr.house_number) {
                            displayName += adr.house_number + ', ';
                        }
                        if (adr.city) {
                            displayName += adr.city + ', ';
                        } else if (adr.village) {
                            displayName += adr.village + ', ';
                        } else if (adr.town) {
                            displayName += adr.town + ', ';
                        }
                        if (adr.neighbourhood) {
                            displayName += adr.neighbourhood + ', ';
                        }
                        if (adr.country) {
                            displayName += adr.country + ', ';
                        }
                        return { value:obj, label:obj.display_name };
                     });
                    setPlaceOptions(res);
                    setMenuOpen(true);
                })
                .catch((error) => {
                    console.warn('Error occured with respect to searching.', error);
                });
        }
        if (search === false) {
            console.log("NO fetch.");
            return;
        }
        setSearch(false);
        console.log("fetch start");
        if (props.text === '') {
            return;
        }
        console.log("text:", props.text);
        fetchPlaces();
    
    }, [search]);

    useEffect(() => {
        if (props.text === '' || props.text === null) {
            setPlaceOptions([]);
        }
    }, [props.text])

    function handleInputChange(value, {action}) {
        console.log("Input change:", action, value);
        switch (action) {
            case 'input-change':
                props.setText(value);
                break;
            case 'input-blur':
                break;

            case 'menu-close':
                setMenuOpen(false);
                break;
            case 'set-value':
                // props.setText(value);
                break;
            default:
                break;
        }
    }

    function handleOnChange(place, {action}) {
        props.setSelectedPlace(place); 
        props.showOnMap(place);
    }


    return (
        <div className="SearchInput">
           
            <Select 
                defaultValue=""
                className="Select"
                menuIsOpen={menuOpen}
                onFocus={() => {props.handleClick(); setMenuOpen(true); }}
                onChange={handleOnChange}
                options={placeOptions}
                inputValue={props.text}
                // value={{value:props.text, label:props.text}}
                filterOption={() => true} // Always show all options.
                onInputChange={handleInputChange}
             />
            {props.children}
            <Button onClick={() => { setSearch(true); /*fetchPlaces(props.text) */}}><Search/></Button>
            
        </div>
    );
}

export default SearchInput;

//<FormControl type='select' value={text} onChange={(e) => setText(e.target.value)}></FormControl>
//            <Button onClick={() => props.dispatchSearchPoint({type:'render'})}>Go</Button>
//<Select
//                defaultValue={selectedOption}
//                onChange={setSelectedOption}
//                options={placeOptions}
//            />
// <Select
//                 defaultValue={""}
//                 onChange={(place) => { setSelectedPlace(place); showOnMap(place) }}
//                 options={placeOptions}
//                 inputValue={text}
//                 onInputChange={(t) => { fetchPlaces(text); if (t !== '') setText(t);}}
//             />
//<AsyncSelect
//                cacheOptions   
//                defaultOptions
//                loadOptions={fetchPlaces}
//                onChange={(place) => { props.setSelectedPlace(place); props.setText(place.display_name); props.showOnMap(place); }}
//                inputValue={props.text}
//                onInputChange={(t) => {if (t !== '') props.setText(t);} }
        //    {placeOptions.map((place, key) => {console.log(place.label); return <option key={key} value={place.label}>{place.label}</option>})}
            //            />
  //          <label>
  //          Choose a browser from this list:
  //          <input list="placeOptions" name="myBrowser" autocomplete="on" value={props.text} onChange={(e) => {console.log("ONCHANGE:", props.text); props.setText(e.target.value)}} />  
  //      </label>   
  //      <datalist id="placeOptions" filterOption={() => true}>
  //          {placeOptions.map((place, key) => <option key={key} value={props.text}>{place.label} </option>)}
  //      </datalist>
  //          const browsers = placeOptions;
  //          console.log("browsers", browsers);
// const data = browsers.map((place, key) => <option key={key} value={place.label}/>);
    // async function fetchPlaces2() {
    //     const options = {
    //         method: 'GET'
    //     };
    //     await fetch('https://nominatim.openstreetmap.org/search?q=' + props.text + '&format=json', options)
    //         .then((response) => { console.log("DATA FETCHED"); return response.json();})
    //         .then((data) => {
    //             console.log(data); 
    //             const res = data.map((obj) => { return { value:obj, label:obj.display_name} });
    //             setPlaceOptions(res);
    //            
    //         });
    // }

    // function fetchPlaces(inputValue) {
    //     if (buttonClicked === false) {
    //         console.log("NO fetch.");
    //         return [];
    //     }
    //     setButtonClicked(false);
    //     console.log("fetch start");
    //     if (!inputValue) {
    //         return [];
    //     }
    //     const options = {
    //         method: 'GET'
    //     };
    //     return fetch('https://nominatim.openstreetmap.org/search?q=' + inputValue + '&format=json', options)
    //         .then((response) => { console.log("DATA FETCHED"); return response.json();})
    //         .then((data) => {
    //             console.log(data); 
    //             const res = data.map((obj) => { return { value:obj, label:obj.display_name} });
    //             setPlaceOptions(res);
    //             return res;
    //         });
    // }
    // <AsyncSelect
    // cacheOptions   
    // defaultOptions
    // loadOptions={fetchPlaces}
    // onChange={(place) => { props.setSelectedPlace(place); props.setText(place.display_name); props.showOnMap(place); }}
    // inputValue={props.text}
    // onInputChange={handleInputChange}
    // />

//<Button onClick={() => { if (!props.selectedPlace) { props.showOnMap(props.selectedPlace) } }}>Show</Button>
