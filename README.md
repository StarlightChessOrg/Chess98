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
- [ ] search: filter self-check in main search (not only qsearch)
- [ ] search: store root bestmove (do not rely only on tt_get_move)
- [ ] search: LMR (late move reductions)
- [ ] search: check extensions
- [ ] search: aspiration windows at root
- [ ] search: IID when no TT move
- [ ] search: countermove / continuation history
- [ ] search: ProbCut / singular extensions (later)
- [ ] ucci

## Progress

The progress is measured under MSVC Release Compilation:

- position: from shit to excellence
- heuristic: the simplified styles
- movegen: 239017 -> 847821 ~3x faster than it used to be
- movepicker: more optimized than the past all-in-one move generation
