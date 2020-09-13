import React, { useState, useEffect, useCallback } from 'react';
import Button from 'react-bootstrap/Button';
import './searchInput.css';
import Select from "react-select";
import AsyncSelect from "react-select/async"
import Dropdown from 'react-bootstrap/Dropdown'
import { Camera, Search, PlusSquare, PlusCircle, Trash2, RefreshCcw } from 'react-feather';

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
        if (props.text === '') {
            return;
        }
        fetchPlaces();
    
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