# SVG Editor with Qt C++
***Course - COP290***
***Name - Abhishek Jain***
***Entry No. - 2024CS10328***
***********************************************************


## 1. How to compile and run
Run the following commands in the project directory (for linux systems). Qt and CMake must be installed.
```bash
sudo apt install libqt6svg6-dev # to make sure svg icons used in the app are rendered
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH={Path to Qt} -DCMAKE_BUILD_TYPE=Debug
cmake --build .
./svg_editorr
```
- For mac run `./svg_editorr.app/Contents/MacOS/svg_editorr`


## 2. File structure
Adhering to the guildlines set by the problem statement; the code is modularized into several files and naming is done corresponding to Google's conventions. The code also makes heavy use of class inheritence and Qt signals and slots for clean ui interaction.
```bash
.
├── CMakeLists.txt
├── forms   # The ui-forms for the basic ui elements such as buttons
│   ├── mainwindow.ui
│   ├── menubuttonswidget.ui
│   └── toolbarwidget.ui
├── images # Contains the icons for the ui buttons
├── include # Contains all the header files
│   ├── canvaswidget.h
│   ├── commands.h
│   ├── doccontroller.h
│   ├── Enums.h
│   ├── graphicscene.h
│   ├── mainwindow.h
│   ├── menubuttonswidget.h
│   ├── propertiesdock.h
│   ├── shapes.h
│   ├── toolbarwidget.h
│   ├── undostack.h
│   └── xmlparser.h
├── README.md
├── resources.qrc
└── src  # Implements the header files
    ├── canvaswidget.cpp
    ├── commands  # Implements the classes defined in commands.h
    │   ├── add_delete_shape_command.cpp
    │   ├── move_shape_command.cpp
    │   ├── shape_xml_command.cpp
    │   └── viewport_change_command.cpp
    ├── doccontroller.cpp
    ├── graphicscene.cpp
    ├── main.cpp
    ├── mainwindow.cpp
    ├── menubuttonswidget.cpp
    ├── propertiesdock  # Implements the classes defined in propertiesdock.h
    │   ├── circlepropform.cpp
    │   ├── freehandpropform.cpp
    │   ├── linepropform.cpp
    │   ├── polygonpropform.cpp
    │   ├── propertiesdock.cpp
    │   ├── propertiesform.cpp
    │   ├── rectpropform.cpp
    │   ├── textpropform.cpp
    │   └── viewportpropform.cpp
    ├── shapes  # Implements the classes defined in shapes.h
    │   ├── circle.cpp
    │   ├── freehandpath.cpp
    │   ├── line.cpp
    │   ├── polygon.cpp
    │   ├── rectangle.cpp
    │   ├── shape.cpp
    │   └── textshape.cpp
    ├── toolbarwidget.cpp
    ├── undostack.cpp
    └── xmlparser.cpp
```


## 3. XML Parsing
The app allows only `.svg` files to be openned and supports the restricted-SVG subset required by the problem. The xml-parsing related functions are defined and implemented in `xmlparser.h` and `xmlparser.cpp` respectively. 
It uses a stack based approach to to handle nested XML tags. It assumes the tag names are lower case alphabet letters and the property names can contain lower case alphabets and hyphens; to smoothly parse the file.
Here is how each shape is expected and outputted by the xml parser. (All of this is in line with the standard SVG specification). All shapes have the style attributes of `fill`, `stroke`, `stroke-width` and `fill-opacity`.
- ***Rectangle*** - Uses the `<rect>` tag. Has attributes `x`, `y`, `width`, `height`; and additionally `rx` and `ry` to support the rounded rectangle functionality.
- ***Circle*** - Uses the `<circle>` tag. Has attributes `cx`, `cy` and `r`.
- ***Line*** - Uses `<line>` tag. Has attributes `x1`, `y1`, `x2`, `y2`.
- ***Freehand*** - Uses `<path>` tag with the attribute `d`. The path is defined as `d="M 10 20 L 20 30 ..."` where `M` means move to and `L` means line to. 
- ***Polygon*** - Uses the `<polygon>` tag where the attribute `points="1,20 4,50 ..."`defines the points of the polygon. Although only drawing of regular hexagon is supported. The editor can still be used to edit other polygon shapes loaded.
- ***Text*** - Defined using `<text>Example text</text>`. It also has the additional property of `font-size`.

**All** of these properties are editable through the ui-editor in some form.


## 4. Inheritence patterns in the code
- `shapes.h` defines base class `Shape` having **virtual** and **static** functions for converting a Shape to XML format and XML to Shape.
For each shape we then define classes such as `Rectangle` which inherits from both `Shape` (for the XML conversion) and `QGraphicsRectItem` (for the rendering). Each shape is similarly implemented. The classes in `shapes.h` is implemented in `src/shapes` folder for readability.
**Note**: We make heavy use of the fact that `QGraphicsItem` and `QAbstractGraphicsShape` are parent classes for all our shapes too.

- `commands.h`defines the base class `Command` which is base data-type used by the undo-redo stacks. It contains virtual functions implemented by the various types of commands executed in the editor. All these classes are implemented in the `src/commands` folder.
    1. `AddDeleteShapeCommand` - For adding/deleting shapes from the canvas.
    2. `MoveShapeCommand` - For moving shapes on the canvas.
    3. `ViewportChangeCommand` - For user changing the viewport rectangle dimensions
    4. `ShapeXmlCommand` - For various edits made by the user such as fill color, height or width; instead of having a seperate command class for each of them; the shape is converted to XML format in order to preserve its previous state and current state.

- `propertiesdock.h` defines the ui classes `PropertiesDock` which renders a class inheriting from `PropertiesForm` based on which shape is selected. For example if a rectangle is currently selected then `PropertiesDock` object would create and render an object of type `RectPropForm` (which inherits from `PropertiesForm`). This is done in order to support the different kind of attributes each shape has. The `Shape` class has the virtual function `GetPropertyForm` to support this functionality. All classes are implemented in `src/propertiesdock` folder.


## 5. Canvas and Scene
`graphicsscene.h` defines the `GraphicsScene` class which inherits from `QGraphicsScene`. This object is responsible for handling all the shape pointers on the canvas. It also implements the mouse events for moving and drawing new shapes.
`canvaswidget.h` defines the `CanvasWidget` class inheriting from `QGraphicsView`. It simply renders the scene on the screen.
- ***Memory management*** - Qt has its own memory management system where parent widgets own the children widget's raw pointers and hence handle their deletion. By default the `QGraphicsScene` owns the shapes. However this is prone to memory leaks as a shape pointer is not owned until `QGraphicsScene::addItem` is called. To deal with this `std::unique_ptr` is used to safely carry shape pointers. The unique pointer is released after giving its ownership to the internal `QGraphicsScene`.
When deleting an object our `GraphicsScene` object re-takes the ownership ownership of the raw pointer by storing in `std::map<QAbstractGraphicsShape*, std::unique_ptr<QAbstractGraphicsShape> > deleted_shapes_`. So we can find and again safely add the shape to the scene in case of undo after deletion.


## 6. Undo Stack
`undostack.h` defines the `UndoStack` class which stores the undo and redo stacks consisting of `std::unique_ptr<Command>` for memory safety. Each command must execute in its constructor and have the `undo()` and `redo()` functions implemented which the object calls internally.
It has a `counter_` which resets to `0` every time the user saves the current file or opens a new one. It is incremented and decremented on every command to keep track if any change was made in the file and is reflected on the MainWindow title.
Additionally it emits the `StackChanged()` signals whenever a command is executed so other objects can catch it and know some change was made to the current document.
The `UndoStack` object is owned by the `GraphicsScene` object.


## 7. File Handling
The code related to file handling is written in the `doccontroller.h` and `doccontroller.cpp` files which define the `DocController` class. An object of this class is initialized in the `MainWindow` constructor. It handles all the user requests to save, open or make a new file. The dialog to save current file in case any change was made is shown in case the current file is being closed.
Importantly in addition to file handling it also implements the `ExtractShapes()` function which converts an `XMLTag` object into a vector of unique pointers for the shapes contained in the tag which are then rendered.


## 8. Clipboard Actions

The cut, copy and paste actions work by converting the shape into its XML format and storing that text in the `QClipboard` object provided by Qt. 
It uses the `AddDeleteShapeCommand` in order to support undo and redo functionality. (Note the clipboard doesn't change on undo as one would expect). All these functions are implemented in the `GraphicsScene` object as it owns all the shapes.


## 9. Small note on UI interaction
The `toolbarwidget.h` implements the ui class for the right-side toolbar. It sends signals to the graphics scene in case the tool changes.
The `PropertyDock` renders ways for the user to edit the object styles and resize them. It catches signals from graphics scene to know which shape is currently selected and signals from undo stack to know that the change requested from the form is executed.
All the button clicks and keyboard shortcuts are mapped to their respective slots in the `MainWindow` constructor. It also maps some other ui signals and slots.
**User Note**: Please use the *Select* tool in order to move and select shapes around the canvas.
