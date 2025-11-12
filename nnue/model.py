import torch
import torch.nn as nn
import numpy as np

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

    def dump_to_txt(self, prefix=""):
        """
        将模型的权重和偏置导出为 TXT 文件，格式如下：
        - layer_1_weights.txt: 每行是一个输入特征对应的 256 维权重向量（共 input_size 行）
        - layer_1_biases.txt:  每行一个偏置值（共 256 行）
        - layer_2_weights.txt: 每行是一个 32 维权重向量（共 256 行）
        - ...以此类推

        权重已转置为 (in_features, out_features) 布局，便于 C++ 稀疏加减。
        """
        state = self.state_dict()

        # 定义层映射：(weight_key, bias_key) -> layer_name
        layers = [
            ('fc.0.weight', 'fc.0.bias', 'layer_1'),
            ('fc.2.weight', 'fc.2.bias', 'layer_2'),
            ('fc.4.weight', 'fc.4.bias', 'layer_3'),
            ('fc.6.weight', 'fc.6.bias', 'layer_4'),
        ]

        for w_key, b_key, name in layers:
            w = state[w_key].T.numpy()  # 转置为 (in_features, out_features)
            b = state[b_key].numpy()    # (out_features,)

            # 保存权重：每行 = 一个输入维度对应的输出权重向量
            with open(f"{prefix}{name}_weights.txt", "w") as f:
                for row in w:
                    f.write(" ".join(f"{val:.9e}" for val in row) + "\n")

            # 保存偏置：每行一个值
            with open(f"{prefix}{name}_biases.txt", "w") as f:
                for val in b:
                    f.write(f"{val:.9e}\n")

        print(f"✅ 模型参数已导出到 TXT 文件（前缀: '{prefix}'）")


if __name__ == "__main__":
    model = NNUE()
    model.load_state_dict(torch.load("models/epoch_2025_9_20.pth","cpu",weights_only=True))
    model.dump_to_txt(prefix="")
