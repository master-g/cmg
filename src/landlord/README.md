Landlord
========

Dou Dizhu is the most popular poker game in China, here is a C implementation  

Migrated from <https://github.com/master-g/Landlord> (branch `bleeding`, commit `b38315e`); the full history stays there. MIT licensed.

Build and run from the repo root: `make build TARGET=landlord && ./bin/landlord` plays 10000 AI-vs-AI games and prints the win counts.

`binding/` holds the Lua and JavaScript bindings. They are not part of the build: `lualandlord.c` needs Lua headers and an `mt19937.h` that was never checked in.


**TODO:**  
beat might diffuse a bomb  
hand prompt  
end game strategy  
bid  
better AI  
interface / lib  
documentation  
optimization  
