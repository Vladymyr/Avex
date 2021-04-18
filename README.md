<h1 align="center">
  Avex
</h1>

<h4 align="center">Another way to delete your data</a>.</h4>

<p align="center">
  <img src="https://img.shields.io/badge/Windows-0078D6?style=flat&logo=windows&logoColor=white">
  <img src="https://img.shields.io/github/v/release/Vladymyr/avex">
  <img src="https://img.shields.io/github/license/Vladymyr/avex">
</p>

<p align="center">
  <a href="#introduction">Introduction</a> •
  <a href="#how-to-use">How To Use</a> •
  <a href="#download">Download</a> •
  <a href="#credits">Credits</a> •
  <a href="#contributing">Contributing</a> •
  <a href="#license">License</a> 
</p>

Avex is a simple tool aiming to remove data from your hard drive making it unrecoverable. This project is to practice C++ as I'm still learning. **If you're looking for a reliable tool, I highly recommend checking out alternatives such as [Eraser](https://eraser.heidi.ie/), [SDelete](https://docs.microsoft.com/en-us/sysinternals/downloads/sdelete) and many others.**

## Introduction
There is an issue with normal deletion. Deleting something in OS such as Windows doesn't actually mean it's gone, even if you empty the Recycle Bin. The data is still on the disk, unless it got overwritten but even then, professional recovery tools out there can restore it to some degree.

## How to use
Usage: avex [-p file/directory] [-ps passes] [-r] [-s]

| Parameter | Description |
| ------------- | ------------- |
| -p | Path to a file/folder for erasion  |
| -ps | Number of times the deleted data will be overwritten (default: 1). If no overwriting is wanted, set the value to 0 or below  |
| -s | Erase the subdirectories, the content inside these and the targeted directory itself  |
| -r | Rename the directories/folders before overwriting and removing them  |

## Download

You can [download](https://github.com/Vladymyr/Avex/releases/tag/1.0) the latest version of Avex for Windows.

## Credits

This software uses the following open source packages:

- [Loguru - logging library](https://github.com/emilk/loguru)
- [CmdParser](https://github.com/FlorianRappl/CmdParser)

## Contributing
Pull requests and/or optimization proposals are welcome. For major changes, please open an issue first to discuss what you would like to change and the reason of it.

## License
>You can check out the full license [here](https://github.com/Vladymyr/Avex/blob/main/LICENSE)

This project is licensed under the terms of the [**GNU GPLv3**](https://choosealicense.com/licenses/gpl-3.0/). Copyright and license notices must be preserved for forks.
