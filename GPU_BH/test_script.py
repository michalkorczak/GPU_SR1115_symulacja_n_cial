import subprocess
import time
import json
import pandas as pd
import os

def generate_config(number_of_bodies, iterations, save_interval, dt, output_filename):
    config = {
        "numberOfBodies": number_of_bodies,
        "iterations": iterations,
        "saveInterval": save_interval,
        "dt": dt,
        "outputFilename": output_filename
    }
    
    config_filename = "config.json"
    with open(config_filename, "w") as f:
        json.dump(config, f, indent=4)
    
    return config_filename


def run_simulation(number_of_bodies, iterations, save_interval, dt, output_filename):
    config_filename = generate_config(number_of_bodies, iterations, save_interval, dt, output_filename)
    
    command = [
        "./NBodyProblem",
        "--config", config_filename  
    ]
    
    start_time = time.time()
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    end_time = time.time()
    
    execution_time = (end_time - start_time) * 1000
    execution_time = round(execution_time)
    
    print(f"Symulacja zakończona w czasie: {execution_time:.4f} ms")
    
    os.remove(config_filename)
    
    return {
        "N": number_of_bodies,
        "ExecutionTime(ms)": execution_time
    }

def test_different_parameters():
    number_of_bodies_list = list(range(10, 100, 10))
    iterations = 1
    save_interval = 10
    dt = 0.1

    output_filename = "output.json"
    results = []

    for number_of_bodies in number_of_bodies_list:
        #print(f"Test: {number_of_bodies} ciał, {iterations} iteracji, save co {save_interval} krok, dt = {dt}")
        result = run_simulation(number_of_bodies, iterations, save_interval, dt, output_filename)
        results.append(result)

    df = pd.DataFrame(results)
    
    df.to_excel("symulacja_wyniki.xlsx", index=False)
    print("Wyniki zapisane do pliku symulacja_wyniki.xlsx")

if __name__ == "__main__":
    test_different_parameters()
