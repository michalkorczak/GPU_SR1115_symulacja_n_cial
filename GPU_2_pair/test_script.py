import subprocess
import time

import pandas as pd

def run_simulation(number_of_bodies, iterations, save_interval, dt, output_filename):
    command = [
        "./NBodyProblem",
        str(number_of_bodies),
        str(iterations),
        str(save_interval),
        str(dt),
        output_filename
    ]
    
    start_time = time.time()
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    end_time = time.time()
    execution_time = (end_time - start_time) * 1000
    execution_time = round(execution_time)
    
    print(f"Symulacja zakończona w czasie: {execution_time:.4f} ")

    
    return {
        "N": number_of_bodies,
        "ExecutionTime(ms)": execution_time
    }

def test_different_parameters():
    number_of_bodies_list = list(range(10, 4121, 10))
    iterations = 1
    save_interval = 10
    dt = 0.8

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
