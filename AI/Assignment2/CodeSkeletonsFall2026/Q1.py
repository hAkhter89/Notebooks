# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.19.6
#   kernelspec:
#     display_name: Python 3 (ipykernel)
#     language: python
#     name: python3
# ---

# %%
import numpy as np
import matplotlib.pyplot as plt

np.random.seed(42)   # makes the random weights repeatable


# %%
# Data Generation
c = 1e-3

def target_function(x, y, c=c):
    # TODO: implement f(x, y) = exp(-(x^2 + y^2) / 8) * cos(sqrt(x^2 + y^2) + c)    
    r2 = x**2 + y**2;
    fxy = np.exp((-r2)/8) * np.cos(np.sqrt(r2) + c);
    return fxy;

def generate_data(start, end, step=0.2):

# TODO: generate training data -> x, y in [-2, 2], step 0.2
# TODO: generate testing data  -> x, y in [-1.9, 1.9], step 0.2
    x = np.arange(start, end + step/2, step);
    y = np.arange(start, end + step/2, step);
    X =  np.column_stack([x, y])
    target = target_function(X[:,0], X[:, 1]).reshape(-1,1);
    return X, target

X_train, y_train = generate_data(-2.0, 2.0)
X_test, y_test = generate_data(-1.9, 1.9)

print(X_train.shape, y_train.shape)   #  (21, 2) (21, 1)
print(X_test.shape, y_test.shape)     # (20, 2) (20, 1)



# Deciding which activiation function
def get_activation(name):
    """Returns (function, derivative). Both take z, the weighted sum."""
    if name == "tansig":
        func = lambda x: np.tanh(x)
        deriv = lambda x: 1-np.tanh(x)**2
    elif name == "logsig":
        func = lambda x: 1/(1+np.exp(-x))
        deriv = lambda x: func(x)*(1-func(x))
    elif name == "relu":
        func = lambda x: np.maximum(0,x)
        deriv = lambda x: (x>0).astype(float)
    else:
        raise ValueError("unknown activation: " + name)
    return func, deriv

# %%
# Model Training

def train(X_train, y_train,
                epochs=5000,
                lr=0.05,
                hidden_layers=[40],
                activation="tansig",
                bias_init=0.5):
    """
    Parameters:
        X_train, y_train : training data
        epochs : number of iterations
        lr : learning rate
        hidden_layers : list of hidden layer sizes, e.g. [40] or [64,32]
        activation : activation function name ('tansig', 'logsig', 'relu', etc.)
        bias_init : initial bias value (float)

    Returns:
        model_params, losses
    """

    # TODO: initialize weights and biases based on hidden_layers
    # TODO: select activation function based on parameter
    # TODO: implement training loop
    initial_sizes = [X_train.shape[1]] + list(hidden_layers) + [1];
    weights = []
    biases = []
    for i in range(len(initial_sizes)-1):
        w = np.random.randn(initial_sizes[i], initial_sizes[i+1]) * 0.5 # creates a 2x40 table of random weights, multiplied by 0.5 to shrink the SD.
        b = np.full((1, initial_sizes[i+1]), float(bias_init)) # table of biases, all neurons are 0.5
        weights.append(w)
        biases.append(b)
    # Deciding which activiation function
    func, deriv = get_activation(activation)
    # Training loop
    N = X_train.shape[0]
    losses = []

    for epoch in range(epochs):
        # Forward pass
        a = X_train
        a_list = [X_train] # activated values
        z_list = [] # stores values before activation

        for i in range(len(weights) - 1):
            z = a @ weights[i] + biases[i] # zij = (xi.wij) + (yi.wij) + bi
            a = func(z) # applies the relevant acitivation
            z_list.append(z)
            a_list.append(a)
        out = a @ weights[-1] + biases[-1] # 21x1 matrix for the output layer, outi = ai0.v0 + ... + ai39v39 + bi
        
        # Errors Mean squared
        error = out - y_train
        losses.append(0.5 * np.mean(error**2))

        # Backpass
        delta = error / N 
        for j in range(len(weights)-1, -1, -1):
            dW = a_list[j].T @ delta
            db = np.sum(delta, axis=0, keepdims=True)
            if j > 0:
                delta = (delta @ weights[j].T) * deriv(z_list[j-1])
            weights[j] -= lr * dW
            biases[j] -= lr * db 
    model_params = {"weights": weights, "biases": biases, "activation": activation}
    return model_params, np.array(losses)
        
# TODO: call train and capture losses
# TODO: plot cost vs epoch
model, losses = train(X_train, y_train)
print(losses[0], losses[-1])

plt.figure(figsize=(6, 4))
plt.semilogy(losses)
plt.xlabel("Epoch"); plt.ylabel("Cost J (log scale)")
plt.title("Cost vs epoch"); plt.grid(alpha=0.3)
plt.show()


# %%
# Model Testing
def predict(model, X_test):
    # TODO: forward pass only
    func, _ = get_activation(model["activation"])
    weights, biases = model["weights"], model["biases"]

    a = X_test
    for i in range(len(weights) - 1):               # hidden layers
        a = func(a @ weights[i] + biases[i])
    return a @ weights[-1] + biases[-1]             # linear output

# TODO: run predictions on train & test
pred_train = predict(model, X_train)
pred_test  = predict(model, X_test)

print("Train MSE:", np.mean((pred_train - y_train)**2))
print("Test  MSE:", np.mean((pred_test - y_test)**2))

# TODO: plot predicted vs ground truth
plt.figure(figsize=(8, 4))

# Ground truth curves/points
plt.plot(X_train[:, 0], y_train, "b-o", ms=5, label="True Train")
plt.plot(X_test[:, 0], y_test, "g-s", ms=5, label="True Test")

# Predicted curves
plt.plot(X_train[:, 0], pred_train, "r--x", ms=6, label="Predicted Train")
plt.plot(X_test[:, 0], pred_test, "m--^", ms=6, label="Predicted Test")

plt.xlabel("x (where y = x)")
plt.ylabel("f(x, y)")
plt.title("Predicted vs Ground Truth (Train and Test)")
plt.legend()
plt.grid(alpha=0.3)
plt.show()


# %%
# Exploring hyperparameters
# Suggested learning rates to compare: 0.01, 0.1, 0.3
# Also compare: number of hidden layers, activation function (tansig vs logsig vs relu), initial bias value

experiments = {
    "Learning Rate": [
        ("lr=0.01", {"lr": 0.01}),
        ("lr=0.05 (base)", {"lr": 0.05}),
        ("lr=0.1", {"lr": 0.1}),
        ("lr=0.3", {"lr": 0.3}),
    ],
    "Number of Layers": [
        ("1 layer [40]", {"hidden_layers": [40]}),
        ("2 layers [40, 20]", {"hidden_layers": [40, 20]}),
        ("3 layers [40, 20, 10]", {"hidden_layers": [40, 20, 10]}),
    ],
    "Activation Function": [
        ("tansig", {"activation": "tansig"}),
        ("logsig", {"activation": "logsig"}),
        ("relu", {"activation": "relu"}),
    ],
    "Initial Bias": [
        ("bias=0.0", {"bias_init": 0.0}),
        ("bias=0.5 (base)", {"bias_init": 0.5}),
        ("bias=1.0", {"bias_init": 1.0}),
        ("bias=2.0", {"bias_init": 2.0}),
    ],
}

for group_name, configs in experiments.items():
    print(f"\n=== {group_name} ===")
    fig, (ax_loss, ax_pred) = plt.subplots(1, 2, figsize=(12, 4))
    
    # Plot ground truth test curve once on the right subplot
    ax_pred.plot(X_test[:, 0], y_test, "k-", lw=2, label="True Test")

    for label, params in configs:
        np.random.seed(42)  # Keep initial weights identical
        m, l = train(X_train, y_train, **params)
        p_tr = predict(m, X_train)
        p_te = predict(m, X_test)
        
        tr_mse = np.mean((p_tr - y_train)**2)
        te_mse = np.mean((p_te - y_test)**2)
        print(f"{label:<22} | Train MSE: {tr_mse:.6f} | Test MSE: {te_mse:.6f}")

        ax_loss.semilogy(l, label=label)
        ax_pred.plot(X_test[:, 0], p_te, "--", label=label)

    ax_loss.set_title(f"{group_name} — Cost vs Epoch")
    ax_loss.set_xlabel("Epoch"); ax_loss.set_ylabel("Cost J (log scale)")
    ax_loss.legend(); ax_loss.grid(alpha=0.3)

    ax_pred.set_title(f"{group_name} — Test Predictions")
    ax_pred.set_xlabel("x (where y = x)"); ax_pred.set_ylabel("f(x, y)")
    ax_pred.legend(); ax_pred.grid(alpha=0.3)

    plt.tight_layout()
    plt.show()
