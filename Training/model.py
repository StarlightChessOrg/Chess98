import copy
import numpy as np
import torch
import torch.nn as nn
from convert import *

public_device = 'cuda' if torch.cuda.is_available() else 'cpu'
print(f"public device is {public_device}")

class nnue(nn.Module):
    def __init__(self):
        super(nnue,self).__init__()
        self.fc1 = nn.Linear(in_features=7 * 90 + 1,out_features=256)
        self.fc2 = nn.Linear(in_features=256,out_features=32)
        self.fc3 = nn.Linear(in_features=32,out_features=32)
        self.fc4 = nn.Linear(in_features=32,out_features=1)
        self.relu = nn.ReLU()

    def forward(self,x):
        y = self.fc1(x)
        y = self.relu(y)
        #first output
        y = self.fc2(y)
        y = self.relu(y)
        y = self.fc3(y)
        y = self.relu(y)
        y = self.fc4(y)
        return y

    def convert_board_to_x(self,board : np.ndarray,side):
        input = np.zeros(shape=7 * 90 + 1,dtype=np.float32)
        if side == Black:
            input[-1] = 1
        for x in range(9):
            for y in range(10):
                p = board[x][y]
                if p < 0:
                    input[(abs(p) - 1) * 90 + x * 10 + y] = -1
                elif p > 0:
                    input[(p - 1) * 90 + x * 10 + y] = 1
        input = torch.from_numpy(input)
        return input.to(public_device)

    def convert_boards_to_xs(self,boards,sides):
        inputs = []
        for i in range(len(boards)):
            inputs.append(self.convert_board_to_x(boards[i],sides[i]))
        inputs = torch.stack(inputs,dim=0)
        return inputs.to(public_device)

def check_trained_model_output():
    model = nnue().to(public_device)
    weights = torch.load("model_epoch_1_train_acc_0.1648_train_loss_3.0780_test_acc_0.1729_test_loss_3.1130.pth")
    model.load_state_dict(weights)
    model.eval()
    #
    check_board = [
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
    check_board = np.asarray(check_board,dtype=np.int32)
    input = model.convert_board_to_x(check_board,Red).to(public_device)
    y = model(input)
    print(float(y))


if __name__ == "__main__":
    check_trained_model_output()