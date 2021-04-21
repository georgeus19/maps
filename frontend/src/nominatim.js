export function findPossiblePlaces(placeName) {
    const options = {
        method: 'GET'
    };//https://nominatim.openstreetmap.org - nominatim free test api server
    return fetch('/nominatim/search?q=' + placeName + '&format=json&addressdetails=1', options)
        .then((response) => {
            return response.json();
        })
        .then((data) => {
            console.log( "Nominatim fetched data for " + placeName + ". ", data); 
            return data.map((obj) => { 
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
        });
};

export function findPlaceInformation(lat, lon) {
    const options = {
        method: 'GET'
    };
    // https://nominatim.openstreetmap.org/reverse?lat - nominatim free test api server
    return fetch('/nominatim/reverse?lat=' + lat + '&lon=' + lon + '&format=json', options)
        .then((response) => { 
            console.log("DATA FETCHED");
            return response.json();
        })
        .then((data) => {
            console.log("Fetched reverse geocoding data ", data); 
            let adr;
            if (data.display_name) {
                adr = data.display_name;
            } else if (data.length > 0) {
                adr = data[0].display_name;
            } else {
                adr = lat + 'N, ' + lon + 'E';
            }
            return { name: adr, latLon:[lat, lon]};
        });
};