import React, { useState, useEffect, useCallback } from 'react';
import Button from 'react-bootstrap/Button';
import './searchInput.css';
import './primaryPanel.css'
import Select from "react-select";
import AsyncSelect from "react-select/async"
import Dropdown from 'react-bootstrap/Dropdown'
import { Camera, Search, PlusSquare, PlusCircle, Trash2, RefreshCcw } from 'react-feather';
import { findPossiblePlaces } from './nominatim.js'

/**
 * Component `SearchInput` contains a select that user can type in and a search button.
 * When search button is clicked a location is tried to be found which matches
 * user input. All such location are visible in a dropdown menu. When an option is selected
 * rerender is triggered to show place on the map (via `MapSection`).
 * @param {*} props 
 */
function SearchInput(props) {
    /**
     * Specifies if a search request should be sent or not.
     */
    const [search, setSearch] = useState(false);

    /**
     * Represents option in dropdown menu that user can select from.
     */
    const [placeSuggestions, setPlaceSuggestions] = useState([]);

    /**
     * Specifies if dropdown menu should be open or not.
     */
    const [menuOpen, setMenuOpen] = useState(false);

    /**
     * Fetch possible location that match user input if `search` is true.
     */
    useEffect(() => {
        //if (search === false) {
        //    console.log("NO fetch.");
        //    return;
        //}
        //setSearch(false);
        if (props.text === '') {
            return;
        }
        findPossiblePlaces(props.text)
            .then((places) => {
                if (places.length !== 0) {
                    setPlaceSuggestions(places);
                    setMenuOpen(true);
                }
            })
            .catch((error) => {
                console.warn('Error occured with respect to searching possible places.', error);
            });
    }, [search]);

    /**
     * If `props.text` is empty not old options should be visible in dropdown menu.
     */
    useEffect(() => {
        if (props.text === '' || props.text === null) {
            setPlaceSuggestions([]);
        } 
    }, [props.text])

    const suggestions = placeSuggestions.map((suggestion) => {
        return <div 
            className="Suggestion"
            onClick={() => { 
                console.log("PLACE!");
                props.setSelectedPlace(suggestion); 
                props.showOnMap(suggestion);
                props.setText(suggestion.label);
                setPlaceSuggestions([]);
            }}>
                {suggestion.label}
            </div>
    }).filter((suggestion, index) => { return index < 5; });

    console.log('suggestions', suggestions);

    return ( 
        <div className="SearchInput">
            <input 
                placeholder={props.placeholder} 
                onChange={(e) => {console.log("e", e); props.setText(e.target.value); setSearch(!search); }} 

                value={props.text}
                onFocus={() => props.onFocus()} >
            </input>
            <div className="Suggestions">
                {suggestions}
            </div>
        </div>
    );
}
//<Button className="GreenButton" onClick={() => { setSearch(!search); /*fetchPlaces(props.text) */}}><Search/></Button>

export default SearchInput;