#!/usr/bin/env python3

import argparse
import os
import numpy as np
import matplotlib.pyplot as plt
from scipy import optimize

def clip(x):
    return x[1:-2]

def sensor_model(x, a, b):
    return -a*pow(x, b) + 180

def bigger_sensor_model(x, a, b, c):
    return -a*pow(x, b) + c

def weighting_model():
    return np.array([1, 1, 1, 0.95, 0.8, 0.5, 0.3, 0.3, 0.5, 0.8, 0.95, 1, 1, 1])


def main():
    np.warnings.simplefilter("ignore", optimize.OptimizeWarning)
    np.set_printoptions(suppress=True, precision=6)

    parser = argparse.ArgumentParser("plot a log file from analog_test.cpp")
    parser.add_argument("logs", help="csv file output form logging serial", nargs="+")
    parser.add_argument("--no-plot", help="skip plotting", action="store_true")
    parser.add_argument("--samples-per-interval", '-n', help="number of readings at each distance", type=int, default=10)
    parser.add_argument("--number-of-distances", '-d', help="number of distances", type=int, default=17)
    parser.add_argument("--spacing", '-s', help="spacing between each interval in meters", type=float, default=0.005)

    args = parser.parse_args()

    # load data
    MAX_DIST = 0.1
    MIN_DIST = 0.01
    S = len(args.logs)
    N = args.samples_per_interval
    D = args.number_of_distances
    distances = np.arange(D, 0, -1) * args.spacing
    distances[0] = MAX_DIST
    columns = {"B": 0, "A": 1, "F": 2, "E": 3, "G": 4, "D": 5, "H": 6}

    data = np.ndarray((len(args.logs), D, N))
    for i, log in enumerate(args.logs):
        letter = os.path.basename(log).strip(".csv")
        column = columns[letter]
        x = np.genfromtxt(log, delimiter=', ')[:, column]
        x = x.reshape((D, N))
        data[i] = x

    # compute the mean and errors for each interval
    means = data.mean(axis=2)
    ranges = np.stack((data.max(axis=2) - means, means - data.min(axis=2)), axis=1)

    # fit our model Y=a*X^b+c for each sensor
    params = np.ndarray((S, 2))
    model_errors = np.ndarray((S, D-3))
    model_predictions = np.ndarray((S, D-3))
    print("|sensor|a|b|")
    print("|------|-|-|")
    for i, m in enumerate(means):
        # we ignore the first data point here because it doesn't have a proper distance, it's infinitly far
        # we also ignore the last three points where shit starts to go down
        m = clip(m)
        d = clip(distances)
        weights = weighting_model()
        p, _ = optimize.curve_fit(sensor_model, m, d, sigma=weights, maxfev=100000)
        model_prediction = sensor_model(m, *p)
        model_error = model_prediction - d
        print("|{:s}|{:0.6f}|{:0.6f}|".format(args.logs[i].strip(".csv"), *p))
        params[i] = p
        model_predictions[i] = model_prediction
        model_errors[i] = model_error

    # compute the "average" model
    average_model_params = params.mean(axis=0)
    average_model_errors = np.ndarray((S, D-3))
    for i, m in enumerate(means):
        m = clip(m)
        average_model_prediction = sensor_model(m, *average_model_params)
        average_model_error = average_model_prediction - d
        average_model_errors[i] = average_model_error

    if not args.no_plot:
        colors = { 'A': 'r', 'B': 'b', 'D': 'g', 'E': 'y', 'F': 'm', 'G': 'k', 'H': 'sienna'}
        plt.figure()
        plt.plot([distances[1], distances[-3]], [0, 0], label='zero', linestyle='--')
        for model_error, log in zip(model_errors, args.logs):
            plt.plot(clip(distances), model_error, label=log)
        plt.title("Modeling Error")
        plt.xlabel("distance")
        plt.ylabel("error (meters)")
        plt.legend()

        plt.figure()
        plt.plot([distances[1], distances[-3]], [0, 0], label='zero', linestyle='--')
        for average_model_error, log in zip(average_model_errors, args.logs):
            plt.plot(clip(distances), average_model_error, label=log)
        plt.title("Error with 'Average' Model: a={:0.3f} b={:0.6f}".format(*average_model_params))
        plt.xlabel("distance")
        plt.ylabel("error (meters)")
        plt.legend()

        plt.figure()
        for d, predictions, xerr, log in zip(data, model_predictions, ranges, args.logs):
            letter = os.path.basename(log).strip(".csv")
            #plt.scatter(x=d.mean(axis=1), y=distances, label=log, s=4)
            plt.errorbar(x=d.mean(axis=1), y=distances, xerr=xerr, linewidth=1, c=colors[letter])
            plt.plot(clip(d.mean(axis=1)), predictions, label=log + " model", alpha=0.7, linestyle='--', c=colors[letter])
        plt.title("Averaged Data")
        plt.xlabel("ADC Value 0-8192")
        plt.ylabel("Distance (meters)")
        plt.legend()

        plt.show()


if __name__ == '__main__':
    main()
