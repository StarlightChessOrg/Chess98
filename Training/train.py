import os
import torch
import torch.nn as nn
import numpy as np
from tqdm import tqdm
from model import nnue
from convert import get_filepaths, get_data

public_device = 'cuda' if torch.cuda.is_available() else 'cpu'

def train():
    model = nnue().to(public_device)
    cross_loss = nn.CrossEntropyLoss().to(public_device)
    opt = torch.optim.RAdam(model.parameters(), lr=3e-4)

    filepaths = get_filepaths(r"D:\dump_3", "txt")
    split_idx = int(0.9 * len(filepaths))
    train_filepaths = filepaths[:split_idx]
    test_filepaths = filepaths[split_idx:]

    for epoch in range(10000):
        all_train_loss = 0
        all_train_acc = 0
        all_train_num = 0

        model.train()  # Set the model to training mode

        with tqdm(total=len(train_filepaths), desc=f"Training Epoch {epoch+1}", unit="file") as train_progress:
            for path in train_filepaths:
                inputs, input_sides, label_indexes = get_data(path)

                for i in range(len(inputs)):
                    _input_boards = inputs[i]
                    _input_sides = [input_sides[i] for _ in range(len(_input_boards))]
                    direct_inputs = model.convert_boards_to_xs(_input_boards, _input_sides)

                    # Pass the inputs through the model
                    vls = model(direct_inputs)

                    # Ensure vls is of shape (N, C)
                    vls = vls.view(1, -1)

                    # Create the target tensor where the first element is the highest value
                    label = torch.tensor([0], dtype=torch.long).to(public_device)

                    # Calculate the loss
                    _loss = cross_loss(vls, label)

                    opt.zero_grad()  # Zero the gradients
                    _loss.backward()  # Backpropagation
                    opt.step()  # Optimizer step

                    all_train_loss += _loss.item()

                    # Calculate accuracy
                    _, predicted = torch.max(vls, 1)
                    all_train_acc += (predicted == label).sum().item()
                    all_train_num += label.size(0)

                train_progress.set_postfix(
                    train_loss=all_train_loss / all_train_num if all_train_num > 0 else float('inf'),
                    train_acc=all_train_acc / all_train_num if all_train_num > 0 else 0
                )
                train_progress.update(1)

        avg_train_loss = all_train_loss / all_train_num
        avg_train_acc = all_train_acc / all_train_num

        # Evaluation on the test set
        model.eval()
        all_test_loss = 0
        all_test_acc = 0
        all_test_num = 0

        with tqdm(total=len(test_filepaths), desc=f"Testing Epoch {epoch+1}", unit="file") as test_progress:
            for path in test_filepaths:
                inputs, input_sides, label_indexes = get_data(path)

                with torch.no_grad():
                    for i in range(len(inputs)):
                        _input_boards = inputs[i]
                        _input_sides = [input_sides[i] for _ in range(len(_input_boards))]
                        direct_inputs = model.convert_boards_to_xs(_input_boards, _input_sides)

                        # Pass the inputs through the model
                        vls = model(direct_inputs)

                        # Ensure vls is of shape (N, C)
                        vls = vls.view(1, -1)

                        # Create the target tensor where the first element is the highest value
                        label = torch.tensor([0], dtype=torch.long).to(public_device)

                        # Calculate the loss
                        _loss = cross_loss(vls, label)

                        all_test_loss += _loss.item()

                        # Calculate accuracy
                        _, predicted = torch.max(vls, 1)
                        all_test_acc += (predicted == label).sum().item()
                        all_test_num += label.size(0)

                test_progress.set_postfix(
                    test_loss=all_test_loss / all_test_num if all_test_num > 0 else float('inf'),
                    test_acc=all_test_acc / all_test_num if all_test_num > 0 else 0
                )
                test_progress.update(1)

        avg_test_loss = all_test_loss / all_test_num
        avg_test_acc = all_test_acc / all_test_num

        # Save the model
        model_path = f"model_epoch_{epoch+1}_train_acc_{avg_train_acc:.4f}_train_loss_{avg_train_loss:.4f}_test_acc_{avg_test_acc:.4f}_test_loss_{avg_test_loss:.4f}.pth"
        torch.save(model.state_dict(), model_path)

        print(f"Epoch {epoch+1}, Train Loss: {avg_train_loss:.4f}, Train Accuracy: {avg_train_acc:.4f}, "
              f"Test Loss: {avg_test_loss:.4f}, Test Accuracy: {avg_test_acc:.4f}")

if __name__ == "__main__":
    train()