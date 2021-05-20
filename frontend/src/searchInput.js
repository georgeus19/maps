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
    const [placeOptions, setPlaceOptions] = useState([]);

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
                setPlaceOptions(places);
                setMenuOpen(true);
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
            setPlaceOptions([]);
        } 
    }, [props.text])

    /**
     * Handle input change - happens on each user's key stroke.
     * @param {string} value string value of new value of user input.
     * @param {*} param1 Object with action.
     */
    function handleInputChange(value, {action}) {
        console.log("Input change:", action, value);
        switch (action) {
            case 'input-change':
                props.setText(value);
                setSearch(!search);
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

    /**
     * Handle that triggers when user selects an option from dropdown menu.
     * @param {string} place Value of selected option (=location) - {label:..., value:...}
     * @param {*} param1 Object with action.
     */
    function handleOnChange(place, {action}) {
        props.setSelectedPlace(place); 
        props.showOnMap(place);
    }

    return (
            <Select 
                className="SearchInput"
                defaultValue=""
                menuIsOpen={menuOpen}
                onFocus={() => {props.handleClick();  }}
                onChange={handleOnChange}
                options={placeOptions}
                inputValue={props.text}
                placeholder={props.placeholder}
                // value={{value:props.text, label:props.text}}
                filterOption={() => true} // Always show all options.
                onInputChange={handleInputChange}
                onClick={(e) => {e.target.select();}}
             />
    );
}
//<Button className="GreenButton" onClick={() => { setSearch(!search); /*fetchPlaces(props.text) */}}><Search/></Button>

export default SearchInput;