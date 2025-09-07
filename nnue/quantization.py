import torch
import torch.nn as nn
from torch.utils.data import DataLoader
from model import NNUE, qNNUE
from board import Situation, Red, Black
from train import NNUEDataset, collect_json_files, create_dataloader, public_device
import os
import copy
import torch.ao.quantization as quantization

def quantize_and_save_model(model_path, data_dir, output_path):
    # 1. 加载原始NNUE模型权重
    original_model = NNUE(input_size=7 * 9 * 10).to('cpu')
    original_model.load_state_dict(torch.load(model_path, map_location='cpu'))
    original_model.eval()

    # 2. 实例化qNNUE模型并加载NNUE的权重
    quant_model = qNNUE(input_size=7 * 9 * 10).to('cpu')
    quant_model.load_weights_from_nnue(original_model)
    quant_model.eval()

    # 3. 模块融合以提高量化性能
    fused_model = quantization.fuse_modules(
        quant_model,
        [['linear1', 'relu1'], ['linear2', 'relu2'], ['linear3', 'relu3']],
        inplace=False
    )

    # 4. 配置和准备量化 (使用手动配置以消除警告)
    quant_config = torch.ao.quantization.QConfig(
        activation=torch.ao.quantization.MinMaxObserver.with_args(
            qscheme=torch.per_tensor_affine,
            dtype=torch.quint8,
            quant_min=0,
            quant_max=255
        ),
        weight=torch.ao.quantization.MinMaxObserver.with_args(
            qscheme=torch.per_tensor_symmetric,
            dtype=torch.qint8,
            quant_min=-128,
            quant_max=127
        )
    )
    fused_model.qconfig = quant_config
    quantization.prepare(fused_model, inplace=True)

    # 5. 校准
    json_files = collect_json_files(root_path=data_dir, num=50)
    calibration_dataloader = create_dataloader(json_files, batch_size=128, num_workers=1)

    with torch.no_grad():
        for i, (x_batch, _, _) in enumerate(calibration_dataloader):
            fused_model(x_batch)
            if i >= 10:
                break

    # 6. 转换模型
    quantized_model = quantization.convert(fused_model, inplace=False)

    # 7. JIT Trace并保存模型
    dummy_input = torch.randn(1, 7 * 9 * 10).to('cpu')
    scripted_model = torch.jit.trace(quantized_model, dummy_input)
    scripted_model.save(output_path)

    print(f"Quantized and scripted model saved to: {output_path}")

    orig_size = os.path.getsize(model_path) / 1e6
    quant_size = os.path.getsize(output_path) / 1e6
    print(f"\nOriginal model size: {orig_size:.2f} MB")
    print(f"Quantized model size: {quant_size:.2f} MB")
    print(f"Size reduction: {(1 - quant_size/orig_size) * 100:.2f}%")

if __name__ == "__main__":
    trained_model_path = "models/epoch_1.pth"
    data_directory = "nnue/data/"
    output_model_path = "models/nnue_quantized.pt"

    if not os.path.exists(trained_model_path):
        print(f"Error: The trained model file '{trained_model_path}' was not found.")
    else:
        try:
            quantize_and_save_model(
                model_path=trained_model_path,
                data_dir=data_directory,
                output_path=output_model_path
            )
        except Exception as e:
            print(f"An error occurred during quantization: {e}")
