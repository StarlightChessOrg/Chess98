import os
import copy
import numpy as np

[K,S,X,M,C,P,B] = [1,2,3,4,5,6,7]
[k,s,x,m,c,p,b] = [-1,-2,-3,-4,-5,-6,-7]

init_board = [
    [C,0,0,B,0,0,b,0,0,c],
    [M,0,P,0,0,0,0,p,0,m],
    [X,0,0,B,0,0,b,0,0,x],
    [S,0,0,0,0,0,0,0,0,s],
    [K,0,0,B,0,0,b,0,0,k],
    [S,0,0,0,0,0,0,0,0,s],
    [X,0,0,B,0,0,b,0,0,x],
    [M,0,P,0,0,0,0,p,0,m],
    [C,0,0,B,0,0,b,0,0,c],
]

init_board = np.asarray(init_board,dtype=np.int32)

[Red,Black] = [1,-1]

def get_all_legal_moves():
    _move_id2move_action = {}
    _move_action2move_id = {}
    row = ['0', '1', '2', '3', '4', '5', '6', '7', '8']
    column = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
    # 士的全部走法
    advisor_labels = ['0314', '1403', '0514', '1405', '2314', '1423', '2514', '1425',
                      '9384', '8493', '9584', '8495', '7384', '8473', '7584', '8475']
    # 象的全部走法
    bishop_labels = ['2002', '0220', '2042', '4220', '0224', '2402', '4224', '2442',
                     '2406', '0624', '2446', '4624', '0628', '2806', '4628', '2846',
                     '7052', '5270', '7092', '9270', '5274', '7452', '9274', '7492',
                     '7456', '5674', '7496', '9674', '5678', '7856', '9678', '7896']
    idx = 0
    for l1 in range(10):
        for n1 in range(9):
            destinations = [(t, n1) for t in range(10)] + \
                           [(l1, t) for t in range(9)] + \
                           [(l1 + a, n1 + b) for (a, b) in
                            [(-2, -1), (-1, -2), (-2, 1), (1, -2), (2, -1), (-1, 2), (2, 1), (1, 2)]]  # 马走日
            for (l2, n2) in destinations:
                if (l1, n1) != (l2, n2) and l2 in range(10) and n2 in range(9):
                    action = column[l1] + row[n1] + column[l2] + row[n2]
                    _move_id2move_action[idx] = action
                    _move_action2move_id[action] = idx
                    idx += 1

    for action in advisor_labels:
        _move_id2move_action[idx] = action
        _move_action2move_id[action] = idx
        idx += 1

    for action in bishop_labels:
        _move_id2move_action[idx] = action
        _move_action2move_id[action] = idx
        idx += 1

    return _move_id2move_action, _move_action2move_id


move_id2move_action, move_action2move_id = get_all_legal_moves()

def flipLR(board : np.ndarray):
    return -np.fliplr(board)

def flipUD(board : np.ndarray):
    return np.flipud(board)

def flipLRUD(board : np.ndarray):
    return np.flipud(-np.fliplr(board))

def get_filepaths(directory,extension="txt"):
    filepaths = []
    for root,dirs,files in os.walk(directory):
        for _file in files:
            if _file.endswith(extension):
                filepaths.append(os.path.join(root,_file))
    print("size of filepaths = ",len(filepaths))
    return filepaths

def mv_to_xy(mv):
    src = mv & 255
    dst = mv >> 8
    xSrc = (src & 15) - 3
    ySrc = 12 - (src >> 4)
    xDst = (dst & 15) - 3
    yDst = 12 - (dst >> 4)
    return xSrc,ySrc,xDst,yDst

def get_data(recoard_path : str):
    inputs = []
    input_sides = []
    label_indexs = []
    side = Red
    board = copy.deepcopy(init_board)
    with open(recoard_path,"r",encoding="utf-8") as f:
        lines = f.readlines()
        for index,line in enumerate(lines):
            line = line.strip()
            split_items = line.split(" <- ")
            mv = split_items[0]
            mvs = split_items[1].split(" ")
            #
            mv = int(mv)
            mvs = [int(move) for move in mvs]
            #
            index = mvs.index(mv)
            mvs[0],mvs[index] = mvs[index],mvs[0]
            #
            input = [copy.deepcopy(board) for _ in range(len(mvs))]
            for i in range(len(input)):
                _xSrc,_ySrc,_xDst,_yDst = mv_to_xy(mvs[i])
                input[i][_xDst][_yDst] = input[i][_xSrc][_ySrc]
                input[i][_xSrc][_ySrc] = 0
            #
            inputs.append(np.asarray(input,dtype=np.int32))
            label_indexs.append(0)
            input_sides.append(copy.deepcopy(side))
            #
            xSrc,ySrc,xDst,yDst = mv_to_xy(mv)
            board[xDst][yDst] = board[xSrc][ySrc]
            board[xSrc][ySrc] = 0
            side = -side
    input_sides = np.asarray(input_sides,dtype=np.int32)
    label_indexs = np.asarray(label_indexs,dtype=np.int32)
    return inputs,input_sides,label_indexs

def clear():
    filepaths = get_filepaths(r"D:\dump_3", "txt")
    for path in filepaths:
        try:
            get_data(path)
        except:
            print(f"remove bad case {path}")
            os.remove(path)

if __name__ == "__main__":
    #get_data(r"D:\dump_3\10000.txt")
    clear()