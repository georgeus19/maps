# Maps - web application for finding routes for cyclists per their custom preferences

Many current web routing editors supporting creating routes for cyclists. However, there are usually some predefined preference profiles (e.g. mountain bike on mapy.cz) that alter the routing algorithm to prefer and suggest different types of roads. Since the profiles preference might not fit the user, they have to alter found route themselves.

The main idea of this application is to allow custom setting these profiles dynamically by user. For example, users can choose whether to they prefer to bike in green areas, preferred elevation (avoid/prefer hills, going uphill), what type of paths they prefer, ...

This application was done as part of my [bachelor thesis](https://dspace.cuni.cz/bitstream/handle/20.500.11956/148394/130315842.pdf?sequence=1&isAllowed=y) which can be viewed for more information about the core concepts of routing and documentation of technologies and the code.

This repository code is archived and the application is currently not running anywhere - the idea is to reuse the knowledge gained from this application and create a new clean solution without some unnecessary components and better design using the found experience in this project.

## UI Example

I also provide a description of the user interface to help convey the main idea of the project. The main part of the client user interface is shown on the picture below. The map editor on the left shows a map. It is possible to zoom and drag the map to other areas. The map editor also displays any found routes. A route point specified by a user is represented by a red marker. The primary panel on the right provides three tabs: Search, Route and Export.

![Country Code List Recommendation Description View](/img/ui.png)

**Search** tab provides a text field for searching in the map. Place suggestions are displayed below the text field as a user types in a description (i.e. address) of a place. A place can be selected by clicking on a suggestion. Once a place is selected, the map editor zooms in on the area around the place.

**Route** tab serves for defining route points and routing profile. It is the tab that is shown in the figure. A route point corresponds to a text field. Route points can be specified in two ways:

-   A route point can be specified by typing a place description (i.e. address) into its corresponding text field. Place suggestions are displayed below the text area as a user types. The route point is specified once a suggestion is clicked on.

-   A route point can also be selected in the map editor by clicking on the route point's text field and then clicking on a place in the map which the point should represent.

Once at least two route points are defined, the optimal route between the route points is found and displayed in the map editor. The topmost point represents the start point of the route and the bottommost the last. As long as at least two points are defined, any change triggers route recalculation and update.

Additional points can be added by clicking on plus buttons. A new point (with a new plus button below it) is always inserted just below the plus button which it was clicked on. Any route point can be deleted by clicking on a corresponding trash button.

How routes are found is based on a selected routing profile. There is a list of preferences below the route points where a user can specify whether and how much of a given preference should be considered in routing. Currently, there are three available preferences: green preference, peak distance preference and road type preference.

-   Green preference determines if the route to be found should be rather in green areas or not.

-   Peak distance preference determines whether the route should be near hills and peaks or not.

-   Road type preference determines what types of roads the route should be primarily on. For example, paths and trails can be preferred over large roads.

**Export** tab contains a button which exports a found route in GPX format.
