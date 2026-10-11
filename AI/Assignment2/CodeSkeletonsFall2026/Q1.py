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
    axis = np.arange(start, end + step/2, step);
    #axis = np.arange(-1.9, 1.9 + 0.1, 0.2);
    xx, yy = np.meshgrid(axis, axis);
    X =  np.column_stack([xx.ravel(), yy.ravel()]);
    y = target_function(X[:,0], X[:, 1]).reshape(-1,1);
    return X, y, axis



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
    initial_sizes = [X_train.shape[1]] + hidden layers + [1];
    weights = []
    biases = []
    for i in range(len(initial_sizes)-1):
        w = np.random.randn(initial_sizes[i], initial_sizes[i+1]) * 0.5 # creates a 2x40 table of random weights, multiplied by 0.5 to shrink the SD.
        b = np.full((1, initial_sizes[i+1]),bias_init) # table of biases, all neurons are 0.5
        weights.append (w)
        biases.append(b)
    # Deciding which activiation function
    if activation == "tansig":
        func = lambda x: np.tanh(x)
        deriv = lambda x: 1-np.tanh(x)**2
    elif activation == "logsig":
        func = lambda x: 1/(1+np.exp(-x))
        deriv = lambda x: func(x)*(1-func(x))
    else:
        func = lambda x: np.maximum(0,x)
        deriv = lambda x: (x>0).astype(float)
    # Training loop
    N = X_train.shape[0]
    losses = []

    for epoch in range(epochs):
        # Forward pass
        a = X_train
        a_list = [X_train] # activated values
        z_list = [] # stores values before activation

        for i in range(len(weights) - 1):
            z = a @ weights[i] + biases[i]
            a = func(z)
            z_list.append(z)
            a_list.append(a)
        out = a @ weights[-1] + biases[-1] # 441x1 matrix for the output layer
        
        # Errors Mean squared
        error = out - y_train
        losses.append(0.5 * np.mean(error**2))

        # Backpass
        delta = error / N 
        for j in range(len(weights)-1, -1, -1):
            dW = a_list[j].T @ delta
            db = np.sum(delta, axis=0, keepdims=True)
            if j > 0:
                detla = (delta @ weights[j].T) * deriv(z_list[j-1])
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
    pass

# TODO: run predictions on train & test
# TODO: plot predicted vs ground truth


# %%
# Exploring hyperparameters
# Suggested learning rates to compare: 0.01, 0.1, 0.3
# Also compare: number of hidden layers, activation function (tansig vs logsig vs relu), initial bias value

