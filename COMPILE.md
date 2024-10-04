# Compilation Instructions

To compile the GIF player, use the following command:

```bash
g++ -std=c++17 -o gif_player gif_player.cpp `pkg-config --cflags --libs opencv4` -lcurl -pthread
