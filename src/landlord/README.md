Landlord
========

Dou Dizhu is the most popular poker game in China, here is a C implementation  

Migrated from <https://github.com/master-g/Landlord> (branch `bleeding`, commit `b38315e`); the full history stays there. MIT licensed.

Run everything from the repo root:

- `make build TARGET=landlord && ./bin/landlord` — the benchmark: plays 10000 AI-vs-AI games and prints the win counts.
- `make test` — builds and runs `landlord_test`, the assert-based self-check (rules table + whole games by seed).
- `make landlord-asan` — runs the self-check under ASan/UBSan.
- `make build TARGET=landlord CMAKE_ARGS=-DLANDLORD_STRICT=ON` — builds with strict warnings.

The Lua and JavaScript bindings (`binding/`) were not migrated; they remain in the original repository.


**TODO:**  
beat might diffuse a bomb  
hand prompt  
end game strategy  
bid  
better AI  
interface / lib  
documentation  
optimization  
