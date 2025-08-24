import os
import torch
from model import NNUE
from board import Situation, Red, Black

# 仅使用CPU
device = "cpu"

# 与训练参数保持一致
input_size = 7 * 9 * 10
model_path = "models/epoch_1.pth"

# 初始化模型
model = NNUE(input_size=input_size)
model.load_state_dict(torch.load(model_path, map_location=torch.device(device),weights_only=True))
model.to(device)
model.eval()

def evaluate(fen):
    sit = Situation(fen)
    input_tensor = torch.tensor(sit.matrix.copy(), dtype=torch.float32).view(1, -1).to(device)
    with torch.no_grad():
        output = model(input_tensor)  # [1, 2]
    flag = Red if 'w' in fen else Black
    score = output[0, 1 - flag].item() * 1000
    return score

# 示例
if __name__ == "__main__":
    fens = [
        ["rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR b - - 0 1","炮二平五"],
        ["rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/4C2C1/9/RNBAKABNR w - - 0 1","炮八平五"],
        ["rnbakab1r/9/1c5cn/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR w - - 0 1","炮二平五->马8进9"],
        ["rnbakab1r/9/1c4nc1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR w - - 0 1","炮二平五->马8进7"],
        ["rnbakab1r/9/1c5cn/p1p1C1p1p/9/9/P1P1P1P1P/1C7/9/RNBAKABNR b - - 0 1","炮二平五->马8进9->炮五进四"],
        ["rCbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/7C1/9/RNBAKABNR b - - 0 1","炮八进七"],
        ["1rbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/7C1/9/RNBAKABNR w - - 0 1","炮八进七->车2平1"],
        ["rnbakabnr/1N1R1R3/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/2BAKABN1 w - - 0 1","红方双车卡黑方九宫肋道，辅以红马叫杀"],
        ["2bakabn1/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/1n1r1r3/RNBAKABNR b - - 0 1","黑方双车卡红方九宫肋道，辅以黑马叫杀"],
        ["1Cbak4/9/3a5/p1p1c1p1p/5r3/2P1P4/P7P/9/4A4/2BK1Ar2 w - - 0 1","红方沉底炮，黑方双车带炮占大优"],
        ["2bak4/9/3a5/p1p1c1p1p/5r3/2P1P4/P7P/9/4A4/2BK1Ar2 w - - 0 1","去掉红方沉底炮，黑方双车带炮占大优"],
        ["r1bakabnr/9/1cn4c1/p1p1p1p1p/9/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1","兵七进一->马2进3"],
        ["rnbakab1r/9/1c4nc1/p1p1p1p1p/9/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1","兵七进一->马8进7"],
        ["rnbakabnr/9/1c5c1/p1p1p3p/6p2/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1","兵七进一->兵7进1"],
        ["rnbakabnr/9/1c7/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/1NBAKABN1 w - - 0 1","开局红方让双车，黑方让单炮"],
        ["3k5/9/9/9/9/9/9/4K4/4A4/4C4 w - - 0 1","残局，红帅升天居中，帅后店士，士后垫炮，除黑将外再无任何子力"],
        ["3k5/9/9/9/9/9/9/5K3/4A4/4C4 w - - 0 1","残局，红帅升天居右，帅后店士，士后垫炮，除黑将外再无任何子力"],
        ["3k5/9/9/9/9/9/9/9/4A4/4CK3 w - - 0 1","残局，红帅居起始位置右侧，帅后店士，士后垫炮，除黑将外再无任何子力"],
        ["rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1","开局尚未动任何一子"],
        ["rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/4K4/RNBA1ABNR b - - 0 1","帅五进一"],

    ]
    for fen,info in fens:
        score = evaluate(fen)
        flag = '红方' if 'w' in fen else '红方'
        print(f'{info} | {flag}先行动 => {score:.2f}')
