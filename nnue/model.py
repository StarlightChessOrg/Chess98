import torch
import torch.nn as nn

class NNUE(nn.Module):
    def __init__(self, input_size=7 * 9 * 10):
        super(NNUE, self).__init__()
        self.input_size = input_size

        self.fc = nn.Sequential(
            nn.Linear(in_features=input_size, out_features=256),
            nn.ReLU(),
            nn.Linear(in_features=256, out_features=32),
            nn.ReLU(),
            nn.Linear(in_features=32, out_features=32),
            nn.ReLU(),
            nn.Linear(in_features=32, out_features=2)
        )

    def forward(self, x):
        return self.fc(x)
