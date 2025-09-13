import torch
import torch.nn as nn
import numpy as np
import random

# NNUE 模型类定义... (保持不变)
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

def organize_weights_for_manual_inference_numpy(model: NNUE):
    """
    将 PyTorch 模型的权重和偏置重新组织成 NumPy 数组。

    Returns:
        dict: 包含 NumPy 数组的字典。
    """
    state_dict = model.state_dict()
    organized_data = {}

    w0_pt, b0_pt = state_dict['fc.0.weight'], state_dict['fc.0.bias']
    w1_pt, b1_pt = state_dict['fc.2.weight'], state_dict['fc.2.bias']
    w2_pt, b2_pt = state_dict['fc.4.weight'], state_dict['fc.4.bias']
    w3_pt, b3_pt = state_dict['fc.6.weight'], state_dict['fc.6.bias']

    # 转换为 NumPy 数组
    organized_data['layer_1'] = {
        'weights': w0_pt.T.numpy(),
        'biases': b0_pt.numpy()
    }
    organized_data['layer_2'] = {
        'weights': w1_pt.T.numpy(),
        'biases': b1_pt.numpy()
    }
    organized_data['layer_3'] = {
        'weights': w2_pt.T.numpy(),
        'biases': b2_pt.numpy()
    }
    organized_data['layer_4'] = {
        'weights': w3_pt.T.numpy(),
        'biases': b3_pt.numpy()
    }

    return organized_data


def manual_nnue_inference_numpy(organized_params, input_data):
    """
    使用 NumPy 实现高效推理。
    """
    def relu(val):
        return np.maximum(0, val)

    if isinstance(input_data, torch.Tensor):
        x = input_data.flatten().numpy()
    else:
        x = np.array(input_data)

    # --- 第一层: 高效的 NumPy 广播操作 ---
    w0_manual = organized_params['layer_1']['weights']
    b0_manual = organized_params['layer_1']['biases']

    # 找到非零输入的索引
    active_indices = np.nonzero(x)[0]

    # 初始化输出向量为偏置
    output_l1 = b0_manual.copy()

    # 遍历非零输入的索引，进行向量加减
    for i in active_indices:
        if x[i] == 1:
            output_l1 += w0_manual[i]
        elif x[i] == -1:
            output_l1 -= w0_manual[i]

    # 应用 ReLU 激活函数
    output_l1 = relu(output_l1)
    x = output_l1

    # --- 剩余层: 使用 NumPy 的点积运算 ---
    # 第二层 (256 -> 32)
    w1_manual = organized_params['layer_2']['weights']
    b1_manual = organized_params['layer_2']['biases']
    output_l2 = np.dot(x, w1_manual) + b1_manual
    output_l2 = relu(output_l2)
    x = output_l2

    # 第三层 (32 -> 32)
    w2_manual = organized_params['layer_3']['weights']
    b2_manual = organized_params['layer_3']['biases']
    output_l3 = np.dot(x, w2_manual) + b2_manual
    output_l3 = relu(output_l3)
    x = output_l3

    # 最终层 (32 -> 2)
    w3_manual = organized_params['layer_4']['weights']
    b3_manual = organized_params['layer_4']['biases']
    final_output = np.dot(x, w3_manual) + b3_manual

    return final_output.tolist()


if __name__ == "__main__":
    # 1. 实例化模型并获取组织好的 NumPy 参数
    model = NNUE()
    organized_params = organize_weights_for_manual_inference_numpy(model)

    # 2. 创建一个稀疏输入样本
    input_data = torch.zeros(7 * 9 * 10, dtype=torch.float32)
    input_data[0] = 1.0
    input_data[1] = 1.0
    input_data[5] = -1.0

    print("输入的非零位置:", torch.nonzero(input_data).flatten().tolist())

    # 3. 使用 NumPy 手动推理函数进行计算
    manual_result = manual_nnue_inference_numpy(organized_params, input_data)
    print("\nNumPy 手动推理结果:", [f"{x:.6f}" for x in manual_result])

    # 4. 使用 PyTorch 前向传播进行验证
    with torch.no_grad():
        pytorch_result = model(input_data).tolist()
    print("PyTorch 推理结果:", [f"{x:.6f}" for x in pytorch_result])

    # 5. 验证结果
    is_correct = all(abs(m - p) < 1e-6 for m, p in zip(manual_result, pytorch_result))

    if is_correct:
        print("\n✅ 验证成功：NumPy 推理结果与 PyTorch 结果一致。")
    else:
        print("\n❌ 验证失败：NumPy 推理结果与 PyTorch 结果不一致。")
        diffs = [f"{abs(m - p):.10f}" for m, p in zip(manual_result, pytorch_result)]
        print("差异值:", diffs)
