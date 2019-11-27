# SOFT356 C1: Model Loader

#### Student No: 10550420 / Jamie Everett

<!-- Which version Visual Studio, OpenGL/Vulkan -->

## Software Used for Development:

<ul>
    <li>Visual Studio 2019</li>
    <li>nupengl.core v0.1.0.1 (NuGet)</li>
    <li>glm v0.9.9.600 (NuGet)</li>
</ul>

<!-- How do I work it? -->

## How To Use the Model Loader:

The model loader accepts two model types, <b>obj/mtl combination</b> and <b>blender dae</b>.
<br><br>
When loading an obj file, the loader will look for an mtl file with the same name in the same directory. If one is found, the texture and/or material effects will be applied. If not, a black polygon model will be rendered with no material data.

### Keybindings

The model loader comes with the following controls to provide a nicer user experience (all bindings are not case sensitive):

<b>Camera Controls:</b>

<ul>
    <b>Mouse:</b>
    <li>Mouse Movement : Pitch & Yaw</li>
    <li>Mouse Scroll : Increase/Decrease Zoom</li>
    <b>Keyboard:</b>
    <li>W : Camera Forward</li>
    <li>A : Camera Left</li>
    <li>S : Camera Back</li>
    <li>D : Camera Right</li>
    <li>Left Shift : Camera Up</li>
    <li>Left Control : Camera Down</li>
    <li>Page Up : Scale Models Up</li>
    <li>Page Down : Scale Models Down</li>
    <li>1 : Toggle wireframe mode</li>
    <li>3 : Swap model textures</li>
</ul>

<b>Model Controls:</b>

<ul>
    <li>Backspace : Remove the last model that was added</li>
</ul>

<b>Window Controls:</b>

<ul>
    <li>Escape : Close the program</li>
    <li>2 : Toggle viewport mouse capturing</li>
</ul>

### Loading Models

<ol>
    <li>When the program is run, a command prompt is opened which asks for the number of models you wish to load. Enter the required amount and press <i>enter</i>.</li>
    <li>Then, enter the file locations for each model requested.</li>
    <li>The models will then be displayed in a new OpenGL window. Use the above listed keybindings to explore the models.</li>
    <li>When you are finished using the program, press <i>escape</i> to exit.</li>
</ol>

### Code Structure

<!-- How does your code fit together and how should a programmer navigate it (not intended to be an exercise in formally documenting program code) -->

<!-- Anything else which will help us to understand how your prototype works. -->

### Future Improvements

<!-- what I'd like to add, what would've done -->
