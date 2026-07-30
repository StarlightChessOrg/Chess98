# Chess98

此项目正在进行重构...

This project is being refactored now...

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
- [ ] ucci

## Progress

The progress is measured under MSVC Release Compilation:

- position: enhanced position structure
- heuristic: the simplified styles
- movegen: 239017 -> 1292404 ~5.4x faster than it used to be
- movepicker: more optimized than the past all-in-one move generation
