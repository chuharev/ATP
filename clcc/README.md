## Clipboard Char Counter (CLCC)

### Description 

*Clcc* is a win32 application designed for quick counting chars in strings. The main goal of creating the program
was to simplify working with Valmet DNA Explorer (see snapshot.png for details). Concept and implementation
made by A. Chuharev, ATP team (Petrozavodsk, 2016).

### Using

Just run *clcc.exe* and look at the tray: you'll see 'HI!' string. You can now copy line to clipboard for counting number of chars. 
The limit of char number is 99. If string length is greater then '>L' is shown in the icon.

Feel free for getting source code of *clcc* under the terms of GPLv3!

### Technical details

[Mingw](http://www.mingw.org/) toolchain was used for developing. Cross-compilation was made on Ubuntu 16.04. 
*Clcc* does not use any frameworks (i.e. it works directly with win api) so it should be lightweight enough.
Also *clcc* uses unicode version of win api therefore troubles with non latin languages should not exist. 

### Sources

Here are some usefull links to sites used as examples of windows-programming:
 * [Win32 API Programming Tutorial](http://winprog.org/tutorial/);
 * [MSDN](https://msdn.microsoft.com/);
 * [Adding an icon system tray Win32 C++](https://bobobobo.wordpress.com/2009/03/30/adding-an-icon-system-tray-win32-c/).
 * [How to display text in system tray icon with win32 API?](https://stackoverflow.com/questions/457050/how-to-display-text-in-system-tray-icon-with-win32-api);

### Keywords

Char counting, mingw, win32 api, unicode, clipboard, tray icon.

