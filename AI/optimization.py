import math
import random


def f_booth(x, y) -> float:
    return (x + 2 * y - 7) ** 2 + (2 * x + y - 5) ** 2


def f_himmelblau(x, y) -> float:
    return (x**2 + y - 11) ** 2 + (x + y**2 - 7) ** 2


def f_griewank(x, y) -> float:
    return (1 + (x**2 + y**2) / 4000) - (math.cos(x) * math.cos(y / math.sqrt(2)))


def returnWithinBound(value, min_val, max_val):
    # Makes sure the values remain in the given range
    return max(min_val, min(value, max_val))


def optimise(N, temp, K, function, minBound, maxBound, minimizing=True):
    """
    N is the neighbour-hood size
    T is the starting temperature, reduced by 0.1 after K iterations
    K is the number of iterations to run for a given temperature
    """
    current_x, current_y = random.uniform(minBound, maxBound), random.uniform(
        minBound, maxBound
    )
    # To start from x and y as 0, uncomment the line below
    # current_x, current_y = 0, 0

    best_x, best_y = current_x, current_y
    best_f = f_booth(current_x, current_y)
    # Picking a random number in a given range

    # Runs while the temperature is greater than 0, otherwise there will be a division error in line 52
    while temp > 0:
        for i in range(1, K):
            step_x = random.uniform(-N, N)
            step_y = random.uniform(-N, N)

            trial_x, trial_y = returnWithinBound(
                current_x + step_x, minBound, maxBound
            ), returnWithinBound(current_y + step_y, minBound, maxBound)
            current_f = function(current_x, current_y)
            trial_f = function(trial_x, trial_y)

            if minimizing:
                delta = (
                    current_f - trial_f
                )  # If we are minimising, ideally current_f > trial_f
            else:
                delta = (
                    trial_f - current_f
                )  # If we are maximising, ideally trial_f > current_f

            # Delta will be positive (delta > 0) in either of the ideal cases, based on if we are tryign to maximize or minimize

            if delta > 0:
                current_x, current_y = trial_x, trial_y

                if (minimizing and trial_f < best_f) or (
                    not minimizing and trial_f > best_f
                ):  # Chooses the scenario based on if we are trying to find the minima or the maxima
                    best_x, best_y = trial_x, trial_y
                    best_f = trial_f

            else:
                m = math.exp(
                    delta / temp
                )  # Based on whether we are trying to find the maxima or minima, delta is flipped earlier, therefore this line works as is
                p = random.random()
                if p < m:
                    current_x, current_y = trial_x, trial_y

        temp *= 0.1

    return best_x, best_y


def main():
    # Parameters provided in the assignment
    neighbourhoodSize = 0.5
    startingTemperature = 1.0
    iterations = 100

    # Booth function
    print("Booth Function: ")
    print(
        optimise(
            neighbourhoodSize, startingTemperature, iterations, f_booth, -10, 10, True
        )
    )  # MIN
    print(
        optimise(
            neighbourhoodSize, startingTemperature, iterations, f_booth, -10, 10, False
        )
    )  # MAX
    # Himmelblau function
    print("Himmelblau Function: ")
    print(
        optimise(
            neighbourhoodSize,
            startingTemperature,
            iterations,
            f_himmelblau,
            -5,
            5,
            True,
        )
    )  # MIN
    print(
        optimise(
            neighbourhoodSize,
            startingTemperature,
            iterations,
            f_himmelblau,
            -5,
            5,
            False,
        )
    )  # MAX
    # Griewank function
    print("Griewank Function: ")
    print(
        optimise(
            neighbourhoodSize,
            startingTemperature,
            iterations,
            f_griewank,
            -30,
            30,
            True,
        )
    )  # MIN
    print(
        optimise(
            neighbourhoodSize,
            startingTemperature,
            iterations,
            f_griewank,
            -30,
            30,
            False,
        )
    )  # MAX


main()
