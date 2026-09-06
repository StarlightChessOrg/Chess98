# Chess98

Chess98的新版本

## TODO

- [x] basic
- [x] position do undo move
- [x] history tt and killer
- [x] gen moves
- [x] bitboard
- [x] legal move and pos protector, in check validation
- [x] fix bugs of cannon & rook move generator and improvements
- [x] MVV/LVA and SEE
- [x] move picker
- [x] basic evaluation
- [x] search
- [x] ucci

## Progress

The progress is measured under Clang++ O3 Compilation:

- position: enhanced the performance and the api
- heuristic: applied a more simple interfaces
- movegen: 239017 -> 7928229 ~33.2x faster than old version
- movepicker: never run a move generation logic in your search
