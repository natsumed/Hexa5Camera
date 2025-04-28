```markdown
# JoystickIdentifier

A small Qt6 application that:

- Shows an RTSP camera stream in a widget  
- Lets you control a PTZ (pan-tilt-zoom) camera with keyboard or a gamepad  
- Detects USB joysticks and shows axis/button state  
- Sends commands over UDP (via the SIYI SDK) to move the camera

---

## Features

- **Live video** from RTSP (`rtsp://<IP>:<port><path>`)  
- **Keyboard control** (Z/S for tilt, Q/D for pan, +/- for zoom)  
- **Joystick support** via SDL (axis & button mapping)  
- **Camera status** indicator (Working / Not Working)  
- **Clean shutdown** (exit button or Ctrl+C kills any leftover processes)  

---

## Requirements

- **Qt 6** (Widgets, Multimedia, MultimediaWidgets, Core)  
- **CMake** ≥ 3.16  
- **GStreamer 1.0** (core + good/bad/ugly plugins + libav)  
- **SDL2** (for joystick support)  
- **X11** development libraries  
- **A C++17-capable compiler** (GCC ≥ 9, Clang ≥ 10, MSVC 2019+)  

On Ubuntu you can install deps with:

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-multimedia-dev \
                 gstreamer1.0-tools gstreamer1.0-plugins-{good,bad,ugly,libav} \
                 libsdl2-dev libx11-dev
```

---

## Building natively

1. Clone this repo:
   ```bash
   git clone https://github.com/your-user/JoystickIdentifier.git
   cd JoystickIdentifier
   ```

2. Create a build directory and run CMake:
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make -j$(nproc)
   ```

3. Run the app:
   ```bash
   ./JoystickIdentifier
   ```

---

## Building with Docker / AppImage

We provide a `run-docker.sh` script that:

- Builds the project inside Ubuntu 22.04  
- Uses linuxdeploy to bundle into an AppImage  

Simply:

```bash
chmod +x run-docker.sh
./run-docker.sh
```

When it finishes, you’ll find `JoystickIdentifier-<version>.AppImage` in `build/`. You can copy it to any Linux laptop and run it directly.

---

## Configuration

By default your camera URI is stored in:

```
~/.config/Haxa5Camera/Hexa5CameraConfig.json
```

You can edit that JSON or use the **Save Config** / **Save Default Config** buttons in the UI to change:

- IP address  
- RTSP port  
- Stream path (e.g. `/main.264`)  

---

## Usage

1. Launch the app.  
2. If the camera is reachable, you’ll see **“Camera Working”** in green. Otherwise **“Camera Not Working”** in red.  
3. Switch to **Keyboard** or **Joystick** mode with the two buttons.  
4. Use Z/S/Q/D (or arrows on a gamepad) to pan/tilt, +/- to zoom.  
5. Close the window or press Ctrl+C—any leftover processes are automatically killed.  

---

## License

This project is released under the [MIT License](LICENSE).  
Feel free to copy, modify, and distribute!  
```
