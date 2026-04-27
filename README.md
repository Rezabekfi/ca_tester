# ca tester

This app is designed to test and have fun with CA (cellular automata) rules. This app was developed as a supporting project for my bachelor thesis which might in the future be linked here.

## Strucuture

If you want to use this app in your own project, all you need to import is the `core` package. This package contains all the logic for creating and simulating cellular automata.

The `aditional_rules` package contains some aditional rules core contains only Conway's Game of Life. I recommend always importing even the additional rules and just deleting the ones you don't need, since they are not that big and you might want to use them in the future. + if you don't import thme you'll need to change the main a bit you'll figure it out.

`app_sdl` is the package that contains the code for the app itself. It uses SDL2 library to create a window and render the cellular automata. You can use this package as a reference for how to use the core package to create your own app if you want.

⚠️ **WARNING:** Important note about the `app_sdl` package.
The class `renderer` ended up with even some logic for the app itself which is design problem so be wary in using this. This is hopefully going to be fixed very soon (most likely after the bachelor thesis is done). I am very sorry for this but I had to do some quick fixes to get the app working so I could test the rules.

## Adding new rules

To add new rules to the app you need to create a new class that implements the `Rule` interface and also add it to the `RuleFactory` class, which you can do by adding this line to your rule. And then adding it to CmakeLists.txt. Make sure that the new rule is included somewhere. Either in main.cpp if you're using only core or in the all_aditional_rules.cpp if you're using the additional rules. Otherwise it won't be registered even if done correctly. The line to add is this:

```cpp
inline static AutoRegisterRule<YourClass> auto_register{"YourClassName", "Description describing your class"};
```

ofcource in the new rule you also need to implement the `apply` method which you can see in the `Rule` interface. For your convenience there is also a `ExampleRule` class in the `aditional_rules` package and you can basically just copy paste it and just change the name and the logic in the `apply` method.

## Building the app
To insall and run the app you need to have CMake and a C++ compiler installed on your system. (For windows I tested it using MSYS2 and MinGW-w64 and it worked fine). 

First you need to clone the repository or download the source code and navigate to the project folder:
```bash
git clone https://github.com/Rezabekfi/ca_tester.git 
git submodule update --init --recursive
cd ca_tester
```
Submodule is used for the SDL2 library which is required for the app to run. If you don't have git or you don't want to use it, you can also download the SDL2 library manually and place it in the `third_party` folder. Or just ask me for the compiled version of the app. If you're my opponent for the bachelor thesis, you already have the zip file with the source code and the compiled version of the app so you don't need to do this.

Then you can build the app using CMake:
```bash
cmake -S . -B build
cmake --build build
```

This will create a `build` folder and inside it you will find the executable file `ca_app` which you can run to start the app.

```bash
./build/ca_app
```
