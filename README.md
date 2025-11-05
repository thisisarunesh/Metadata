## About

A simple command-line tool to extract video and audio stream metadata from multimedia files using FFmpeg libraries. 

## Requirements
- C compiler (`gcc`)  
- FFmpeg development libraries (`libavformat`, `libavcodec`, `libavutil`)  

## Usage
To compile:
```bash
gcc Metadata.c -o Metadata $(pkg-config --cflags --libs libavformat libavcodec libavutil)
```
To extract metadata:
```bash
./Metadata [ input ]
```