# ChessMate
Creation of repository
The ChessMate is an AVR ATmega328P running at 25 MHz with chess Engine based on recursive negamax search micro-Max 4.8 by 
H.G. Muller [1]
The hash table was removed by Andre Adrian [2]


Features included:
- Included Threefold repetition rule using Compact Chess Representation of history boards (C.C.R.) [3], we made it valid 
for the last 15 boards - 32 bytes per board so the history boards occupies 480 bytes of RAM.
- Included the Fifty-move rule check (from fairy-Max [4]).

Processing Speed:
For instance, the 32-bit test version of Rybka on AMD 64-bit CPU running on 2.4Ghz achieves 104 Knps (thousands of nodes 
per second) [3].


The ATmega328P ChessMate at 25 MHz approach, can reach ~1.3 Knps (1298.1 nodes/s).

A node refers to a branch of play the computer investigated. 

- [1]: <Micro-Max, a 133-line Chess Source http://home.hccnet.nl/h.g.muller/max-src2.html>
- [2]: http://chessprogramming.wikispaces.com/Andre+Adrian
- [3]: Vladan VUCKOVIC, "AN ALTERNATIVE EFFICIENT CHESSBOARD REPRESENTATION BASED ON 4-BIT PIECE CODING"
- [4]: Fairy-Max: an AI for playing user-defined Chess variants (http://home.hccnet.nl/h.g.muller/CVfairy.html)

> Soon we will post the first working release :)
