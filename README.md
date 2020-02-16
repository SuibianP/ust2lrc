# ust2lrc
## Introduction
A tool to generate [simple](https://en.wikipedia.org/wiki/LRC_(file_format)#Simple_format)/[enhanced](https://en.wikipedia.org/wiki/LRC_(file_format)#Enhanced_format) lyric file from existing UTAU project.

This program is programmed with compability in mind, i.e., using only platform-independent C standard functions. However, kindly notice that [strdup\(\)](https://en.cppreference.com/w/c/experimental/dynamic/strdup) is only merged into mainline ISO C standard since C2x.

Throretically, it could be utilised as an UTAU plugin as well, whose plugin.txt will be released later. FYI, as a plugin, statement of "[notes=all](https://w.atwiki.jp/utaou/pages/64.html#id_0439d63b)" in plugin.txt or selecting all before execution is compulsory, or the lyric of \[\#NEXT\] will overwrite the lyric of its preceding note.
## Functionality
This program runs in a shell environment. In Windows it could be executed in command prompt or ust file drag-and-drop.

Usage syntax: `ust2lrc <ust file> [[-a|-b] [<delimiters> [<threshold>]]]`<br />
Examples: `ust2lrc source.ust -a R,r 240` `ust2lrc this.ust -a "a R,i R,u R,e R,o R"` `ust2lrc input.ust` `ust2lrc whatever.ust -b`

`-a` refers to enhanced lyric output, where the delimiters of each lyric line (and their length threshold) specified in the succeeding arguments.<br />
`-b` refers to simple lyric output, with each note treated as a separate lyric line.

If the third argument is not specified, the program will interact with the user to acquire necessary arguments via command line.
## Acknowledgements
**Rye-Catcher** for kind guidance regarding memory allocation<br />
**Ray Lum** for the constructive idea of interactive operation
