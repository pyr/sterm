sterm
=====

Custom terminal based on VTE. There are many terminals available. Many
of them are based on VTE, a library implementing a terminal emulator
widget for GTK+. Some are highly configurable, some are small but none
of them match exactly my expectations.

Here is a short survey:

 - [ROXterm](http://roxterm.sourceforge.net/)
 - [Sakura](http://pleyades.net/david/sakura)
 - [Termit](http://github.com/nonstop/termit/wiki)
 - [evilvte][]
 - [gnome-terminal](http://en.wikipedia.org/wiki/gnome_terminal)
 - [lilyterm](http://lilyterm.luna.com.tw/)
 - [st](http://st.suckless.org/)
 - [xfce-terminal](http://www.xfce.org/projects/terminal/)
 - [vbeterm](https://github.com/vincentbernat/vbeterm)

`sterm` is very lightweight, do not expect much from it, it's
a derivation of `vbeterm`.

Features
--------

 - No tab support
 - No server support
 - Use of VTE 2.90 (GTK3)
 - Single text file configuration
 - Color-theme support

Example Configuration
---------------------

```
[main]
WordChars=-A-Za-z0-9:./?%&#_=+@~" 
Themes=SolarizedDark;SolarizedLight

[SolarizedDark]
Font=Terminus 12
Opacity=1.0
Bold=false
Cursor=#0f0f49499999
Foreground=#838394949696
Background=#00002b2b3636
Palette=#070736364242;#dcdc32322f2f;#858599990000;#b5b589890000;#26268b8bd2d2;#d3d336368282;#2a2aa1a19898;#eeeee8e8d5d5;#00002b2b3636;#cbcb4b4b1616;#58586e6e7575;#65657b7b8383;#838394949696;#6c6c7171c4c4;#9393a1a1a1a1;#fdfdf6f6e3e3

[SolarizedLight]
Font=Terminus 12
Opacity=1.0
Bold=false
Cursor=#586e75
Foreground=#657b83
Background=#fdf6e3
Palette=#eee8d5;#dc322f;#859900;#b58900;#268bd2;#d33682;#2aa198;#073642;#fdf6e3;#cb4b16;#93a1a1;#839496;#657b83;#6c71c4;#586e75;#002b36
```

Installation
------------

Execute the following commands:

    $ ./configure
    $ make
    $ sudo make install
