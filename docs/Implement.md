# Implement

Listed below are the ways to incorporate this library into supported development environments. All guides assume that a project has already been created in each development environment.

- [Arduino IDE](#arduino-ide)
- [ESP-IDF](#esp-idf)
- [Raspberry Pi Pico SDK](#raspberry-pi-pico-sdk)
- [PlatformIO](#platformio)

## Arduino IDE

Arduino IDE manages external libraries by downloading and sourcing through the built in library manager.

- Download the repository as a `.zip` file
- In Arduino IDE, Navigate to `Sketch` > `Include Library` > `Add .ZIP Library...`
- Select the `.zip` file downloaded

## ESP-IDF

ESP-IDF manages external libraries through its `components` folder, automatically sourcing libraries in the folder.

- Create a `components` folder in the `root` folder of the project
- Download the repository
  - Cloning
    - Clone the repository in the `components` folder
  - Downloading .zip
    - Download the repository as a `.zip` file
	- Extract the `.zip` file into the `components` folder

## Raspberry Pi Pico SDK

Raspberry Pi Pico SDK manages external libraries through linking with CMake.

- Download the repository
  - Cloning
    - Clone the repository in the `root` folder
  - Downloading .zip
    - Download the repository as a `.zip` file
	- Extract the `.zip` file into the `root` folder
- Add the following to the root `CMakeLists.txt` file:

```cmake
# ... Begining of file ...

# Included in given CMakeLists.txt
pico_sdk_init()

add_subdirectory(universal_hardware_timer) # Looks in universal_hardware_timer folder

# Choose one linking method below

# Adds universal_hardware_timer to all targets
link_libraries(
    universal_hardware_timer
)
# Adds universal_hardware_timer to specific target
target_link_libraries([Target_Name] # Replace [Target_Name] with real target name
	universal_hardware_timer
)

# ... Rest of file ...
```

## PlatformIO

PlatformIO manages external libraries through it's `platformio.ini` file, automatically sourcing libraries listed as dependencies.

- Add `https://github.com/CamrenChraplak/universal_hardware_timer` under `lib_deps` in `platformio.ini`

```ini
; ... Begining of file ...

lib_deps =
	; Other dependencies
	https://github.com/CamrenChraplak/universal_hardware_timer

; ... Rest of file ...
```