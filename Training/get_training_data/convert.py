import os
from XQlightPy.position import Position,MIRROR_MOVE

def get_filepaths(dir,extension="cbf"):
    filepaths = []
    for file in os.listdir(dir):
        if file.endswith(extension):
            filepaths.append(os.path.join(dir,file))
        # if len(filepaths) > 8000:
        #     break
    return filepaths

def gen():
    def get_dump_line(mv,mvs):
        dump_line = f"{mv} <- "
        for legal_mv in mvs:
            dump_line += f"{legal_mv} "
        dump_line = dump_line.strip() + "\n"
        return dump_line

    filepaths = get_filepaths("D:\\dump_2","txt")
    for path in filepaths:
        with open(path,"r",encoding="utf-8") as f:
            lines = f.readlines()
            dump_lines = []
            mirror_dump_lines = []
            pos = Position()
            pos.fromFen("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1")
            for i,line in enumerate(lines):
                try:
                    line = line.strip()
                    #
                    mv = int(line)
                    mvs = pos.generateMoves()
                    #
                    mirrorMove = MIRROR_MOVE(mv)
                    mirrorMoves = [MIRROR_MOVE(move) for move in mvs]
                    #
                    dump_lines.append(get_dump_line(mv,mvs))
                    mirror_dump_lines.append(get_dump_line(mirrorMove,mirrorMoves))
                    #
                    pos.makeMove(mv)
                except:
                    break
            with open(f"D:\\dump_3\\{i}.txt","w+",encoding="utf-8") as f:
                f.writelines(dump_lines)
            with open(f"D:\\dump_3\\{i}_mirror.txt","w+",encoding="utf-8") as f:
                f.writelines(mirror_dump_lines)



if __name__ == "__main__":
    gen()