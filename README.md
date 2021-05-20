# FLOWER POWER  

Using Cmake as a build tool with WSL Ubuntu 20.04 bash in a VSCode editor.  

## Installing prerequisites

1. Firstly, update your apt  
```
sudo apt update  
sudo apt upgrade  
```  
2. Install pistache as described [here](https://github.com/pistacheio/pistache)  
3. Install cmake and make  
```
sudo apt install make  
sudo apt install cmake  
```  
## Build  

Go to the /build folder and enter `cmake .. ; make` in the bash terminal.

## Running the binary

1. In build/demo/ the file named `main` is our binary executable.
2. Enter `./main` to run our binary file.
3. In your browser go to `localhost:9080/test` and see if it works.

## Testing if it the settings work (if you don't know how, of course).

1. Open a new bash terminal so we can make some curl requests (but keep the old terminal with the server running).
2. Type  `curl -X GET http://localhost:9080/settings/soilType`, you should receive the answer "soilType is Negru".
3. Type `curl -X PUT http://localhost:9080/settings/soilType/Roz`, you should receive "soilType was set to Roz".
4. Try some setting that do not exist, like `curl -X GET http://localhost:9080/settings/mortiSiRanitiInGhiveci`, you should receive "mortiSiRanitiInGhiveci was not found".  

## How to add code?

As long as you don't add files or add god knows what weird libraries, you can simple go to the build/ folder and run `make` after each change (we don't have to run `cmake ..` again) and the code will compile with the last changes.  

 But if you want to add files to be compiled or additional libraries, we'll need to get our hands dirty and touch the CMakeLists.txt files.

##### Installing libraries:
 You can install the library in ubuntu and link them in demo/CMakeLists.txt in the function `target_link_libraries(main SmartPotLib pistache crypto ssl pthread DESIRED_INSTALLED_LIBRARY)`. DONE  
 If you need to download the library folder and use it in our programme separately from the ubuntu then I'll look into it another time cuz it's late now and I need to test this approach.

##### Cleaning the build
For now you can simply delete ALL the contents of build/ and rerun the cmake for a fresh build.

#### GG
