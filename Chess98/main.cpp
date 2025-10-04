#include "test.hpp"
#include "genfiles.hpp"

void testModelInference(torch::jit::script::Module& model) {
    try {
        // 1. 生成随机输入张量 [1, 630] (与Python的torch.randn(1, 630)等价)
        torch::Tensor test_input = torch::randn({ 1, 630 }, torch::kFloat32);

        // 2. 运行模型推理
        torch::NoGradGuard no_grad;
        torch::Tensor test_output = model.forward({ test_input }).toTensor();

        // 3. 提取并打印输出结果 (与Python的test_output[0].tolist()等价)
        auto output_accessor = test_output.accessor<float, 2>(); // [1, 2]的二维访问器
        float first_val = output_accessor[0][0];
        float second_val = output_accessor[0][1];

        // 4. 格式化输出
        std::cout << "C++ output: [" << first_val << ", " << second_val << "]" << std::endl;

        // 可选：打印更多调试信息
        std::cout << "Input shape: " << test_input.sizes() << std::endl;
        std::cout << "Output shape: " << test_output.sizes() << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Test inference failed: " << e.what() << std::endl;
    }
}

int main()
{
    NNUE model = NNUE("./nnue.pt");
    //testModelInference(model.model);
    auto res = model.evaluate(fenToPieceidMap("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1"), RED);
    std::cout << res << std::endl;
#ifndef GENFILES
    testByUI();
#else
    genfiles();
#endif

    return 0;
}
